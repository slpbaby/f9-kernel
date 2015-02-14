/* Copyright (c) 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <libposix/pthread.h>
#include <l4/ipc.h>
#include <l4/utcb.h>
#include <l4/pager.h>
#include <l4io.h>
#include <platform/link.h>

int __USER_TEXT pthread_create(pthread_t *restrict thread,
                               const pthread_attr_t *restrict attr,
                               void *(*start_routine)(void*), void *restrict arg)
{
	L4_ThreadId_t tid = pager_create_thread();

	pager_start_thread(tid, start_routine, NULL);

	return *(int *) &tid;
}

__USER_TEXT int pthread_detach(pthread_t thread)
{
	return 0;	
}

__USER_TEXT void pthread_exit(void *value_ptr)
{
	L4_Msg_t msg;
	struct join_thread *join;

	/*
	  Get joined thread id
	  and send value_ptr to them
	*/
	join = pager_get_joined();

	L4_MsgClear(&msg);
	L4_MsgAppendWord(&msg, (L4_Word_t)value_ptr);
	L4_MsgLoad(&msg);
	while (join != NULL) {
		L4_Send(join->join_id);
		/* TODO: free struct join_thread */
		join = join->next;
	}

    pager_stop_thread(value_ptr);

	/* Thread should not reach */
	while (1)
		;
}

__USER_TEXT int pthread_join(pthread_t thread, void **value_ptr)
{
	return 0;
}

__USER_TEXT int pthread_mutex_init(pthread_mutex_t *mutex,
		                   const pthread_mutexattr_t *restrict attr)
{
	return 0;
}

__USER_TEXT int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	return 0;
}

__USER_TEXT int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	/* Busy trying */
	while (pthread_mutex_trylock(mutex));

	return 0;
}

__USER_TEXT int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	register int result = 1;

	__asm__ __volatile__(
	    "mov r1, #1\n"
	    "mov r2, %[mutex]\n"
	    "ldrex r0, [r2]\n"	/* Load value [r2] */
	    "cmp r0, #0\n"	/* Checking is word set to 1 */

	    "itt eq\n"
	    "strexeq r0, r1, [r2]\n"
	    "moveq %[result], r0\n"
	    : [result] "=r"(result)
	    : [mutex] "r"(mutex)
	    : "r0", "r1", "r2");

	return result;
}

__USER_TEXT int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	*mutex = 0;
	return 0;
}

__USER_TEXT int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
                                        const struct timespec *restrict abstime)
{
	return 0;
}
