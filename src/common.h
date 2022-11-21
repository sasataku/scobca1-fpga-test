/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_COMMON_H_
#define SCOBCA1_FPGA_TEST_COMMON_H_

#include <zephyr/kernel.h>

//#define DEBUG (1)

#define REG_READ_RETRY(count) (count)

#ifdef DEBUG
#define debug(format, ...) printk(format, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif
#define info(format, ...) printk(format, ##__VA_ARGS__)
#define err(format, ...) printk(format, ##__VA_ARGS__)
#define assert() printk("  %s:%d\n", __FILE__, __LINE__);

uint32_t read32(uint32_t addr);
void write32(uint32_t addr, uint32_t val);
bool assert32(uint32_t addr, uint32_t exp, uint32_t retry);
void print_result();

#endif /* SCOBCA1_FPGA_TEST_COMMON_H_ */
