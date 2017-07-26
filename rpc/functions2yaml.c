/*
 * Copyright (c) 2017, [Ribose Inc](https://www.ribose.com).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "../config.h"

#include <error.h>
#include <string.h>
#include <stdio.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <ctype.h>

enum rpc_type {
	RPC_VOID,
	RPC_PTR,
	RPC_INT,
	RPC_UINT,
	RPC_STR
};

struct type {
	char *ctype;
	char *rpctype;
	enum rpc_type rpctypenum;
	const char *header;
	const char *format;
} types[] = {
	{"char *",	"char *",	RPC_PTR,	NULL,		"%p"},
	{"const char *", "char *",	RPC_STR,	NULL,		"%p"},
	{"DIR *",	"DIR *",	RPC_PTR,	"dirent.h",	"%p"},
	{"FILE *",	"FILE *",	RPC_PTR,	"stdio.h",	"%p"},
	{"int",		"int",		RPC_INT,	NULL,		"%d"},
	{"pid_t",	"pid_t",	RPC_UINT,	"sys/types.h",	"%d"},
	{"size_t",	"size_t",	RPC_UINT,	"stdlib.h",	"%lu"},
	{"ssize_t",	"ssize_t",	RPC_INT,	"sys/types.h",	"%ld"},
	{"struct dirent *",
			"struct dirent*",
					RPC_PTR,	"dirent.h",	"%p"},
	{"struct dirent **",
			"struct dirent**",
					RPC_PTR,	"dirent.h",	"%p"},
	{"va_list",	"void *",	RPC_PTR,	"stdarg.h",	"%p"},
	{"void",	NULL,		RPC_VOID,	NULL,		NULL},
	{"void *",	"void *",	RPC_PTR,	NULL,		"%p"},
	{"const void *",
			"void *",	RPC_PTR,	NULL,		"%p"},
	{NULL,		NULL,		RPC_VOID,	NULL,		NULL}
};

struct param {
	STAILQ_ENTRY(param) next;
	char *name;
	struct type *type;
	char *inout;
};

STAILQ_HEAD(param_list, param);

struct header {
	STAILQ_ENTRY(header) next;
	char *name;
};

STAILQ_HEAD(header_list, header);

struct function {
	STAILQ_ENTRY(function) next;
	char *name;
	struct type *type;
	struct param_list params;
	char *va_fn;
};

STAILQ_HEAD(function_list, function);

const char *
rpc_typename(enum rpc_type type)
{
	switch (type) {
	case RPC_VOID:
		return "RPC_VOID";
	case RPC_PTR:
		return "RPC_PTR";
	case RPC_INT:
		return "RPC_INT";
	case RPC_UINT:
		return "RPC_UINT";
	case RPC_STR:
		return "RPC_STR";
	}
	return NULL;
}

void
*check_alloc(void *p)
{
	if (p == NULL)
		error(1, 0, "Out of memory.");
	return p;
}

struct type *
get_type(unsigned int line)
{
	struct type *p;
	char *ctype;

	/*
	 * type is remainder of line and won't contain a ';'
	 */
	ctype = strtok(NULL, ";");

	if (ctype == NULL)
		ctype = "";

	for (p = types; p->ctype != NULL; p++)
		if (!strcmp(p->ctype, ctype))
			return p;

	error(1, 0, "Unknown type [%s] at line %d.", ctype, line);

	return NULL;
}

void add_parameter(unsigned int line, struct param_list *params)
{
	struct param *param;
	const char *name, *inout;

	param = check_alloc(malloc(sizeof(struct param)));

	inout = strtok(NULL, " ");
	if (inout == NULL)
		error(1, 0, "Parameter in/out not found at line %d.", line);

	if (!strcmp(inout, "in"))
		param->inout = "RPC_INPARAM";
	else if (!strcmp(inout, "out"))
		param->inout = "RPC_OUTPARAM";
	else if (!strcmp(inout, "inout"))
		param->inout = "RPC_INOUTPARAM";
	else
		error(1, 0, "Parameter missing in/out at line %d.", line);

	name = strtok(NULL, " ");
	if (name == NULL)
		error(1, 0, "Parameter name not found at line %d.", line);

	param->name = check_alloc(strdup(name));

	param->type = get_type(line);

	if (param->type->rpctype == NULL)
		error(1, 0, "Parameters can't have 'void' type at line %d.", line);

	STAILQ_INSERT_TAIL(params, param, next);
}

void add_header(struct header_list *headers, const char *name)
{
	struct header *header;

	if (name == NULL)
		return;

	STAILQ_FOREACH(header, headers, next)
		if (!strcmp(header->name, name))
			return;

	header = check_alloc(malloc(sizeof(struct header)));
	header->name = (char *)name;
	STAILQ_INSERT_TAIL(headers, header, next);
}

