/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "user_io_crack_test.h"

/*
 * make USER IO pins pair to test
 */
static struct user_io_test_regs user_io_pairs[] =
{
    // user io 1/2
    { TEST_CTRL_UIO1_00 ,TEST_CTRL_UIO2_00, TEST_MONI_USER_IO1, MONI_BIT_UIO1_00 },
    { TEST_CTRL_UIO1_01 ,TEST_CTRL_UIO2_01, TEST_MONI_USER_IO1, MONI_BIT_UIO1_01 },
    { TEST_CTRL_UIO1_02 ,TEST_CTRL_UIO2_02, TEST_MONI_USER_IO1, MONI_BIT_UIO1_02 },
    { TEST_CTRL_UIO1_03 ,TEST_CTRL_UIO2_03, TEST_MONI_USER_IO1, MONI_BIT_UIO1_03 },
    { TEST_CTRL_UIO1_04 ,TEST_CTRL_UIO2_04, TEST_MONI_USER_IO1, MONI_BIT_UIO1_04 },
    { TEST_CTRL_UIO1_05 ,TEST_CTRL_UIO2_05, TEST_MONI_USER_IO1, MONI_BIT_UIO1_05 },
    { TEST_CTRL_UIO1_06 ,TEST_CTRL_UIO2_06, TEST_MONI_USER_IO1, MONI_BIT_UIO1_06 },
    { TEST_CTRL_UIO1_07 ,TEST_CTRL_UIO2_07, TEST_MONI_USER_IO1, MONI_BIT_UIO1_07 },
    { TEST_CTRL_UIO1_08 ,TEST_CTRL_UIO2_08, TEST_MONI_USER_IO1, MONI_BIT_UIO1_08 },
    { TEST_CTRL_UIO1_09 ,TEST_CTRL_UIO2_09, TEST_MONI_USER_IO1, MONI_BIT_UIO1_09 },
    { TEST_CTRL_UIO1_10 ,TEST_CTRL_UIO2_10, TEST_MONI_USER_IO1, MONI_BIT_UIO1_10 },
    { TEST_CTRL_UIO1_11 ,TEST_CTRL_UIO2_11, TEST_MONI_USER_IO1, MONI_BIT_UIO1_11 },
    { TEST_CTRL_UIO1_12 ,TEST_CTRL_UIO2_12, TEST_MONI_USER_IO1, MONI_BIT_UIO1_12 },
    { TEST_CTRL_UIO1_13 ,TEST_CTRL_UIO2_13, TEST_MONI_USER_IO1, MONI_BIT_UIO1_13 },
    { TEST_CTRL_UIO1_14 ,TEST_CTRL_UIO2_14, TEST_MONI_USER_IO1, MONI_BIT_UIO1_14 },
    { TEST_CTRL_UIO1_15 ,TEST_CTRL_UIO2_15, TEST_MONI_USER_IO1, MONI_BIT_UIO1_15 },
    // user io 4
    { TEST_CTRL_UIO4_06 ,TEST_CTRL_UIO4_09, TEST_MONI_USER_IO4, MONI_BIT_UIO4_06 },
    { TEST_CTRL_UIO4_07 ,TEST_CTRL_UIO4_10, TEST_MONI_USER_IO4, MONI_BIT_UIO4_07 },
    { TEST_CTRL_UIO4_08 ,TEST_CTRL_UIO4_11, TEST_MONI_USER_IO4, MONI_BIT_UIO4_08 }
};

uint32_t user_io_crack_test(uint32_t test_no)
{
	printk("*** User IO crack test starts ***\n");
    uint32_t err_num = 0;

    // control UIO2 and use UIO1 as an input, for UIO4 see array above.
    uint32_t test_item_num = sizeof(user_io_pairs) / sizeof(struct user_io_test_regs);

    for(int i = 0; i < test_item_num; i++){
        struct user_io_test_regs *pair = &user_io_pairs[i];
        err_num += test_paired_pins_connection(
						pair->in_ctrl_reg,
						pair->out_ctrl_reg,
						pair->in_moni_reg,
						pair->moni_bitpos
						);
    }
	printk("*** test done, error count: %d ***\n", err_num);

    return err_num;
}
