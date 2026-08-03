#include "ie64_file.h"

_Static_assert(sizeof(off_t) == 8, "IE64 off_t must be 64 bits");
_Static_assert(sizeof(__off_t) == 8, "IE64 __off_t must be 64 bits");
_Static_assert(sizeof(uintptr_t) == 8, "IE64 uintptr_t must be 64 bits");

static ssize_t
ie64_read(void *cookie, void *buffer, size_t size)
{
    size_t done;
    int error = __ie64_file_read((uint64_t)(uintptr_t)cookie, buffer, size, &done);
    if (error) {
        errno = error;
        return -1;
    }
    return (ssize_t)done;
}

static ssize_t
ie64_write(void *cookie, const void *buffer, size_t size)
{
    size_t done;
    int error = __ie64_file_write((uint64_t)(uintptr_t)cookie, buffer, size, &done);
    if (error || (size && !done)) {
        errno = error ? error : EIO;
        return -1;
    }
    return (ssize_t)done;
}

static __off_t
ie64_seek(void *cookie, __off_t offset, int whence)
{
    int origin;
    int64_t position;
    int error;
    switch (whence) {
    case SEEK_SET: origin = IE64_SEEK_SET; break;
    case SEEK_CUR: origin = IE64_SEEK_CUR; break;
    case SEEK_END: origin = IE64_SEEK_END; break;
    default: errno = EINVAL; return (__off_t)-1;
    }
    error = __ie64_file_seek((uint64_t)(uintptr_t)cookie, (int64_t)offset, origin, &position);
    if (error) {
        errno = error;
        return (__off_t)-1;
    }
    return (__off_t)position;
}

static int
ie64_close(void *cookie)
{
    int error = __ie64_file_close((uint64_t)(uintptr_t)cookie);
    if (error) {
        errno = error;
        return -1;
    }
    return 0;
}

int
__ie64_mode_flags(const char *mode, unsigned int *flags)
{
    unsigned int f;
    bool seen_b = false;
    bool seen_plus = false;
    const char *p;
    if (!mode || !mode[0]) {
        errno = EINVAL;
        return -1;
    }
    switch (mode[0]) {
    case 'r': f = IE64_OPEN_READ; break;
    case 'w': f = IE64_OPEN_WRITE | IE64_OPEN_CREATE | IE64_OPEN_TRUNCATE; break;
    case 'a': f = IE64_OPEN_WRITE | IE64_OPEN_CREATE | IE64_OPEN_APPEND; break;
    default: errno = EINVAL; return -1;
    }
    for (p = mode + 1; *p; p++) {
        if (*p == 'b' && !seen_b)
            seen_b = true;
        else if (*p == '+' && !seen_plus)
            seen_plus = true;
        else {
            errno = EINVAL;
            return -1;
        }
    }
    if (seen_plus)
        f |= IE64_OPEN_READ | IE64_OPEN_WRITE;
    *flags = f;
    return 0;
}

void
__ie64_stream_set_handle(FILE *stream, uint64_t handle, unsigned int flags,
                         bool own_backend)
{
    struct __file_bufio *bf = (struct __file_bufio *)stream;

    stream->flags = (stream->flags & ~(__SRD | __SWR | __SERR | __SEOF))
                    | (flags & IE64_OPEN_READ ? __SRD : 0)
                    | (flags & IE64_OPEN_WRITE ? __SWR : 0);
    bf->ptr = (void *)(uintptr_t)handle;
    bf->dir = 0;
    bf->pos = 0;
    bf->len = 0;
    bf->off = 0;
    bf->bflags = (bf->bflags & (__BALL | __BFALL | __BLBF)) | __BFPTR
                 | (own_backend ? __BFOWN : 0);
    bf->read_ptr = flags & IE64_OPEN_READ ? ie64_read : NULL;
    bf->write_ptr = flags & IE64_OPEN_WRITE ? ie64_write : NULL;
    bf->lseek_ptr = ie64_seek;
    bf->close_ptr = ie64_close;
    bf->xfile.cfile.close = __bufio_close;
}

FILE *
__ie64_stream_from_handle(uint64_t handle, const char *mode)
{
    unsigned int flags;
    FILE *stream;
    if (__ie64_mode_flags(mode, &flags) < 0)
        return NULL;
    stream = funopen((void *)(uintptr_t)handle,
                     flags & IE64_OPEN_READ ? ie64_read : NULL,
                     flags & IE64_OPEN_WRITE ? ie64_write : NULL,
                     ie64_seek, ie64_close);
    if (stream) {
        struct __file_bufio *bf = (struct __file_bufio *)stream;
        /* Preserve the full opaque handle independently of host-fd ABI
         * compatibility decisions made by generic funopen setup. */
        bf->ptr = (void *)(uintptr_t)handle;
        bf->bflags |= __BFOWN | __BFPTR;
        bufio_add_file(stream);
    }
    return stream;
}
