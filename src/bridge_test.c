/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "bridge_test.h"

static struct bridge_test_regs test_targets[] =
{
	// SRAM
	{ TEST_CTRL_SRAM_A19, TEST_MONI_SRAM, MONI_BIT_SRAM_A19, 0, 0 },
	{ TEST_CTRL_SRAM_A18, TEST_MONI_SRAM, MONI_BIT_SRAM_A18, 0, 0 },
	{ TEST_CTRL_SRAM_A17, TEST_MONI_SRAM, MONI_BIT_SRAM_A17, 0, 0 },
	{ TEST_CTRL_SRAM_A16, TEST_MONI_SRAM, MONI_BIT_SRAM_A16, 0, 0 },
	{ TEST_CTRL_SRAM_A15, TEST_MONI_SRAM, MONI_BIT_SRAM_A15, 0, 0 },
	{ TEST_CTRL_SRAM_A14, TEST_MONI_SRAM, MONI_BIT_SRAM_A14, 0, 0 },
	{ TEST_CTRL_SRAM_A13, TEST_MONI_SRAM, MONI_BIT_SRAM_A13, 0, 0 },
	{ TEST_CTRL_SRAM_A12, TEST_MONI_SRAM, MONI_BIT_SRAM_A12, 0, 0 },
	{ TEST_CTRL_SRAM_A11, TEST_MONI_SRAM, MONI_BIT_SRAM_A11, 0, 0 },
	{ TEST_CTRL_SRAM_A10, TEST_MONI_SRAM, MONI_BIT_SRAM_A10, 0, 0 },
	{ TEST_CTRL_SRAM_A9, TEST_MONI_SRAM, MONI_BIT_SRAM_A9, 0, 0 },
	{ TEST_CTRL_SRAM_A8, TEST_MONI_SRAM, MONI_BIT_SRAM_A8, 0, 0 },
	{ TEST_CTRL_SRAM_A7, TEST_MONI_SRAM, MONI_BIT_SRAM_A7, 0, 0 },
	{ TEST_CTRL_SRAM_A6, TEST_MONI_SRAM, MONI_BIT_SRAM_A6, 0, 0 },
	{ TEST_CTRL_SRAM_A5, TEST_MONI_SRAM, MONI_BIT_SRAM_A5, 0, 0 },
	{ TEST_CTRL_SRAM_A4, TEST_MONI_SRAM, MONI_BIT_SRAM_A4, 0, 0 },
	{ TEST_CTRL_SRAM_A3, TEST_MONI_SRAM, MONI_BIT_SRAM_A3, 0, 0 },
	{ TEST_CTRL_SRAM_A2, TEST_MONI_SRAM, MONI_BIT_SRAM_A2, 0, 0 },
	{ TEST_CTRL_SRAM_A1, TEST_MONI_SRAM, MONI_BIT_SRAM_A1, 0, 0 },
	{ TEST_CTRL_SRAM_A0, TEST_MONI_SRAM, MONI_BIT_SRAM_A0, 0, 0 },
	{ TEST_CTRL_SRAM1_CE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_CE_B, 0, 0 },
	{ TEST_CTRL_SRAM1_OE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_OE_B, 0, 0 },
	{ TEST_CTRL_SRAM1_WE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_WE_B, 0, 0 },
	{ TEST_CTRL_SRAM1_BHE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_BHE_B, 0, 0 },
	{ TEST_CTRL_SRAM1_BLE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_BLE_B, 0, 0 },
	{ TEST_CTRL_SRAM2_CE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_CE_B, 0, 0 },
	{ TEST_CTRL_SRAM2_OE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_OE_B, 0, 0 },
	{ TEST_CTRL_SRAM2_WE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_WE_B, 0, 0 },
	{ TEST_CTRL_SRAM2_BHE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_BHE_B, 0, 0 },
	{ TEST_CTRL_SRAM2_BLE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_BLE_B, 0, 0 },

