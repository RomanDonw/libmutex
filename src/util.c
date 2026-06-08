#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void __logerr(const char *msgformat, va_list args)
{
    fprintf(stderr, "[libmutex]: ");
    vfprintf(stderr, msgformat, args);
}

#ifdef LIBMUTEX_DEBUG
    void __libmutex_logdbgerr(const char *msgformat, ...)
    {
        va_list args;
        va_start(args, msgformat);
        
        __logerr(msgformat, args);
        fputs(".", stderr);

        va_end(args);
    }
#endif

void __libmutex_fault(const char *msgformat, ...)
{
    va_list args;
    va_start(args, msgformat);

    __logerr(msgformat, args);
    fputs(". Application terminated.", stderr);

    va_end(args);
    abort();
}