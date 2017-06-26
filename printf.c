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

#include "printf.h"
#include "file.h"

int
RETRACE_IMPLEMENTATION(printf)(const char *fmt, ...)
{
	int result;
	rtr_vprintf_t vprintf_;
	va_list ap;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&fmt, &ap};

	vprintf_ = RETRACE_GET_REAL(vprintf);

	va_start(ap, fmt);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "printf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, fmt);
	result = vprintf_(fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap);

	return result;
}

RETRACE_REPLACE(printf)

int
RETRACE_IMPLEMENTATION(fprintf)(FILE *stream, const char *fmt, ...)
{
	int result;
	rtr_vfprintf_t vfprintf_;
	va_list ap;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream, &fmt, &ap};

	vfprintf_	= RETRACE_GET_REAL(vfprintf);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, fmt);
	result = vfprintf_(stream, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap);

	return result;
}

RETRACE_REPLACE(fprintf)

int
RETRACE_IMPLEMENTATION(dprintf)(int fd, const char *fmt, ...)
{
	int result;
	rtr_vdprintf_t vdprintf_;
	va_list ap;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&fd, &fmt, &ap};

	vdprintf_ = RETRACE_GET_REAL(vdprintf);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "dprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, fmt);
	result = vdprintf_(fd, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap);

	return result;
}

RETRACE_REPLACE(dprintf)

int
RETRACE_IMPLEMENTATION(sprintf)(char *str, const char *fmt, ...)
{
	int result;
	rtr_vsprintf_t vsprintf_;
	va_list ap;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&str, &fmt, &ap};

	vsprintf_ = RETRACE_GET_REAL(vsprintf);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "sprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, fmt);
	result = vsprintf_(str, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap);

	return result;
}

RETRACE_REPLACE(sprintf)

int
RETRACE_IMPLEMENTATION(snprintf)(char *str, size_t size, const char *fmt, ...)
{
	int result;
	rtr_vsnprintf_t vsnprintf_;
	va_list ap;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&str, &size, &fmt, &ap};

	vsnprintf_ = RETRACE_GET_REAL(vsnprintf);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "snprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap);

	va_start(ap, fmt);
	result = vsnprintf_(str, size, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap);

	return result;
}

RETRACE_REPLACE(snprintf)

int
RETRACE_IMPLEMENTATION(vprintf)(const char *fmt, va_list ap)
{
	int result;
	rtr_vprintf_t vprintf_;
	va_list ap1;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&fmt, &ap1};

	vprintf_ = RETRACE_GET_REAL(vprintf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = vprintf_(fmt, ap);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vprintf)

int
RETRACE_IMPLEMENTATION(vfprintf)(FILE *stream, const char *fmt, va_list ap)
{
	int result;
	rtr_vfprintf_t vfprintf_;
	va_list ap1;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream, &fmt, &ap1};

	vfprintf_	= RETRACE_GET_REAL(vfprintf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vfprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = vfprintf_(stream, fmt, ap);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vfprintf)

int
RETRACE_IMPLEMENTATION(vdprintf)(int fd, const char *fmt, va_list ap)
{
	int result;
	rtr_vdprintf_t vdprintf_;
	va_list ap1;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&fd, &fmt, &ap1};

	vdprintf_ = RETRACE_GET_REAL(vdprintf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vdprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = vdprintf_(fd, fmt, ap);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vdprintf)

int
RETRACE_IMPLEMENTATION(vsprintf)(char *str, const char *fmt, va_list ap)
{
	int result;
	rtr_vsprintf_t vsprintf_;
	va_list ap1;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&str, &fmt, &ap1};

	vsprintf_ = RETRACE_GET_REAL(vsprintf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vsprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = vsprintf_(str, fmt, ap);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vsprintf)

int
RETRACE_IMPLEMENTATION(vsnprintf)(char *str, size_t size, const char *fmt, va_list ap)
{
	int result;
	rtr_vsnprintf_t vsnprintf_;
	va_list ap1;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT, PARAMETER_TYPE_PRINTF_FORMAT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&str, &size, &fmt, &ap1};

	vsnprintf_ = RETRACE_GET_REAL(vsnprintf);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "vsnprintf";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);
	va_end(ap1);

	__va_copy(ap1, ap);
	result = vsnprintf_(str, size, fmt, ap1);
	va_end(ap1);

	__va_copy(ap1, ap);
	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);
	va_end(ap1);

	return result;
}

RETRACE_REPLACE(vsnprintf)
