/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"
#include "sram_addr_crack_test.h"

/*
 * This test checks if each address pin of SRAM works
 */
uint32_t sram_addr_crack_test(uint32_t test_no)
{
	/*
	 * SRAM area for test is between 0x60000000 and 0x603fffff.
	 * To check if crack exist as follows:
	 * 
	 * 1. set unique data to address which only one address pin
	 * asserts and to all zero.
	 * (e.g.)
	 * addr   : data
	 * 0b0000 : 0000
	 * 0b0001 : 0001
	 * 0b0010 : 0010
	 * 0b0100 : 0100
	 * 0b1000 : 1000
	 *
	 * 2. read back to chack all data written stay same
	 *
	 * If some pins are broken and stick to same signal level,
	 * it can be detected by checking all zero addr's value,
	 * I suppose.
	 *
	 */

	info("*** SRAM addr crack test starts ***\n");
	uint32_t err_count = 0;

	uint32_t test_addr;

	/* write unique data to all addr and then read back */
	uint32_t compare_addr = SRAM_MIRROR_BASE; /* 0x60000000 */
	write32(compare_addr, compare_addr);

	for(uint32_t i = 0x4; i <= 0x200000; i = i << 1){
		test_addr = compare_addr + i;
		write32(test_addr, test_addr);
	}

	if(!assert32(compare_addr, compare_addr, 0)){
		err_count++;
	}

	for(uint32_t i = 0x4; i <= 0x200000; i = i << 1){
		test_addr = compare_addr + i;
		if(!assert32(test_addr, test_addr, 0)){
			err_count++;
		}
	}

	info("*** test done, error count: %d ***\n", err_count);

	return err_count;
}
