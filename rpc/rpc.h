#ifndef __RTR_RPC_H__
#define __RTR_RPC_H__

#include <sys/types.h>
#include <sys/socket.h>
#include "shim.h"

#define RPC_MSG_LEN_MAX 256

extern const char *rpc_version;

enum rpc_msg_type {
	RPC_MSG_ERROR,
	RPC_MSG_CALL_INIT,
	RPC_MSG_DONE,
	RPC_MSG_GET_RESULT,
	RPC_MSG_SET_RESULT,
	RPC_MSG_DO_CALL,
	RPC_MSG_SET_PARAMETERS,
	RPC_MSG_GET_PARAMETERS,
};

struct rpc_control_header {
	pid_t pid;
	pthread_t tid;
};

int rpc_sockfd();

void rpc_send(int fd, enum rpc_msg_type msg_type, const void *buf, size_t len);
enum rpc_msg_type rpc_recv(int fd, void *buf);
void rpc_handle_message(int _fd, enum rpc_msg_type msg_type, void *buf);
int do_send(struct msghdr *msg);
int do_recv(struct msghdr *msg);

#endif
