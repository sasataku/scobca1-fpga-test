/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "bridge_test.h"

static struct bridge_test_regs memory_targets[] =
{
	// SRAM
	{ TEST_CTRL_SRAM_A19, TEST_MONI_SRAM, MONI_BIT_SRAM_A19, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A18, TEST_MONI_SRAM, MONI_BIT_SRAM_A18, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A17, TEST_MONI_SRAM, MONI_BIT_SRAM_A17, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A16, TEST_MONI_SRAM, MONI_BIT_SRAM_A16, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A15, TEST_MONI_SRAM, MONI_BIT_SRAM_A15, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A14, TEST_MONI_SRAM, MONI_BIT_SRAM_A14, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A13, TEST_MONI_SRAM, MONI_BIT_SRAM_A13, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A12, TEST_MONI_SRAM, MONI_BIT_SRAM_A12, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A11, TEST_MONI_SRAM, MONI_BIT_SRAM_A11, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A10, TEST_MONI_SRAM, MONI_BIT_SRAM_A10, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A9, TEST_MONI_SRAM, MONI_BIT_SRAM_A9, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A8, TEST_MONI_SRAM, MONI_BIT_SRAM_A8, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A7, TEST_MONI_SRAM, MONI_BIT_SRAM_A7, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A6, TEST_MONI_SRAM, MONI_BIT_SRAM_A6, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A5, TEST_MONI_SRAM, MONI_BIT_SRAM_A5, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A4, TEST_MONI_SRAM, MONI_BIT_SRAM_A4, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A3, TEST_MONI_SRAM, MONI_BIT_SRAM_A3, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A2, TEST_MONI_SRAM, MONI_BIT_SRAM_A2, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A1, TEST_MONI_SRAM, MONI_BIT_SRAM_A1, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM_A0, TEST_MONI_SRAM, MONI_BIT_SRAM_A0, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM1_CE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_CE_B, MONI, 0, 0 },
	{ TEST_CTRL_SRAM1_OE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_OE_B, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM1_WE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_WE_B, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM1_BHE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_BHE_B, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM1_BLE_B, TEST_MONI_SRAM, MONI_BIT_SRAM1_BLE_B, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM2_CE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_CE_B, MONI, 0, 0 },
	{ TEST_CTRL_SRAM2_OE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_OE_B, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM2_WE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_WE_B, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM2_BHE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_BHE_B, CTRL, 0, 0 },
	{ TEST_CTRL_SRAM2_BLE_B, TEST_MONI_SRAM, MONI_BIT_SRAM2_BLE_B, CTRL, 0, 0 },

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

static uint32_t sram_err_init_status;
static uint32_t sram_io_init_status;

static void setup_memory_bridge_test(void)
{
	// set all test pins to GPIO IN mode and store default status
	for(int i = 0; i < ARRAY_SIZE(memory_targets); i++){
		struct bridge_test_regs* target = &memory_targets[i];

		target->orig_mode = get_test_gpio_mode(target->ctrl_offset);
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_IN);
		target->init_status = get_test_moni_status(
						target->moni_offset, target->moni_bitpos);
	}

	sram_err_init_status = (read32(TEST_REG_ADDR(TEST_MONI_SRAM_ERR)) & 0x3);
	sram_io_init_status = read32(TEST_REG_ADDR(TEST_MONI_SRAM_IO));
}

static void cleanup_memory_bridge_test(void)
{
	// restore original mode
	for(int i = 0; i < ARRAY_SIZE(memory_targets); i++){
		struct bridge_test_regs* target = &memory_targets[i];

		set_test_gpio_mode(target->ctrl_offset, target->orig_mode);
	}
}

static uint32_t check_sram_err_unchanged()
{
	uint32_t err_count = 0;

	if(sram_err_init_status != (read32(TEST_REG_ADDR(TEST_MONI_SRAM_ERR)) & 0x3)){
		err("sram err status changed, moni addr: 0x%08X, cur: 0x%08x, init: 0x%08x\n",
					TEST_REG_ADDR(TEST_MONI_SRAM_ERR),
				   	read32(TEST_REG_ADDR(TEST_MONI_SRAM_ERR)),
					sram_err_init_status);
		err_count++;
	}

	return err_count;
}

static uint32_t check_sram_io_unchanged()
{
	uint32_t err_count = 0;

	if(sram_io_init_status != read32(TEST_REG_ADDR(TEST_MONI_SRAM_IO))){
		err("sram data changed, moni addr: 0x%08X, cur: 0x%08x, init: 0x%08x\n",
					TEST_REG_ADDR(TEST_MONI_SRAM_IO),
				   	read32(TEST_REG_ADDR(TEST_MONI_SRAM_IO)),
					sram_err_init_status);
		err_count++;
	}

	return err_count;
}

static uint32_t check_others_unchanged(uint32_t self_index)
{
	// check if each status is same as default status except current target
	uint32_t err_count = 0;

	for(int i = 0; i < ARRAY_SIZE(memory_targets); i++){
		if(i == self_index) continue; // skip the controlled pin itself

		struct bridge_test_regs *target = &memory_targets[i];
		if(!check_test_moni_status(
						target->moni_offset,
					   	target->moni_bitpos,
					   	target->init_status))
		{
			err_count++;
		}
	}

	err_count += check_sram_err_unchanged();
	err_count += check_sram_io_unchanged();

	return err_count;
}

uint32_t memory_bridge_test(uint32_t test_no)
{
	/*
	 * This test is to check bridge between all target pins.
	 *
	 * 1. set all test target pins to input mode and save init status
	 * 2. select one pin to set high output and monitor if it's high
	 * 3. check all other pins if they stay init status
	 * 4. set the pin low output and doing same when setting high.
	 * 5. go back to 2. until all pins are tested
	 */

	int err_count = 0;
	info("* Start Memory Bridge Test\n");
	setup_memory_bridge_test();

	for(int i = 0; i < ARRAY_SIZE(memory_targets); i++){
		struct bridge_test_regs *target = &memory_targets[i];

		if(target->moni_only) continue;

		// Set High
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_OUT_HIGH);
		if(!test_moni_status_high(target->moni_offset, target->moni_bitpos)){
			err_count++;
		}
		err_count += check_others_unchanged(i); // pass self index to avoid

		// Set Low
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_OUT_LOW);
		if(!test_moni_status_low(target->moni_offset, target->moni_bitpos)){
			err_count++;
		}
		err_count += check_others_unchanged(i);

		// Set back to Input for next testing
		set_test_gpio_mode(target->ctrl_offset, TEST_GPIO_IN);
	}

	cleanup_memory_bridge_test();
	info("found bridge count: %d\n", err_count);

	return err_count;
}
