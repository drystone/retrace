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

#include "common.h"
#include "str.h"
#include "netdb.h"
#include "printf.h"
#include "netfuzz.h"

static rtr_netfuzz_config_t g_netfuzz_config;

/* fuzzing functions for network */
struct {
	enum RTR_NET_FUNC_ID id;
	const char *name;
	int fuzzing_set;
} rtr_net_funcs[] = {
	{NET_FUNC_ID_SOCKET, "socket", NET_FUZZ_TYPE_NOMEM | NET_FUZZ_TYPE_LIMIT},
	{NET_FUNC_ID_CONNECT, "connect", NET_FUZZ_TYPE_INUSE | NET_FUZZ_TYPE_UNREACH | NET_FUZZ_TYPE_TIMEOUT},
	{NET_FUNC_ID_BIND, "bind", NET_FUZZ_TYPE_INUSE},
	{NET_FUNC_ID_LISTEN, "listen", NET_FUZZ_TYPE_INUSE},
	{NET_FUNC_ID_ACCEPT, "accept", NET_FUZZ_TYPE_LIMIT | NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_SEND, "send", NET_FUZZ_TYPE_RESET | NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_RECV, "recv", NET_FUZZ_TYPE_REFUSE | NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_SENDTO, "sendto", NET_FUZZ_TYPE_RESET | NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_RECVFROM, "recvfrom", NET_FUZZ_TYPE_REFUSE | NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_SENDMSG, "sendmsg", NET_FUZZ_TYPE_RESET | NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_RECVMSG, "recvmsg", NET_FUZZ_TYPE_REFUSE | NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_GETHOSTNAME, "gethostbyname", NET_FUZZ_TYPE_HOST_NOT_FOUND | NET_FUZZ_TYPE_TRY_AGAIN},
	{NET_FUNC_ID_GETHOSTADDR, "getaddrbyname", NET_FUZZ_TYPE_HOST_NOT_FOUND | NET_FUZZ_TYPE_TRY_AGAIN},
	{NET_FUNC_ID_GETADDRINFO, "getaddrinfo", NET_FUZZ_TYPE_HOST_NOT_FOUND | NET_FUZZ_TYPE_TRY_AGAIN |
					NET_FUZZ_TYPE_NOMEM},
	{NET_FUNC_ID_MAX, NULL}
};

/* fuzzing type and error code */
struct {
	int type;
	const char *str;
	int err;
} rtr_netfuzz_types[] = {
	{NET_FUZZ_TYPE_NOMEM, "NO_MEMORY", ENOMEM},
	{NET_FUZZ_TYPE_LIMIT, "LIMIT_SOCKET", ENFILE},
	{NET_FUZZ_TYPE_INUSE, "ADDR_INUSE", EADDRINUSE},
	{NET_FUZZ_TYPE_UNREACH, "NET_UNREACHABLE", ENETUNREACH},
	{NET_FUZZ_TYPE_TIMEOUT, "CONN_TIMEOUT", ETIMEDOUT},
	{NET_FUZZ_TYPE_RESET, "CONN_RESET", ECONNRESET},
	{NET_FUZZ_TYPE_REFUSE, "CONN_REFUSE", ECONNREFUSED},
	{NET_FUZZ_TYPE_HOST_NOT_FOUND, "HOST_NOT_FOUND", HOST_NOT_FOUND},
	{NET_FUZZ_TYPE_TRY_AGAIN, "SERVICE_NOT_AVAIL", TRY_AGAIN},
	{NET_FUZZ_TYPE_END, NULL, 0}
};

/* initialize network fuzzing configuration */
static void init_netfuzz_config(void)
{
	RTR_CONFIG_HANDLE config = RTR_CONFIG_START;

	do {
		char *func_name = NULL;
		int fuzz_set;
		char *fuzz_type_str = NULL;
		double fuzz_rate;

		int i, func_id = NET_FUNC_ID_MAX, fuzz_type = NET_FUZZ_TYPE_END, fuzz_err = 0;

		if (rtr_get_config_multiple(&config, "fuzzing-net", ARGUMENT_TYPE_STRING, ARGUMENT_TYPE_STRING,
			ARGUMENT_TYPE_DOUBLE, ARGUMENT_TYPE_END, &func_name, &fuzz_type_str, &fuzz_rate) == 0)
			break;

		/* get func index from func name */
		for (i = 0; rtr_net_funcs[i].name != NULL; i++) {
			if (real_strcmp(func_name, rtr_net_funcs[i].name) == 0) {
				func_id = rtr_net_funcs[i].id;
				fuzz_set = rtr_net_funcs[i].fuzzing_set;
				break;
			}
		}

		/* get fuzzing type */
		for (i = 0; rtr_netfuzz_types[i].str != NULL; i++) {
			if (real_strcmp(fuzz_type_str, rtr_netfuzz_types[i].str) == 0) {
				fuzz_type = rtr_netfuzz_types[i].type;
				fuzz_err = rtr_netfuzz_types[i].err;
				break;
			}
		}

		/* set fuzzing value */
		if (func_id != NET_FUNC_ID_MAX && (fuzz_type & fuzz_set)) {
			g_netfuzz_config.fuzz_types[func_id] = fuzz_type;
			g_netfuzz_config.fuzz_err[func_id] = fuzz_err;
			g_netfuzz_config.fuzz_rates[func_id] = fuzz_rate;
		}
	} while (config != NULL);

	g_netfuzz_config.init_flag = 1;
}

/* get network fuzzing error value */
int rtr_get_net_fuzzing(enum RTR_NET_FUNC_ID func_id, int *err_val)
{
	int matched = 0;

	/* init fuzzing config */
	if (!g_netfuzz_config.init_flag)
		init_netfuzz_config();

	/* get socket function and fuzzing type from configuration */
	if (rtr_get_fuzzing_flag(g_netfuzz_config.fuzz_rates[func_id])) {
		/* set matching flag */
		matched = 1;

		/* get error code for fuzzing type */
		*err_val = g_netfuzz_config.fuzz_err[func_id];
	}

	return matched;
}
