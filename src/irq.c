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
#include "can.h"
#include "system_monitor_reg.h"

#define IRQ_PRIO (2u)
#define HRMEM_IER_ALL            (0x00000103)
#define CAN_ISR_TXDONE_MASK      (0x00000001)
#define CAN_ISR_RXDONE_MASK      (0x00000030)
#define CAN_ISR_ERR_MASK         (0x00003FCE)
#define CAN_IER_ALL              (0x00003FFF)
#define SYSMON_HW_IER_ALL        (0x00000F9F)
#define SYSMON_BHM_ISR_INIT_MASK (0x00000001)
#define SYSMON_BHM_ISR_SWA_MASK  (0x00000002)
#define SYSMON_BHM_ISR_ERR_MASK  (0x00073F00)

uint32_t irq_err_cnt = 0;

extern bool i2c_initalized;
extern bool i2c_sw_access_done;
extern uint32_t last_i2c_isr;

void hrmem_irq_cb(void *arg)
{
	uint32_t isr = sys_read32(SCOBCA1_FPGA_HRMEM_INTSTR);

	/* HRMEM ISR is all error bit */
	err("  !!! Assertion failed: Invalid HRMEM ISR: 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_HRMEM_INTSTR, isr);
	irq_err_cnt++;
}

void can_irq_cb(void *arg)
{
	uint32_t isr = sys_read32(SCOBCA1_FPGA_CAN_ISR);

	/*
	 * Disable ISR, but In the case of CAN abnormality test,
	 * a large amount of ISR log is output, so debug logs
	 * are not output.
	 */
	sys_write32(isr, SCOBCA1_FPGA_CAN_ISR);

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
	last_i2c_isr = isr;

	debug("* BHM ISR 0x%08x\n", isr);
	write32(SCOBCA1_FPGA_SYSMON_BHM_ISR, isr);

	/* Check I2C Init done bit */
	if ((isr & SYSMON_BHM_ISR_INIT_MASK) != 0) {
		i2c_initalized = true;
	}

	/* Check I2C SW access done */
	if ((isr & SYSMON_BHM_ISR_SWA_MASK) != 0) {
		i2c_sw_access_done = true;;
	}

	/* Check error bit */
	if ((isr & SYSMON_BHM_ISR_ERR_MASK) != 0) {
		irq_err_cnt++;
		err("  !!! Assertion failed: Invalid System Monitor (BHM) ISR: 0x%08x\n", isr);
	}
}

void irq_init(void) {
	/* Enable All IRQ */
	IRQ_CONNECT(IRQ_NO_HRMEM, IRQ_PRIO, hrmem_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_HRMEM);

	IRQ_CONNECT(IRQ_NO_CAN, IRQ_PRIO, can_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_CAN);

	IRQ_CONNECT(IRQ_NO_SYSMON_HW, IRQ_PRIO, sysmon_hw_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_SYSMON_HW);

	IRQ_CONNECT(IRQ_NO_SYSMON_BHM, IRQ_PRIO, sysmon_bhm_irq_cb, NULL, 0);
	irq_enable(IRQ_NO_SYSMON_BHM);

	/* Enable IER Ragister */
	write32(SCOBCA1_FPGA_HRMEM_INTENR, HRMEM_IER_ALL);
	write32(SCOBCA1_FPGA_CAN_IER, CAN_IER_ALL);

	write32(SCOBCA1_FPGA_SYSMON_INT_ENABLE, SYSMON_HW_IER_ALL);
	/* BHM IER is enabled by bhm_test.c */

	/* Enable HRMEM Scrubing */
	write32(SCOBCA1_FPGA_HRMEM_ECCCOLENR, 0x01);
}
