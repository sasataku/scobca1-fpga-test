/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hrmem_test.h"
#include "common.h"

static uint32_t hrmem_rw_test()
{
	bool ret;
	uint32_t err_cnt = 0;

	write32(SCOBCA1_FPGA_HRMEM_MIRROR_BASE_ADDR, 0xFFFFFFFF);
	ret = assert32(SCOBCA1_FPGA_HRMEM_MIRROR_BASE_ADDR, 0xFFFFFFFF, REG_READ_RETRY(10));
	if (!ret) {
		assert();
		err_cnt++;
	}

	return err_cnt;
}

uint32_t hrmem_test(uint32_t test_no)
{
	uint32_t ret;
	uint32_t err_cnt = 0;

	printk("* [%d] Start HRMEM Test\n", test_no);

	ret = hrmem_rw_test();
	err_cnt += ret;

	print_result(test_no, err_cnt);

	return err_cnt;
}
