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
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>

#include "file.h"
#include "str.h"

#ifdef __FreeBSD__
#undef fileno
#endif

int RETRACE_IMPLEMENTATION(stat)(const char *path, struct stat *buf)
{
	rtr_stat_t real_stat;
	char perm[10];
	int r;

	real_stat = RETRACE_GET_REAL(stat);

	trace_printf(1, "stat(\"%s\", buf);\n", path);

	r = real_stat(path, buf);

	if (r == 0) {
		trace_printf(1, "struct stat {\n");
		trace_printf(1, "\tst_dev = %lu\n", buf->st_dev);
		trace_printf(1, "\tst_ino = %i\n", buf->st_ino);
		trace_mode(buf->st_mode, perm);
		trace_printf(1, "\tst_mode = %d [%s]\n", buf->st_mode, perm);
		trace_printf(1, "\tst_nlink = %lu\n", buf->st_nlink);
		trace_printf(1, "\tst_uid = %d\n", buf->st_uid);
		trace_printf(1, "\tst_gid = %d\n", buf->st_gid);
		trace_printf(1, "\tst_rdev = %r\n", buf->st_rdev);
		trace_printf(1, "\tst_atime = %lu\n", buf->st_atime);
		trace_printf(1, "\tst_mtime = %lu\n", buf->st_mtime);
		trace_printf(1, "\tst_ctime = %lu\n", buf->st_ctime);
		trace_printf(1, "\tst_size = %zu\n", buf->st_size);
		trace_printf(1, "\tst_blocks = %lu\n", buf->st_blocks);
		trace_printf(1, "\tst_blksize = %lu\n", buf->st_blksize);
#if __APPLE__
		trace_printf(1, "\tst_flags = %d\n", buf->st_flags);
		trace_printf(1, "\tst_gen = %d\n", buf->st_gen);
#endif
		trace_printf(1, "}\n");
	}

	return r;
}

RETRACE_REPLACE(stat)

int RETRACE_IMPLEMENTATION(chmod)(const char *path, mode_t mode)
{
	char perm[10];
	char *perm_p = &perm[0];
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT_OCTAL | PARAMETER_FLAG_STRING_NEXT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&path, &mode, &perm_p};
	rtr_chmod_t real_chmod;
	int r;

	real_chmod = RETRACE_GET_REAL(chmod);
	trace_mode(mode, perm);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "chmod";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_chmod(path, mode);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(chmod)

int RETRACE_IMPLEMENTATION(fchmod)(int fd, mode_t mode)
{
	char perm[10];
	char *perm_p = &perm[0];
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_INT_OCTAL | PARAMETER_FLAG_STRING_NEXT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&fd, &mode, &perm_p};
	rtr_fchmod_t real_fchmod;
	int r;

	real_fchmod = RETRACE_GET_REAL(fchmod);

	trace_mode(mode, perm);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fchmod";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fchmod(fd, mode);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(fchmod)

int RETRACE_IMPLEMENTATION(fileno)(FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream};
	int fd;
	rtr_fileno_t real_fileno;

	real_fileno = RETRACE_GET_REAL(fileno);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fileno";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &fd;
	retrace_event (&event_info);

	fd = real_fileno(stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return real_fileno(stream);
}

RETRACE_REPLACE(fileno)

int RETRACE_IMPLEMENTATION(fseek)(FILE *stream, long offset, int whence)
{
	char *operation = NULL;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_INT, PARAMETER_TYPE_INT | PARAMETER_FLAG_STRING_NEXT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream, &offset, &whence, &operation};
	rtr_fseek_t real_fseek;
	int r;

	real_fseek	= RETRACE_GET_REAL(fseek);

	if (whence == 0)
		operation = "SEEK_SET";
	else if (whence == 1)
		operation = "SEEK_CUR";
	else if (whence == 2)
		operation = "SEEK_END";
	else
		operation = "UNDEFINED";


	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fseek";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fseek(stream, offset, whence);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return r;
}

RETRACE_REPLACE(fseek)

