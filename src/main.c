#include "libmutex.h"

#include <stdio.h>

#ifdef LIBMUTEX_OS_WINDOWS
    #include <windows.h>

    #define MUTEXDESC CRITICAL_SECTION
#else
    #include <pthread.h>
    #include <errno.h>

    #define MUTEXDESC pthread_mutex_t
#endif

struct mutex_s { MUTEXDESC desc; };

#define UNHANDLEDSYSERRALERT(syserrcode, funcname) fprintf(stderr, "Unhandled system error %i in function %s.\n", syserrcode, funcname)

mutexerror_t mutex_init(mutex_t **mutex)
{
    mutex_t *ret = libmutex_malloc(sizeof(mutex_t));
    if (!ret) return MUTEXERROR_NOMEM;

    #ifdef LIBMUTEX_OS_WINDOWS
        InitializeCriticalSection(&ret->desc);
    #else
        pthread_mutex_init(&ret->desc, NULL);
    #endif

    *mutex = ret;
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_destroy(mutex_t *mutex)
{
    #ifdef LIBMUTEX_OS_WINDOWS
        DeleteCriticalSection(mutex->desc);
    #else
        {
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
        }
    #endif
    
    libmutex_free(mutex);
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_lock(mutex_t *mutex)
{
    #ifdef LIBMUTEX_OS_WINDOWS
        EnterCriticalSection(mutex->desc);
    #else
        {
            int err = pthread_mutex_lock(&mutex->desc);
            if (err) switch (err)
            {
                case EINVAL:
                    return MUTEXERROR_INCORRINIT;

                case EDEADLK:
                    return MUTEXERROR_DEADLOCK;

                default:
                    #ifdef LIBMUTEX_DEBUG
                        UNHANDLEDSYSERRALERT(err, "mutex_lock");
                    #endif
                    return MUTEXERROR_INTRSYSERR;
            }
        }
    #endif
    return MUTEXERROR_SUCCESS;
}

mutexerror_t mutex_unlock(mutex_t *mutex)
{
    #ifdef LIBMUTEX_OS_WINDOWS
        LeaveCriticalSection(mutex->desc);
    #else
        {
            int err = pthread_mutex_unlock(&mutex->desc);
            if (err) switch (err)
            {
                case EINVAL:
                    return MUTEXERROR_INCORRINIT;

                case EPERM:
                    return MUTEXERROR_PERMDENIED;

                default:
                    #ifdef LIBMUTEX_DEBUG
                        UNHANDLEDSYSERRALERT(err, "mutex_unlock");
                    #endif
                    return MUTEXERROR_INTRSYSERR;
            }
        }
    #endif
    return MUTEXERROR_SUCCESS;
}