const char *last_parameter_name(struct param_list *params)
{
	struct param *param;

	STAILQ_FOREACH(param, params, next)
		if (!STAILQ_NEXT(param, next))
			return param->name;

	return NULL;
}

void yaml(struct function_list *fns)
{
	struct function *function;
	struct param *param;
	char *ucname, *p;
	struct header_list headers;
	struct header *header;
	int num_params, has_data;

	printf("---\n");
	printf("functions:\n");
	STAILQ_FOREACH(function, fns, next) {
		num_params = 0;
		has_data = 0;
		STAILQ_FOREACH(param, &(function->params), next) {
			if (param->type->rpctypenum == RPC_STR)
				has_data = 1;
			++num_params;
		}

		ucname = check_alloc(strdup(function->name));
		for (p = ucname; *p; ++p)
			*p = toupper(*p);

		printf("- name: %s\n", function->name);
		printf("  enum: RPC_%s\n", ucname);
		printf("  type: %s\n", function->type->ctype);
		if (function->type->rpctype) {
			printf("  rpctype: %s\n", function->type->rpctype);
			printf("  format: \"%s\"\n", function->type->format);
			if (function->type->rpctypenum == RPC_STR)
				printf("  is_string: true\n");
		} else
			printf("  is_void: true\n");
		printf("  has_params: %s\n", num_params ? "true" : "false");
		printf("  has_data: %s\n", has_data ? "true" : "false");
		printf("  paramcount: %d\n", num_params);

		if (function->va_fn != NULL) {
			printf("  variadic:\n");
			printf("    function: %s\n", function->va_fn);
			printf("    start: %s\n",
			    last_parameter_name(&(function->params)));
		}
		if (!STAILQ_EMPTY(&(function->params))) {
			printf("  params:\n");
			STAILQ_FOREACH(param, &(function->params), next) {
				printf("  - name: %s\n", param->name);
				printf("    type: %s\n", param->type->ctype);
				printf("    rpctype: %s\n",
				    param->type->rpctype);
				printf("    inout: %s\n", param->inout);
				printf("    spacing: %s\n",
				    strchr(param->type->ctype, '*')
				    ? "\"\"" : "\" \"");
				printf("    format: \"%s\"\n",
				    param->type->format);
				if (param->type->rpctypenum == RPC_STR)
					printf("    is_string: true\n");
				if (!STAILQ_NEXT(param, next))
					printf("    last: true\n");
			}
		}
		free(ucname);
	}

	STAILQ_INIT(&headers);

	printf("headers:\n");
	STAILQ_FOREACH(function, fns, next) {
		if (function->type)
			add_header(&headers, function->type->header);

		STAILQ_FOREACH(param, &(function->params), next)
			add_header(&headers, param->type->header);
	}

	STAILQ_FOREACH(header, &headers, next)
		printf("  - header: %s\n", header->name);

	printf("---\n");
}

int main(void)
{
	char *buf = NULL;
	size_t buflen = 0;
	ssize_t len;
	unsigned int line = 0;
	const char *tok;
	struct function *fn = NULL;
	struct function_list functions;

	STAILQ_INIT(&functions);

	for (len = 0; len >= 0; ++line, len = getline(&buf, &buflen, stdin)) {

		if (len == 0)
			continue;

		if (*buf == ';')
			continue;

		/* strip \n from buffer */
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		tok = strtok(buf, " ");
		if (tok == NULL)
			continue;

		if (fn == NULL && strcmp(tok, "function") != 0)
			error(1, 0, "First line must be a function.");

		if (strcmp(tok, "function") == 0) {
			if (fn)
				STAILQ_INSERT_TAIL(&functions, fn, next);

			fn = check_alloc(malloc(sizeof(struct function)));
			memset(fn, 0, sizeof(struct function));
			STAILQ_INIT(&(fn->params));

			tok = strtok(NULL, " ");
			if (tok == NULL)
				error(1, 0, "Keyword 'function' must be followed by a function name at line %d.", line);

			fn->name = check_alloc(strdup(tok));
			fn->type = get_type(line);
		} else if (strcmp(tok, "parameter") == 0) {
			add_parameter(line, &(fn->params));
		} else if (strcmp(tok, "variadic") == 0) {
			if (fn->va_fn != NULL)
				error(1, 0, "Second 'variadic' found at line %d.", line);
			tok = strtok(NULL, " ");
			if (tok == NULL)
				error(1, 0, "Keyword 'variadic' must be followed the name of a fixed param version at line %d.", line);
			fn->va_fn = check_alloc(strdup(tok));
		}
	}

	if (fn)
		STAILQ_INSERT_TAIL(&functions, fn, next);

	yaml(&functions);

	return 0;
}