int RETRACE_IMPLEMENTATION(fclose)(FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream};
	int fd;
	rtr_fclose_t real_fclose;
	rtr_fileno_t real_fileno;
	int r;

	real_fclose = RETRACE_GET_REAL(fclose);
	real_fileno = RETRACE_GET_REAL(fileno);

	fd = real_fileno(stream);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fclose";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fclose(stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	if (fd > 0)
		file_descriptor_remove(fd);

	return r;
}

RETRACE_REPLACE(fclose)

FILE *RETRACE_IMPLEMENTATION(fopen)(const char *file, const char *mode)
{
	int did_redirect = 0;
	int fd = 0;
	char *match_file = NULL;
	FILE *ret;
	char *redirect_file = NULL;
	rtr_fopen_t real_fopen;
	rtr_strcmp_t real_strcmp;
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&file, &mode};

	real_fopen	= RETRACE_GET_REAL(fopen);
	real_strcmp	= RETRACE_GET_REAL(strcmp);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fopen";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_FILE_STREAM;
	event_info.return_value = &ret;
	retrace_event (&event_info);

	if (get_tracing_enabled() && file) {
		RTR_CONFIG_HANDLE config = NULL;

		while (1) {
			int r;

			r = rtr_get_config_multiple(&config,
					"fopen",
					ARGUMENT_TYPE_STRING,
					ARGUMENT_TYPE_STRING,
					ARGUMENT_TYPE_END,
					&match_file,
					&redirect_file);
			if (r == 0)
				break;
			if (real_strcmp(match_file, file) == 0) {
				did_redirect = 1;

				ret = real_fopen(redirect_file, mode);

				break;
			}
		}
	}

	if (!did_redirect)
		ret = real_fopen(file, mode);

	if (ret) {
		int fd;

		fd = RETRACE_GET_REAL(fileno)(ret);

		file_descriptor_update(
                        fd, FILE_DESCRIPTOR_TYPE_FILE, file, 0);
	}

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	trace_printf(1, "fopen(\"%s%s\", \"%s\"); [%d]\n",
	    did_redirect ? redirect_file : file,
	    did_redirect ? " [redirected]" : "",
	    mode, fd);

	return ret;
}

RETRACE_REPLACE(fopen)

int RETRACE_IMPLEMENTATION(close)(int fd)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&fd};
	rtr_close_t real_close;
	int r;

	real_close = RETRACE_GET_REAL(close);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "close";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_close(fd);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	file_descriptor_remove(fd);

	return (r);
}

RETRACE_REPLACE(close)

int RETRACE_IMPLEMENTATION(dup)(int oldfd)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&oldfd};
	rtr_dup_t real_dup;
	int r;

	real_dup = RETRACE_GET_REAL(dup);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "dup";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_FILE_DESCRIPTOR;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_dup(oldfd);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(dup)

int RETRACE_IMPLEMENTATION(dup2)(int oldfd, int newfd)
{

	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_FILE_DESCRIPTOR, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&oldfd, &newfd};
	rtr_dup2_t real_dup2;
	int r;

	real_dup2 = RETRACE_GET_REAL(dup2);


	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "dup2";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_dup2(oldfd, newfd);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return (r);
}

RETRACE_REPLACE(dup2)

mode_t RETRACE_IMPLEMENTATION(umask)(mode_t mask)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_INT,  PARAMETER_TYPE_END};
	void const *parameter_values[] = {&mask};
	mode_t old_mask;
	rtr_umask_t real_umask;

	real_umask = RETRACE_GET_REAL(umask);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "umask";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &old_mask;
	retrace_event (&event_info);

	old_mask = real_umask(mask);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return old_mask;
}

RETRACE_REPLACE(umask)

int RETRACE_IMPLEMENTATION(mkfifo)(const char *pathname, mode_t mode)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&pathname, &mode};
	int ret;
	rtr_mkfifo_t real_mkfifo;

	real_mkfifo = RETRACE_GET_REAL(mkfifo);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "mkfifo";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &ret;
	retrace_event (&event_info);

	ret = real_mkfifo(pathname, mode);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return ret;
}

