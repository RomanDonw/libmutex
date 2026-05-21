#ifndef LIBMUTEX_H
#define LIBMUTEX_H

#ifdef __cplusplus
    extern "C"
    {
#endif

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
    #define LIBMUTEX_OS_WINDOWS
#endif

#ifdef LIBMUTEX_OS_WINDOWS
    #ifdef LIBMUTEX_STATIC
        #ifdef _MSC_VER
            #define LIBMUTEX_API
        #else
            #define LIBMUTEX_API __attribute__((visibility("default")))
        #endif
    #else
        #ifdef _MSC_VER
            #ifdef LIBMUTEX_EXPORT
                #define LIBMUTEX_API __declspec(dllexport)
            #else
                #define LIBMUTEX_API __declspec(dllimport)
            #endif
        #else
            #define LIBMUTEX_API __attribute__((visibility("default")))
        #endif
    #endif
#else
    #define LIBMUTEX_API __attribute__((visibility("default")))
#endif

#define LIBMUTEX_ABI

#include <stdbool.h>
#include <stddef.h>

enum mutexerror_e
{
    MUTEXERROR_SUCCESS = 0,

    MUTEXERROR_NOMEM,
    MUTEXERROR_INCORRINIT,
    MUTEXERROR_DEADLOCK,
    MUTEXERROR_BUSY,
    MUTEXERROR_PERMDENIED,
    MUTEXERROR_INTRSYSERR
} typedef mutexerror_t;

typedef struct mutex_s mutex_t;

LIBMUTEX_API extern void *(*libmutex_malloc)(size_t);
//LIBMUTEX_API extern void *(*libmutex_realloc)(void *, size_t);
LIBMUTEX_API extern void (*libmutex_free)(void *);

LIBMUTEX_API const char * LIBMUTEX_ABI mutex_strerror(mutexerror_t err);

LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_init(mutex_t **mutex);
LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_destroy(mutex_t *mutex);

LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_lock(mutex_t *mutex);
LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_unlock(mutex_t *mutex);

#ifdef __cplusplus
    }
#endif

#endif