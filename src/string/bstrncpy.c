#include <string.h>
#include <errno.h>
#include <bstrncpy.h>

/*
 * bstrncpy - safe, always-NUL-terminating string copy for BlueyOS.
 *
 * See include/bstrncpy.h for the full API contract.
 */
int bstrncpy(char *dest, const char *src, size_t dest_size)
{
	size_t slen;
	size_t copy_len;

	if (!dest || !src) {
		errno = EINVAL;
		return BSTRNCPY_ERR_NULL;
	}

	if (dest_size == 0) {
		errno = ERANGE;
		return BSTRNCPY_ERR_SIZE;
	}

	/*
	 * Use strnlen to find the end of src, looking at most dest_size bytes.
	 * This naturally stops at any embedded NUL byte (preventing treatment of
	 * binary data past an early NUL as part of the string) while avoiding a
	 * full traversal of very long strings when dest_size is small.
	 */
	slen = strnlen(src, dest_size);

	if (slen < dest_size) {
		/* src fits: copy slen bytes plus the NUL terminator. */
		memcpy(dest, src, slen + 1);
		return BSTRNCPY_OK;
	}

	/* src does not fit: copy dest_size-1 bytes and add a NUL terminator. */
	copy_len = dest_size - 1;
	memcpy(dest, src, copy_len);
	dest[copy_len] = '\0';
	return BSTRNCPY_TRUNCATED;
}
