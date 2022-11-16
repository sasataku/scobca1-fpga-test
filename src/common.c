/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"

void write32(uint32_t addr, uint32_t val)
{
	sys_write32(val, addr);
	printk("     write32 [addr:0x%08X] 0x%08x\n", addr, val);
}

bool assert32(uint32_t addr, uint32_t exp, uint32_t retry)
{
	uint32_t regval;

	for (uint32_t i=0; i<retry; i++) {
		regval = sys_read32(addr);
		printk("     read32  [addr:0x%08X] 0x%08x (expected:0x%08x) (%d times)\n",
				addr, regval, exp, i+1);
		if (regval == exp) {
			return true;
		}
	}

	printk("     !!! Assertion failed: retry count: %d\n", retry);
	return false;
}
