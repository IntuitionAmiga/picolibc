#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

extern char __ie64_heap_start[];
extern char __ie64_heap_end[];

void *
sbrk(ptrdiff_t increment)
{
    static uintptr_t current;
    uintptr_t start = (uintptr_t)__ie64_heap_start;
    uintptr_t end = (uintptr_t)__ie64_heap_end;
    uintptr_t old;

    if (!current)
        current = start;
    old = current;

    if ((increment > 0 && (uintptr_t)increment > end - current)
        || (increment < 0 && (uintptr_t)(-(increment + 1)) + 1 > current - start)) {
        errno = ENOMEM;
        return (void *)-1;
    }
    current = increment < 0 ? current - ((uintptr_t)(-(increment + 1)) + 1)
                            : current + (uintptr_t)increment;
    return (void *)old;
}
