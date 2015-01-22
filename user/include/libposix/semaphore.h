/* Copyright (c) 2015 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <libposix/sys/types.h>

struct posix_sem_t {
    uint16_t refcount;
    uint16_t value;
};
typedef struct posix_sem_t sem_t;
#endif
