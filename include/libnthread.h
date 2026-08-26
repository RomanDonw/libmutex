/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef LIBNTHREAD_H
#define LIBNTHREAD_H

#ifdef __cplusplus
    extern "C"
    {
#endif

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
    #define LIBNTHREAD_OS_WINDOWS

    #ifdef LIBNTHREAD_STATIC
        #ifdef _MSC_VER
            #define LIBNTHREAD_API
        #else
            #define LIBNTHREAD_API __attribute__((visibility("default")))
        #endif
    #else
        #ifdef _MSC_VER
            #ifdef LIBNTHREAD_EXPORT
                #define LIBNTHREAD_API __declspec(dllexport)
            #else
                #define LIBNTHREAD_API __declspec(dllimport)
            #endif
        #else
            #define LIBNTHREAD_API __attribute__((visibility("default")))
        #endif
    #endif

    #include <windows.h>
    #include <process.h>

    typedef uintptr_t NTHREAD_THREADDESCRIPTOR;
    typedef CRITICAL_SECTION NTHREAD_MUTEXDESCRIPTOR;
#else
    #define LIBNTHREAD_API __attribute__((visibility("default")))

    #include <pthread.h>

    typedef pthread_t NTHREAD_THREADDESCRIPTOR;
    typedef pthread_mutex_t NTHREAD_MUTEXDESCRIPTOR;
#endif

#include <libncore.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
    #define LIBNTHREAD_USEC11ATOMICS
    #include <stdatomic.h>
#elif defined(_MSC_VER)
    #define LIBNTHREAD_USEMSVCATOMICS
    #include <intrin.h>

    #if defined(_M_ARM) || defined(_M_ARM64)
        #define LIBNTHREAD_USEDMSVCONARM

        #if defined(_ARM64_BARRIER_ISH)
            #define LIBNTHREAD_ARMMEMORYBARRIER _ARM64_BARRIER_ISH
        #elif defined(_ARM_BARRIER_ISH)
            #define LIBNTHREAD_ARMMEMORYBARRIER _ARM_BARRIER_ISH
        #endif
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define LIBNTHREAD_USEGCCORCLANGATOMICS
#else
    #error This compiler doesn't support atomic operations, or compiler atomic operations doesn't supported by this library.
#endif

#define LIBNTHREAD_ABI

/*
    #########################
             General
    #########################
*/

struct LibNThreadStartupOptions
{
    const NMemoryAllocators *allocators; // can be NULL.
    NPanicHandler *panichandler; // can be NULL.
    NAlertHandler *alerthandler; // can be NULL.
} typedef LibNThreadStartupOptions;

#define LIBNTHREADSTARTUPOPTIONS_DEFAULTINIT (LibNThreadStartupOptions){0}

extern const char *LIBNTHREAD_MODULENAME;

LIBNTHREAD_API bool LIBNTHREAD_ABI libnthread_initialized(void); // can be accessed without library initialization.
LIBNTHREAD_API NError LIBNTHREAD_ABI libnthread_startup(const LibNThreadStartupOptions *options);
LIBNTHREAD_API NError LIBNTHREAD_ABI libnthread_cleanup(void);

/*
    #########################
             Threads
    #########################
*/

typedef struct NThread NThread;
typedef uint8_t NThreadReturnType;
typedef NThreadReturnType (*NThreadHandler)(void *);

LIBNTHREAD_API NError LIBNTHREAD_ABI nthread_create(NThread **thread, NThreadHandler handler, void *userdata); // userdata can be NULL.
LIBNTHREAD_API NError LIBNTHREAD_ABI nthread_join(NThread *thread, NThreadReturnType *exitcode); // exitcode can be NULL.
//LIBNTHREAD_API void LIBNTHREAD_ABI nthread_exit(NThreadReturnType exitcode);

/*
    #########################
             Mutexes
    #########################
*/

typedef struct NThreadMutex NThreadMutex;

LIBNTHREAD_API NError LIBNTHREAD_ABI nthread_mutex_create(NThreadMutex **mutex);
LIBNTHREAD_API NError LIBNTHREAD_ABI nthread_mutex_destroy(NThreadMutex *mutex);

LIBNTHREAD_API NError LIBNTHREAD_ABI nthread_mutex_lock(NThreadMutex *mutex);
LIBNTHREAD_API NError LIBNTHREAD_ABI nthread_mutex_trylock(NThreadMutex *mutex);
LIBNTHREAD_API NError LIBNTHREAD_ABI nthread_mutex_unlock(NThreadMutex *mutex);

/*
    #########################
            Unsafe API
    #########################
*/

#if defined(LIBNTHREAD_ALLOWUNSAFEACCESS) || defined(LIBNTHREAD_EXPORT)
    LIBNTHREAD_API NTHREAD_THREADDESCRIPTOR LIBNTHREAD_ABI nthread_gethandle(const NThread *thread);
    LIBNTHREAD_API NTHREAD_MUTEXDESCRIPTOR LIBNTHREAD_ABI nthread_mutex_gethandle(const NThreadMutex *mutex);
#endif

/*
    #########################
             Atomics
    #########################
*/

/*
    ..._cmpxhg function postfixes info:
        p - pointer (*var, *expected, desired).    if *var changed returns 'true', otherwise returns 'false' and stores *var to *expected.
        t - try (*var, expected, desired).         if *var changed returns 'true', otherwise returns 'false'.
        v - (old) value (*var, expected, desired). returns *var before performing operation.
*/

#include "libnthread/atomicimpl/bool.h"

#ifdef __cplusplus
    }
#endif

#endif