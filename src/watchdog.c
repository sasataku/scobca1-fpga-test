/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "system_monitor_reg.h"

#define STACK_SIZE (1024)
#define THREAD_PRIORITY (7)

K_THREAD_STACK_DEFINE(_kick_wdt_thread_stack, STACK_SIZE);
static struct k_thread _k_thread_data;

static void kick_watchdog_timer(void *p1, void *p2, void *p3)
{
	uint32_t regval;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (true) {
		regval = sys_read32(SCOBCA1_FPGA_SYSMON_WDOG_CTRL);
		sys_write32(regval, SCOBCA1_FPGA_SYSMON_WDOG_CTRL);
		k_sleep(K_MSEC(1000));
	}
}

void start_kick_wdt_thread(void)
{
	k_tid_t tid;
	
	tid = k_thread_create(&_k_thread_data, _kick_wdt_thread_stack, STACK_SIZE,
					kick_watchdog_timer, NULL, NULL, NULL,
					THREAD_PRIORITY, 0, K_NO_WAIT);
	printk("Start kick the Watch Dog Timer thread (ID: %x)\n", (unsigned int)tid);
}
