/* Copyright (c) 2015 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <l4/ipc.h>
#include <l4/utcb.h>
#include <l4/pager.h>
#include <l4io.h>
#include <platform/link.h>
#include <libposix/semaphore.h>

__USER_TEXT int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    if (sem == NULL)
        return -1;

    sem->refcount = 1;
    sem->value = value;

    return 0;
}

__USER_TEXT int sem_trywait(sem_t *sem)
{
    register int result = 1;

    if (sem == NULL)
        return -1;

    __asm__ __volatile__(
        "mov r1, #1\n"
        "mov r2, %[value]\n"
        "ldrex r0, [r2]\n"
        "cmp r0, #0\n"
        "itee eq\n"
        "moveq %[result], #1\n"
        "subne r0, r1, r0\n"
        "movne %[result], #0\n"
        "mov r1, #0\n"
        "strex r0, r1, [r2]\n"
        : [result] "=r"(result)
        : [value] "r"(&sem->value)
        : "r0", "r1", "r2");

    return result;
}

__USER_TEXT int sem_wait(sem_t *sem)
{
    if (sem == NULL)
        return -1;

    while (sem_trywait(sem))
        L4_Sleep(L4_TimePeriod(1000 * 100));

    return 0;
}

__USER_TEXT int sem_post(sem_t *sem)
{

}
