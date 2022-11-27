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

uint32_t user_io_crack_test(uint32_t test_no)
{
    /*
     * Test if external lines connection. Connect any two lines outside of the
     * board, Then control signal level of one line and see if it changes by the
     * other one.
     */

    info("*** User IO crack test starts ***\n");
    uint32_t err_count = 0;

    /* control UIO2 and use UIO1 as an input (default) */
    uint32_t test_item_num =
               sizeof(user_io_pairs) / sizeof(struct loopback_test_regs);

    for(int i = 0; i < test_item_num; i++){
        err_count += test_paired_pins_connection(&user_io_pairs[i]);
    }
    info("*** test done, error count: %d ***\n", err_count);

    return err_count;
}
