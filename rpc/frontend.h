#ifndef __RETRACE_FRONTEND_H__
#define __RETRACE_FRONTEND_H__

#include <sys/types.h>
#include <sys/queue.h>

#include "rpc.h"
#include "shim.h"

struct retrace_rpc_endpoint {
	SLIST_ENTRY(retrace_rpc_endpoint) next;
	int fd;
	pid_t pid;
	int thread_num;
	unsigned int call_num;
	unsigned int call_depth;
};

SLIST_HEAD(retrace_endpoints, retrace_rpc_endpoint);

struct retrace_process_info {
	SLIST_ENTRY(retrace_process_info) next;
	pid_t pid;
	int next_thread_num;
};

SLIST_HEAD(process_list, retrace_process_info);

struct retrace_handle {
	struct retrace_endpoints endpoints;
	struct process_list processes;
	int control_fd;
};

typedef void (*retrace_call_handler_t)(struct retrace_rpc_endpoint *ep, void *buf);

extern retrace_call_handler_t g_call_handlers[];

struct retrace_handle *retrace_start(char *const argv[]);
void retrace_close(struct retrace_handle *handle);
void retrace_trace(struct retrace_handle *handle);
void retrace_handle_call(const struct retrace_rpc_endpoint *ep);
void retrace_set_call_handler(enum rpc_function_id,
	retrace_call_handler_t handler);
void do_call(struct retrace_rpc_endpoint *ep, void *buf, size_t len);

void *trace_buffer(void *buffer, size_t length);
#endif
