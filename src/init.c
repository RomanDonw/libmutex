/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "init.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"

static NThreadAtomicBool inited = NTHREAD_ATOMICBOOLINIT(false);
static NThreadAtomicBool funcslock = NTHREAD_ATOMICBOOLINIT(false);

bool libnthread_initialized(void) { nthread_atomicbool_load(&inited); }

NError libnthread_startup(const LibNThreadStartupOptions *options)
{
    if (!nthread_atomicbool_cmpxchgt(&funcslock, false, true)) return NError_OperationInProgress;
    if (nthread_atomicbool_load(&inited)) { nthread_atomicbool_store(&funcslock, false); return NError_AlreadyInitialized; }

    static const LibNThreadStartupOptions defaultopts = LIBNTHREADSTARTUPOPTIONS_DEFAULTINIT;
    if (!options) options = &defaultopts;

    if (options->allocators) allocs = *options->allocators;
    else
    {
        allocs.malloc = malloc;
        allocs.realloc = realloc;
        allocs.free = free;
    }

    if (options->panichandler) __panichandler = *options->panichandler;
    else __panichandler = __defaultpanichandler;

    if (options->alerthandler) __alerthandler = *options->alerthandler;
    else __alerthandler = __defaultalerthandler;

    nthread_atomicbool_store(&inited, true);
    nthread_atomicbool_store(&funcslock, false);
    return NError_Success;
}

NError libnthread_cleanup(void)
{
    if (!nthread_atomicbool_cmpxchgt(&funcslock, false, true)) return NError_OperationInProgress;
    if (!nthread_atomicbool_load(&inited)) { nthread_atomicbool_store(&funcslock, false); return NError_NotInitialized; }

    __alerthandler = NULL;
    __panichandler = NULL;
    memset(&allocs, 0, sizeof(allocs));

    nthread_atomicbool_store(&inited, false);
    nthread_atomicbool_store(&funcslock, false);
    return NError_Success;
}