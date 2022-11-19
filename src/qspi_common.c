/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"
#include "common.h"

uint32_t qspi_init(uint32_t test_no)
{
	uint32_t err_cnt = 0;

	printk("* [%d] Start QSPI Initializing\n", test_no);

	err_cnt += qspi_norflash_initialize(test_no);
	err_cnt += qspi_fram_initialize(test_no);

	print_result(test_no, err_cnt);

	return err_cnt;
}
