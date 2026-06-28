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
#endif

#if defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))
    #define LIBNTHREAD_USEDMSVCONARM
#endif

#ifdef LIBNTHREAD_OS_WINDOWS
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

    typedef CRITICAL_SECTION NTHREAD_MUTEXDESCRIPTOR;
#else
    #define LIBNTHREAD_API __attribute__((visibility("default")))

    #include <pthread.h>

    typedef pthread_mutex_t NTHREAD_MUTEXDESCRIPTOR;
#endif

#define LIBNTHREAD_ABI

#include <stdbool.h>
#include <stddef.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
    #define LIBNTHREAD_USEC11ATOMICS

    #include <stdatomic.h>
#elif defined(_MSC_VER)
    #define LIBNTHREAD_USEMSVCATOMICS

    #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
    #define LIBNTHREAD_USEGCCORCLANGATOMICS
#else
    #error This compiler doesn't support atomic operations, or compiler atomic operations doesn't supported by this library.
#endif

// general:

enum NThreadError
{
    NThreadError_Success = 0,

    NThreadError_InternalUnknownError,
    NThreadError_NotInitialized,
    NThreadError_AlreadyInitialized,

    NThreadError_OperationInProgress

    /*
    NTHREADERROR_SUCCESS = 0,

    NTHREADERROR_NOMEM,
    NTHREADERROR_INVAL,
    NTHREADERROR_DEADLOCK,
    NTHREADERROR_BUSY,
    NTHREADERROR_PERMDENIED,
    NTHREADERROR_INTRSYSERR
    */
} typedef NThreadError;

struct LibNThreadAllocators
{
    void *(*malloc)(size_t);
    void *(*realloc)(void *, size_t); // must be safe for NULL.
    void (*free)(void *); // must be safe for NULL.
} typedef LibNThreadAllocators;

struct LibNThreadStartupOptions
{
    const LibNThreadAllocators *allocators;
} typedef LibNThreadStartupOptions;

#define LIBNTHREADSTARTUPOPTIONS_DEFAULTINIT (LibNThreadStartupOptions){0}

LIBNTHREAD_API const unsigned char * LIBNTHREAD_ABI nthread_strerror(NThreadError err); // can be accessed without library initialization.

LIBNTHREAD_API bool LIBNTHREAD_ABI libnthread_initialized(void); // can be accessed without library initialization.
LIBNTHREAD_API NThreadError LIBNTHREAD_ABI libnthread_startup(const LibNThreadStartupOptions *options);
LIBNTHREAD_API NThreadError LIBNTHREAD_ABI libnthread_cleanup(void);

// threads:

typedef struct NThread NThread;

// mutexes:

typedef struct NThreadMutex NThreadMutex;

LIBNTHREAD_API NThreadError LIBNTHREAD_ABI nthread_mutex_create(NThreadMutex **mutex);
LIBNTHREAD_API NThreadError LIBNTHREAD_ABI nthread_mutex_destroy(NThreadMutex *mutex);

LIBNTHREAD_API NThreadError LIBNTHREAD_ABI nthread_mutex_lock(NThreadMutex *mutex);
LIBNTHREAD_API NThreadError LIBNTHREAD_ABI nthread_mutex_trylock(NThreadMutex *mutex);
LIBNTHREAD_API NThreadError LIBNTHREAD_ABI nthread_mutex_unlock(NThreadMutex *mutex);

// atomics:

/*
    ..._cmpxhg function postfixes info:
        p - pointer (*var, *expected, desired).    if *var changed returns 'true', otherwise returns 'false' and stores *var to *expected.
        t - try (*var, expected, desired).         if *var changed returns 'true', otherwise returns 'false'.
        v - (old) value (*var, expected, desired). returns *var before performing operation.
*/

