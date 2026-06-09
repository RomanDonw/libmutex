#ifndef UTIL_H
#define UTIL_H

#ifdef LIBMUTEX_DEBUG
    void __libmutex_logdbgerr(const char *msgformat, ...);
    #define LOGDBGERR(msgformat, ...) (__libmutex_logdbgerr(msgformat, __VA_ARGS__))
#else
    #define LOGDBGERR(msgformat, ...)
#endif

void __libmutex_fault(const char *msgformat, ...);
#define fault(...) (__libmutex_fault(__VA_ARGS__))

#endif