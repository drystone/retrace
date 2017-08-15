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

enum pre_or_post {BOTH, PRE, POST};

struct data {
	const char *length_format;
};

struct type {
	const char *name;
	const char *ctype;
	const char *length_fn;
} types[] = {
	{"char",	"int ",			NULL		},
	{"cstring",	"const char *",		"SAFE_STRLEN"	},
	{"dir",		"DIR *",		NULL		},
	{"dirent",	"struct dirent *",	"DIRENT_SIZE"	},
	{"fd",		"int ",			NULL		},
	{"file",	"FILE *",		NULL		},
	{"int",		"int ",			NULL		},
	{"pchar",	"char *",		NULL		},
	{"pcvoid",	"const void *",		NULL		},
	{"pdirent",	"struct dirent **",	NULL		},
	{"pid_t",	"pid_t ",		NULL	 	},
	{"pvoid",	"void *",		NULL		},
	{"size_t",	"size_t ",		NULL		},
	{"ssize_t",	"ssize_t ",		NULL		},
	{"string",	"char *",		"SAFE_STRLEN"	},
	{"va_list",	"va_list ",		NULL		},
	{"void",	"void ",		NULL		},
	{NULL,		NULL,			NULL		}
};

struct rpctype {
	const char *name;
	const char *ctype;
} rpctypes[] = {
	{"int",		"int "		},
	{"pid_t",	"pid_t "	},
	{"ptr",		"void *"	},
	{"size_t",	"size_t "	},
	{"ssize_t",	"ssize_t "	},
	{NULL,		NULL		}
};

struct param {
	TAILQ_ENTRY(param) next;
	const char *name;
	enum pre_or_post pre_or_post;
	const char *ctype;
	const char *length_fn;
};

TAILQ_HEAD(param_list, param);

struct extra {
	TAILQ_ENTRY(extra) next;
	const char *name;
	enum pre_or_post pre_or_post;
	const char *init;
	const char *ctype;
	const char *length_fn;
};

TAILQ_HEAD(extra_list, extra);

struct function {
	STAILQ_ENTRY(function) next;
	const char *name;
	const struct type *type;
	struct param_list params;
	struct extra_list extras;
	const char *va_fn;
};

STAILQ_HEAD(function_list, function);

void
*check_alloc(void *p)
{
	if (p == NULL)
		error(1, 0, "Out of memory.");
	return p;
}

char *
_strdup(const char *s)
{
	if (s == NULL)
		return NULL;
	return check_alloc(strdup(s));
}

const struct type *
lookup_type(const char *s)
{
	struct type *p;

	for (p = types; p->name; p++)
		if (!strcmp(p->name, s))
			return p;
	return NULL;
}

void yaml(struct function_list *fns)
{
	struct function *fn;
	struct param *param;
	struct extra *extra;
	int rpccount, preredirect_data, postredirect_data;

	printf("---\n");
	printf("functions:\n");
	STAILQ_FOREACH(fn, fns, next) {

		printf("- name: %s\n", fn->name);
		printf("  type: \"%s\"\n", fn->type->ctype);
		if (strcmp(fn->type->name, "void") != 0)
			printf("  result: true\n");

		if (!TAILQ_EMPTY(&(fn->params))) {
			printf("  params:\n");
			TAILQ_FOREACH(param, &(fn->params), next) {
				printf("  - name: %s\n", param->name);
				printf("    type: \"%s\"\n",
				    param->ctype);
			}
			printf("    last: true\n");
		}
		
		if (fn->va_fn) {
			printf("  variadic: %s\n", fn->va_fn);
			printf("  last_param: %s\n",
			    TAILQ_LAST(&(fn->params), param_list)->name);
		}

		if (!TAILQ_EMPTY(&(fn->extras))) {
			printf("  extras:\n");
			TAILQ_FOREACH(extra, &(fn->extras), next) {
				printf("  - name: %s\n", extra->name);
				printf("    type: \"%s\"\n",
				    extra->ctype);
				if (extra->pre_or_post == PRE)
					printf("    pre: true\n");
				else
					printf("    post: true\n");
				printf("    init: %s\n", extra->init);
			}
		}
		
		rpccount = 0;
		printf("  rpcvalues:\n");
		TAILQ_FOREACH(param, &(fn->params), next) {
			printf("  - param: true\n");
			printf("    pre: true\n");
			printf("    post: true\n");
			printf("    base: \"&%s\"\n", param->name);
			printf("    len: sizeof(%s)\n", param->name);
			++rpccount;
		}
		TAILQ_FOREACH(extra, &(fn->extras), next) {
			printf("  - extra: true\n");
			if (extra->pre_or_post == PRE)
				printf("    pre: true\n");
			else
				printf("    post: true\n");
			printf("    base: \"&%s\"\n", extra->name);
			printf("    len: sizeof(%s)\n", extra->name);
			++rpccount;
		}
		if (strcmp(fn->type->name, "void") != 0) {
			printf("  - result: true\n");
			printf("    post: true\n");
			printf("    base: \"&_result\"\n");
			printf("    len: sizeof(_result)\n");
			++rpccount;
		}
		printf("  - errno: true\n");
		printf("    post: true\n");
		printf("    base: \"&errno\"\n");
		printf("    len: sizeof(errno)\n");
		++rpccount;

		preredirect_data = 0;
		postredirect_data = 0;
		TAILQ_FOREACH(param, &(fn->params), next) {
			if (param->length_fn != NULL) {
				printf("  - param-data: true\n");
				if (param->pre_or_post != POST) {
					printf("    pre: true\n");
					preredirect_data = 1;
				}
				if (param->pre_or_post != PRE) {
					printf("    post: true\n");
					postredirect_data = 1;
				}
				printf("    name: %s\n", param->name);
				printf("    base: \"(void *)%s\"\n", param->name);
				printf("    len: %s(%s)\n", param->length_fn, param->name);
				++rpccount;
			}
		}

		if (fn->type->length_fn != NULL) {
			printf("  - result-data: true\n");
			printf("    post: true\n");
			printf("    name: _result\n");
			printf("    base: (void *)_result\n");
			printf("    len: %s(_result)\n", fn->type->length_fn);
			postredirect_data = 1;
			++rpccount;
		}

		printf("  max-rpcvalues: %d\n", rpccount);
		if (preredirect_data)
			printf("  preredirect-data: true\n");
		if (postredirect_data)
			printf("  postredirect-data: true\n");
	}
	printf("---\n");
}

