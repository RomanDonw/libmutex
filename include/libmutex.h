/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

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

#ifdef LIBMUTEX_OS_WINDOWS
    #include <windows.h>

    typedef CRITICAL_SECTION mutex_t;
#else
    #include <pthread.h>

    typedef pthread_mutex_t mutex_t;
#endif

#define LIBMUTEX_ABI

#include <stdbool.h>
#include <stddef.h>

enum mutexerror_e
{
    MUTEXERROR_SUCCESS = 0,

    MUTEXERROR_INVAL,
    MUTEXERROR_DEADLOCK,
    MUTEXERROR_BUSY,
    MUTEXERROR_PERMDENIED,
    MUTEXERROR_INTRSYSERR
} typedef mutexerror_t;

LIBMUTEX_API const char * LIBMUTEX_ABI mutex_strerror(mutexerror_t err);

LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_init(mutex_t *mutex);
LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_destroy(mutex_t *mutex);

LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_lock(mutex_t *mutex);
LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_trylock(mutex_t *mutex);
LIBMUTEX_API mutexerror_t LIBMUTEX_ABI mutex_unlock(mutex_t *mutex);

#ifdef __cplusplus
    }
#endif

#endif