RETRACE_REPLACE(mkfifo)

int RETRACE_IMPLEMENTATION(open)(const char *pathname, int flags, ...)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_INT, PARAMETER_TYPE_INT, PARAMETER_TYPE_END};
	int fd;
	mode_t mode;
	rtr_open_t real_open;
	va_list arglist;
	void const *parameter_values[] = {&pathname, &flags, &mode};

	real_open = RETRACE_GET_REAL(open);

	va_start(arglist, flags);
	mode = va_arg(arglist, int);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "open";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_FILE_DESCRIPTOR;
	event_info.return_value = &fd;
	retrace_event (&event_info);

	fd = real_open(pathname, flags, mode);

	if (fd > 0) {
		file_descriptor_update(
			fd, FILE_DESCRIPTOR_TYPE_FILE, pathname, 0);
	}


	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	va_end(arglist);

	return fd;
}

RETRACE_REPLACE(open)

size_t RETRACE_IMPLEMENTATION(fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_MEM_BUFFER_ARRAY, PARAMETER_TYPE_INT, PARAMETER_TYPE_INT, PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&size, &nmemb, &ptr, &size, &nmemb, &stream};
	int r;
	rtr_fwrite_t real_fwrite;

	real_fwrite = RETRACE_GET_REAL(fwrite);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fwrite";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fwrite(ptr, size, nmemb, stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return r;
}

RETRACE_REPLACE(fwrite)

size_t RETRACE_IMPLEMENTATION(fread)(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_MEM_BUFFER_ARRAY, PARAMETER_TYPE_INT, PARAMETER_TYPE_INT, PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&size, &nmemb, &ptr, &size, &nmemb, &stream};
	int r;
	rtr_fread_t real_fread;

	real_fread	= RETRACE_GET_REAL(fread);


	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fread";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fread(ptr, size, nmemb, stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return r;
}

RETRACE_REPLACE(fread)

int RETRACE_IMPLEMENTATION(fputc)(int c, FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_CHAR, PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&c, &stream};
	int r;
	rtr_fputc_t real_fputc;

	real_fputc	= RETRACE_GET_REAL(fputc);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fputc";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fputc(c, stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return r;
}

RETRACE_REPLACE(fputc)

int RETRACE_IMPLEMENTATION(fputs)(const char *s, FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_STRING, PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&s, &stream};
	int r;
	rtr_fputs_t real_fputs;

	real_fputs	= RETRACE_GET_REAL(fputs);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fputs";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fputs(s, stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return r;
}

RETRACE_REPLACE(fputs)

int RETRACE_IMPLEMENTATION(fgetc)(FILE *stream)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_FILE_STREAM, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&stream};
	int r;
	rtr_fgetc_t real_fgetc;

	real_fgetc      = RETRACE_GET_REAL(fgetc);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "fgetc";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_INT;
	event_info.return_value = &r;
	retrace_event (&event_info);

	r = real_fgetc(stream);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	return r;
}

RETRACE_REPLACE(fgetc)

void RETRACE_IMPLEMENTATION(strmode)(int mode, char *bp)
{
	struct rtr_event_info event_info;
	unsigned int parameter_types[] = {PARAMETER_TYPE_INT, PARAMETER_TYPE_STRING, PARAMETER_TYPE_END};
	void const *parameter_values[] = {&mode, &bp};
	rtr_strmode_t real_strmode;

	real_strmode = RETRACE_GET_REAL(strmode);

	event_info.event_type = EVENT_TYPE_BEFORE_CALL;
	event_info.function_name = "strmode";
	event_info.parameter_types = parameter_types;
	event_info.parameter_values = (void **) parameter_values;
	event_info.return_value_type = PARAMETER_TYPE_END;
	retrace_event (&event_info);

	real_strmode(mode, bp);

	event_info.event_type = EVENT_TYPE_AFTER_CALL;
	retrace_event (&event_info);

	trace_printf(1, "strmode(%d, \"%s\");\n", mode, bp);
}

RETRACE_REPLACE(strmode)
