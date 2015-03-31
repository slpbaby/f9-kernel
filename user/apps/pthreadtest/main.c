/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <platform/link.h>
#include <user_runtime.h>
#include <l4/ipc.h>
#include <l4/utcb.h>
#include <l4/thread.h>
#include <libposix/pthread.h>
#include <l4io.h>

#define STACK_SIZE 512

#define RET_VALUE 101

__USER_TEXT
void *alpha_thread(void *arg)
{
	unsigned int i;

	printf("pthread_create: alpha pthread start\n");

	i = RET_VALUE;
	L4_Sleep(L4_TimePeriod(100 * 1000));

	printf("pthread_exit: alpha pthread exit\n");
	pthread_exit(&i);

	return 0;
}

__USER_TEXT
static void *main(void *user)
{
	pthread_t alpha;
	int ret;
	void *ret_ptr;

	ret = 0;
	ret_ptr = (void *)&ret;

	printf("\nF9 pthread test suite start\n");

	pthread_create(&alpha, NULL, alpha_thread, NULL);

	if (pthread_join(alpha, &ret_ptr) != 0) {
		printf("pthread_join: FAILED\n");
		return 0;
	}

	if (ret == RET_VALUE) 
		printf("pthread_join: OK\n");
	else
		printf("pthread_join: FAILED\n");

	return 0;
}

DECLARE_USER(
	0,
	pthreadtest,
	main,
	DECLARE_FPAGE(0x0, 4 * UTCB_SIZE + 4 * STACK_SIZE)
	DECLARE_FPAGE(0x0, 512)
);
