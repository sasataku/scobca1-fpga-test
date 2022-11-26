/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_BHM_TEST_H_
#define SCOBCA1_FPGA_TEST_BHM_TEST_H_

#include <zephyr/kernel.h>

#define SCOBCA1_TEMP_LIMIT_UPPER (110)
#define SCOBCA1_TEMP_LIMIT_LOWER (-40)

bool bhm_enable(void);
void bhm_disable(void);
uint32_t bhm_read_sensor_data(void);
uint32_t bhm_test(uint32_t test_no);
uint32_t i2c_internal_crack_test(uint32_t test_no);

#endif /* SCOBCA1_FPGA_TEST_BHM_TESET_H_ */
