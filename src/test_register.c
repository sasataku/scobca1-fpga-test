/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"

uint32_t get_test_moni_status(uint32_t addr_offset, uint8_t bit_pos)
{
	return read32(TEST_REG_ADDR(addr_offset)) & (0x1 << bit_pos);
}

uint32_t get_test_gpio_mode(uint32_t addr_offset)
{
	return read32(TEST_REG_ADDR(addr_offset));
}

void set_test_gpio_mode(uint32_t addr_offset, uint32_t mode)
{
	write32(TEST_REG_ADDR(addr_offset), mode);
}

/*
 * BEFORE TESTING, Connect two test pins together to check crack.
 * It checks when toggling the level of the first pin, if another follows
 */
uint32_t test_paired_pins_connection(
		uint32_t in_ctrl_addr, // control register to set as INPUT
		uint32_t out_ctrl_addr, // control register to change HI/LOW
		uint32_t in_moni_addr, // monitor register to read INPUT status
		uint8_t moni_pos // bit position in monitor register
		)
{
	int err_num = 0;

	uint32_t old_in_mode = get_test_gpio_mode(in_ctrl_addr);
	uint32_t old_out_mode = get_test_gpio_mode(out_ctrl_addr);

	// control first one and use it as an input
	set_test_gpio_mode(in_ctrl_addr, TEST_GPIO_IN);

	set_test_gpio_mode(out_ctrl_addr, TEST_GPIO_OUT_HIGH);
	if(!(get_test_moni_status(in_moni_addr, moni_pos))){
		err_num++;
	}

	set_test_gpio_mode(out_ctrl_addr, TEST_GPIO_OUT_LOW);
	if(get_test_moni_status(in_moni_addr, moni_pos)){
		err_num++;
	}

	// it's not necessary but to make sure, testing one more time
	set_test_gpio_mode(out_ctrl_addr, TEST_GPIO_OUT_HIGH);
	if(!(get_test_moni_status(in_moni_addr, moni_pos))){
		err_num++;
	}

	set_test_gpio_mode(in_ctrl_addr, old_in_mode);
	set_test_gpio_mode(out_ctrl_addr, old_out_mode);

	return err_num;
}
