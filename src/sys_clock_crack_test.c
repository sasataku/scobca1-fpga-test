/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"

uint32_t sys_clock_crack_test(uint32_t test_no)
{
	printk("*** System Clock crack test starts ***\n");
	uint32_t err_num = 0;

	// check if SYSCLK1 and SYSCLK2 work
	if(!(get_test_moni_status(TEST_MONI_SYSCLK, MONI_BIT_SYSCLK2))){
		err_num++;
	}
	if(!(get_test_moni_status(TEST_MONI_SYSCLK, MONI_BIT_SYSCLK1))){
		err_num++;
	}
	printk("*** test done, error count: %d ***\n", err_num);

	return err_num;
}
