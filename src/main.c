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
#include "irq.h"
#include "watchdog.h"
#include "i2c_test.h"
#include "hrmem_test.h"
#include "qspi_common.h"
#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"
#include "usb_crack_test.h"
#include "pudc_crack_test.h"
#include "sys_clock_crack_test.h"
#include "user_io_crack_test.h"
#include "sram_addr_crack_test.h"
#include "sram_byte_crack_test.h"
#include "sram_err_crack_test.h"
#include "sram_data_crack_test.h"
#include "bridge_test.h"
#include "can_test.h"
#include "bhm_test.h"
#include "system_reg.h"
#include "longrun_test.h"
#include "trch_test.h"
#include "hardware_options_test.h"
#include "pdi.h"

enum ScTestNo {
	SC_TEST_QSPI_INIT = 1,
	SC_TEST_LONG_RUN,
	SC_TEST_HRMEM,
	SC_TEST_QSPI_CFG_MEM,
	SC_TEST_QSPI_CFG_MEM_SECTOR,
	SC_TEST_QSPI_CFG_MEM_BLOCK,
	SC_TEST_QSPI_DATA_MEM,
	SC_TEST_QSPI_DATA_MEM_SECTOR,
	SC_TEST_QSPI_DATA_MEM_BLOCK,
	SC_TEST_QSPI_FRAM,
	SC_TEST_CAN,
	SC_TEST_CAN_SEND_CMD,
	SC_TEST_BOARD_HEALTH_MONITOR,
	SC_TEST_CRACK_PUDC,
	SC_TEST_CRACK_USER_IO,
	SC_TEST_CRACK_SYS_CLOCK,
	SC_TEST_CRACK_USB,
	SC_TEST_CRACK_SRAM_ADDR,
	SC_TEST_CRACK_SRAM_BYTE,
	SC_TEST_CRACK_SRAM_ERR,
	SC_TEST_CRACK_SRAM_DATA,
	SC_TEST_BRIDGE,
	SC_TEST_TRCH,
	SC_TEST_CRACK_CAN,
	SC_TEST_CRACK_I2C_INTERNAL,
	SC_TEST_TRCH_CFG_MEM_MONI,
	SC_TEST_HARDWARE_OPTIONS,
	SC_TEST_PDI = 99,
};

bool is_exit;

void print_menu(void)
{
	info("[%d] QSPI Initialize\n", SC_TEST_QSPI_INIT);
	info("[%d] Long Run Test\n", SC_TEST_LONG_RUN);
	info("[%d] HRMEM Test\n", SC_TEST_HRMEM);
	info("[%d] QSPI Config Memory Test (only 16byte)\n", SC_TEST_QSPI_CFG_MEM);
	info("[%d] QSPI Config Memory Test (Sector)\n", SC_TEST_QSPI_CFG_MEM_SECTOR);
	info("[%d] QSPI Config Memory Test (Block)\n", SC_TEST_QSPI_CFG_MEM_BLOCK);
	info("[%d] QSPI Data Memory Test (only 16byte)\n", SC_TEST_QSPI_DATA_MEM);
	info("[%d] QSPI Data Memory Test (Sector)\n", SC_TEST_QSPI_DATA_MEM_SECTOR);
	info("[%d] QSPI Data Memory Test (Block)\n", SC_TEST_QSPI_DATA_MEM_BLOCK);
	info("[%d] QSPI FRAM Test\n", SC_TEST_QSPI_FRAM);
	info("[%d] CAN Test\n", SC_TEST_CAN);
	info("[%d] CAN send command to TRCH\n", SC_TEST_CAN_SEND_CMD);
	info("[%d] Board Health Monitor Test\n", SC_TEST_BOARD_HEALTH_MONITOR);

	/* for crack and bridge testing */
	info("[%d] PUDC crack Test\n", SC_TEST_CRACK_PUDC);
	info("[%d] User IO crack Test\n", SC_TEST_CRACK_USER_IO);
	info("[%d] System Clock crack Test\n", SC_TEST_CRACK_SYS_CLOCK);
	info("[%d] USB crack Test\n", SC_TEST_CRACK_USB);
	info("[%d] SRAM addr crack Test\n", SC_TEST_CRACK_SRAM_ADDR);
	info("[%d] SRAM byte crack Test\n", SC_TEST_CRACK_SRAM_BYTE);
	info("[%d] SRAM err crack Test\n", SC_TEST_CRACK_SRAM_ERR);
	info("[%d] SRAM data crack Test\n", SC_TEST_CRACK_SRAM_DATA);
	info("[%d] Bridge Test\n", SC_TEST_BRIDGE);

	info("[%d] Shared Pin Test\n", SC_TEST_TRCH);
	info("[%d] CAN crack Test\n", SC_TEST_CRACK_CAN);
	info("[%d] Internal I2C crack Test\n", SC_TEST_CRACK_I2C_INTERNAL);
	info("[%d] Config Memory TRCH_CFG_MEM_MONI Test\n", SC_TEST_TRCH_CFG_MEM_MONI);
	info("[%d] Hardware Option Pin Test\n", SC_TEST_HARDWARE_OPTIONS);
	info("[%d] Pre Delivery Inspection\n", SC_TEST_PDI);
}

