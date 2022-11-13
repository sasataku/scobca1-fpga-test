/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/console/console.h>
#include <string.h>
#include "i2c_test.h"
#include "hrmem_test.h"

void print_menu()
{
	printk("[1] Internal I2C Test\n");
	printk("[2] HRMEM Test\n");
}

void main(void)
{
	char *s;

	console_getline_init();

	printk("This is the FPGA test program for SC-OBC-A1\n");
	printk("Please input `h` to show the test program menu\n");

	while (true) {

		printk("> ");

		s = console_getline();
		if (strcmp(s, "h") == 0) {
			print_menu();
		} else if (strcmp(s, "1") == 0) {
			internal_i2c_test();
		} else if (strcmp(s, "2") == 0) {
			hrmem_test();
		}
	}
}

