#include <errno.h>
#include <ie64/platform.h>

int
remove(const char *path)
{
    int error = __ie64_file_remove(path);
    if (error) {
        errno = error;
        return -1;
    }
    return 0;
}
