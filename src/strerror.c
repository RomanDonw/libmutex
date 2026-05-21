#include "libmutex.h"

#include <stddef.h>

const char *mutex_strerror(mutexerror_t err)
{
    switch (err)
    {
        case MUTEXERROR_SUCCESS:
            return "success";

        case MUTEXERROR_NOMEM:
            return "out of memory";
        
        case MUTEXERROR_BUSY:
            return "mutex busy";

        case MUTEXERROR_DEADLOCK:
            return "mutex deadlock";

        case MUTEXERROR_INCORRINIT:
            return "mutex incorrected or not initialized";

        case MUTEXERROR_PERMDENIED:
            return "permission denied";

        case MUTEXERROR_INTRSYSERR:
            return "internal system error";

        default:
            return NULL;
    }
}