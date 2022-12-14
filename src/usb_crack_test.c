/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"
#include "usb_crack_test.h"

/*
 * Test if USB CS and reset connection works
 */
uint32_t usb_crack_test(uint32_t test_no)
{
	info("*** USB crack test starts ***\n");
	uint32_t err_count = 0;

	/*
	 * Check CS and RESET by Clock status, test flow is follows
	 * 1. USB Ready (CS: High, RESET: High) -> Clock ON
	 * 2. USB Power OFF (CS: Low, RESET: High) -> Clock OFF
	 * 3. USB Ready (CS: High, RESET: High) -> Clock ON
	 * 4. USB Reset ON (CS: High, RESET: Low) -> Clock OFF
	 * 5. USB Ready (CS: High, RESET: High) -> Clock ON
	 */

	uint32_t saved_cs_mode = get_test_gpio_mode(TEST_CTRL_ULPI_CS);
	uint32_t saved_reset_mode = get_test_gpio_mode(TEST_CTRL_ULPI_RESET_B);

	set_test_gpio_mode(TEST_CTRL_ULPI_CS, TEST_GPIO_OUT_HIGH);
	set_test_gpio_mode(TEST_CTRL_ULPI_RESET_B, TEST_GPIO_OUT_HIGH);
	if(!test_moni_status_high(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK)){
		err_count++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_CS, TEST_GPIO_OUT_LOW);
	if(!test_moni_status_low(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK)){
		err_count++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_CS, TEST_GPIO_OUT_HIGH);
	if(!test_moni_status_high(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK)){
		err_count++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_RESET_B, TEST_GPIO_OUT_LOW);
	if(!test_moni_status_low(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK)){
		err_count++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_RESET_B, TEST_GPIO_OUT_HIGH);
	if(!test_moni_status_high(TEST_MONI_ULPI_CLOCK, MONI_BIT_ULPI_CLOCK)){
		err_count++;
	}

	set_test_gpio_mode(TEST_CTRL_ULPI_CS, saved_cs_mode);
	set_test_gpio_mode(TEST_CTRL_ULPI_RESET_B, saved_reset_mode);

	info("*** test done, error count: %d ***\n", err_count);

	return err_count;
}
