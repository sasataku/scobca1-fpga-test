/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "sram_byte_crack_test.h"

uint32_t sram_byte_crack_test(uint32_t test_no)
{
	info("*** SRAM byte crack test starts ***\n");
	uint32_t err_num = 0;

	const uint8_t byte_data1 = 0x12;
	const uint8_t byte_data2 = 0x34;
	const uint8_t byte_data3 = 0x56;
	const uint8_t byte_data4 = 0x78;

	const uint32_t word_data =
		   	(byte_data1 << 24) + (byte_data2 << 16) + (byte_data3 << 8) + byte_data4;

	uint32_t work_mem = 0;
	mem_addr_t target_addr = (mem_addr_t)&work_mem;

    /* write 32bit work area by byte access */
	sys_write8(byte_data1, (mem_addr_t)(target_addr + 3));
	sys_write8(byte_data2, (mem_addr_t)(target_addr + 2));
	sys_write8(byte_data3, (mem_addr_t)(target_addr + 1));
	sys_write8(byte_data4, (mem_addr_t)(target_addr));

	debug("data: 0x%08x\n", work_mem);

	if(work_mem != word_data){
			err_num++;
	}

    /* read 32bit work area by byte access */
	if(sys_read8((mem_addr_t)(target_addr + 3)) != byte_data1 ||
	   sys_read8((mem_addr_t)(target_addr + 2)) != byte_data2 ||
	   sys_read8((mem_addr_t)(target_addr + 1)) != byte_data3 ||
	   sys_read8((mem_addr_t)(target_addr)) != byte_data4)
	{
			err_num++;
	}

	info("*** test done, error count: %d ***\n", err_num);

	return err_num;
}
