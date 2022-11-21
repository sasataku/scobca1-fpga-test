/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"

int pudc_crack_test(uint32_t test_no)
{
	printk("*** PUDC crack test starts ***\n");
	uint32_t err_num = 0;

	// must be HIGH (1)
	if(!(read32(TEST_REG_ADDR(TEST_MONI_FPGA_CFG)) & 0x1 << MONI_BIT_FPGA_CFG)){
		err_num++;
	}
	printk("*** test done, error count: %d ***\n", err_num);

	return err_num;
}
