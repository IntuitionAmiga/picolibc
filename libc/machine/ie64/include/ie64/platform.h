#ifndef _IE64_PLATFORM_H
#define _IE64_PLATFORM_H

#include <stddef.h>
#include <stdint.h>

enum {
    IE64_OPEN_READ = 0x01,
    IE64_OPEN_WRITE = 0x02,
    IE64_OPEN_CREATE = 0x04,
    IE64_OPEN_TRUNCATE = 0x08,
    IE64_OPEN_APPEND = 0x10,
    IE64_OPEN_EXCLUSIVE = 0x20,
    IE64_SEEK_SET = 0,
    IE64_SEEK_CUR = 1,
    IE64_SEEK_END = 2,
};

/* Hooks return zero on success or a positive errno value on failure. On
 * failure they leave output counts, handles and positions unchanged. File
 * handles are opaque 64-bit values. Read and write may complete partially;
 * a successful non-empty write must report a non-zero count. Applications
 * overriding file hooks implement the flag, append and seek rules below. */
int __ie64_console_read(void *, size_t, size_t *);
int __ie64_console_write(const void *, size_t, size_t *);
int __ie64_file_open(const char *, unsigned int, uint64_t *);
int __ie64_file_read(uint64_t, void *, size_t, size_t *);
int __ie64_file_write(uint64_t, const void *, size_t, size_t *);
int __ie64_file_seek(uint64_t, int64_t, int, int64_t *);
int __ie64_file_close(uint64_t);
int __ie64_file_remove(const char *);
int __ie64_file_rename(const char *, const char *);
int __ie64_file_tmp(uint64_t *);
int __ie64_monotonic_ticks(uint64_t *);
_Noreturn void __ie64_terminate(int);

#endif
