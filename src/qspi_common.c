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

uint32_t qspi_create_fifo_data(uint8_t start_val, uint32_t *data, size_t size, bool fill)
{
	for (uint32_t i=0; i<size; i++) {
		data[i] = start_val;
		if (!fill) {
			start_val++;
		}
	}

	return start_val;
}
