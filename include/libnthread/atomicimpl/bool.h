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

    typedef volatile char NThreadAtomicBool;
    #define NTHREAD_ATOMICBOOLINIT(value) ((bool)(value))

    static inline bool nthread_atomicbool_load(NThreadAtomicBool *variable)
    {
        #ifdef LIBNTHREAD_USEDMSVCONARM
            bool ret = __iso_volatile_load8(variable);
            __dmb(LIBNTHREAD_ARMMEMORYBARRIER);
        #else
            bool ret = *(volatile bool *)variable;
            _ReadWriteBarrier();
        #endif
        return ret;
    }
    static inline void nthread_atomicbool_store(NThreadAtomicBool *variable, bool desired)
    {
        #ifdef LIBNTHREAD_USEDMSVCONARM
            __dmb(LIBNTHREAD_ARMMEMORYBARRIER);
            __iso_volatile_store8(variable, desired);
        #else
            _ReadWriteBarrier();
            *variable = desired;
        #endif
    }

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

    typedef bool NThreadAtomicBool;
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