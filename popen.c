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
#include "file.h"
#include "popen.h"

FILE *RETRACE_IMPLEMENTATION(popen)(const char *command, const char *type)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING,  PARAMETER_TYPE_END};
	void const *parameter_values[] = {&command, &type};
	FILE *ret;
	rtr_popen_t real_popen;

	real_popen	= RETRACE_GET_REAL(popen);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "popen";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_FILE_STREAM;
	event_info.return_value = &ret;
	retrace_event (&event_info);

	ret = real_popen(command, type);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return ret;
}

RETRACE_REPLACE(popen)

int RETRACE_IMPLEMENTATION(pclose)(FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream};
	int ret;
	rtr_pclose_t real_pclose;

	real_pclose = RETRACE_GET_REAL(pclose);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "pclose";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &ret;
	retrace_event (&event_info);

	ret = real_pclose(stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return ret;
}

RETRACE_REPLACE(pclose)
