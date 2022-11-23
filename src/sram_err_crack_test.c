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

/* set SRAM address where never accessed before */
#define TEST_START_ADDR 0x603F0000
#define TEST_SKIP_ADDR 0x10

uint32_t sram_err_crack_test(uint32_t test_no)
{
	/* 
	 * clear ECC error count at first and access some sram address
	 * which seems never accessed after booting. It cause ECC error.
	 * check if error count is increasing
	 */

	info("*** SRAM ECC err crack test starts ***\n");
	uint32_t err_num = 0;
	const uint32_t max_test_count = 5;

	// clear ECC error count
	write32(ECCERRCNTCLRR_REG, 0x1);

	if(read32(ECC1ERR_CNTR_REG)){
		// couldn't reset the counter
		info("Couldn't clear ECC error counter, %d\n",
				read32(ECC1ERR_CNTR_REG));
		err_num++;
	}
	else{
		mem_addr_t test_addr = TEST_START_ADDR;
		int i = 0;
		for(; i < max_test_count; i++){
			read32(test_addr);
			if(read32(ECC1ERR_CNTR_REG)){
				break;
			}
			test_addr += TEST_SKIP_ADDR;
		}
		if(i == max_test_count) err_num++;
	}

	info("*** test done, error count: %u ***\n", err_num);

	return err_num;
}
