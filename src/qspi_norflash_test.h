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
uint32_t qspi_config_memory_block_test(uint32_t test_no);
uint32_t qspi_data_memory_test(uint32_t test_no);
uint32_t qspi_data_memory_sector_test(uint32_t test_no);
uint32_t qspi_data_memory_block_test(uint32_t test_no);
bool qspi_norflash_erase(uint32_t base, uint8_t mem_no, enum QspiEraseType type,
								uint32_t mem_addr, bool is_wait_idle);
bool qspi_norflash_multi_read(uint32_t base, uint8_t mem_no, uint32_t mem_addr,
							uint32_t size, uint8_t start_val, bool is_init);
bool qspi_norflash_multi_write(uint32_t base, uint8_t mem_no, uint32_t mem_addr,
							uint32_t size, uint8_t start_val);

#endif /* SCOBCA1_FPGA_TEST_QSPI_NORFLASH_TESET_H_ */
