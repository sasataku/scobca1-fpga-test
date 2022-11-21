/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_QSPI_NORFLASH_TEST_H_
#define SCOBCA1_FPGA_TEST_QSPI_NORFLASH_TEST_H_

#include <zephyr/kernel.h>

uint32_t qspi_norflash_initialize(uint32_t test_no);
uint32_t qspi_config_memory_test(uint32_t test_no);
uint32_t qspi_config_memory_sector_test(uint32_t test_no);
uint32_t qspi_data_memory_test(uint32_t test_no);
uint32_t qspi_data_memory_sector_test(uint32_t test_no);

#endif /* SCOBCA1_FPGA_TEST_QSPI_NORFLASH_TESET_H_ */
