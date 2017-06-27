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
#include "dir.h"

DIR *RETRACE_IMPLEMENTATION(opendir)(const char *dirname)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_STRING,  PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];

	parameters[0].pchar = dirname;
	event_info.function_name = "opendir";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_DIR;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.dirp = real_opendir(dirname);

	retrace_event (EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.dirp);
}

RETRACE_REPLACE(opendir, DIR *, (const char *dirname), (dirname))


int RETRACE_IMPLEMENTATION(closedir)(DIR *dirp)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_DIR, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];

	parameters[0].dirp = dirp;
	event_info.function_name = "closedir";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event (EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real_closedir(dirp);

	retrace_event (EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(closedir, int, (DIR *dirp), (dirp))


DIR *RETRACE_IMPLEMENTATION(fdopendir)(int fd)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];

	parameters[0].intval = fd;
	event_info.function_name = "fdopendir";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_DIR;
	retrace_event (EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.dirp = real_fdopendir(fd);

	retrace_event (EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.dirp);
}

RETRACE_REPLACE(fdopendir, DIR *, (int fd), (fd))


int RETRACE_IMPLEMENTATION(readdir_r)(DIR *dirp, struct dirent *entry, struct dirent **result)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_DIR, PARAMETER_TYPE_POINTER, PARAMETER_TYPE_POINTER, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[3];

	parameters[0].dirp = dirp;
	parameters[1].pvoid = entry;
	parameters[2].pvoid = result;

	event_info.function_name = "readdir_r";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real_readdir_r(dirp, entry, result);

	retrace_event(EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(readdir_r, int,
	(DIR *dirp, struct dirent *entry, struct dirent **result),
	(dirp, entry, result))


long RETRACE_IMPLEMENTATION(telldir)(DIR *dirp)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_DIR, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];

	parameters[0].dirp = dirp;
	event_info.function_name = "telldir";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real_telldir(dirp);

	retrace_event(EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(telldir, long, (DIR *dirp), (dirp))


void RETRACE_IMPLEMENTATION(seekdir)(DIR *dirp, long loc)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_DIR, PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[2];

	parameters[0].dirp = dirp;
	parameters[1].intval = loc;

	event_info.function_name = "seekdir";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_END;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	real_seekdir(dirp, loc);

	retrace_event(EVENT_TYPE_AFTER_CALL, &event_info);
}

RETRACE_REPLACE(seekdir, void, (DIR *dirp, long loc), (dirp, loc))


void RETRACE_IMPLEMENTATION(rewinddir)(DIR *dirp)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_DIR, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];

	parameters[0].dirp = dirp;
	event_info.function_name = "rewinddir";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_END;
	retrace_event(EVENT_TYPE_BEFORE_CALL, &event_info);

	real_rewinddir(dirp);

	retrace_event (EVENT_TYPE_AFTER_CALL, &event_info);
}

RETRACE_REPLACE(rewinddir, void, (DIR *dirp), (dirp))


int RETRACE_IMPLEMENTATION(dirfd)(DIR *dirp)
{
	static const unsigned int parameter_types[] = {PARAMETER_TYPE_DIR, PARAMETER_TYPE_END};
	struct rtr_event_info event_info;
	union rtr_parameter_value parameters[1];

	parameters[0].dirp = dirp;

	event_info.function_name = "dirfd";
	event_info.parameter_types = parameter_types;
	event_info.parameters = parameters;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	retrace_event (EVENT_TYPE_BEFORE_CALL, &event_info);

	event_info.return_value.intval = real_dirfd(dirp);

	retrace_event (EVENT_TYPE_AFTER_CALL, &event_info);

	return (event_info.return_value.intval);
}

RETRACE_REPLACE(dirfd, int, (DIR *dirp), (dirp))
