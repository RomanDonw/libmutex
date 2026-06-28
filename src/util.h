#ifndef UTIL_H
#define UTIL_H

#include "libnthread.h"

extern LibNThreadAllocators __libnthread_allocators;
#define allocs __libnthread_allocators

#endif