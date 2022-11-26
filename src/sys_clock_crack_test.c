/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "sys_clock_crack_test.h"

uint32_t sys_clock_crack_test(uint32_t test_no)
{
	/*
	 * Test if two system clocks (SYSCLK1 and SYSCLK2) work with
	 * the value of monitor registers
	 */
	info("*** System Clock crack test starts ***\n");
	uint32_t err_count = 0;

	if(!(get_test_moni_status(TEST_MONI_SYSCLK, MONI_BIT_SYSCLK2))){
		err_count++;
	}
	if(!(get_test_moni_status(TEST_MONI_SYSCLK, MONI_BIT_SYSCLK1))){
		err_count++;
	}
	info("*** test done, error count: %d ***\n", err_count);

	return err_count;
}
