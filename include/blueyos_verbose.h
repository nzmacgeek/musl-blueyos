#ifndef _BLUEYOS_VERBOSE_H
#define _BLUEYOS_VERBOSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Boot-argument verbose logging levels for BlueyOS.
 *
 * The kernel command line may contain "verbose=N" where N is one of:
 *   0  (default) – quiet; only critical/fatal messages are shown
 *   1            – standard info logging; shows INFO-level messages
 *   2            – full debug; shows all DEBUG and trace messages
 *
 * blueyos_verbose_level() reads /proc/cmdline on the first call and
 * caches the result for subsequent calls.  If /proc/cmdline is absent
 * or contains no "verbose=" token, 0 (quiet) is returned.
 */

#define BLUEYOS_VERBOSE_QUIET  0
#define BLUEYOS_VERBOSE_INFO   1
#define BLUEYOS_VERBOSE_DEBUG  2

int blueyos_verbose_level(void);

#ifdef __cplusplus
}
#endif

#endif
