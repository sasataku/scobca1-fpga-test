/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "usb_crack_test.h"

/*
 * control CS and RESET, then check if clock works
 */
uint32_t usb_crack_test(uint32_t test_no)
{
	printk("*** USB crack test starts ***\n");
	uint32_t err_num = 0;

	/*
	 * Check CS and RESET by Clock status, test flow is follows
	 * 1. USB Ready (CS: High, RESET: High) -> Clock ON
	 * 2. USB Power OFF (CS: Low, RESET: High) -> Clock OFF
	 * 3. USB Ready (CS: High, RESET: High) -> Clock ON
	 * 4. USB Reset ON (CS: High, RESET: Low) -> Clock OFF
	 * 5. USB Ready (CS: High, RESET: High) -> Clock ON
	 */

	set_test_gpio_mode(TEST_CTRL_ULPI_CS, TEST_GPIO_OUT_HIGH);
	set_test_gpio_mode(TEST_CTRL_ULPI_RESET_B, TEST_GPIO_OUT_HIGH);
	if(!(get_test_moni_status(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK))){
			err_num++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_CS, TEST_GPIO_OUT_LOW);
	if(get_test_moni_status(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK)){
			err_num++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_CS, TEST_GPIO_OUT_HIGH);
	if(!(get_test_moni_status(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK))){
			err_num++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_RESET_B, TEST_GPIO_OUT_LOW);
	if(get_test_moni_status(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK)){
			err_num++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_RESET_B, TEST_GPIO_OUT_HIGH);
	if(!(get_test_moni_status(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK))){
			err_num++;
	}

	printk("*** test done, error count: %d ***\n", err_num);

	return err_num;
}
