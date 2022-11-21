/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "longrun_test.h"
#include "common.h"
#include "bhm_test.h"

#define STACK_SIZE (1024)
#define THREAD_PRIORITY (7)

K_THREAD_STACK_DEFINE(_longrun_thread_stack, STACK_SIZE);
static struct k_thread _k_thread_data;

extern bool is_exit;

static void start_longrun_test(void *p1, void *p2, void *p3)
{
	bool ret;
	uint32_t err_cnt = 0;
	uint16_t loop_count = 0;
	uint32_t start_time;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* Enable Board Health Monitoring */
	for (uint8_t i=0; i<5; i++) {
		ret = bhm_enable();
		if (ret) {
			break;
		}
	}
	if (!ret) {
		err("* Abort. Can not enable Board Health Moniting\n");
	}

	while (true) {
		loop_count++;

		info("* Board Halth Monitoring\n");
		err_cnt += bhm_read_sensor_data();

		info("* Loop [%d] Total assertion: %d\n", loop_count,  err_cnt);

		k_sleep(K_MSEC(1000));

		if (is_exit) {
			printk("* Stop Long Run Test\n");
			break;
		}
	}
}

uint32_t longrun_test(uint32_t test_no)
{
	k_tid_t tid;
	
	tid = k_thread_create(&_k_thread_data, _longrun_thread_stack, STACK_SIZE,
					start_longrun_test, NULL, NULL, NULL,
					THREAD_PRIORITY, 0, K_NO_WAIT);
	printk("* [%d] Start Long Run Test [tid:%d]\n", test_no, (unsigned int)tid);

	return 0;
}
