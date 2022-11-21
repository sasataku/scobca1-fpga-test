/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_USER_IO_CRACK_H_
#define SCOBCA1_FPGA_TEST_USER_IO_CRACK_H_

#include "test_register.h"

struct user_io_test_regs
{
	uint32_t in_ctrl_reg;
	uint32_t out_ctrl_reg;
	uint32_t in_moni_reg;
	uint8_t moni_bitpos;
};

uint32_t user_io_crack_test(uint32_t test_no);

#endif /*  SCOBCA1_FPGA_TEST_USER_IO_CRACK_H_ */
