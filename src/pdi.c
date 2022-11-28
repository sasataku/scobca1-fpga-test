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
#include "can.h"

void start_pdi(uint32_t test_no)
{
	uint32_t no = 1;

	/* Disable HRMEM IRQ */
	irq_disable(IRQ_NO_HRMEM);

	info("* [%d] Start Pre Delivery Inspection\n", test_no);


	info("* [%d-1] Start crack test\n", test_no);
	/* I2C Alert signal Test */
/*
	if (i2c_internal_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}
*/

	/* CAN crack Test */
	if (can_crack_test(no) > 0) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}

	info("* [%d-2] Start bridge test\n", test_no);

	info("* [%d-3] Start functional test\n", test_no);

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

	/* Internal I2C (BHM) Test */
	if (bhm_test(no) > 0 ) {
		err("* [%d] !!! Abort Pre Delivery Inspection\n", test_no);
		goto end_of_test;
	}

	info("* [%d] Finish Pre Delivery Inspection. (Successed)\n", test_no);

end_of_test:
	/* Enable HRMEM IRQ */
	irq_enable(IRQ_NO_HRMEM);
}
