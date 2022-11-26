/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "sram_err_crack_test.h"

#define HRMEM_REG_BASE 0x40500000
#define ECC1ERR_CNTR_OFFSET 0x0020
#define ECCERRCNTCLRR_OFFSET 0x002C
#define ECC1ERR_CNTR_REG (HRMEM_REG_BASE + ECC1ERR_CNTR_OFFSET)
#define ECCERRCNTCLRR_REG (HRMEM_REG_BASE + ECCERRCNTCLRR_OFFSET)

/* set SRAM address where never access before */
#define TEST_START_ADDR 0x603F0000
#define TEST_SKIP_ADDR 0x10
#define HALFWORD_OFFSET 0x2

uint32_t sram_err_crack_test(uint32_t test_no)
{
	/*
	 * Test to check if ECC error signal connection is OK.
	 * Clear ECC error counter at first, then read some SRAM addr
	 * where never accessed after boot. It causes ECC error.
	 * Ckeck if error count increases
	 * To see SRAM1 and 2 error each, use 16bit access
	 */

	info("*** SRAM ECC err crack test starts ***\n");
	uint32_t err_count = 0;
	const uint32_t max_test_count = 5;

	// clear ECC error count
	write32(ECCERRCNTCLRR_REG, 0x1);

	if(read32(ECC1ERR_CNTR_REG)){
		// couldn't reset the counter
		info("Couldn't clear ECC error counter, %d\n",
				read32(ECC1ERR_CNTR_REG));
		err_count++;
	    info("*** test done, error count: %u ***\n", err_count);
		return err_count;
	}

	// Test lower addr
	mem_addr_t test_addr = TEST_START_ADDR;
	int test_count = 0;
	for(; test_count < max_test_count; test_count++){
		sys_read16(test_addr);
		if(read32(ECC1ERR_CNTR_REG)){
			break;
		}
		test_addr += TEST_SKIP_ADDR;
	}
	if(test_count == max_test_count) err_count++;

	// clear ECC error count
	write32(ECCERRCNTCLRR_REG, 0x1);

	if(read32(ECC1ERR_CNTR_REG)){
		// couldn't reset the counter
		info("Couldn't clear ECC error counter, %d\n",
				read32(ECC1ERR_CNTR_REG));
		err_count++;
	    info("*** test done, error count: %u ***\n", err_count);
		return err_count;
	}

	// Test higher addr
	test_addr = TEST_START_ADDR;
	test_count = 0;
	for(; test_count < max_test_count; test_count++){
		sys_read16(test_addr + HALFWORD_OFFSET);
		if(read32(ECC1ERR_CNTR_REG)){
			break;
		}
		test_addr += TEST_SKIP_ADDR;
	}
	if(test_count == max_test_count) err_count++;

	info("*** test done, error count: %u ***\n", err_count);

	return err_count;
}
