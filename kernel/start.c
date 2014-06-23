/* Copyright (c) 2013, 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * __l4_start initializes microcontroller
 */

#include INC_PLAT(gpio.h)
#include INC_PLAT(rcc.h)

#include <platform/debug_device.h>
#include <platform/irq.h>
#include <error.h>
#include <types.h>
#include <debug.h>
#include <kdb.h>
#include <ipc.h>
#include <ktimer.h>
#include <softirq.h>
#include <syscall.h>
#include <systhread.h>
#include <kprobes.h>
#include <ksym.h>
#include <init_hook.h>
#include <lib/stdio.h>

static char banner[] =
	"\n"
	"====================================================\n"
	" Copyright(C) 2013-2014 The F9 Microkernel Project  \n"
	"====================================================\n"
	"Git head: " GIT_HEAD "\n"
	"Host: " MACH_TYPE "\n"
	"Build: "  BUILD_TIME "\n"
	"\n";

int main(void)
{
	run_init_hook(INIT_LEVEL_PLATFORM_EARLY);
	irq_init();
	irq_disable();

#ifdef CONFIG_FPU
	*SCB_CPACR |= (SCB_CPACR_CP10_FULL | SCB_CPACR_CP11_FULL);
#endif

	run_init_hook(INIT_LEVEL_PLATFORM);

	__l4_printf("%s", banner);

	run_init_hook(INIT_LEVEL_KERNEL_EARLY);

	run_init_hook(INIT_LEVEL_KERNEL);

	/* Not creating kernel thread here because it corrupts current stack
	 */
	create_idle_thread();
	create_root_thread();

	ktimer_event_create(64, ipc_deliver, NULL);

	mpu_enable(MPU_ENABLED);

	run_init_hook(INIT_LEVEL_LAST);

	switch_to_kernel();

	/* Not reached */
	return 0;
}

static inline void init_zero_seg(uint32_t *dst, uint32_t *dst_end)
{
	while (dst < dst_end)
		*dst++ = 0;
}

static inline void init_copy_seg(uint32_t *src, uint32_t *dst, uint32_t *dst_end)
{
	while (dst < dst_end)
		*dst++ = *src++;
}

void __l4_start(void)
{
	run_init_hook(INIT_LEVEL_EARLIEST);

#ifndef CONFIG_LOADER
	/* Copy data segments */
	init_copy_seg(&kernel_text_end,
	              &kernel_data_start, &kernel_data_end);
	/* DATA (ROM) -> DATA (RAM) */
	init_copy_seg(&user_text_flash_start,
	              &user_text_start, &user_text_end);
	/* USER TEXT (ROM) -> USER TEXT (RAM) */
	init_copy_seg(&user_text_flash_end,
	              &user_data_start, &user_data_end);
	/* USER DATA (ROM) -> USER DATA (RAM) */
#endif

	/* Fill bss with zeroes */
	init_zero_seg(&bss_start, &bss_end);
	init_zero_seg(&kernel_ahb_start, &kernel_ahb_end);
	init_zero_seg(&user_bss_start, &user_bss_end);

	sys_clock_init();

	/* entry point */
	main();
}
