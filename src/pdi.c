/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "irq.h"
#include "common.h"
#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"
#include "bhm_test.h"
#include "can_test.h"
#include "can.h"
#include "hardware_options_test.h"
#include "pudc_crack_test.h"
#include "memory_bridge_test.h"
#include "sram_addr_crack_test.h"
#include "sram_byte_crack_test.h"
#include "sram_data_crack_test.h"
#include "sram_err_crack_test.h"
#include "sys_clock_crack_test.h"
#include "trch_test.h"
#include "usb_crack_test.h"
#include "user_io_bridge_test.h"
#include "user_io_crack_test.h"

void start_pdi(uint32_t test_no)
{
	uint32_t no = 1;

	/* Disable HRMEM IRQ */
	irq_disable(IRQ_NO_HRMEM);

	/* Wait a one seconds */
	k_sleep(K_MSEC(1000));

	info("* [%d] Start Pre Delivery Inspection\n", test_no);

	info("* [%d-1] Start Initalize NOR flash/FRAM\n", no);
	if (qspi_init(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	info("* [%d-2] Start Testing\n", no);

	/* Wait a one seconds */
	k_sleep(K_MSEC(1000));

	/* PUDC crack test */
	if (pudc_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* SYS_CLOCK crack test */
	if (sys_clock_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* User IO crack test */
	if (user_io_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* SRAM addr crack test */
	if (sram_addr_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* SRAM byte crack test */
	if (sram_byte_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/*  SRAM err crack test */
	if (sram_err_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* SRAM data crack test */
	if (sram_data_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* USB brack test */
	if (usb_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* I2C Alert signal Test */
	if (i2c_internal_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* TRCH_CFG_MEM_MONI Test */
	if (qspi_config_memory_trch_moni_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* Config Memory Test */
	if (qspi_config_memory_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* Data Memory Test */
	if (qspi_data_memory_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* FRAM Test */
	if (qspi_fram_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* User IO bridge test */
	if (user_io_bridge_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* Memory bridge test */
	if (memory_bridge_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* TRCH Shard pin test */
	if (trch_test() > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* Hardware options test */
	if (hardware_options_test() > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/*
	 * CAN crack Test (This test will failing CAN transmission intentinaly, so
	 * need to doing after the TRCH relating test
	 */
	if (can_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
	no++;

	/* Internal I2C (BHM) Test */
	if (bhm_test(no) > 0 ) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}

	info("* [%d] Finish Pre Delivery Inspection. (Successed) Total test num: %d\n",
			test_no, no);

end_of_test:
	return;
}
