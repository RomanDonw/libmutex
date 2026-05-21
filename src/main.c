#include "libmutex.h"

#include <stdio.h>

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

struct mutex_s { MUTEXDESC desc; };

#define UNHANDLEDSYSERRALERT(syserrcode, funcname) fprintf(stderr, "Unhandled system error %i in function %s.\n", syserrcode, funcname)

mutexerror_t mutex_init(mutex_t **mutex)
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
                    #ifdef LIBMUTEX_DEBUG
                        UNHANDLEDSYSERRALERT(err, "mutex_init");
                    #endif
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
                #ifdef LIBMUTEX_DEBUG
                    UNHANDLEDSYSERRALERT(err, "mutex_destroy");
                #endif
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
                #ifdef LIBMUTEX_DEBUG
                    UNHANDLEDSYSERRALERT(err, "mutex_unlock");
                #endif
                return MUTEXERROR_INTRSYSERR;
        }
    #endif
    return MUTEXERROR_SUCCESS;
}