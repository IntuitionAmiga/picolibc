#include "ie64_file.h"

FILE *
freopen(const char *path, const char *mode, FILE *stream)
{
    struct __file_bufio *bf = (struct __file_bufio *)stream;
    unsigned int flags;
    uint64_t handle = 0;
    int error;
    bool registered;

    if (!stream || __ie64_mode_flags(mode, &flags) < 0)
        return NULL;

    if (!path) {
        if (!(stream->flags & __SBUF)) {
            errno = EBADF;
            return NULL;
        }
        (void)fflush(stream);
        stream->flags = (stream->flags & ~(__SRD | __SWR | __SERR | __SEOF))
                        | (flags & IE64_OPEN_READ ? __SRD : 0)
                        | (flags & IE64_OPEN_WRITE ? __SWR : 0);
        bf->dir = 0;
        bf->len = 0;
        bf->off = 0;
        return stream;
    }

    if (!(stream->flags & __SBUF)) {
        errno = EBADF;
        return NULL;
    }
    error = __ie64_file_open(path, flags, &handle);
    if (error) {
        errno = error;
        return NULL;
    }

    registered = bufio_file_registered(stream);
    (void)fflush(stream);
    if (bf->bflags & __BFOWN)
        (void)bufio_close(bf);
    __ie64_stream_set_handle(stream, handle, flags, true);
    if (!registered)
        bufio_add_file(stream);
    return stream;
}
