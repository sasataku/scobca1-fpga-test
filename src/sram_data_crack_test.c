/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"
#include "sram_data_crack_test.h"

#define TARGET_TEST_ADDR 0x00380000

uint32_t sram_data_crack_test(uint32_t test_no)
{
	/*
	 * test if each SRAM data connection works.
	 * at first, set zero data to the SRAM address for testing
	 * then toggles each data connection's signal level and
	 * confirm it changes.
	 */

	info("*** SRAM data crack test starts ***\n");
	uint32_t err_count = 0;

	write32(TARGET_TEST_ADDR, 0x0);
	if(!assert32(TARGET_TEST_ADDR, 0x0, 0)){
		err_count++;
	}

	for(int i = 0; i < 32 ; i++){
		write32(TARGET_TEST_ADDR, 0x1 << i);
		if(!assert32(TARGET_TEST_ADDR, 0x1 << i, 0)){
			err_count++;
		}
	}

	info("*** test done, error count: %d ***\n", err_count);

	return err_count;
}
