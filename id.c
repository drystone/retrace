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
#include "id.h"

#include <unistd.h>

int RETRACE_IMPLEMENTATION(setuid)(uid_t uid)
{
  	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	void *parameter_values[] = {&uid};
	rtr_setuid_t real_setuid;
	int r;

	real_setuid = RETRACE_GET_REAL(setuid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "setuid";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_setuid(uid);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(setuid)

int RETRACE_IMPLEMENTATION(seteuid)(uid_t uid)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	void *parameter_values[] = {&uid};
	rtr_seteuid_t real_seteuid;
	int r;

	real_seteuid = RETRACE_GET_REAL(seteuid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "seteuid";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_seteuid(uid);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(seteuid)

int RETRACE_IMPLEMENTATION(setgid)(gid_t gid)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	void *parameter_values[] = {&gid};
	rtr_setgid_t real_setgid;
	int r;

	real_setgid = RETRACE_GET_REAL(setgid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "setgid";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;

	retrace_event (&event_info);

	r = real_setgid(gid);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(setgid)

gid_t RETRACE_IMPLEMENTATION(getgid)()
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_END};
	int gid;
	rtr_getgid_t real_getgid;

	real_getgid = RETRACE_GET_REAL(getgid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "getgid";
	event_info.parameter_types = parameter_types;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &gid;

	retrace_event (&event_info);

	gid = real_getgid();

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return gid;
}

RETRACE_REPLACE(getgid)

gid_t RETRACE_IMPLEMENTATION(getegid)()
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_END};
	int egid;
	rtr_getegid_t real_getegid;

	real_getegid = RETRACE_GET_REAL(getegid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "getegid";
	event_info.parameter_types = parameter_types;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &egid;

	retrace_event (&event_info);

	egid = real_getegid();

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return egid;
}

RETRACE_REPLACE(getegid)

uid_t RETRACE_IMPLEMENTATION(getuid)()
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_END};
	int redirect_id;
	int uid;
	rtr_getuid_t real_getuid;

	if (rtr_get_config_single("getuid", ARGUMENT_TYPE_INT, ARGUMENT_TYPE_END, &redirect_id)) {
		trace_printf(1, "getuid(); [redirection in effect: '%i']\n", redirect_id);

		return redirect_id;
	}

	real_getuid = RETRACE_GET_REAL(getuid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "getuid";
	event_info.parameter_types = parameter_types;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &uid;

	retrace_event (&event_info);

	uid = real_getuid();

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);


	return uid;
}

RETRACE_REPLACE(getuid)

uid_t RETRACE_IMPLEMENTATION(geteuid)()
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_END};
	int euid;
	int redirect_id;
	rtr_geteuid_t real_geteuid;

	if (rtr_get_config_single("geteuid", ARGUMENT_TYPE_INT, ARGUMENT_TYPE_END, &redirect_id)) {
		trace_printf(1, "geteuid(); [redirection in effect: '%i']\n", redirect_id);

		return redirect_id;
	}

	real_geteuid = RETRACE_GET_REAL(geteuid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "geteuid";
	event_info.parameter_types = parameter_types;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &euid;
	retrace_event (&event_info);

	euid = real_geteuid();

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return euid;
}

RETRACE_REPLACE(geteuid)

pid_t RETRACE_IMPLEMENTATION(getpid)(void)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_END};
	int pid;
	rtr_getpid_t real_getpid;

	real_getpid = RETRACE_GET_REAL(getpid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "getpid";
	event_info.parameter_types = parameter_types;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &pid;

	retrace_event (&event_info);

	pid = real_getpid();

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return pid;
}

RETRACE_REPLACE(getpid)

pid_t RETRACE_IMPLEMENTATION(getppid)(void)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_END};
	int ppid;
	rtr_getppid_t real_getppid;

	real_getppid = RETRACE_GET_REAL(getppid);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "getppid";
	event_info.parameter_types = parameter_types;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &ppid;

	retrace_event (&event_info);

	ppid = real_getppid();

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return ppid;
}

RETRACE_REPLACE(getppid)
