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
#include <string.h>

#include "str.h"

char *RETRACE_IMPLEMENTATION(strstr)(const char *s1, const char *s2)
{
	rtr_strstr_t real_strstr;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s1, &s2};
	char *result = NULL;

	real_strstr = RETRACE_GET_REAL(strstr);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strstr";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_STRING;
	event_info.return_value = &result;
	retrace_event (&event_info);

	result = real_strstr(s1, s2);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strstr)

size_t RETRACE_IMPLEMENTATION(strlen)(const char *s)
{
	size_t len;
	rtr_strlen_t real_strlen;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s};


	real_strlen = RETRACE_GET_REAL(strlen);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strlen";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &len;
	retrace_event (&event_info);

	len = real_strlen(s);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return len;
}

RETRACE_REPLACE(strlen)

int RETRACE_IMPLEMENTATION(strncmp)(const char *s1, const char *s2, size_t n)
{
	rtr_strncmp_t real_strncmp;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s1, &s2, &n};
	int result;

	real_strncmp = RETRACE_GET_REAL(strncmp);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strncmp";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);

	result = real_strncmp(s1, s2, n);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strncmp)

int RETRACE_IMPLEMENTATION(strcmp)(const char *s1, const char *s2)
{
	rtr_strcmp_t real_strcmp;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s1, &s2};
	int result;

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strcmp";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &result;
	retrace_event (&event_info);

	real_strcmp = RETRACE_GET_REAL(strcmp);

	result = real_strcmp(s1, s2);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strcmp)

char *RETRACE_IMPLEMENTATION(strncpy)(char *s1, const char *s2, size_t n)
{
	rtr_strncpy_t real_strncpy;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s1, &s2, &n};
	char *result = NULL;

	real_strncpy	= RETRACE_GET_REAL(strncpy);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strncpy";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_STRING;
	event_info.return_value = &result;
	retrace_event (&event_info);

	result = real_strncpy(s1, s2, n);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strncpy)

char *RETRACE_IMPLEMENTATION(strcat)(char *s1, const char *s2)
{
	rtr_strcat_t real_strcat;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s1, &s2};
	char *result = NULL;

	real_strcat	= RETRACE_GET_REAL(strcat);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strcat";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_STRING;
	event_info.return_value = &result;
	retrace_event (&event_info);

	result = real_strcat(s1, s2);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strcat)

char *RETRACE_IMPLEMENTATION(strncat)(char *s1, const char *s2, size_t n)
{
	rtr_strncat_t real_strncat;
	char *result = NULL;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s1, &s2, &n};

	real_strncat	= RETRACE_GET_REAL(strncat);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strncat";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_STRING;
	event_info.return_value = &result;
	retrace_event (&event_info);

	result = real_strncat(s1, s2, n);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strncat)

char *RETRACE_IMPLEMENTATION(strcpy)(char *s1, const char *s2)
{
	rtr_strcpy_t real_strcpy;
	char *result = NULL;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s1, &s2};

	real_strcpy = RETRACE_GET_REAL(strcpy);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strcpy";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_STRING;
	event_info.return_value = &result;
	retrace_event (&event_info);

	result = real_strcpy(s1, s2);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strcpy)

char *RETRACE_IMPLEMENTATION(strchr)(const char *s, int c)
{
	rtr_strchr_t real_strchr;
	char *result = NULL;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_CHAR, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s, &c};

	real_strchr = RETRACE_GET_REAL(strchr);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strchr";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_STRING;
	event_info.return_value = &result;
	retrace_event (&event_info);

	result = real_strchr(s, c);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (result);
}

RETRACE_REPLACE(strchr)
