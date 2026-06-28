#include "init.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"

static NThreadAtomicBool inited = NTHREAD_ATOMICBOOLINIT(false);
static NThreadAtomicBool funcslock = NTHREAD_ATOMICBOOLINIT(false);

bool libnthread_initialized(void) { nthread_atomicbool_load(&inited); }

NThreadError libnthread_startup(const LibNThreadStartupOptions *options)
{
    if (!nthread_atomicbool_cmpxchgt(&funcslock, false, true)) return NThreadError_OperationInProgress;
    if (nthread_atomicbool_load(&inited)) { nthread_atomicbool_store(&funcslock, false); return NThreadError_AlreadyInitialized; }

    static const LibNThreadStartupOptions defaultopts = LIBNTHREADSTARTUPOPTIONS_DEFAULTINIT;
    if (!options) options = &defaultopts;

    if (options->allocators) allocs = *options->allocators;
    else
    {
        allocs.malloc = malloc;
        allocs.realloc = realloc;
        allocs.free = free;
    }

    nthread_atomicbool_store(&inited, true);
    nthread_atomicbool_store(&funcslock, false);
    return NThreadError_Success;
}

NThreadError libnthread_cleanup(void)
{
    if (!nthread_atomicbool_cmpxchgt(&funcslock, false, true)) return NThreadError_OperationInProgress;
    if (!nthread_atomicbool_load(&inited)) { nthread_atomicbool_store(&funcslock, false); return NThreadError_NotInitialized; }

    memset(&allocs, 0, sizeof(allocs));

    nthread_atomicbool_store(&inited, false);
    nthread_atomicbool_store(&funcslock, false);
    return NThreadError_Success;
}