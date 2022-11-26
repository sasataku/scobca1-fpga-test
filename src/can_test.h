/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_CAN_TEST_H_
#define SCOBCA1_FPGA_TEST_CAN_TEST_H_

#include <zephyr/kernel.h>

uint32_t can_loopback(void);
uint32_t can_test(uint32_t test_no);
uint32_t can_send_cmd(uint32_t test_no);
uint32_t can_crack_test(uint32_t test_no);

#endif /* SCOBCA1_FPGA_TEST_CAN_TESET_H_ */
