/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <platform/link.h>
#include <user_runtime.h>
#include <l4/ipc.h>
#include <l4/utcb.h>
#include <l4io.h>

#define STACK_SIZE 256

enum { PING_THREAD, PONG_THREAD, COUNT_THREAD };

static L4_ThreadId_t threads[3] __USER_DATA;

static L4_Word_t last_thread __USER_DATA;
static L4_Word_t free_mem __USER_DATA;

static uint32_t counting __USER_DATA;

#define TEST_SIZE 15
#define TEST_NUM 1000

void __USER_TEXT ping_thread(void)
{
	L4_Msg_t msg;
	uint32_t i;
	L4_Word_t word[TEST_SIZE];
	for (i = 0; i < TEST_SIZE; i++)
		word[i] = i;
	L4_MsgClear(&msg);
	for (i = 0; i < TEST_SIZE; i++)
		L4_MsgAppendWord(&msg, word[i]);
	L4_MsgLoad(&msg);
	
	printf("Start: counting = %d\n", counting);
	for (i = 0; i < TEST_NUM; i++)
		L4_Send(threads[PONG_THREAD]);
	while (1)
		L4_Sleep(L4_Never);
}

void __USER_TEXT pong_thread(void)
{
	L4_MsgTag_t msgtag;
	L4_Msg_t msg;
	uint32_t i;
	for (i = 0; i < TEST_NUM; i++) {
	//while (1) {
		msgtag = L4_Receive(threads[PING_THREAD]);
		L4_MsgStore(msgtag, &msg);
	}
	printf("End: counting = %d\n", counting);

	while (1)
		L4_Sleep(L4_Never);
}

void __USER_TEXT count_thread(void)
{
	while (1) {
		counting++;
	}
}

static void __USER_TEXT start_thread(L4_ThreadId_t t, L4_Word_t ip,
                                     L4_Word_t sp, L4_Word_t stack_size)
{
	L4_Msg_t msg;

	L4_MsgClear(&msg);
	L4_MsgAppendWord(&msg, ip);
	L4_MsgAppendWord(&msg, sp);
	L4_MsgAppendWord(&msg, stack_size);
	L4_MsgLoad(&msg);

	L4_Send(t);
}

static L4_ThreadId_t __USER_TEXT create_thread(user_struct *user, void (*func)(void))
{
	L4_ThreadId_t myself = L4_MyGlobalId();
	L4_ThreadId_t child;

	child.raw = myself.raw + (++last_thread << 14);

	L4_ThreadControl(child, myself, L4_nilthread, myself, (void *) free_mem);
	free_mem += UTCB_SIZE + STACK_SIZE;

	start_thread(child, (L4_Word_t)func, free_mem, STACK_SIZE);

	return child;
}

static void __USER_TEXT main(user_struct *user)
{
	free_mem = user->fpages[0].base;
	counting = 0;
	threads[COUNT_THREAD] = create_thread(user, count_thread);
	threads[PING_THREAD] = create_thread(user, ping_thread);
	threads[PONG_THREAD] = create_thread(user, pong_thread);
}

DECLARE_USER(
	0,
	pingpong,
	main,
	DECLARE_FPAGE(0x0, 3 * UTCB_SIZE + 3 * STACK_SIZE)
);
