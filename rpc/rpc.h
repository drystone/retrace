#ifndef __RTR_RPC_H__
#define __RTR_RPC_H__

#include <sys/types.h>
#include <sys/socket.h>
#include "shim.h"

extern const char *rpc_version;

enum rpc_call_type {
	RPC_PRECALL,
	RPC_POSTCALL
};

struct rpc_control_header {
	pid_t pid;
	pthread_t tid;
};

struct call_header {
	enum rpc_call_type call_type;
	enum rpc_function_id function_id;
};

struct redirect_header {
	int preredirect;
	int postredirect;
};

int rpc_sockfd();

int do_send(struct msghdr *msg);
int do_recv(struct msghdr *msg);

#endif
