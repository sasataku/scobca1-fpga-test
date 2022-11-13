/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/console/console.h>

void main(void)
{
	char *s;

	console_getline_init();

	while (true) {

		printk("> ");

		s = console_getline();

		printk("%s\n", s);
	}
}

