#ifndef _IE64_FILE_INTERNAL_H
#define _IE64_FILE_INTERNAL_H

#include "local-stdio.h"
#include <intuitionengine.h>

FILE *__ie64_stream_from_handle(uint64_t handle, const char *mode);
int __ie64_mode_flags(const char *mode, unsigned int *flags);
void __ie64_stream_set_handle(FILE *stream, uint64_t handle,
                              unsigned int flags, bool own_backend);

#endif
