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

#include "config.h"

#include <sys/socket.h>
#include <error.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "rpc.h"
#include "frontend.h"
#include "inspect.h"

#define IOBUFLEN 64 * 1024

struct retrace_handle *
retrace_start(char *const argv[])
{
	int sv[2];
	char fd_str[16], md5_buf[32];
	pid_t pid;
	struct retrace_handle *handle = 0;

	if (socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sv))
		error(1, 0, "Unable to create socketpair.");

	pid = fork();

	if (pid == 0) {
		close(sv[0]);

		/*
		 * TODO: get proper path for shared library
		 */
		putenv("LD_PRELOAD=.libs/libretracerpc.so");
		sprintf(fd_str, "%d", sv[1]);
		setenv("RTR_SOCKFD", fd_str, 1);

		execv(argv[0], argv);
		error(1, 0, "Failed to exec (%s.)", strerror(errno));
	} else {
		handle = malloc(sizeof(struct retrace_handle));
		if (handle == NULL)
			error(1, 0, "Out of memory.");
		close(sv[1]);

		/*
		 * first message from tracee is MD5 version
		 */
		if (recv(sv[0], md5_buf, 32, 0) <= 0)
			error(1, 0, "Failed to trace program");

		if (memcmp(md5_buf, rpc_version, 32) != 0)
			error(1, 0, "Version mismatch");

		handle->fd = sv[0];
		handle->pid = pid;
	}
	return handle;
}

void
retrace_close(struct retrace_handle *handle)
{
	if (handle)
	{
		close(handle->fd);
		waitpid(handle->pid, NULL, 0);
	}
}

void
retrace_trace(struct retrace_handle *handle)
{
	char *buf;
	struct rpc_call_header call_header;
	struct rpc_redirect_header redirect_header;
	struct iovec call_iov[2], redirect_iov[2];
	struct msghdr call_msghdr, redirect_msghdr;
	ssize_t iolen;
	struct retrace_call_info *info;
	struct retrace_arg_info *parg;

	buf = malloc(IOBUFLEN);
	if (buf == NULL)
		error(1, 0, "Out of memory.");

	call_iov[0].iov_base = &call_header;
	call_iov[0].iov_len = sizeof(struct rpc_call_header);
	call_iov[1].iov_base = buf;
	call_iov[1].iov_len = IOBUFLEN;

	memset(&call_msghdr, 0, sizeof(struct msghdr));
	call_msghdr.msg_iov = call_iov;
	call_msghdr.msg_iovlen = 2;

	redirect_iov[0].iov_base = &redirect_header;
	redirect_iov[0].iov_len = sizeof(struct rpc_redirect_header);

	memset(&redirect_msghdr, 0, sizeof(struct msghdr));
	redirect_msghdr.msg_iov = redirect_iov;
	redirect_msghdr.msg_iovlen = 1;

	for (;;) {
		iolen = recvmsg(handle->fd, &call_msghdr, 0);

		if (iolen == -1)
			error(1, 0, "Error receiving call info (%s.)", strerror(errno));

		if (iolen == 0)
			break;

		if (call_header.call_type == RPC_POSTCALL) {
			info = retrace_get_call_info(call_header.function_id, buf);
			printf("%s(", info->name);
			for (parg = info->args; parg->name; ++parg) {
				switch (parg->rpctype) {
				case RPC_INT:
					printf("%d", (int)parg->value);
					break;
				case RPC_UINT:
					printf("%u", parg->value);
					break;
				case RPC_PTR:
					printf("%p", parg->value);
					break;
				case RPC_STR:
					printf("\"%s\"", parg->value);
					break;
				}
				if (parg[1].name)
					printf(", ");
			}
			printf(") = ");
			switch (info->rpctype) {
			case RPC_VOID:
				printf("void\n");
				break;
			case RPC_INT:
				printf("%d\n", info->result);
				break;
			case RPC_UINT:
				printf("%u\n", info->result);
				break;
			case RPC_PTR:
				printf("%p\n", info->result);
				break;
			case RPC_STR:
				printf("\"%s\"\n", info->result);
				break;
			}
			free(info);
		}
		redirect_header.complete = 0;
		redirect_header.redirect = 0;
		iolen = sendmsg(handle->fd, &redirect_msghdr, 0);

		if (iolen == -1)
			error(1, 0, "Error sending redirect info (%s.)", strerror(errno));
	}
}
