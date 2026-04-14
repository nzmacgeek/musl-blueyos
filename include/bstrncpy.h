#ifndef _BSTRNCPY_H
#define _BSTRNCPY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#define __NEED_size_t
#include <bits/alltypes.h>

/*
 * Error codes returned by bstrncpy().
 */
#define BSTRNCPY_OK          0  /* complete copy - all of src was copied */
#define BSTRNCPY_TRUNCATED   1  /* src was longer than dest_size-1; output truncated */
#define BSTRNCPY_ERR_NULL   -1  /* dest or src is NULL */
#define BSTRNCPY_ERR_SIZE   -2  /* dest_size is 0; no NUL terminator can be written */

/*
 * bstrncpy - safe, always-NUL-terminating string copy for BlueyOS.
 *
 * Copies at most dest_size-1 bytes from src into dest, stopping at the
 * first embedded NUL in src.  dest is always NUL-terminated when
 * dest_size > 0.
 *
 * Unlike strncpy(3):
 *   - dest is never left without a terminating NUL byte.
 *   - dest is never written beyond dest_size bytes (buffer overflow safe).
 *   - NULL pointer arguments are detected and rejected.
 *   - The return value distinguishes success, truncation, and error.
 *
 * Parameters:
 *   dest       - destination buffer of at least dest_size bytes.
 *   src        - NUL-terminated (or partially NUL-terminated) source string.
 *   dest_size  - total capacity of dest in bytes, including the NUL terminator.
 *
 * Returns:
 *   BSTRNCPY_OK (0)       - src fit entirely within dest; no truncation.
 *   BSTRNCPY_TRUNCATED (1)- src was longer than dest_size-1 bytes; dest holds
 *                           the first dest_size-1 bytes of src followed by NUL.
 *   BSTRNCPY_ERR_NULL (-1)- dest or src is NULL; dest is not modified.
 *   BSTRNCPY_ERR_SIZE (-2)- dest_size is 0; dest is not modified.
 *
 * On any negative (error) return the destination buffer is left unchanged.
 */
int bstrncpy(char *dest, const char *src, size_t dest_size);

#ifdef __cplusplus
}
#endif

#endif
