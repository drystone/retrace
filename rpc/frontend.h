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
	pthread_t tid;
};

SLIST_HEAD(retrace_handle, retrace_rpc_endpoint);

typedef void (*retrace_handler_t) (const struct retrace_rpc_endpoint *ep,
	const struct rpc_call_header *call_header,
	void * call, struct rpc_redirect_header *redirect_header);

struct retrace_handle *retrace_start(char *const argv[]);
void retrace_close(struct retrace_handle *handle);
void retrace_trace(struct retrace_handle *handle);
void retrace_handle(const struct retrace_rpc_endpoint *ep,
	const struct rpc_call_header *call_header,
	void *call, struct rpc_redirect_header *redirect_header);
void retrace_set_handler(enum rpc_function_id, retrace_handler_t handler);
#endif
