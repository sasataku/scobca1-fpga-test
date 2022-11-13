/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_HRMEM_TEST_H_
#define SCOBCA1_FPGA_TEST_HRMEM_TEST_H_

#include <zephyr/kernel.h>

/* Base address */
#define SCOBCA1_FPGA_HRMEM_MIRROR_BASE_ADDR  (0x60000000)

uint32_t hrmem_test(void);

#endif /* SCOBCA1_FPGA_TEST_HRMEM_TESET_H_ */