#if defined(LIBNTHREAD_USEC11ATOMICS)

    typedef atomic_bool NThreadAtomicBool;
    #define NTHREAD_ATOMICBOOLINIT(value) (ATOMIC_VAR_INIT((bool)(value)))

    static inline bool nthread_atomicbool_load(NThreadAtomicBool *variable) { return atomic_load(variable); }
    static inline void nthread_atomicbool_store(NThreadAtomicBool *variable, bool desired) { atomic_store(variable, desired); }

    static inline bool nthread_atomicbool_cmpxchgv(NThreadAtomicBool *variable, bool expected, bool desired)
    { bool exponstackcopy = expected; atomic_compare_exchange_strong(variable, &exponstackcopy, desired); return exponstackcopy; }

    static inline bool nthread_atomicbool_cmpxchgt(NThreadAtomicBool *variable, bool expected, bool desired)
    { bool exponstackcopy = expected; return atomic_compare_exchange_strong(variable, &exponstackcopy, desired); }

    static inline bool nthread_atomicbool_cmpxchgp(NThreadAtomicBool *variable, bool *expected, bool desired)
    { return atomic_compare_exchange_strong(variable, expected, desired); }

#elif defined(LIBNTHREAD_USEMSVCATOMICS)

    typedef volatile unsigned char NThreadAtomicBool;
    #define NTHREAD_ATOMICBOOLINIT(value) ((bool)(value))

    static inline bool nthread_atomicbool_load(NThreadAtomicBool *variable)
    {
        #ifdef LIBNTHREAD_USEDMSVCONARM
            return _InterlockedExchangeAdd8(variable, 0);
        #else
            register bool ret = *variable;
            _ReadWriteBarrier();
            return ret;
        #endif
    }
    static inline void nthread_atomicbool_store(NThreadAtomicBool *variable, bool desired) { _InterlockedExchange8(variable, desired); }

    static inline bool nthread_atomicbool_cmpxchgv(NThreadAtomicBool *variable, bool expected, bool desired)
    { return _InterlockedCompareExchange8(variable, (unsigned char)desired, (unsigned char)expected); }

    static inline bool nthread_atomicbool_cmpxchgt(NThreadAtomicBool *variable, bool expected, bool desired)
    { return nthread_atomicbool_cmpxchgv(variable, expected, desired) == expected; }

    static inline bool nthread_atomicbool_cmpxchgp(NThreadAtomicBool *variable, bool *expected, bool desired)
    {
        register bool expval = *expected;
        register bool oldvarval = nthread_atomicbool_cmpxchgv(variable, expval, desired);
        if (oldvarval == expval) return true;
        *expected = oldvarval;
        return false;
    }

#elif defined(LIBNTHREAD_USEGCCORCLANGATOMICS)

    typedef volatile bool NThreadAtomicBool;
    #define NTHREAD_ATOMICBOOLINIT(value) ((bool)(value))

    static inline bool nthread_atomicbool_load(NThreadAtomicBool *variable) { return __atomic_load_n(variable, __ATOMIC_SEQ_CST); }
    static inline void nthread_atomicbool_store(NThreadAtomicBool *variable, bool desired) { __atomic_store_n(variable, desired, __ATOMIC_SEQ_CST); }

    static inline bool nthread_atomicbool_cmpxchgv(NThreadAtomicBool *variable, bool expected, bool desired)
    {
        bool exponstackcopy = expected;
        __atomic_compare_exchange_n(variable, &exponstackcopy, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return exponstackcopy;
    }

    static inline bool nthread_atomicbool_cmpxchgt(NThreadAtomicBool *variable, bool expected, bool desired)
    {
        bool exponstackcopy = expected;
        return __atomic_compare_exchange_n(variable, &exponstackcopy, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    }

    static inline bool nthread_atomicbool_cmpxchgp(NThreadAtomicBool *variable, bool *expected, bool desired)
    { return __atomic_compare_exchange_n(variable, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); }

#endif

// unsafe API:

#if defined(LIBNTHREAD_ALLOWUNSAFEACCESS) || defined(LIBNTHREAD_EXPORT)
    LIBNTHREAD_API NTHREAD_MUTEXDESCRIPTOR LIBNTHREAD_ABI nthread_mutex_gethandle(const NThreadMutex *mutex);
#endif

#ifdef __cplusplus
    }
#endif

#endif