/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_COMMON_H_
#define SCOBCA1_FPGA_TEST_COMMON_H_

#include <zephyr/kernel.h>

#define REG_READ_RETRY(count) (count)

#ifdef CONFIG_DEBUG_PRINT
#define debug(format, ...) printk(format, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif
#define info(format, ...) printk(format, ##__VA_ARGS__)
#define err(format, ...) printk(format, ##__VA_ARGS__)
#define assert() printk("  %s:%d\n", __FILE__, __LINE__);

uint8_t read8(mem_addr_t addr);
void write8(mem_addr_t addr, uint8_t val);
uint16_t read16(mem_addr_t addr);
void write16(mem_addr_t addr, uint16_t val);
uint32_t read32(mem_addr_t addr);
void write32(mem_addr_t addr, uint32_t val);
bool assert32(uint32_t addr, uint32_t exp, uint32_t retry);
void print_result();

#endif /* SCOBCA1_FPGA_TEST_COMMON_H_ */
