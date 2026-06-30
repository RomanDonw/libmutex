/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef INIT_H
#define INIT_H

#include "libnthread.h"

#define ENSURE_INIT { if (!libnthread_initialized()) return NError_NotInitialized; }

#endif