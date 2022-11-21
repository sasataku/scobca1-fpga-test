/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/console/console.h>
#include <string.h>
#include <stdlib.h>
#include "watchdog.h"
#include "i2c_test.h"
#include "hrmem_test.h"
#include "qspi_common.h"
#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"
#include "usb_crack_test.h"
#include "pudc_crack_test.h"
#include "sys_clock_crack_test.h"
#include "i2c_ext_crack_test.h"
#include "user_io_crack_test.h"
#include "sram_addr_crack_test.h"
#include "sram_byte_crack_test.h"
#include "bridge_test.h"
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
	SC_TEST_CRACK_PUDC,
	SC_TEST_CRACK_USER_IO,
	SC_TEST_CRACK_I2C_EXT,
	SC_TEST_CRACK_SYS_CLOCK,
	SC_TEST_CRACK_USB,
	SC_TEST_CRACK_SRAM_ADDR,
	SC_TEST_CRACK_SRAM_BYTE,
	SC_TEST_BRIDGE,
};

void print_menu(void)
{
	printk("[%d] QSPI Initialize\n", SC_TEST_QSPI_INIT);
	printk("[%d] Internal I2C Test\n", SC_TEST_INTERNAL_I2C);
	printk("[%d] HRMEM Test\n", SC_TEST_HRMEM);
	printk("[%d] QSPI Data Memory Test\n", SC_TEST_QSPI_DATA_MEM);
	printk("[%d] QSPI Data Memory Test (Sector)\n", SC_TEST_QSPI_DATA_MEM_SECTOR);
	printk("[%d] QSPI FRAM Test\n", SC_TEST_QSPI_FRAM);
	printk("[%d] CAN Test\n", SC_TEST_CAN);
	printk("[%d] Board Health Monitor Test\n", SC_TEST_BOARD_HEALTH_MONITOR);
	printk("[%d] PUDC crack Test\n", SC_TEST_CRACK_PUDC);
	printk("[%d] User IO crack Test\n", SC_TEST_CRACK_USER_IO);
	printk("[%d] External I2C crack Test\n", SC_TEST_CRACK_I2C_EXT);
	printk("[%d] System Clock crack Test\n", SC_TEST_CRACK_SYS_CLOCK);
	printk("[%d] USB crack Test\n", SC_TEST_CRACK_USB);
	printk("[%d] SRAM addr crack Test\n", SC_TEST_CRACK_SRAM_ADDR);
	printk("[%d] SRAM byte crack Test\n", SC_TEST_CRACK_SRAM_BYTE);
	printk("[%d] Bridge Test\n", SC_TEST_BRIDGE);
}

void main(void)
{
	char *s;
	uint32_t test_no;

	start_kick_wdt_thread();
	console_getline_init();

	printk("This is the FPGA test program for SC-OBC-A1\n");
	printk("\n");
	printk("* System Register IP Version : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_VER));
	printk("* Build Information          : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_BUILDINFO));
	printk("* Device DNA 1               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA1));
	printk("* Device DNA 2               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA2));
	printk("\n");
	printk("Please input `h` to show the test program menu\n");
	printk("\n");

	while (true) {

		printk("> ");

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
		case SC_TEST_BRIDGE:
			bridge_test(test_no);
			break;
		case SC_TEST_CRACK_USB:
			usb_crack_test(test_no);
			break;
		case SC_TEST_CRACK_I2C_EXT:
			i2c_ext_crack_test(test_no);
			break;
		case SC_TEST_CRACK_PUDC:
			pudc_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SYS_CLOCK:
			sys_clock_crack_test(test_no);
			break;
		case SC_TEST_CRACK_USER_IO:
			user_io_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SRAM_ADDR:
			sram_addr_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SRAM_BYTE:
			sram_byte_crack_test(test_no);
			break;
		default:
			break;
		}
	}
}
