/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "sram_byte_crack_test.h"

#define TEST_SRAM_ADDR 0x60310000

uint32_t sram_byte_crack_test(uint32_t test_no)
{
	/*
	 * test if byte access connection works by the actuall byte access using C.
	 * prepare four differenct byte data and fill 32 bit field using each.
	 * Then read them back to see it's correct
	 */

	info("*** SRAM byte crack test starts ***\n");
	uint32_t err_count = 0;

	const uint8_t byte_data1 = 0x12;
	const uint8_t byte_data2 = 0x34;
	const uint8_t byte_data3 = 0x56;
	const uint8_t byte_data4 = 0x78;

	const uint32_t word_data =
		   	(byte_data1 << 24) + (byte_data2 << 16) + (byte_data3 << 8) + byte_data4;

	uint32_t target_addr = (uint32_t)TEST_SRAM_ADDR;

    /* write 32bit work area by byte access */
	write8((target_addr + 3), byte_data1);
	write8((target_addr + 2), byte_data2);
	write8((target_addr + 1), byte_data3);
	write8((target_addr), byte_data4);

	debug("data: 0x%08x\n", *(uint32_t*)target_addr);

	if(*(uint32_t*)(target_addr) != word_data){
		err("byte write access failed, data(32): %08x, data(8x4): %08x\n", 
			*(uint32_t*)target_addr, word_data);
		err_count++;
	}

    /* read 32bit work area by byte access */
	if(read8(target_addr + 3) != byte_data1 ||
	   read8(target_addr + 2) != byte_data2 ||
	   read8(target_addr + 1) != byte_data3 ||
	   read8(target_addr) != byte_data4)
	{
		err("byte write access failed, data(32): %08x, data(8x4): %08x\n", 
			*(uint32_t*)target_addr, word_data);
		err_count++;
	}

	info("*** test done, error count: %d ***\n", err_count);

	return err_count;
}
