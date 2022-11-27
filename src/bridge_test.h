/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_BRIDGE_TEST_H_
#define SCOBCA1_FPGA_TEST_BRIDGE_TEST_H_

#include "test_register.h"

#define MONI (1)
#define CTRL (0)

struct bridge_test_regs
{
	uint32_t ctrl_offset; // control resigter
	uint32_t moni_offset; // monitor register
	uint8_t moni_bitpos; // bit position in monitor register
	uint8_t moni_only; // 0: controllable, 1: monitor only
	uint32_t orig_mode; // original mode to restore
	uint32_t init_status; // init value (High or Low)
};

#endif /* SCOBCA1_FPGA_TEST_BRIDGE_TEST_H_ */
