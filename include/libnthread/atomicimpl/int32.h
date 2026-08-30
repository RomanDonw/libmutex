#if defined(LIBNTHREAD_USEC11ATOMICS)

    typedef _Atomic int32_t NThreadAtomicInt32;
    #define NTHREAD_ATOMICINT32INIT(value) (ATOMIC_VAR_INIT((int32_t)(value)))

    static inline int32_t nthread_atomicint32_load(NThreadAtomicInt32 *variable) { return atomic_load(variable); }
    static inline void nthread_atomicint32_store(NThreadAtomicInt32 *variable, int32_t desired) { atomic_store(variable, desired); }

    static inline int32_t nthread_atomicint32_cmpxchgv(NThreadAtomicInt32 *variable, int32_t expected, int32_t desired)
    { int32_t exponstackcopy = expected; atomic_compare_exchange_strong(variable, &exponstackcopy, desired); return exponstackcopy; }

    static inline int32_t nthread_atomicint32_cmpxchgt(NThreadAtomicInt32 *variable, int32_t expected, int32_t desired)
    { int32_t exponstackcopy = expected; return atomic_compare_exchange_strong(variable, &exponstackcopy, desired); }

    static inline int32_t nthread_atomicint32_cmpxchgp(NThreadAtomicInt32 *variable, int32_t *expected, int32_t desired)
    { return atomic_compare_exchange_strong(variable, expected, desired); }

#elif defined(LIBNTHREAD_USEMSVCATOMICS)

    typedef volatile int32_t NThreadAtomicInt32;
    #define NTHREAD_ATOMICINT32INIT(value) ((int32_t)(value))

    static inline int32_t nthread_atomicint32_load(NThreadAtomicInt32 *variable)
    {
        #ifdef LIBNTHREAD_USEDMSVCONARM
            int32_t ret = __iso_volatile_load32(variable);
            __dmb(LIBNTHREAD_ARMMEMORYBARRIER);
        #else
            int32_t ret = *(volatile int32_t *)variable;
            _ReadWriteBarrier();
        #endif
        return ret;
    }
    static inline void nthread_atomicint32_store(NThreadAtomicInt32 *variable, int32_t desired)
    {
        #ifdef LIBNTHREAD_USEDMSVCONARM
            __dmb(LIBNTHREAD_ARMMEMORYBARRIER);
            __iso_volatile_store32(variable, desired);
        #else
            _ReadWriteBarrier();
            *variable = desired;
        #endif
    }

    static inline int32_t nthread_atomicint32_cmpxchgv(NThreadAtomicInt32 *variable, int32_t expected, int32_t desired)
    { return _InterlockedCompareExchange(variable, desired, expected); }

    static inline int32_t nthread_atomicint32_cmpxchgt(NThreadAtomicInt32 *variable, int32_t expected, int32_t desired)
    { return nthread_atomicint32_cmpxchgv(variable, expected, desired) == expected; }

    static inline int32_t nthread_atomicint32_cmpxchgp(NThreadAtomicInt32 *variable, int32_t *expected, int32_t desired)
    {
        register int32_t expval = *expected;
        register int32_t oldvarval = nthread_atomicint32_cmpxchgv(variable, expval, desired);
        if (oldvarval == expval) return true;
        *expected = oldvarval;
        return false;
    }

#elif defined(LIBNTHREAD_USEGCCORCLANGATOMICS)

    typedef int32_t NThreadAtomicInt32;
    #define NTHREAD_ATOMICINT32INIT(value) ((int32_t)(value))

    static inline int32_t nthread_atomicint32_load(NThreadAtomicInt32 *variable) { return __atomic_load_n(variable, __ATOMIC_SEQ_CST); }
    static inline void nthread_atomicint32_store(NThreadAtomicInt32 *variable, int32_t desired) { __atomic_store_n(variable, desired, __ATOMIC_SEQ_CST); }

    static inline int32_t nthread_atomicint32_cmpxchgv(NThreadAtomicBool *variable, int32_t expected, int32_t desired)
    {
        int32_t exponstackcopy = expected;
        __atomic_compare_exchange_n(variable, &exponstackcopy, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
        return exponstackcopy;
    }

    static inline int32_t nthread_atomicint32_cmpxchgt(NThreadAtomicBool *variable, int32_t expected, int32_t desired)
    {
        int32_t exponstackcopy = expected;
        return __atomic_compare_exchange_n(variable, &exponstackcopy, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    }

    static inline int32_t nthread_atomicint32_cmpxchgp(NThreadAtomicBool *variable, int32_t *expected, int32_t desired)
    { return __atomic_compare_exchange_n(variable, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); }

#endif