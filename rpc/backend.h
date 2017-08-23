#ifndef __RTR_RPC_H__
#define __RTR_RPC_H__

#include <sys/types.h>
#include <sys/socket.h>

struct rpc_control_header {
	pid_t pid;
	pthread_t tid;
};

int rpc_sockfd();

#endif

