#define _BSD_SOURCE
#include <unistd.h>
#include <errno.h>
#include "syscall.h"

int brk(void *end)
{
	void *result;
	__asm__ volatile (
		"int $0x80"
		: "=a"(result)
		: "0"(SYS_brk), "b"(end)
		: "memory"
	);
	if (result < end) { errno = ENOMEM; return -1; }
	return 0;
}
