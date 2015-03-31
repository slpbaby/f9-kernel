/* Copyright (c) 2015 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <libposix/sys/types.h>

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_trywait(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
#endif