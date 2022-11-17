/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"

uint32_t qspi_init(void)
{
	uint32_t err_cnt = 0;

	err_cnt += qspi_norflash_initialize();
	err_cnt += qspi_fram_initialize();

	if (err_cnt == 0) {
		printk("* Initialize Result: Passed\n");
	} else {
		printk("* Initialize Result: Failed (Assertion total count: %d)\n", err_cnt);
	}

	return err_cnt;
}
