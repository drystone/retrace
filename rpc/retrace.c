#include "frontend.h"

static int g_sockfd;

int main(int argc, char **argv)
{
	struct retrace_handle *trace_handle;

	trace_handle = retrace_start(&argv[1]);

	retrace_trace(trace_handle);

	retrace_close(trace_handle);
}
