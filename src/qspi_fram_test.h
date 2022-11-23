/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_QSPI_FRAM_TEST_H_
#define SCOBCA1_FPGA_TEST_QSPI_FRAM_TEST_H_

#include <zephyr/kernel.h>

uint32_t qspi_fram_initialize(uint32_t test_no);
bool qspi_fram_multi_write(uint8_t mem_no, uint32_t mem_addr, uint32_t size, uint8_t start_val);
bool qspi_fram_multi_read(uint8_t mem_no, uint32_t mem_addr, uint32_t size, uint8_t start_val);
uint32_t qspi_fram_test(uint32_t test_no);

#endif /* SCOBCA1_FPGA_TEST_QSPI_FRAM_TESET_H_ */
