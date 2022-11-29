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
	 * SRAM area for test is between 0x00100000 and 0x003fffff.
	 * to check addr pin works, compare values between addresses
	 * which only target pin's signal level is differ.
	 *
	 * In this case, while 0 to 18 pins, use addr 0x00200000 to
	 * compare value, for 19 (highest) pin, use addr 0x00100000
	 *
	 * compare value with 0x00200000
	 *  0: 0x00200004
	 *  1: 0x00200008
	 *  2: 0x00200010
	 *  3: 0x00200020
	 *  ....
	 * 16: 0x00240000
	 * 17: 0x00280000
	 * 18: 0x00300000
	 *
	 * compare value with 0x00100000
	 * 19: 0x00300000
	 *
	 * write different data to each address to compare
	 * (using addr itself as different data)
	 */

	info("*** SRAM addr crack test starts ***\n");
	uint32_t err_count = 0;

	mem_addr_t test_addr;

	/* between 0 and 18 pins */
	mem_addr_t compare_addr = 0x00200000;
	write32(compare_addr, (uint32_t)compare_addr);

	for(mem_addr_t i = 0x4; i <= 0x100000; i = i << 1){
		test_addr = compare_addr + i;
		write32(test_addr, test_addr);
		if(read32(test_addr) == read32(compare_addr)){
			err_count++;
		}
	}

	/* 19 pin */
	compare_addr = 0x00100000;
	write32(compare_addr, (uint32_t)compare_addr);
	test_addr = 0x00300000;
	write32(test_addr, (uint32_t)test_addr);

	if(read32(test_addr) == read32(compare_addr)){
		err_count++;
	}

	info("*** test done, error count: %d ***\n", err_count);

	return err_count;
}
