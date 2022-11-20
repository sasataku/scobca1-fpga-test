/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/console/console.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "watchdog.h"
#include "i2c_test.h"
#include "hrmem_test.h"
#include "qspi_common.h"
#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"
#include "can_test.h"
#include "bhm_test.h"
#include "system_reg.h"

enum ScTestNo {
	SC_TEST_QSPI_INIT = 1,
	SC_TEST_INTERNAL_I2C,
	SC_TEST_HRMEM,
	SC_TEST_QSPI_DATA_MEM,
	SC_TEST_QSPI_DATA_MEM_SECTOR,
	SC_TEST_QSPI_FRAM,
	SC_TEST_CAN,
	SC_TEST_BOARD_HEALTH_MONITOR,
};

void print_menu(void)
{
	info("[%d] QSPI Initialize\n", SC_TEST_QSPI_INIT);
	info("[%d] Internal I2C Test\n", SC_TEST_INTERNAL_I2C);
	info("[%d] HRMEM Test\n", SC_TEST_HRMEM);
	info("[%d] QSPI Data Memory Test\n", SC_TEST_QSPI_DATA_MEM);
	info("[%d] QSPI Data Memory Test (Sector)\n", SC_TEST_QSPI_DATA_MEM_SECTOR);
	info("[%d] QSPI FRAM Test\n", SC_TEST_QSPI_FRAM);
	info("[%d] CAN Test\n", SC_TEST_CAN);
	info("[%d] Board Health Monitor Test\n", SC_TEST_BOARD_HEALTH_MONITOR);
}

void main(void)
{
	char *s;
	uint32_t test_no;

	start_kick_wdt_thread();
	console_getline_init();

	info("This is the FPGA test program for SC-OBC-A1\n");
	info("\n");
	info("* System Register IP Version : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_VER));
	info("* Build Information          : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_BUILDINFO));
	info("* Device DNA 1               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA1));
	info("* Device DNA 2               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA2));
	info("\n");
	info("Please input `h` to show the test program menu\n");
	info("\n");

	while (true) {

		info("> ");

		s = console_getline();
		if (strcmp(s, "h") == 0) {
			print_menu();
			continue;
		}

		test_no = strtol(s, NULL, 10);
		switch (test_no) {
		case SC_TEST_QSPI_INIT: 
			qspi_init(test_no);
			break;
		case SC_TEST_INTERNAL_I2C:
			internal_i2c_test(test_no);
			break;
		case SC_TEST_HRMEM:
			hrmem_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM:
			qspi_data_memory_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM_SECTOR:
			qspi_data_memory_sector_test(test_no);
			break;
		case SC_TEST_QSPI_FRAM:
			qspi_fram_test(test_no);
			break;
		case SC_TEST_CAN:
			can_test(test_no);
			break;
		case SC_TEST_BOARD_HEALTH_MONITOR:
			bhm_test(test_no);
			break;
		default:
			break;
		}
	}
}
