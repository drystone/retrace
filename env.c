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

#ifdef __FreeBSD__
#define _KERNEL
#endif

#include "common.h"
#include "env.h"

int RETRACE_IMPLEMENTATION(unsetenv)(const char *name)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&name};
	int r;
	rtr_unsetenv_t real_unsetenv;

	real_unsetenv = RETRACE_GET_REAL(unsetenv);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "unsetenv";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_unsetenv(name);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(unsetenv)

int RETRACE_IMPLEMENTATION(putenv)(char *string)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&string};
	int r;

	rtr_putenv_t real_putenv;

	real_putenv = RETRACE_GET_REAL(putenv);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "putenv";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_putenv(string);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(putenv)

char *RETRACE_IMPLEMENTATION(getenv)(const char *envname)
{
  	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&envname};
	rtr_getenv_t real_getenv;
	char *env = NULL;

	real_getenv = RETRACE_GET_REAL(getenv);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "getenv";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_STRING;
	event_info.return_value = &env;

	retrace_event (&event_info);

	env = real_getenv(envname);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (env);
}

RETRACE_REPLACE(getenv)

int RETRACE_IMPLEMENTATION(uname)(struct utsname *buf)
{

	int ret;
	rtr_uname_t real_uname;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_UTSNAME, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&buf};

	real_uname = RETRACE_GET_REAL(uname);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "uname";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &ret;
	retrace_event (&event_info);

	ret = real_uname(buf);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	if (ret == 0)
		trace_printf(1, "uname(); [%s, %s, %s, %s, %s]\n", buf->sysname, buf->nodename,
				buf->release, buf->version, buf->machine);
	else
		trace_printf(1, "uname(); NULL");

	return ret;
}

RETRACE_REPLACE(uname)
