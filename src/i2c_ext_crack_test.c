/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_register.h"
#include "i2c_ext_crack_test.h"

/*
 * Connect SCL and SDA pins via B2B connector to test
 */
uint32_t i2c_ext_crack_test(uint32_t test_no)
{
	printk("*** External I2C crack test starts ***\n");
	uint32_t err_num = 0;

	// control SCL and use SDA as an input
	err_num += test_paired_pins_connection(
			TEST_CTRL_I2C_EXT_SDA,
			TEST_CTRL_I2C_EXT_SCL,
			TEST_MONI_I2C_EXT,
			MONI_BIT_I2C_EXT_SDA
			);
	printk("*** test done, error count: %d ***\n", err_num);

	return err_num;
}
