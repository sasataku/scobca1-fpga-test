/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_BHM_TEST_H_
#define SCOBCA1_FPGA_TEST_BHM_TEST_H_

#include <zephyr/kernel.h>

#define SCOBCA1_TEMP_LIMIT_UPPER (50)
#define SCOBCA1_TEMP_LIMIT_LOWER (-10)

uint32_t bhm_enable(void);
uint32_t bhm_read_sensor_data_test(void);
uint32_t bhm_test(uint32_t test_no);



#endif /* SCOBCA1_FPGA_TEST_BHM_TESET_H_ */
