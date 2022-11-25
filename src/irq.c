/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/irq.h>
#include "irq.h"
#include "common.h"
#include "hrmem_test.h"
#include "qspi_common.h"
#include "can_test.h"
#include "system_monitor_reg.h"

#define IRQ_PRIO (2u)
#define HRMEM_IER_ALL         (0x00000103)
#define CAN_ISR_TXDONE_MASK   (0x00000001)
#define CAN_ISR_RXDONE_MASK   (0x00000030)
#define CAN_ISR_ERR_MASK      (0x00003FCE)
#define CAN_IER_ALL           (0x00003FFF)
#define QSPI_IER_ALL          (0x07070001)
#define QSPI_ISR_SPIDONE_MASK (0x00000001)
#define QSPI_ISR_ERR_MASK     (0x07070000)
#define SYSMON_HW_IER_ALL     (0x00000F9F)
#define SYSMON_BHM_IER_ALL    (0x00073F03)

uint32_t irq_err_cnt = 0;

extern bool can_tx_done;
extern bool can_rx_done;
extern bool qspi_norflash_done;
extern bool qspi_fram_done;
extern bool first_can_err_isr;

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

void hrmem_irq_cb(void *arg)
{
	uint32_t isr = sys_read32(SCOBCA1_FPGA_HRMEM_INTSTR);

	/* HRMEM ISR is all error bit */
	err("  !!! Assertion failed: Invalid HRMEM ISR: 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_HRMEM_INTSTR, isr);
	irq_err_cnt++;
}

void qspi_cfg_irq_cb(void *arg)
{
	uint32_t base = SCOBCA1_FPGA_CFG_BASE_ADDR;
	uint32_t isr = sys_read32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base));

	debug("* QSPI CFG ISR 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base), isr);

	/* Check SPI DONE bit */
	if ((isr & QSPI_ISR_SPIDONE_MASK) != 0) {
		qspi_norflash_done = true;
	}
	/* Check error bit */
	if ((isr & QSPI_ISR_ERR_MASK) != 0) {
		irq_err_cnt++;
		err("  !!! Assertion failed: Invalid QSPI NOR Flash (Config) ISR: 0x%08x\n", isr);
	}
}

void qspi_data_irq_cb(void *arg)
{
	uint32_t base = SCOBCA1_FPGA_DATA_BASE_ADDR;
	uint32_t isr = sys_read32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base));

	debug("* QSPI DATA ISR 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base), isr);

	/* Check SPI DONE bit */
	if ((isr & QSPI_ISR_SPIDONE_MASK) != 0) {
		qspi_norflash_done = true;
	}
	/* Check error bit */
	if ((isr & QSPI_ISR_ERR_MASK) != 0) {
		irq_err_cnt++;
		err("  !!! Assertion failed: Invalid QSPI NOR Flash (Data) ISR: 0x%08x\n", isr);
	}
}

void qspi_fram_irq_cb(void *arg)
{
	uint32_t isr = sys_read32(SCOBCA1_FPGA_FRAM_QSPI_ISR);

	debug("* QSPI FRAM ISR 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_FRAM_QSPI_ISR, isr);

	/* Check SPI DONE bit */
	if ((isr & QSPI_ISR_SPIDONE_MASK) != 0) {
		qspi_fram_done = true;
	}
	/* Check error bit */
	if ((isr & QSPI_ISR_ERR_MASK) != 0) {
		irq_err_cnt++;
		err("  !!! Assertion failed: Invalid QSPI NOR Flash (FRAM) ISR: 0x%08x\n", isr);
	}
}

void can_irq_cb(void *arg)
{
	uint32_t isr = sys_read32(SCOBCA1_FPGA_CAN_ISR);

	debug("* CAN ISR 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_CAN_ISR, isr);

	/* Check TX DONE bit */
	if ((isr & CAN_ISR_TXDONE_MASK) != 0) {
		can_tx_done = true;
	}
	/* Check RX DONE / RX VALIDbit */
	if ((isr & CAN_ISR_RXDONE_MASK) != 0) {
		can_rx_done = true;
	}
	/* Check error bit */
	if ((isr & CAN_ISR_ERR_MASK) != 0) {
		irq_err_cnt++;
		if (!first_can_err_isr) {
			err("  !!! Assertion failed: Invalid CAN ISR: 0x%08x\n", isr);
			first_can_err_isr = true;
		}
	}
}

void sysmon_hw_irq_cb(void *arg)
{
	uint32_t isr = sys_read32(SCOBCA1_FPGA_SYSMON_INT_STATUS);

	/* System Monitor (HW) ISR is all error bit */
	err("  !!! Assertion failed: Invalid System Monitor (HW) ISR: 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_SYSMON_INT_STATUS, isr);
	irq_err_cnt++;
}

void sysmon_bhm_irq_cb(void *arg)
{
	uint32_t isr = sys_read32(SCOBCA1_FPGA_SYSMON_BHM_ISR);

	/* System Monitor (BHM) ISR is all error bit */
	err("  !!! Assertion failed: Invalid System Monitor (BHM) ISR: 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_SYSMON_BHM_ISR, isr);
	irq_err_cnt++;
}

void irq_init(void) {
	/* Enable All IRQ */
	IRQ_CONNECT(IRQ_NO_HRMEM, IRQ_PRIO, hrmem_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_HRMEM);

	IRQ_CONNECT(IRQ_NO_QSPI_CFG, IRQ_PRIO, qspi_cfg_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_QSPI_CFG);

	IRQ_CONNECT(IRQ_NO_QSPI_DATA, IRQ_PRIO, qspi_data_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_QSPI_DATA);
	
	IRQ_CONNECT(IRQ_NO_QSPI_FRAM, IRQ_PRIO, qspi_fram_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_QSPI_FRAM);

	IRQ_CONNECT(IRQ_NO_CAN, IRQ_PRIO, can_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_CAN);

	IRQ_CONNECT(IRQ_NO_SYSMON_HW, IRQ_PRIO, sysmon_hw_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_SYSMON_HW);

	IRQ_CONNECT(IRQ_NO_SYSMON_BHM, IRQ_PRIO, sysmon_bhm_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_SYSMON_BHM);

	/* Enable IER Ragister */
	write32(SCOBCA1_FPGA_HRMEM_INTENR, HRMEM_IER_ALL);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_IER(SCOBCA1_FPGA_CFG_BASE_ADDR), QSPI_IER_ALL);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_IER(SCOBCA1_FPGA_DATA_BASE_ADDR), QSPI_IER_ALL);
	write32(SCOBCA1_FPGA_FRAM_QSPI_IER, QSPI_IER_ALL);
	write32(SCOBCA1_FPGA_CAN_IER, CAN_IER_ALL);
	write32(SCOBCA1_FPGA_SYSMON_INT_ENABLE, SYSMON_HW_IER_ALL);
	write32(SCOBCA1_FPGA_SYSMON_BHM_IER, SYSMON_BHM_IER_ALL);

	/* Enable HRMEM Scrubing */
	write32(SCOBCA1_FPGA_HRMEM_ECCCOLENR, 0x01);
}
