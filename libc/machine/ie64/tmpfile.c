#include "ie64_file.h"

FILE *
tmpfile(void)
{
    uint64_t handle;
    int error = __ie64_file_tmp(&handle);
    FILE *stream;
    if (error) {
        errno = error;
        return NULL;
    }
    stream = __ie64_stream_from_handle(handle, "w+");
    if (!stream) {
        int saved = errno;
        (void)__ie64_file_close(handle);
        errno = saved;
    }
    return stream;
}
