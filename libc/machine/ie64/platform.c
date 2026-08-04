#include <errno.h>
#include <intuitionengine.h>
#include <sys/types.h>

#define TERM_OUT (*(volatile unsigned char *)(uintptr_t)0xF0700)
#define TERM_STATUS (*(volatile const unsigned int *)(uintptr_t)0xF0704)
#define TERM_IN (*(volatile const unsigned char *)(uintptr_t)0xF0708)

_Noreturn void __builtin_ie64_halt(void);

__attribute__((weak)) int
__ie64_console_read(void *buffer, size_t size, size_t *done)
{
    unsigned char *bytes = buffer;
    size_t count = 0;
    while (count < size && (TERM_STATUS & 1u))
        bytes[count++] = TERM_IN;
    *done = count;
    return 0;
}

__attribute__((weak)) int
__ie64_console_write(const void *buffer, size_t size, size_t *done)
{
    const unsigned char *bytes = buffer;
    size_t count;
    for (count = 0; count < size; count++) {
        while (!(TERM_STATUS & 2u))
            ;
        TERM_OUT = bytes[count];
    }
    *done = count;
    return 0;
}

#define IE64_ENOSYS_HOOK(name, params) __attribute__((weak)) int name params { return ENOSYS; }
IE64_ENOSYS_HOOK(__ie64_file_open, (const char *p, unsigned int f, uint64_t *h))
IE64_ENOSYS_HOOK(__ie64_file_read, (uint64_t h, void *b, size_t s, size_t *d))
IE64_ENOSYS_HOOK(__ie64_file_write, (uint64_t h, const void *b, size_t s, size_t *d))
IE64_ENOSYS_HOOK(__ie64_file_seek, (uint64_t h, int64_t o, int w, int64_t *p))
IE64_ENOSYS_HOOK(__ie64_file_close, (uint64_t h))
IE64_ENOSYS_HOOK(__ie64_file_remove, (const char *p))
IE64_ENOSYS_HOOK(__ie64_file_rename, (const char *o, const char *n))
IE64_ENOSYS_HOOK(__ie64_file_tmp, (uint64_t *h))
IE64_ENOSYS_HOOK(__ie64_monotonic_ticks, (uint64_t *t))

ssize_t
read(int fd, void *buffer, size_t size)
{
    size_t done;
    int error;
    if (fd != 0) {
        errno = EBADF;
        return -1;
    }
    error = __ie64_console_read(buffer, size, &done);
    if (error) {
        errno = error;
        return -1;
    }
    return (ssize_t)done;
}

ssize_t
write(int fd, const void *buffer, size_t size)
{
    size_t done;
    int error;
    if (fd != 1 && fd != 2) {
        errno = EBADF;
        return -1;
    }
    error = __ie64_console_write(buffer, size, &done);
    if (error || (size && !done)) {
        errno = error ? error : EIO;
        return -1;
    }
    return (ssize_t)done;
}

int close(int fd) { return fd >= 0 && fd <= 2 ? 0 : (errno = EBADF, -1); }

off_t
lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    errno = ESPIPE;
    return (off_t)-1;
}

__attribute__((weak)) _Noreturn void
__ie64_terminate(int status)
{
    (void)status;
    __builtin_ie64_halt();
    __builtin_unreachable();
}

_Noreturn void _exit(int status) { __ie64_terminate(status); }