int main(void)
{
	char *buf = NULL;
	size_t buflen = 0;
	ssize_t len;
	unsigned int line;
	const char *tok;
	struct function *fn = NULL;
	struct function_list functions;
	struct param *param;
	struct extra *extra;
	const struct type *type;

	STAILQ_INIT(&functions);

	line = 0;
	for (;;) {
		len = getline(&buf, &buflen, stdin);

		if (len <= 0)
			break;

		++line;

		/* strip \n from buffer */
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		if (*buf == ';')
			continue;

		tok = strtok(buf, " ");
		if (tok == NULL)
			continue;

		if (fn == NULL && strcmp(tok, "function") != 0)
			error(1, 0, "First line must be a function.");

		if (strcmp(tok, "function") == 0) {
			if (fn) {
				if (fn->va_fn && TAILQ_EMPTY(&(fn->params)))
					error(1, 0, "Variadic must have "
					    "parameters at line %d.",
					    line - 1);
				STAILQ_INSERT_TAIL(&functions, fn, next);
			}

			fn = check_alloc(malloc(sizeof(struct function)));
			memset(fn, 0, sizeof(struct function));
			TAILQ_INIT(&(fn->params));
			TAILQ_INIT(&(fn->extras));

			fn->name = _strdup(strtok(NULL, " "));
			if (fn->name == NULL)
				error(1, 0, "Keyword 'function' must be "
				    "followed by a function name at line %d.",
				    line);

			tok = strtok(NULL, " ");

			if (tok == NULL)
				error(1, 0, "Function without type "
				    "at line %d.", line);

			fn->type = lookup_type(tok);

			if (fn->type == NULL)
				error(1, 0, "Bad function type [%s] at "
				    "line %d.", tok, line);

			if (strtok(NULL, " "))
				error(1, 0, "Too many tokens at line %d.",
				    line);

		} else if (strcmp(tok, "parameter") == 0) {
			param = check_alloc(malloc(sizeof(struct param)));

			param->pre_or_post = BOTH;

			param->name = _strdup(strtok(NULL, " "));
			if (param->name == NULL)
				error(1, 0, "Parameter name not found "
				    "at line %d.", line);

			tok = strtok(NULL, " ");
			if (tok == NULL)
				error(1, 0, "Parameter must have a type "
				    "at line %d.", line);

			type = lookup_type(tok);

			if (type == NULL
			    || strcmp(tok, "void") == 0) 
				error(1, 0, "Bad parameter type [%s] at "
				    "line %d.", tok, line);

			param->ctype = type->ctype;
			param->length_fn = type->length_fn;

			if (strtok(NULL, " "))
				error(1, 0, "Too many tokens at line %d.",
				    line);
			
			TAILQ_INSERT_TAIL(&fn->params, param, next);
		} else if (strcmp(tok, "variadic") == 0) {
			if (fn->va_fn != NULL)
				error(1, 0, "Second 'variadic' found at line %d.", line);
			fn->va_fn = _strdup(strtok(NULL, " "));
			if (fn->va_fn == NULL)
				error(1, 0, "Keyword 'variadic' must be followed the name of a fixed param version at line %d.", line);
		} else if (strcmp(tok, "extra") == 0) {
			extra = check_alloc(malloc(sizeof(struct extra)));

			extra->name = _strdup(strtok(NULL, " "));
			if (extra->name == NULL)
				error(1, 0, "extra name not found "
				    "at line %d.", line);

			tok = strtok(NULL, " ");
			if (tok == NULL)
				error(1, 0, "extra must have a type "
				    "at line %d.", line);

			type = lookup_type(tok);

			if (type == NULL
			    || strcmp(tok, "void") == 0) 
				error(1, 0, "Bad extra type [%s] at "
				    "line %d.", tok, line);

			extra->ctype = type->ctype;
			extra->length_fn = type->length_fn;

			tok = strtok(NULL, " ");
			if (tok == NULL)
				error(1, 0, "extra pre_or_post not found at "
				    "line %d.", line);
			else if (strcmp(tok, "pre") == 0)
				extra->pre_or_post = PRE;
			else if (strcmp(tok, "post") == 0)
				extra->pre_or_post = POST;
			else 
				error(1, 0, "extra pre_or_post [%s] should be"
				    " \"pre\" or \"post\"at line %d.",
				    tok, line);

			extra->init = _strdup(strtok(NULL, ";"));

			if (extra->init == NULL)
				error(1, 0, "extra needs initialiser at "
				    "line %d.", line);
			
			TAILQ_INSERT_TAIL(&fn->extras, extra, next);
		}
	}

	if (fn)
		STAILQ_INSERT_TAIL(&functions, fn, next);

	yaml(&functions);

	return 0;
}
