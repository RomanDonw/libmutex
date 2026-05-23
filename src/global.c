/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "libmutex.h"

#include <stdlib.h>

void *(*libmutex_malloc)(size_t) = malloc;
void (*libmutex_free)(void *) = free;