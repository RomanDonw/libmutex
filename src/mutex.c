/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "libnthread.h"

#include "init.h"
#include "util.h"
#include "err.h"

struct NThreadMutex { NTHREAD_MUTEXDESCRIPTOR desc; };

NError nthread_mutex_create(NThreadMutex **mutex)
{
    ENSURE_INIT;

    NThreadMutex *ret = allocs.malloc(sizeof(NThreadMutex));
    if (!ret) return NError_MemoryAllocationFailed;

    #ifdef LIBNTHREAD_OS_WINDOWS
        InitializeCriticalSection(&ret->desc);
    #else
        NError nerr = translateerror(pthread_mutex_init(&ret->desc, &recursivemutexattr));
        if (nerr != NError_Success) { allocs.free(ret); return nerr; }
    #endif

    *mutex = ret;
    return NError_Success;
}

#if 0

NError mutex_create(NThreadMutex **mutex)
{
    mutex_t *ret = libmutex_malloc(sizeof(mutex_t));
    if (!ret) return MUTEXERROR_NOMEM;

    #ifdef LIBMUTEX_OS_WINDOWS
        InitializeCriticalSection(&ret->desc);
    #else
        pthread_mutexattr_t attr;
        if (pthread_mutexattr_init(&attr)) { libmutex_free(ret); return MUTEXERROR_INTRSYSERR; }
        if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEXTYPE_RECURSIVE))
        {
            pthread_mutexattr_destroy(&attr);
            libmutex_free(ret);
            return MUTEXERROR_INTRSYSERR;
        }

        int err = pthread_mutex_init(&ret->desc, &attr);

        pthread_mutexattr_destroy(&attr);

        if (err)
        {
            libmutex_free(ret);

            switch (err)
            {
                case ENOMEM:
                    return MUTEXERROR_NOMEM;

                default:
                    
                    return MUTEXERROR_INTRSYSERR;
            }
        }
    #endif

    *mutex = ret;
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_destroy(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        DeleteCriticalSection(&mutex->desc);
    #else
        int err = pthread_mutex_destroy(&mutex->desc);
        if (err) switch (err)
        {
            case EBUSY:
                return MUTEXERROR_BUSY;

            default:
                UNHANDLEDSYSERRALERT(err, "mutex_destroy");
                return MUTEXERROR_INTRSYSERR;
        }
    #endif
    
    libmutex_free(mutex);
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_lock(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        EnterCriticalSection(&mutex->desc);
    #else
        int err = pthread_mutex_lock(&mutex->desc);
        if (err) switch (err)
        {
            case EINVAL:
                return MUTEXERROR_INVAL;

            case EDEADLK:
                return MUTEXERROR_DEADLOCK;

            default:
                UNHANDLEDSYSERRALERT(err, "mutex_lock");
                return MUTEXERROR_INTRSYSERR;
        }
    #endif
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_trylock(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        return TryEnterCriticalSection(&mutex->desc) ? MUTEXERROR_SUCCESS : MUTEXERROR_BUSY;
    #else
        int err = pthread_mutex_trylock(&mutex->desc);
        if (err) switch (err)
        {
            case EINVAL:
                return MUTEXERROR_INVAL;

            case EBUSY:
                return MUTEXERROR_BUSY;

            default:
                UNHANDLEDSYSERRALERT(err, "mutex_trylock");
                return MUTEXERROR_INTRSYSERR;
        }
        return MUTEXERROR_SUCCESS;
    #endif
}

mutexerror_t mutex_unlock(mutex_t *mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

    #ifdef LIBMUTEX_OS_WINDOWS
        LeaveCriticalSection(&mutex->desc);
    #else
        int err = pthread_mutex_unlock(&mutex->desc);
        if (err) switch (err)
        {
            case EINVAL:
                return MUTEXERROR_INVAL;

            case EPERM:
                return MUTEXERROR_PERMDENIED;

            default:
                UNHANDLEDSYSERRALERT(err, "mutex_unlock");
                return MUTEXERROR_INTRSYSERR;
        }
    #endif
    return MUTEXERROR_SUCCESS;
}

#endif