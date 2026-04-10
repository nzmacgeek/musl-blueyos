# Agent Instructions for musl-blueyos

This file contains standard instructions for AI coding agents working in the
**musl-blueyos** repository.  All agents must follow these conventions.

---

## Verbose Boot-Argument Logging Standard

### Overview

BlueyOS supports a `verbose=N` kernel command-line argument that controls the
verbosity of all system-level logging—both in the kernel and in userspace
components built against this libc.

| Level | Constant                   | Meaning                                      |
|-------|----------------------------|----------------------------------------------|
| 0     | `BLUEYOS_VERBOSE_QUIET`    | Default / quiet – only critical messages     |
| 1     | `BLUEYOS_VERBOSE_INFO`     | Standard info – INFO-level messages shown    |
| 2     | `BLUEYOS_VERBOSE_DEBUG`    | Full debug – all DEBUG / trace messages shown |

### Kernel-side convention

Any `[TAG DBG]` or `[TAG]` print statements in kernel sources (VFS, BISCUITFS,
ELF loader, SYS, PGF, etc.) **must** be gated by the verbose level that was
parsed from the kernel command line.  Use the same three-level scheme:

```c
/* verbose=0: no debug output */
/* verbose=1: top-level INFO prints only */
/* verbose=2: full DBG/trace prints */
if (verbose >= 2) kprintf("[VFS DBG] open request path=%s ...\n", path);
```

### Userspace convention (musl / programs linking against musl-blueyos)

1. Include the public header:

   ```c
   #include <blueyos_verbose.h>
   ```

2. Call `blueyos_verbose_level()` to obtain the current level (cached after the
   first call):

   ```c
   if (blueyos_verbose_level() >= BLUEYOS_VERBOSE_DEBUG)
       fprintf(stderr, "[mymod DBG] detail=%d\n", detail);
   ```

3. At `BLUEYOS_VERBOSE_INFO` (≥ 1) log routine operational events; at
   `BLUEYOS_VERBOSE_DEBUG` (≥ 2) log internal state, addresses, and per-call
   traces.

4. At `BLUEYOS_VERBOSE_QUIET` (0, the default) emit **no** debug or info
   output—only fatal errors that cannot be silenced.

### Retrofitting existing code

When modifying any existing file in this repository that already emits debug or
diagnostic output (e.g. via `fprintf`, `dprintf`, or `__syscall(SYS_write,…)`):

* Wrap the output in a `blueyos_verbose_level() >= …` guard.
* Use `BLUEYOS_VERBOSE_INFO` for high-level status messages.
* Use `BLUEYOS_VERBOSE_DEBUG` for low-level or per-operation traces.
* Never remove existing messages—gate them instead.

### Implementation reference

| File                            | Role                                                  |
|---------------------------------|-------------------------------------------------------|
| `include/blueyos_verbose.h`     | Public header; exposes `blueyos_verbose_level()` and constants |
| `src/misc/blueyos_verbose.c`    | Reads `/proc/cmdline`, parses `verbose=N`, caches result |

`blueyos_verbose_level()` is compiled into `libc.a` / `libc.so` automatically
via the Makefile wildcard (`src/misc/*.c`).  No Makefile changes are needed
when adding new source files to `src/misc/`.
