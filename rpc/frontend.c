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
#include <ctype.h>
#include <assert.h>

#include "rpc.h"
#include "frontend.h"

#define IOBUFLEN 64 * 1024

static struct retrace_rpc_endpoint *
recv_endpoint(int fd)
{
	char version[32];
	struct retrace_rpc_endpoint *endpoint;
	struct rpc_control_header header;
	struct iovec iov[2] = {
		{&header, sizeof(header)},
		{version, 32}};
	struct msghdr msg = { 0 };
	struct cmsghdr *cmsg;
	union {
		char buf[CMSG_SPACE(sizeof(int))];
		struct cmsghdr align;
	} u;
	ssize_t iolen;

	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	msg.msg_control = u.buf;
	msg.msg_controllen = sizeof(u.buf);

	iolen = recvmsg(fd, &msg, 0);
	if (iolen == 0)
		return NULL;
	if (iolen == -1)
		error(1, 0, "error reading control socket");

	/*
	 * check version sent with new fd
	 */
	if (memcmp(version, rpc_version, 32) != 0)
		error(1, 0, "Version mismatch");

	cmsg = CMSG_FIRSTHDR(&msg);

	endpoint = malloc(sizeof(struct retrace_rpc_endpoint));
	endpoint->fd = *(int *)CMSG_DATA(cmsg);
	endpoint->pid = header.pid;

	return endpoint;
}

struct retrace_rpc_endpoint *
add_endpoint(struct retrace_handle *handle)
{
	struct retrace_process_info *pi, *procinfo = NULL;
	struct retrace_rpc_endpoint *endpoint;

	endpoint = recv_endpoint(handle->control_fd);
	if (!endpoint)
		return NULL;

	SLIST_FOREACH(pi, &handle->processes, next) {
		if (pi->pid == endpoint->pid) {
			procinfo = pi;
			break;
		}
	}

	if (!procinfo) {
		procinfo = malloc(sizeof(struct retrace_process_info));
		if (!procinfo)
			error(1, 0, "Out of memory.");
		procinfo->pid = endpoint->pid;
		procinfo->next_thread_num = 0;
		SLIST_INSERT_HEAD(&handle->processes, procinfo, next);
	}

	endpoint->thread_num = procinfo->next_thread_num++;
	endpoint->call_num = 0;
	SLIST_INSERT_HEAD(&handle->endpoints, endpoint, next);

	return endpoint;
}

struct retrace_handle *
retrace_start(char *const argv[])
{
	int sv[2];
	char fd_str[16];
	pid_t pid;
	struct retrace_handle *handle;

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

		return NULL;
	} else {
		close(sv[1]);

		handle = malloc(sizeof(struct retrace_handle));
		if (handle == NULL)
			error(1, 0, "Out of memory.");
		SLIST_INIT(&handle->endpoints);
		SLIST_INIT(&handle->processes);

		handle->control_fd = sv[0];

		return handle;
	}
}

void
retrace_close(struct retrace_handle *handle)
{
	struct retrace_rpc_endpoint *endpoint;
	struct retrace_process_info *procinfo;

	close(handle->control_fd);

	while (!SLIST_EMPTY(&handle->endpoints)) {
		endpoint = SLIST_FIRST(&handle->endpoints);
		SLIST_REMOVE_HEAD(&handle->endpoints, next);
		close(endpoint->fd);
		if (endpoint->pid)
			waitpid(endpoint->pid, NULL, 0);
		free(endpoint);
	}

	while (!SLIST_EMPTY(&handle->processes)) {
		procinfo = SLIST_FIRST(&handle->processes);
		SLIST_REMOVE_HEAD(&handle->processes, next);
		free(procinfo);
	}

	free(handle);
}

void
retrace_trace(struct retrace_handle *handle)
{
	enum rpc_msg_type msg_type;
	enum rpc_function_id fnid;
	char buf[RPC_MSG_LEN_MAX];
	fd_set readfds;
	struct retrace_rpc_endpoint *endpoint;
	int numfds;

	for (;;) {
		FD_ZERO(&readfds);

		numfds = handle->control_fd;
		FD_SET(handle->control_fd, &readfds);

		SLIST_FOREACH(endpoint, &handle->endpoints, next) {
			FD_SET(endpoint->fd, &readfds);
			if (endpoint->fd > numfds)
				numfds = endpoint->fd;
		}
		select(numfds + 1, &readfds, NULL, NULL, NULL);

		if (FD_ISSET(handle->control_fd, &readfds)) {
			if (!add_endpoint(handle))
				break;
			continue;
		}

		SLIST_FOREACH(endpoint, &handle->endpoints, next) {
			if (!FD_ISSET(endpoint->fd, &readfds))
				continue;

			msg_type = rpc_recv(endpoint->fd, buf);
			if (msg_type == RPC_MSG_ERROR) {
				SLIST_REMOVE(&handle->endpoints, endpoint,
				    retrace_rpc_endpoint, next);
				close(endpoint->fd);
				free(endpoint);
				break;
			}

			assert(msg_type == RPC_MSG_CALL_INIT);

			fnid = *(enum rpc_function_id *)buf;
			g_call_handlers[fnid](endpoint);
			rpc_send(endpoint->fd, RPC_MSG_DONE, NULL, 0);
			++(endpoint->call_num);
		}
	}
}

retrace_call_handler_t
retrace_get_call_handler(enum rpc_function_id id)
{
	return g_call_handlers[id];
}

void
retrace_set_call_handler(enum rpc_function_id id, retrace_call_handler_t fn)
{
	g_call_handlers[id] = fn;
}

void *
trace_buffer(void *buffer, size_t length)
{
	char *p = buffer;
	int i;

	for (i = 0; i < length; i++, p++)
		printf("%c", isprint(*p) ? *p : '.');

	return (buffer + length);
}
