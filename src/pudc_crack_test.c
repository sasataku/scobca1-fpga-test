/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"

int pudc_crack_test(uint32_t test_no)
{
	info("*** PUDC crack test starts ***\n");
	uint32_t err_count = 0;

	// must be HIGH (1)
	if(!test_moni_status_high(TEST_MONI_FPGA_CFG, MONI_BIT_FPGA_CFG)){
		err_count++;
	}
	info("*** test done, error count: %d ***\n", err_count);

	return err_count;
}
