/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "test_register.h"

uint32_t sram_data_crack_test(uint32_t test_no)
{
    printk("*** SRAM data crack test starts ***\n");
    uint32_t err_num = 0;

    volatile uint32_t target_mem = 0;

    if(target_mem){
        err_num++;
    }

    for(int i = 0; i < 32 ; i++){
        target_mem = 0x1 << i;
        if(target_mem != 0x1 << i){
        err_num++;
        }
    }

    printk("*** test done, error count: %d ***\n", err_num);

    return err_num;
}
