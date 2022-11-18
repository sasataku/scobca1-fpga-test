/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_COMMON_H_
#define SCOBCA1_FPGA_TEST_COMMON_H_

#include <zephyr/kernel.h>

void write32(uint32_t addr, uint32_t val);
bool assert32(uint32_t addr, uint32_t exp, uint32_t retry);
void print_result();

#endif /* SCOBCA1_FPGA_TEST_COMMON_H_ */
