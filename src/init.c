/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "init.h"

#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "util.h"

#ifndef LIBNTHREAD_OS_WINDOWS
    #if defined(__USE_UNIX98) || defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500
        #define PTHREAD_MUTEXTYPE_RECURSIVE PTHREAD_MUTEX_RECURSIVE
    #else
        #define PTHREAD_MUTEXTYPE_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
    #endif
#endif

static NThreadAtomicBool inited = NTHREAD_ATOMICBOOLINIT(false);
static NThreadAtomicBool funcslock = NTHREAD_ATOMICBOOLINIT(false);

#define ISLIBRARYINITED() (nthread_atomicbool_load(&inited))

bool libnthread_initialized(void) { return ISLIBRARYINITED(); }

NError libnthread_startup(const LibNThreadStartupOptions *options)
{
    if (!nthread_atomicbool_cmpxchgt(&funcslock, false, true)) return NError_OperationInProgress;
    if (ISLIBRARYINITED()) { nthread_atomicbool_store(&funcslock, false); return NError_AlreadyInitialized; }

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

    // =============================================================================

    #ifndef LIBNTHREAD_OS_WINDOWS
        NError nerr;

        if ((nerr = translateerror(pthread_mutexattr_init(&recursivemutexattr)) != NError_Success)) goto errorquit_generic;
        if ((nerr = translateerror(pthread_mutexattr_settype(&recursivemutexattr, PTHREAD_MUTEXTYPE_RECURSIVE))) != NError_Success) goto errorquit_onsetmtxattrtype;
    #endif

    // =============================================================================

    nthread_atomicbool_store(&inited, true);
    nthread_atomicbool_store(&funcslock, false);
    return NError_Success;

    // =============================================================================

    #ifndef LIBNTHREAD_OS_WINDOWS
        {
            NError tmpnerr;

            errorquit_onsetmtxattrtype:
                if ((tmpnerr = translateerror(pthread_mutexattr_destroy(&recursivemutexattr))) != NError_Success)
                { panic_general(tmpnerr, "Unable to destroy pthread mutex attributes structure when handling error."); }
                memset(&recursivemutexattr, 0, sizeof(pthread_mutexattr_t));
            errorquit_generic:
                __alerthandler = NULL;
                __panichandler = NULL;
                memset(&allocs, 0, sizeof(allocs));
        }

        nthread_atomicbool_store(&funcslock, false);
        return nerr;
    #endif
}

NError libnthread_cleanup(void)
{
    if (!nthread_atomicbool_cmpxchgt(&funcslock, false, true)) return NError_OperationInProgress;
    if (!ISLIBRARYINITED()) { nthread_atomicbool_store(&funcslock, false); return NError_NotInitialized; }

    // =============================================================================

    #ifndef LIBNTHREAD_OS_WINDOWS
        NError nerr;

        if ((nerr = translateerror(pthread_mutexattr_destroy(&recursivemutexattr))) != NError_Success)
        { nthread_atomicbool_store(&funcslock, false); return nerr; }
        memset(&recursivemutexattr, 0, sizeof(pthread_mutexattr_t));
    #endif

    // =============================================================================

    __alerthandler = NULL;
    __panichandler = NULL;
    memset(&allocs, 0, sizeof(allocs));

    nthread_atomicbool_store(&inited, false);
    nthread_atomicbool_store(&funcslock, false);
    return NError_Success;
}