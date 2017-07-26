#ifndef __RETRACE_FRONTEND_H__
#define __RETRACE_FRONTEND_H__

#include <sys/types.h>

#include "rpc.h"
#include "shim.h"

struct retrace_handle {
	int fd;
	pid_t pid;
};

typedef void (*retrace_handler_t)(void *);

struct retrace_handle *retrace_start(char *const argv[]);
void retrace_close(struct retrace_handle *handle);
void retrace_trace(struct retrace_handle *handle);
void retrace_handle(enum rpc_function_id, void *call);
void retrace_set_handler(enum rpc_function_id, retrace_handler_t handler);
#endif
