/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "libmutex.h"

#include <stddef.h>

const char *mutex_strerror(mutexerror_t err)
{
    switch (err)
    {
        case MUTEXERROR_SUCCESS:
            return "success";
        
        case MUTEXERROR_BUSY:
            return "mutex busy";

        case MUTEXERROR_DEADLOCK:
            return "mutex deadlock";

        case MUTEXERROR_INVAL:
            return "incorrect argument value";

        case MUTEXERROR_PERMDENIED:
            return "permission denied";

        case MUTEXERROR_INTRSYSERR:
            return "internal system error";

        default:
            return NULL;
    }
}