	// Config NOR Flash
	{ TEST_CTRL_CFG_MEM_CS_B, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_CS_B, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO3, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO3, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO2, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO2, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO1, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO1, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO0, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO0, 0, 0 },

	// Data NOR Flash
	{ TEST_CTRL_DATA_MEM1_CS_B, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_CS_B, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO3, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO3, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO2, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO2, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO1, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO1, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO0, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO0, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_CS_B, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_CS_B, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO3, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO3, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO2, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO2, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO1, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO1, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO0, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO0, 0, 0 },

	// FRAM
	{ TEST_CTRL_FRAM1_CS_B, TEST_MONI_FRAM, MONI_BIT_FRAM1_CS_B, 0, 0 },
	{ TEST_CTRL_FRAM1_IO3, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO3, 0, 0 },
	{ TEST_CTRL_FRAM1_IO2, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO2, 0, 0 },
	{ TEST_CTRL_FRAM1_IO1, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO1, 0, 0 },
	{ TEST_CTRL_FRAM1_IO0, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO0, 0, 0 },
	{ TEST_CTRL_FRAM2_CS_B, TEST_MONI_FRAM, MONI_BIT_FRAM2_CS_B, 0, 0 },
	{ TEST_CTRL_FRAM2_IO3, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO3, 0, 0 },
	{ TEST_CTRL_FRAM2_IO2, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO2, 0, 0 },
	{ TEST_CTRL_FRAM2_IO1, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO1, 0, 0 },
	{ TEST_CTRL_FRAM2_IO0, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO0, 0, 0 },
};

uint32_t get_test_target_status(int pos)
{
	return get_test_moni_status(
			test_targets[pos].moni_offset,
			test_targets[pos].moni_bitpos
			);
}

uint32_t get_test_target_gpio_mode(int pos)
{
	return get_test_gpio_mode(test_targets[pos].ctrl_offset);
}

void set_test_target_gpio_mode(int pos, uint8_t gpio_mode)
{
	set_test_gpio_mode(test_targets[pos].ctrl_offset, gpio_mode);
}

void init_bridge_test(void)
{
	// set all test pins to GPIO IN mode and store default status
	int test_item_num = sizeof(test_targets) / sizeof(struct bridge_test_regs);

	for(int i = 0; i < test_item_num; i++){
		uint32_t curr_mode = get_test_target_gpio_mode(i);
		set_test_target_gpio_mode(i, TEST_GPIO_IN);
		test_targets[i].default_status = get_test_target_status(i);
		set_test_target_gpio_mode(i, curr_mode);
	}
}

int32_t check_others_unchanged(uint32_t target_pos)
{
	// check if each status is same as default status except current target
	uint32_t num_changed = 0;
	int test_item_num = sizeof(test_targets) / sizeof(struct bridge_test_regs);

	for(int i = 0; i < test_item_num; i++){
		if(i == target_pos) continue; // skip when target and checked item are same

		if(get_test_target_status(i) != test_targets[i].default_status){
			printk("status changed, target idx: %d,  changed pin idx: %d\n",
				       	target_pos, i);
			num_changed++;
		}
	}

	return num_changed;
}

uint8_t monitor_only(uint32_t target_pos)
{
	return test_targets[target_pos].moni_only;
}

uint32_t bridge_test(uint32_t test_no)
{
	int num_err = 0;
	printk("* Start Bridge Test\n");
	init_bridge_test();

	int test_item_num = sizeof(test_targets) / sizeof(struct bridge_test_regs);

	for(int i = 0; i < test_item_num; i++){
		// check if only monitoring
		if(monitor_only(i)) continue;

		// Set High
		set_test_target_gpio_mode(i, TEST_GPIO_OUT_HIGH);
		num_err += check_others_unchanged(i);

		// Set Low
		set_test_target_gpio_mode(i, TEST_GPIO_OUT_LOW);
		num_err += check_others_unchanged(i);

		// Set back to Input for next testing
		set_test_target_gpio_mode(i, TEST_GPIO_IN);
	}

	printk("found bridge count: %d\n", num_err);

	return num_err;
}
