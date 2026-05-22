/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "libmutex.h"

#include <stdio.h>

#ifndef LIBMUTEX_OS_WINDOWS
    #include <errno.h>

    #if defined(__USE_UNIX98) || defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500
        #define PTHREAD_MUTEXTYPE_RECURSIVE PTHREAD_MUTEX_RECURSIVE
    #else
        #define PTHREAD_MUTEXTYPE_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
    #endif
#endif

#define UNHANDLEDSYSERRALERT(syserrcode, funcname) fprintf(stderr, "Unhandled system error %i in function %s.\n", syserrcode, funcname)

mutexerror_t mutex_init(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        InitializeCriticalSection(mutex);
    #else
        pthread_mutexattr_t attr;
        if (pthread_mutexattr_init(&attr)) return MUTEXERROR_INTRSYSERR;
        if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEXTYPE_RECURSIVE))
        {
            pthread_mutexattr_destroy(&attr);
            return MUTEXERROR_INTRSYSERR;
        }

        int err = pthread_mutex_init(mutex, &attr);

        pthread_mutexattr_destroy(&attr);

        if (err)
        {
            #ifdef LIBMUTEX_DEBUG
                UNHANDLEDSYSERRALERT(err, "mutex_init");
            #endif
            return MUTEXERROR_INTRSYSERR;
        }
    #endif

    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_destroy(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        DeleteCriticalSection(mutex);
    #else
        int err = pthread_mutex_destroy(mutex);
        if (err) switch (err)
        {
            case EBUSY:
                return MUTEXERROR_BUSY;

            default:
                #ifdef LIBMUTEX_DEBUG
                    UNHANDLEDSYSERRALERT(err, "mutex_destroy");
                #endif
                return MUTEXERROR_INTRSYSERR;
        }
    #endif
    
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_lock(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        EnterCriticalSection(mutex);
    #else
        int err = pthread_mutex_lock(mutex);
        if (err) switch (err)
        {
            case EINVAL:
                return MUTEXERROR_INVAL;

            case EDEADLK:
                return MUTEXERROR_DEADLOCK;

            default:
                #ifdef LIBMUTEX_DEBUG
                    UNHANDLEDSYSERRALERT(err, "mutex_lock");
                #endif
                return MUTEXERROR_INTRSYSERR;
        }
    #endif
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_trylock(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        return TryEnterCriticalSection(mutex) ? MUTEXERROR_SUCCESS : MUTEXERROR_BUSY;
    #else
        int err = pthread_mutex_trylock(mutex);
        if (err) switch (err)
        {
            case EINVAL:
                return MUTEXERROR_INVAL;

            case EBUSY:
                return MUTEXERROR_BUSY;

            default:
                #ifdef LIBMUTEX_DEBUG
                    UNHANDLEDSYSERRALERT(err, "mutex_trylock");
                #endif
                return MUTEXERROR_INTRSYSERR;
        }
        return MUTEXERROR_SUCCESS;
    #endif
}

mutexerror_t mutex_unlock(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        LeaveCriticalSection(mutex);
    #else
        int err = pthread_mutex_unlock(mutex);
        if (err) switch (err)
        {
            case EINVAL:
                return MUTEXERROR_INVAL;

            case EPERM:
                return MUTEXERROR_PERMDENIED;

            default:
                #ifdef LIBMUTEX_DEBUG
                    UNHANDLEDSYSERRALERT(err, "mutex_unlock");
                #endif
                return MUTEXERROR_INTRSYSERR;
        }
    #endif
    return MUTEXERROR_SUCCESS;
}