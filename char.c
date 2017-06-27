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
#include "char.h"
#include "str.h"

#ifdef __FreeBSD__
#define _DONT_USE_CTYPE_INLINE_
#include <runetype.h>
#undef putc
#endif
#include <ctype.h>

#include <string.h>

int
RETRACE_IMPLEMENTATION(putc)(int c, FILE *stream)
{
	static const unsigned int parameter_types[] =
	    {PARAMETER_TYPE_CHAR, PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[2];

	parameters[0].charval = c;
	parameters[1].stream = stream;

	event_info.function_name = "putc";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real_putc(c, stream);

	retrace_event(EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(putc, int, (int c, FILE *stream), (c, stream))


#ifndef __APPLE__
int
RETRACE_IMPLEMENTATION(_IO_putc)(int c, FILE *stream)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_CHAR, PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[2];

	parameters[0].charval = c;
	parameters[1].stream = stream;

	event_info.function_name = "_IO_putc";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real__IO_putc(c, stream);

	retrace_event(EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(_IO_putc, int, (int c, FILE *stream), (c, stream))

#endif

int
RETRACE_IMPLEMENTATION(toupper)(int c)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_CHAR, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];
	int r;

	parameters[0].intval = c;
	event_info.function_name = "toupper";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real_toupper(c);

	retrace_event(EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(toupper, int, (int c), (c))


int
RETRACE_IMPLEMENTATION(tolower)(int c)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_CHAR, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];
	int r;

	parameters[0].charval = c;
	event_info.function_name = "tolower";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event (EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real_tolower(c);

	retrace_event (EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(tolower, int, (int c), (c))
