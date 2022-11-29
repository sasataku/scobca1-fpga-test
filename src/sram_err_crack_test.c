/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"
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
     * Test the connection of ECC error signal
     * Clear ECC error latch register for SRAM1 and 2 at first,
     * then read some lower and higher SRAM addr where never
     * accessed after boot. It causes ECC error for SRAM1 and 2.
     * Ckeck error latch register status to confirm it.
     */

    info("*** SRAM ECC err crack test starts ***\n");
    uint32_t err_count = 0;
    const uint32_t max_test_count = 5;

    // SRAM 1 Test
    write32(TEST_MONI_SRAM_ERR, 0x0);

    if(get_test_moni_status(TEST_MONI_SRAM_ERR, MONI_BIT_SRAM1_ERR)){
        info("Couldn't clear SRAM1 ERR\n");
        return ++err_count;
    }

    uint32_t test_addr = TEST_START_ADDR;
    int test_count = 0;
    for(; test_count < max_test_count; test_count++){
        read16(test_addr);
        if(get_test_moni_status(TEST_MONI_SRAM_ERR, MONI_BIT_SRAM1_ERR)){
            break;
        }
        test_addr += TEST_SKIP_ADDR;
    }
    if(test_count == max_test_count){
		err("SRAM1 error couldn't be detected, moni addr: 0x%08X, val: %08x\n",
			TEST_REG_ADDR(TEST_MONI_SRAM_ERR),
        	get_test_moni_status(TEST_MONI_SRAM_ERR, MONI_BIT_SRAM1_ERR)
			);
	   	err_count++;
	}

    // SRAM 2 Test
    write32(TEST_MONI_SRAM_ERR, 0x0);

    if(get_test_moni_status(TEST_MONI_SRAM_ERR, MONI_BIT_SRAM2_ERR)){
        info("Couldn't clear SRAM2 ERR\n");
        return ++err_count;
    }

    test_addr = TEST_START_ADDR;
    test_count = 0;
    for(; test_count < max_test_count; test_count++){
        read16(test_addr + HALFWORD_OFFSET);
        if(get_test_moni_status(TEST_MONI_SRAM_ERR, MONI_BIT_SRAM2_ERR)){
            break;
        }
        test_addr += TEST_SKIP_ADDR;
    }
    if(test_count == max_test_count){
		err("SRAM2 error couldn't be detected, moni addr: 0x%08X, val: %08x\n",
			TEST_REG_ADDR(TEST_MONI_SRAM_ERR),
        	get_test_moni_status(TEST_MONI_SRAM_ERR, MONI_BIT_SRAM1_ERR)
		   );
	   	err_count++;
	}

    info("*** test done, error count: %u ***\n", err_count);

    return err_count;
}
