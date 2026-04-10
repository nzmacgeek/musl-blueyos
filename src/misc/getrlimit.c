#include <sys/resource.h>
#include <errno.h>
#include "syscall.h"

/* BlueyOS fallback: kernel expects two 32-bit rlimit fields and exposes
 * syscall number 191 (ugetrlimit on i386) for getrlimit. Provide a tiny int0x80 wrapper so
 * musl can call it when other syscalls are unavailable.
 */
static inline long __blueyos_syscall2(long n, long a1, long a2)
{
	long ret;
	__asm__ volatile (
		"int $0x80"
		: "=a" (ret)
		: "0" (n), "b" (a1), "c" (a2)
		: "memory"
	);
	return ret;
}

#define FIX(x) do{ if ((x)>=SYSCALL_RLIM_INFINITY) (x)=RLIM_INFINITY; }while(0)

int getrlimit(int resource, struct rlimit *rlim)
{
	int ret = syscall(SYS_prlimit64, 0, resource, 0, rlim);
	if (!ret) {
		FIX(rlim->rlim_cur);
		FIX(rlim->rlim_max);
	}
#ifdef SYS_getrlimit
	unsigned long k_rlim[2];
	if (!ret || errno != ENOSYS)
		return ret;
	if (syscall(SYS_getrlimit, resource, k_rlim) < 0)
		return -1;
	rlim->rlim_cur = k_rlim[0] == -1UL ? RLIM_INFINITY : k_rlim[0];
	rlim->rlim_max = k_rlim[1] == -1UL ? RLIM_INFINITY : k_rlim[1];
	FIX(rlim->rlim_cur);
	FIX(rlim->rlim_max);
	return 0;
#else
#define BLUEY_SYS_GETRLIMIT 191
	if (!ret || errno != ENOSYS)
		return ret;

	unsigned int krlim32[2];
	long r2 = __blueyos_syscall2(BLUEY_SYS_GETRLIMIT, (long)resource, (long)(uintptr_t)krlim32);
	if (r2 < 0) {
		errno = -r2;
		return -1;
	}
	rlim->rlim_cur = krlim32[0] == 0xFFFFFFFFu ? RLIM_INFINITY : (unsigned long)krlim32[0];
	rlim->rlim_max = krlim32[1] == 0xFFFFFFFFu ? RLIM_INFINITY : (unsigned long)krlim32[1];
	FIX(rlim->rlim_cur);
	FIX(rlim->rlim_max);
	return 0;
#undef BLUEY_SYS_GETRLIMIT
#endif
}
