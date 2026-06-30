#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

const char *LIBNTHREAD_MODULENAME = "libnthread";

NMemoryAllocators __libnthread_allocators = {0};

// =============================================================================

void __libnthread_defaultpanichandler(const char *module, const char *file, long long line, const char *function, const char *description, NError error)
{
    fprintf(stderr, "\n\n\n###################\n# LIBNTHREAD PANIC #\n###################\n\n");

    fprintf(stderr, "In \"%s\" at line %lld (%s):\n", file, line, function);

    if (error != PANIC_NOERRORCODE) fprintf(stderr, "    \"%s\" because\n    ", ncore_strerror(error));

    fprintf(stderr, "    %s\n\n###################\n\n", description);
}

void __libnthread_defaultalerthandler(const char *module, const char *file, long long line, const char *function, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\n\n\n###################\n# LIBNTHREAD ALERT #\n###################\n\n");

    fprintf(stderr, "In \"%s\" at line %lld (%s):\n    ", file, line, function);
    vfprintf(stderr, format, args);

    fputs("\n\n###################\n", stderr);
    va_end(args);
}