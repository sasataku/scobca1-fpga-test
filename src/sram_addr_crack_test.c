/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"

uint32_t sram_addr_crack_test(uint32_t test_no)
{
	info("*** SRAM addr crack test starts ***\n");
	uint32_t err_num = 0;

	const mem_addr_t sram_base_addr = 0x00200000; // avobe 2MByte
	const mem_addr_t sram_last_addr = 0x00400000; // 2MByte
	const uint32_t sram_size = sram_last_addr - sram_base_addr;

	// write different data to each address
	mem_addr_t sram_addr = 0x00000000;
	write32(sram_addr + sram_base_addr, sram_addr);

	for(sram_addr = 0x00000004;  sram_addr < sram_size;){
		write32((sram_addr + sram_base_addr), sram_addr);
		sram_addr = sram_addr << 1;
	}

	// read data from each address and check if correct
	sram_addr = 0x00000000;
	if(read32(sram_addr + sram_base_addr) != sram_addr){
		err_num++;
	}

	for(sram_addr = 0x00000004;  sram_addr < sram_size;){
		if(read32(sram_addr + sram_base_addr) != sram_addr){
			err_num++;
		}
		sram_addr = sram_addr << 1;
	}

	info("*** test done, error count: %d ***\n", err_num);

	return err_num;
}
