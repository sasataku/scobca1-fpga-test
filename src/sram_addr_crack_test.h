/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_SRAM_ADDR_CRACK_H_
#define SCOBCA1_FPGA_TEST_SRAM_ADDR_CRACK_H_

/*
 * use from 1M to 4MByte of SRAM is used for testing
 * (make zephyr not to use this area )
 */
#define TEST_SRAM_START_ADDR 0x00100000
#define TEST_SRAM_LAST_ADDR  0x003fffff

uint32_t sram_addr_crack_test(uint32_t test_no);

#endif /* SCOBCA1_FPGA_TEST_SRAM_ADDR_CRACK_H_ */
