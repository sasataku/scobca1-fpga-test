/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"

void write32(uint32_t addr, uint32_t val)
{
	sys_write32(val, addr);
	printk("  write32 [0x%08X] 0x%08x\n", addr, val);
}

bool assert32(uint32_t addr, uint32_t exp, uint32_t retry)
{
	uint32_t regval;

	for (uint32_t i=0; i<retry; i++) {
		regval = sys_read32(addr);
		if (i==0) {
			printk("  read32  [0x%08X] 0x%08x (exp:0x%08x)\n",
					addr, regval, exp);
		} else {
			printk("  read32  [0x%08X] 0x%08x (exp:0x%08x) (retry:%d)\n",
					addr, regval, exp, i);
		}
		if (regval == exp) {
			return true;
		}
	}

	printk("  !!! Assertion failed: retry count: %d\n", retry);
	return false;
}

void print_result(uint32_t test_no, uint32_t err_cnt)
{
	if (err_cnt == 0) {
		printk("* [%d] Test Result: Passed\n", test_no);
	} else {
		printk("* [%d] Test Result: Failed (Assertion count: %d)\n",
				test_no, err_cnt);
	}
}
