/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "user_io_bridge_test.h"

static struct loopback_test_regs user_io_pairs[] =
{
    // INPUT: UIO2, OUTPUT: UIO1
    { TEST_CTRL_UIO2_00, TEST_CTRL_UIO1_00 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_00 },
    { TEST_CTRL_UIO2_01, TEST_CTRL_UIO1_01 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_01 },
    { TEST_CTRL_UIO2_02, TEST_CTRL_UIO1_02 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_02 },
    { TEST_CTRL_UIO2_03, TEST_CTRL_UIO1_03 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_03 },
    { TEST_CTRL_UIO2_04, TEST_CTRL_UIO1_04 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_04 },
    { TEST_CTRL_UIO2_05, TEST_CTRL_UIO1_05 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_05 },
    { TEST_CTRL_UIO2_06, TEST_CTRL_UIO1_06 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_06 },
    { TEST_CTRL_UIO2_07, TEST_CTRL_UIO1_07 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_07 },
    { TEST_CTRL_UIO2_08, TEST_CTRL_UIO1_08 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_08 },
    { TEST_CTRL_UIO2_09, TEST_CTRL_UIO1_09 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_09 },
    { TEST_CTRL_UIO2_10, TEST_CTRL_UIO1_10 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_10 },
    { TEST_CTRL_UIO2_11, TEST_CTRL_UIO1_11 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_11 },
    { TEST_CTRL_UIO2_12, TEST_CTRL_UIO1_12 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_12 },
    { TEST_CTRL_UIO2_13, TEST_CTRL_UIO1_13 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_13 },
    { TEST_CTRL_UIO2_14, TEST_CTRL_UIO1_14 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_14 },
    { TEST_CTRL_UIO2_15, TEST_CTRL_UIO1_15 ,TEST_MONI_USER_IO2, MONI_BIT_UIO2_15 },
};

static uint32_t init_user_io_mode(void)
{
    uint32_t err_count = 0;

    for(int i = 0; i < ARRAY_SIZE(user_io_pairs); i++){
		struct loopback_test_regs *pair = &user_io_pairs[i];
        set_test_gpio_mode(pair->in_ctrl_reg, TEST_GPIO_IN);
		set_test_gpio_mode(pair->out_ctrl_reg, TEST_GPIO_OUT_LOW);
		if(!test_moni_status_low(pair->in_moni_reg, pair->moni_bitpos)){
			err_count++;
        }
    }

    return err_count;
}

static uint32_t check_others_unchanged(const struct loopback_test_regs *self_pair)
{
    uint32_t err_count = 0;

    for(int i = 0; i < ARRAY_SIZE(user_io_pairs); i++){
		struct loopback_test_regs *pair = &user_io_pairs[i];
		if(self_pair->out_ctrl_reg == pair->out_ctrl_reg){
			continue; // self
		}
		if(!test_moni_status_low(pair->in_moni_reg, pair->moni_bitpos)){
			err_count++; // bridged
        }
    }

    return err_count;
}

uint32_t user_io_bridge_test(uint32_t test_no)
{
    /*
     * < Test method >
     * 1. Connect USER IO1 and IO2 pins respectively outside of SBC
     * 2. Set all USER IO2 pins to input, then USER IO1 pins to Out low
     * 3. Select one USER IO1 pin and set to High out
     * 4. Check whether all other IO2 pins stay Low
     * 5. Check whether the pin set to High is High
     * 6. Set the pin back to Low out
     * 7. Select next pin to test
     */

    info("* Start User IO Bridge Test\n");
    int err_count = init_user_io_mode();

    for(int i = 0; i < ARRAY_SIZE(user_io_pairs); i++){
	    struct loopback_test_regs *pair = &user_io_pairs[i];

        // Set High and check self and others
        set_test_gpio_mode(pair->out_ctrl_reg, TEST_GPIO_OUT_HIGH);
		if(!test_moni_status_high(pair->in_moni_reg, pair->moni_bitpos)){
			err_count++; // bridge with some pin?
		}
        err_count += check_others_unchanged(pair);
        set_test_gpio_mode(pair->out_ctrl_reg, TEST_GPIO_OUT_LOW);
    }

    info("found bridge count: %d\n", err_count);

    return err_count;
}
