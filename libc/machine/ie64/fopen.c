#include "ie64_file.h"

FILE *
fopen(const char *path, const char *mode)
{
    unsigned int flags;
    uint64_t handle;
    int error;
    FILE *stream;
    if (__ie64_mode_flags(mode, &flags) < 0)
        return NULL;
    error = __ie64_file_open(path, flags, &handle);
    if (error) {
        errno = error;
        return NULL;
    }
    stream = __ie64_stream_from_handle(handle, mode);
    if (!stream) {
        int saved = errno;
        (void)__ie64_file_close(handle);
        errno = saved;
    }
    return stream;
}
