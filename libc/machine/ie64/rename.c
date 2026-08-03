#include <errno.h>
#include <ie64/platform.h>

int
rename(const char *old_path, const char *new_path)
{
    int error = __ie64_file_rename(old_path, new_path);
    if (error) {
        errno = error;
        return -1;
    }
    return 0;
}
