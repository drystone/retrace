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

enum rpc_inout {
	RPC_INPARAM,
	RPC_OUTPARAM,
	RPC_INOUTPARAM
};

struct rpc_control_header {
	pid_t pid;
	pthread_t tid;
};

struct rpc_call_header {
	enum rpc_call_type call_type;
	enum rpc_function_id function_id;
};

struct rpc_redirect_header {
	int redirect;
	int complete;
};

int rpc_sockfd();

#endif
