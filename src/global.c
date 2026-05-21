#include "libmutex.h"

#include <stdlib.h>

void *(*libmutex_malloc)(size_t) = malloc;
//void *(*libmutex_realloc)(void *, size_t) = realloc;
void (*libmutex_free)(void *) = free;