/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "libmutex.h"

#ifdef LIBMUTEX_OS_WINDOWS
    #include <windows.h>

    #define MUTEXDESC CRITICAL_SECTION
#else
    #include <pthread.h>
    #include <errno.h>

    #define MUTEXDESC pthread_mutex_t

    #if defined(__USE_UNIX98) || defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500
        #define PTHREAD_MUTEXTYPE_RECURSIVE PTHREAD_MUTEX_RECURSIVE
    #else
        #define PTHREAD_MUTEXTYPE_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
    #endif
#endif

#ifdef LIBMUTEX_DEBUG
    #include <stdio.h>

    #define UNHANDLEDSYSERRALERT(syserrcode, funcname) fprintf(stderr, "Unhandled system error %i in function %s.\n", syserrcode, funcname)
#else
    #define UNHANDLEDSYSERRALERT(syserrcode, funcname)
#endif

struct mutex_s { MUTEXDESC desc; };

mutexerror_t mutex_create(mutex_t **mutex)
{
    if (!mutex) return MUTEXERROR_INVAL;

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
                    UNHANDLEDSYSERRALERT(err, "mutex_create");
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