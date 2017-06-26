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

#include "common.h"
#include "exec.h"
#include "malloc.h"
#include <unistd.h>

int RETRACE_IMPLEMENTATION(system)(const char *command)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&command};
	rtr_system_t real_system;
	int r;

	real_system = RETRACE_GET_REAL(system);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "system";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_system(command);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(system)

int RETRACE_IMPLEMENTATION(execl)(const char *path, const char *arg0, ... /*, (char *)0 */)
{
	int i = 0;
	int argsize = 2;
	const char **argv;
	char *p = NULL;
	rtr_execv_t real_execv;
	va_list arglist;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&path, &argv};

	real_execv = RETRACE_GET_REAL(execv);

	va_start(arglist, arg0);

	while ((p = va_arg(arglist, char *)) != NULL)
		argsize++;

	va_end(arglist);

	argv = RETRACE_GET_REAL(malloc)(argsize * sizeof(char *));

	va_start(arglist, arg0);

	argv[i] = arg0;
	argv[argsize - 1] = NULL;
	while ((p = va_arg(arglist, char *))) {
		argv[++i] = p;
	}

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execl";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	va_end(arglist);

	r = real_execv(path, (char *const *)argv);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	RETRACE_GET_REAL(free)(argv);

	return (r);
}

RETRACE_REPLACE(execl)

int RETRACE_IMPLEMENTATION(execv)(const char *path, char *const argv[])
{
	rtr_execv_t real_execv;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&path, &argv};


	real_execv = RETRACE_GET_REAL(execv);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execv";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_execv(path, argv);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(execv)

int RETRACE_IMPLEMENTATION(execle)(const char *path,
		const char *arg0,
		... /*, (char *)0, char *const envp[]*/)
{
	int argsize = 2;
	int i = 0;
	const char **argv;
	char *const *envp;
	char *p = NULL;
	rtr_execve_t real_execve;
	va_list arglist;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&path, &argv, &envp};


	real_execve = RETRACE_GET_REAL(execve);

	va_start(arglist, arg0);

	while ((p = va_arg(arglist, char *)) != NULL)
		argsize++;

	argv = RETRACE_GET_REAL(malloc)(argsize * sizeof(char *));

	va_start(arglist, arg0);

	argv[i] = arg0;
	argv[argsize - 1] = NULL;

	while ((p = va_arg(arglist, char *)) != NULL) {
		argv[++i] = p;
	}

	envp = va_arg(arglist, char **);

	va_end(arglist);


	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execle";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;

	r = real_execve(path, (char *const *)argv, envp);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	RETRACE_GET_REAL(free)(argv);

	return (r);
}

RETRACE_REPLACE(execle)

int RETRACE_IMPLEMENTATION(execve)(const char *path, char *const argv[], char *const envp[])
{
	rtr_execve_t real_execve;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_INT, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&path, &argv, &envp};

	real_execve = RETRACE_GET_REAL(execve);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execve";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_execve(path, argv, envp);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(execve)

int RETRACE_IMPLEMENTATION(execlp)(const char *file, const char *arg0, ... /*, (char *)0 */)
{
	int argsize = 2;
	int i = 0;
	int r;
	const char **argv;
	char *p = NULL;
	rtr_execvp_t real_execvp;
	va_list arglist;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&file, &argv};

	real_execvp = RETRACE_GET_REAL(execvp);

	va_start(arglist, arg0);

	while ((p = va_arg(arglist, char *)) != NULL)
		argsize++;

	argv = RETRACE_GET_REAL(malloc)(argsize * sizeof(char *));

	va_start(arglist, arg0);

	argv[i] = arg0;
	argv[argsize - 1] = NULL;

	while ((p = va_arg(arglist, char *))) {
		argv[++i] = p;
	}


	va_end(arglist);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execlp";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_execvp(file, (char *const *)argv);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	RETRACE_GET_REAL(free)(argv);

	return (r);
}

RETRACE_REPLACE(execlp)

int RETRACE_IMPLEMENTATION(execvp)(const char *file, char *const argv[])
{
	rtr_execvp_t real_execvp;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&file, &argv};

	real_execvp = RETRACE_GET_REAL(execvp);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execvp";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_execvp(file, argv);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);


	return (r);
}

RETRACE_REPLACE(execvp)

#ifndef __APPLE__
int RETRACE_IMPLEMENTATION(execvpe)(const char *file, char *const argv[], char *const envp[])
{
	rtr_execvpe_t real_execvpe;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&file, &argv, &envp};

	real_execvpe = RETRACE_GET_REAL(execvpe);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execvpe";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_execvpe(file, argv, envp);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(execvpe)

int RETRACE_IMPLEMENTATION(execveat)(int dirfd, const char *pathname,
		char *const argv[], char *const envp[], int flags)
{
	rtr_execveat_t real_execveat;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR,
					  PARAMETER_TYPE_STRING,
					  PARAMETER_TYPE_STRING_ARRAY,
					  PARAMETER_TYPE_STRING_ARRAY,
					  PARAMETER_TYPE_INT,
					  PARAMETER_TYPE_END};
	void const *parameter_values[] = {&dirfd, &pathname, &argv, &envp, &flags};

	real_execveat = RETRACE_GET_REAL(execveat);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "execveat";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_execveat(dirfd, pathname, argv, envp, flags);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(execveat)

int RETRACE_IMPLEMENTATION(fexecve)(int fd, char *const argv[], char *const envp[])
{
	rtr_fexecve_t real_fexecve;
	int r;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_STRING_ARRAY, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&fd, &argv, &envp};

	real_fexecve = RETRACE_GET_REAL(fexecve);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fexecve";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fexecve(fd, argv, envp);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return r;
}

RETRACE_REPLACE(fexecve)

#endif /* !__APPLE__ */
