/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/console/console.h>
#include <string.h>
#include "watchdog.h"
#include "i2c_test.h"
#include "hrmem_test.h"
#include "qspi_common.h"
#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"
#include "can_test.h"

void print_menu()
{
	printk("[1] Internal I2C Test\n");
	printk("[2] HRMEM Test\n");
	printk("[3] QSPI Data Memory Test\n");
	printk("[4] QSPI FRAM Test\n");
	printk("[5] CAN Test\n");
}

void main(void)
{
	char *s;

	start_kick_wdt_thread();
	console_getline_init();

	printk("This is the FPGA test program for SC-OBC-A1\n");
	printk("Please input `h` to show the test program menu\n");

	while (true) {

		printk("> ");

		s = console_getline();
		if (strcmp(s, "h") == 0) {
			print_menu();
		} else if (strcmp(s, "1") == 0) {
			qspi_init();
		} else if (strcmp(s, "2") == 0) {
			hrmem_test();
		} else if (strcmp(s, "3") == 0) {
			qspi_data_memory_test();
		} else if (strcmp(s, "4") == 0) {
			qspi_fram_test();
		} else if (strcmp(s, "5") == 0) {
			can_test();
		}
	}
}
