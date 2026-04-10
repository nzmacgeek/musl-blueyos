#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <blueyos_verbose.h>
#include "atomic.h"

/*
 * Cached verbose level: -1 means not yet initialised.
 * A_cas ensures that only one thread races to initialise the value; once
 * set to a non-negative number it is never modified again.
 */
static volatile int __blueyos_verbose_cached = -1;

static int __read_verbose_level(void)
{
	char buf[4096];
	int fd, n;
	const char *p;

	fd = open("/proc/cmdline", O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		return BLUEYOS_VERBOSE_QUIET;

	n = (int)read(fd, buf, sizeof(buf) - 1);
	close(fd);

	if (n <= 0)
		return BLUEYOS_VERBOSE_QUIET;

	buf[n] = '\0';

	/* Scan each whitespace-delimited token for "verbose=N". */
	p = buf;
	while (*p) {
		if (!strncmp(p, "verbose=", 8)) {
			int level = p[8] - '0';
			if (level >= BLUEYOS_VERBOSE_QUIET && level <= BLUEYOS_VERBOSE_DEBUG)
				return level;
			/* Out-of-range value: fall through to default. */
		}
		/* Advance past the current token. */
		while (*p && *p != ' ' && *p != '\t' && *p != '\n')
			p++;
		/* Skip whitespace (including newlines) between tokens. */
		while (*p == ' ' || *p == '\t' || *p == '\n')
			p++;
	}

	return BLUEYOS_VERBOSE_QUIET;
}

/*
 * blueyos_verbose_level - return the boot-argument verbosity level.
 *
 * Reads verbose=N from /proc/cmdline on the first call and caches the
 * result.  Returns BLUEYOS_VERBOSE_QUIET (0) when no "verbose=" token
 * is present or when /proc/cmdline cannot be opened.
 *
 * Thread safety: uses a_cas so that only one winner stores the value;
 * any thread that loses the race re-reads __blueyos_verbose_cached which
 * is already set to the correct (non-negative) result.
 */
int blueyos_verbose_level(void)
{
	if (__blueyos_verbose_cached < 0) {
		int level = __read_verbose_level();
		a_cas(&__blueyos_verbose_cached, -1, level);
	}
	return __blueyos_verbose_cached;
}
