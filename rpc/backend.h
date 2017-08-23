#ifndef __RTR_BACKEND_H__
#define __RTR_BACKEND_H__

#include <sys/types.h>
#include <sys/socket.h>

int rpc_sockfd();
int rpc_handle_message(int _fd, enum rpc_msg_type msg_type, void *buf);

#endif

