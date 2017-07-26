#ifndef __RETRACE_FRONTEND_H__
#define __RETRACE_FRONTEND_H__

#include <sys/types.h>

struct retrace_handle {
	int fd;
	pid_t pid;
};

struct retrace_handle *retrace_start(char *const argv[]);
void retrace_close(struct retrace_handle *handle);

#endif
