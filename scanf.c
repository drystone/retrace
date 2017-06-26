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
#include <stdarg.h>
#include <stdbool.h>

#include "scanf.h"
#include "printf.h"
#include "file.h"

#define __func__ __extension__ __FUNCTION__

int
RETRACE_IMPLEMENTATION(scanf)(const char *format, ...)
{
	rtr_vscanf_t real_vscanf;
	va_list ap;
	int result;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&format, &ap};

	real_vscanf = RETRACE_GET_REAL(vscanf);

	va_start(ap, format);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "scanf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, format);
	result = real_vscanf(format, ap);
	va_end(ap);

	va_start(ap, format);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	va_end(ap);

	return result;
}

RETRACE_REPLACE(scanf)

int
RETRACE_IMPLEMENTATION(fscanf)(FILE *stream, const char *format, ...)
{
	rtr_vfscanf_t real_vfscanf;
	va_list ap;
	int result;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream, &format, &ap};

	real_vfscanf = RETRACE_GET_REAL(vfscanf);

	va_start(ap, format);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fscanf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, format);
	result = real_vfscanf(stream, format, ap);
	va_end(ap);

	va_start(ap, format);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap);


	return result;
}

RETRACE_REPLACE(fscanf)

int
RETRACE_IMPLEMENTATION(sscanf)(const char *str, const char *format, ...)
{
	rtr_vsscanf_t real_vsscanf;
	va_list ap;
	int result;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&str, &format, &ap};

	real_vsscanf = RETRACE_GET_REAL(vsscanf);

	va_start(ap, format);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "sscanf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, format);
	result = real_vsscanf(str, format, ap);
	va_end(ap);

	va_start(ap, format);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap);

	return result;
}

RETRACE_REPLACE(sscanf)

int
RETRACE_IMPLEMENTATION(vscanf)(const char *format, va_list ap)
{
	rtr_vscanf_t real_vscanf;
	va_list ap1;
	int result;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&format, &ap1};

	real_vscanf = RETRACE_GET_REAL(vscanf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vscanf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = real_vscanf(format, ap);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vscanf)

int
RETRACE_IMPLEMENTATION(vsscanf)(const char *str, const char *format, va_list ap)
{
	rtr_vsscanf_t real_vsscanf;
	va_list ap1;
	int result;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&str, &format, &ap1};

	real_vsscanf = RETRACE_GET_REAL(vsscanf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vsscanf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = real_vsscanf(str, format, ap);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vsscanf)

int
RETRACE_IMPLEMENTATION(vfscanf)(FILE *stream, const char *format, va_list ap)
{
	rtr_vfscanf_t real_vfscanf;
	va_list ap1;
	int result;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream, &format, &ap1};

	real_vfscanf = RETRACE_GET_REAL(vfscanf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vfscanf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = real_vfscanf(stream, format, ap);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vfscanf)