static void print_ids(void)
{
	info("\n");
	info("* System Register IP Version : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_VER));
	info("* Build Information          : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_BUILDINFO));
	info("* Device DNA 1               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA1));
	info("* Device DNA 2               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA2));
	info("\n");
}

#ifndef CONFIG_AUTO_RUN_TEST_NUMBER
#define AUTO_RUN_TEST_NUMBER (-1)
#else
#define AUTO_RUN_TEST_NUMBER (CONFIG_AUTO_RUN_TEST_NUMBER)
#endif

void main(void)
{
	char *s;
	uint32_t test_no;

	start_kick_wdt_thread();
	irq_init();
	console_getline_init();

	info("This is the FPGA test program for SC-OBC-A1\n");
	print_ids();
	info("Please input `h` to show the test program menu\n");
	info("\n");

	while (true) {

		if (IS_ENABLED(CONFIG_AUTO_RUN)) {
			test_no = AUTO_RUN_TEST_NUMBER;
		}
		else {
			info("> ");

			s = console_getline();
			if (strcmp(s, "h") == 0) {
				print_menu();
				continue;
			} else if (strcmp(s, "q") == 0) {
				is_exit = true;
				continue;
			}

			test_no = strtol(s, NULL, 10);
		}

		switch (test_no) {
		case SC_TEST_QSPI_INIT:
			qspi_init(test_no);
			break;
		case SC_TEST_LONG_RUN:
			longrun_test(test_no);
			break;
		case SC_TEST_HRMEM:
			hrmem_test(test_no);
			break;
		case SC_TEST_QSPI_CFG_MEM:
			qspi_config_memory_test(test_no);
			break;
		case SC_TEST_QSPI_CFG_MEM_SECTOR:
			qspi_config_memory_sector_test(test_no);
			break;
		case SC_TEST_QSPI_CFG_MEM_BLOCK:
			qspi_config_memory_block_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM:
			qspi_data_memory_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM_SECTOR:
			qspi_data_memory_sector_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM_BLOCK:
			qspi_data_memory_block_test(test_no);
			break;
		case SC_TEST_QSPI_FRAM:
			qspi_fram_test(test_no);
			break;
		case SC_TEST_CAN:
			can_test(test_no);
			break;
		case SC_TEST_CAN_SEND_CMD:
			can_send_cmd(test_no);
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
		case SC_TEST_CRACK_SRAM_ERR:
			sram_err_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SRAM_DATA:
			sram_data_crack_test(test_no);
			break;
		case SC_TEST_TRCH:
			trch_test();
			break;
		case SC_TEST_CRACK_CAN:
			can_crack_test(test_no);
			break;
		case SC_TEST_CRACK_I2C_INTERNAL:
			i2c_internal_crack_test(test_no);
			break;
		case SC_TEST_TRCH_CFG_MEM_MONI:
			qspi_config_memory_trch_moni_test(test_no);
			break;
		case SC_TEST_HARDWARE_OPTIONS:
			hardware_options_test();
			break;
		case SC_TEST_PDI:
			start_pdi(test_no);
			print_ids();
			/* Exit after Pre Delivery Inspection */
			return;
		default:
			break;
		}
	}
}
