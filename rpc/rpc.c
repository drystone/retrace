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

#include "../config.h"

#include <unistd.h>
#include <sys/types.h>
#include <error.h>

#include "shim.h"
#include "rpc.h"

static int g_sockfd = -1;

static int g_inrpc;

static void
init_sockfd()
{
	const char *p;

	/* TODO something better on error */
	p = real_getenv("RTR_SOCKFD");
	if (p == 0)
		error(1, 0, "retrace env{RTR_SOCKFD} not set.");

	g_sockfd = 0;
	for (; *p; ++p) {
		if (*p < '0' || *p > '9')
			error(1, 0, "retrace env{RTR_SOCKFD} bad.");
		g_sockfd = g_sockfd * 10 + *p - '0';
	}

	send(g_sockfd, rpc_version, 32, 0);
}

void rpc_recv(struct msghdr *msg)
{
	if (g_inrpc == 1)
		return;

	if (g_sockfd == -1)
		init_sockfd();

	g_inrpc = 1;
	recvmsg(g_sockfd, msg, 0);
	g_inrpc = 0;
}

void
rpc_send(struct msghdr *msg)
{
	if (g_inrpc == 1)
		return;

	if (g_sockfd == -1)
		init_sockfd();

	g_inrpc = 1;
	sendmsg(g_sockfd, msg, 0);
	g_inrpc = 0;
}
