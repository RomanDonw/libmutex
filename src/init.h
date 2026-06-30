#ifndef INIT_H
#define INIT_H

#include "libnthread.h"

#define ENSURE_INIT { if (!libnthread_initialized()) return NError_NotInitialized; }

#endif