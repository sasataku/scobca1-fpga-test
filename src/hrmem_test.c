/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hrmem_test.h"
#include "common.h"

#define HRMEM_FREE_MEM_ADDR (0x00200000)
#define HRMEM_WRITE_BYTE (1024*1024)

/*
 *  Write 1K byte to HRMEM and read/verify it
 */
uint32_t hrmem_rw_test(uint32_t size, uint32_t start_val, uint32_t *next_val)
{
	bool ret;
	uint32_t err_cnt = 0;
	uint32_t regval = start_val;
	uint32_t start_mem_addr = SCOBCA1_FPGA_HRMEM_MIRROR_BASE_ADDR + HRMEM_FREE_MEM_ADDR;
	uint32_t mem_addr = start_mem_addr;

	info("* Write 1M byte data to HRMEM (0x%08x)\n", mem_addr);
	for (int16_t i=0; i<HRMEM_WRITE_BYTE/sizeof(regval); i++) {
		write32(mem_addr, regval);
		mem_addr += sizeof(regval);
		regval++;
	}
	*next_val = regval;

	info("* Read 1M byte data from HRMEM and Verify (0x%08x)\n", start_mem_addr);
	for (int16_t i=0; i<HRMEM_WRITE_BYTE/sizeof(regval); i++) {

		ret = assert32(start_mem_addr, start_val, REG_READ_RETRY(0));
		if (!ret) {
			assert();
			err_cnt++;
		}
		start_mem_addr += sizeof(regval);
		start_val++;
	}

	return err_cnt;
}

uint32_t hrmem_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t start_val = 0x00;
	uint32_t next_val;

	printk("* [%d] Start HRMEM Test\n", test_no);

	err_cnt += hrmem_rw_test(HRMEM_WRITE_BYTE, start_val, &next_val);

	print_result(test_no, err_cnt);

	return err_cnt;
}
