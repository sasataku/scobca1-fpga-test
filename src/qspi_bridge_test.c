/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "bridge_test.h"

static struct bridge_test_regs qspi_targets[] =
{
	// Config NOR Flash
	{ TEST_CTRL_CFG_MEM_CS_B, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_CS_B, MONI, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO3, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO3, CTRL, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO2, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO2, CTRL, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO1, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO1, CTRL, 0, 0 },
	{ TEST_CTRL_CFG_MEM_IO0, TEST_MONI_CFG_MEM, MONI_BIT_CFG_MEM_IO0, CTRL, 0, 0 },

	// Data NOR Flash
	{ TEST_CTRL_DATA_MEM1_CS_B, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_CS_B, MONI, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO3, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO3, CTRL, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO2, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO2, CTRL, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO1, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO1, CTRL, 0, 0 },
	{ TEST_CTRL_DATA_MEM1_IO0, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM1_IO0, CTRL, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_CS_B, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_CS_B, MONI, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO3, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO3, CTRL, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO2, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO2, CTRL, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO1, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO1, CTRL, 0, 0 },
	{ TEST_CTRL_DATA_MEM2_IO0, TEST_MONI_DATA_MEM, MONI_BIT_DATA_MEM2_IO0, CTRL, 0, 0 },

	// FRAM
	{ TEST_CTRL_FRAM1_CS_B, TEST_MONI_FRAM, MONI_BIT_FRAM1_CS_B, MONI, 0, 0 },
	{ TEST_CTRL_FRAM1_IO3, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO3, CTRL, 0, 0 },
	{ TEST_CTRL_FRAM1_IO2, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO2, CTRL, 0, 0 },
	{ TEST_CTRL_FRAM1_IO1, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO1, CTRL, 0, 0 },
	{ TEST_CTRL_FRAM1_IO0, TEST_MONI_FRAM, MONI_BIT_FRAM1_IO0, CTRL, 0, 0 },
	{ TEST_CTRL_FRAM2_CS_B, TEST_MONI_FRAM, MONI_BIT_FRAM2_CS_B, MONI, 0, 0 },
	{ TEST_CTRL_FRAM2_IO3, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO3, CTRL, 0, 0 },
	{ TEST_CTRL_FRAM2_IO2, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO2, CTRL, 0, 0 },
	{ TEST_CTRL_FRAM2_IO1, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO1, CTRL, 0, 0 },
	{ TEST_CTRL_FRAM2_IO0, TEST_MONI_FRAM, MONI_BIT_FRAM2_IO0, CTRL, 0, 0 },
};

static void setup_qspi_bridge_test(void)
{
	// set all test pins to GPIO IN mode and store default status
	int test_item_num =
		   	sizeof(qspi_targets) / sizeof(struct bridge_test_regs);

	for(int i = 0; i < test_item_num; i++){
		struct bridge_test_regs* target = &qspi_targets[i];

		target->orig_mode = get_test_gpio_mode(target->ctrl_offset);
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_IN);
		target->init_status = get_test_moni_status(
						target->moni_offset, target->moni_bitpos);
	}
}

static void cleanup_qspi_bridge_test(void)
{
	// restore original mode
	int test_item_num =
		   	sizeof(qspi_targets) / sizeof(struct bridge_test_regs);

	for(int i = 0; i < test_item_num; i++){
		struct bridge_test_regs* target = &qspi_targets[i];

		set_test_gpio_mode(target->ctrl_offset, target->orig_mode);
	}
}

static int32_t check_others_unchanged(struct bridge_test_regs *ctrl_pin)
{
	// check if each status is same as default status except current target
	uint32_t num_changed = 0;
	int test_item_num =
		   	sizeof(qspi_targets) / sizeof(struct bridge_test_regs);

	for(int i = 0; i < test_item_num; i++){
		struct bridge_test_regs *curr_pin = &qspi_targets[i];
		if(ctrl_pin == curr_pin) continue; // skip compared items for are same

		if(get_test_moni_status(curr_pin->moni_offset, curr_pin->moni_bitpos)
					   	!= curr_pin->init_status){
			num_changed++;
		}
	}

	return num_changed;
}

uint32_t qspi_bridge_test(uint32_t test_no)
{
	int err_count = 0;
	info("* Start Bridge Test\n");
	setup_qspi_bridge_test();

	int test_item_num =
		   	sizeof(qspi_targets) / sizeof(struct bridge_test_regs);

	for(int i = 0; i < test_item_num; i++){
		struct bridge_test_regs *target = &qspi_targets[i];

		if(target->moni_only) continue;

		// Set High
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_OUT_HIGH);
		if(!(get_test_moni_status(target->moni_offset, target->moni_bitpos))){
			err_count++;
		}
		err_count += check_others_unchanged(target);

		// Set Low
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_OUT_LOW);
		if(get_test_moni_status(target->moni_offset, target->moni_bitpos)){
			err_count++;
		}
		err_count += check_others_unchanged(target);

		// Set back to Input for next testing
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_IN);
	}

	cleanup_qspi_bridge_test();
	info("found bridge count: %d\n", err_count);

	return err_count;
}
