#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

const char *LIBRARYSTRNAME = "libmutex";

void fault(const char *msgformat, ...)
{
    va_list args;
    va_start(args, msgformat);

    fprintf(stderr, "[%s]: ", LIBRARYSTRNAME);
    vfprintf(stderr, msgformat, args);
    fputs(". Application terminated.", stderr);

    va_end(args);
    abort();
}