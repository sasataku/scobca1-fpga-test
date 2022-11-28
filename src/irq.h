/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_IRQ_H_
#define SCOBCA1_FPGA_TEST_IRQ_H_

#include <zephyr/kernel.h>

enum ScObcA1IrqNo {
	IRQ_NO_UART = 0,
	IRQ_NO_HRMEM,
	IRQ_NO_QSPI_CFG,
	IRQ_NO_QSPI_DATA,
	IRQ_NO_QSPI_FRAM,
	IRQ_NO_CAN,
	IRQ_NO_INTERNAL_I2C,
	IRQ_NO_EXTERNAL_I2C,
	IRQ_NO_SYSMON_HW,
	IRQ_NO_SYSMON_BHM,
};

void irq_init(void);

#endif /* SCOBCA1_FPGA_TEST_IRQ_H_ */
