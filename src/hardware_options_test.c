/*
 * Copyright (c) 2022 Space Cubics, LLC.
 * Yasushi SHOJI <yashi@spacecubics.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

#include "common.h"
#include "can.h"
#include "trch.h"
#include "test_register.h"

#define TRCH_TRISC_INIT (0x94)
#define TRCH_PORTC_INIT (0x00)
#define TRCH_TRISD_INIT (0x0F)
#define TRCH_PORTD_INIT (0x00)

static void init_portc()
{
	trch_set_trisc(TRCH_TRISC_INIT);
	trch_set_portc(TRCH_PORTC_INIT);
}

static void init_portd()
{
	trch_set_trisd(TRCH_TRISD_INIT);
	trch_set_portd(TRCH_PORTD_INIT);
}

/*
 * uio4_06__uio4_10_test
 *
 * We have UIO4_06 to UIO4_10 loop.
 *
 * Related pins are,
 *  - B2B pin 25 side
 *    - B2B Pin 24: GND
 *    - B2B Pin 25: UIO3_00      / UIO4_06
 *    - B2B Pin 26: UIO3_01      / UIO4_07
 *  - B2B pin 31 side
 *    - B2B Pin 30: TRCH_UART_TX / UIO4_09
 *    - B2B Pin 31: TRCH_UART_RX / UIO4_10
 *    - B2B Pin 32: SYS_3V3
 *  - TRCH side
 *    - TRCH Pin 1: TRCH_UART_RX (RC7) / UIO4_10
 *    - TRCH Pin 2: UIO3_00      (RD4) / UIO4_06
 *    - TRCH Pin 3: UIO3_01      (RD5) / UIO4_07
 *  - FPGA side
 *    - FPGA Pin  Y15: UIO4_06 / UIO3_00
 *    - FPGA Pin AE25: UIO4_10 / TRCH_UART_RX
 *
 * With these, the following pins have pull-up with SYS_3V3.
 *  - N/A
 *
 * And, the following pins have pull-up with VDD_3V3
 *  - TRCH_UART_RX
 *
 * And, the following pins have pull-down
 *  - N/A
 */
/* RC6 */
#define TRCH_UART_RX (7)
/* RD4 */
#define TRCH_UIO03_00 (4)

uint32_t uio4_06__uio4_10_test(void)
{
	uint32_t err_count = 0;
	uint32_t uio406_moni, uio410_moni;
	uint32_t prev_trch_uart_rx, trch_uart_rx;
	uint32_t prev_trch_uio300, trch_uio300;
	uint32_t fpga_uio_406;
	uint32_t fpga_uio_410;

	info("* Start of UIO4_06 to UIO04_10 loop test\n");

	/* Save current settings */
	uio406_moni = sys_read32(TEST_REG_ADDR(TEST_CTRL_UIO4_06));
	uio410_moni = sys_read32(TEST_REG_ADDR(TEST_CTRL_UIO4_10));

	/* TTRCH_UART_RX/UIO03_00 is not connected to any pins,
	 * so set the input and save the status */
	trch_set_trisc(set_in(TRCH_TRISC_INIT, TRCH_UART_RX));
	trch_set_trisd(set_in(TRCH_TRISD_INIT, TRCH_UIO03_00));
	prev_trch_uart_rx = get_bit(trch_get_portc(), TRCH_UART_RX);
	prev_trch_uio300 = get_bit(trch_get_portd(), TRCH_UIO03_00);

	info("* [#1] UIO4_06 Hi\n");
	/* Change to input all pin expected UIO4_06 */
	set_pin_input(TEST_CTRL_UIO4_10);

	/* Change output hi to UIO4_06 */
	set_pin_output_high(TEST_CTRL_UIO4_06);

	/*
	 * Check all pin level (expected All Hi without UART RX/UIO03_00)
	 */
	fpga_uio_406 = get_pin(TEST_MONI_USER_IO4, MONI_BIT_UIO4_06);
	fpga_uio_410 = get_pin(TEST_MONI_USER_IO4, MONI_BIT_UIO4_10);
	trch_uart_rx = get_bit(trch_get_portc(), TRCH_UART_RX);
	trch_uio300 = get_bit(trch_get_portd(), TRCH_UIO03_00);
	if (!(fpga_uio_406 && fpga_uio_410 &&
		trch_uart_rx == prev_trch_uart_rx && trch_uio300 == prev_trch_uio300)) {
		err("  !!! Assertion failed: UIO4_06 (Hi) to UIO04_10 loop failed\n");
		err("fpga_uio_406: 0x%x, fpga_uio_410: 0x%x, trch_uart_rx: 0x%x\n",
				fpga_uio_406, fpga_uio_410, trch_uart_rx);
		err("prev_trch_uart_rx: 0x%x, trch_uio300: 0x%x, prev_trch_uio300: 0x%x\n",
				prev_trch_uart_rx, trch_uio300, prev_trch_uio300);
		err_count++;
	}

	info("* [#2] UIO4_06 Low\n");

	/* Change output low to UIO4_06 */
	set_pin_output_low(TEST_CTRL_UIO4_06);

	/*
	 * Check all pin level (expected All Low without UART RX/UIO03_00)
	 */
	fpga_uio_406 = get_pin(TEST_MONI_USER_IO4, MONI_BIT_UIO4_06);
	fpga_uio_410 = get_pin(TEST_MONI_USER_IO4, MONI_BIT_UIO4_10);
	trch_uart_rx = get_bit(trch_get_portc(), TRCH_UART_RX);
	trch_uio300 = get_bit(trch_get_portd(), TRCH_UIO03_00);
	if (!(!fpga_uio_406 && !fpga_uio_410 &&
		trch_uart_rx == prev_trch_uart_rx && trch_uio300 == prev_trch_uio300)) {
		err("  !!! Assertion failed: UIO4_06 (Low) to UIO04_10 loop failed\n");
		err("fpga_uio_406: 0x%x, fpga_uio_410: 0x%x, trch_uart_rx: 0x%x\n",
				fpga_uio_406, fpga_uio_410, trch_uart_rx);
		err("prev_trch_uart_rx: 0x%x, trch_uio300: 0x%x, prev_trch_uio300: 0x%x\n",
				prev_trch_uart_rx, trch_uio300, prev_trch_uio300);
		err_count++;
	}

	/* Restore current settings */
	init_portc();
	init_portd();
	sys_write32(uio406_moni, TEST_REG_ADDR(TEST_CTRL_UIO4_06));
	sys_write32(uio410_moni, TEST_REG_ADDR(TEST_CTRL_UIO4_10));

	return err_count;
}

/*
 * uio4_07__wdog_out_test
 *
 * We have UIO4_07 to WDOG_OUT loop.
 *
 * Related pins are,
 *  - B2B pin 26 side
 *    - B2B Pin 25: UIO3_00 / UIO4_06
 *    - B2B Pin 26: UIO3_01 / UIO4_07
 *    - B2B Pin 27: UIO3_03 / UIO4_08
 *  - B2B pin 36 side
 *    - B2B Pin 35: I2C_EXT_SDA / I2C_FPGA_EXT_SDA
 *    - B2B Pin 36: WDOG_OUT    / UIO4_11
 *    - B2B Pin 37: VDD_3V3 OCP
 *  - TRCH side
 *    - TRCH Pin  2: UIO3_00           (RD4) / UIO4_06
 *    - TRCH Pin  3: UIO3_01           (RD5) / UIO4_07
 *    - TRCH Pin  4: UIO3_02           (RD6) / UIO4_08
 *    - TRCH Pin 24: FPGAPWR_EN        (RA5)
 *    - TRCH Pin 25: WDOG_OUT          (RE0) / UIO4_11
 *    - TRCH Pin 26: FPGA_PROGRAM_B_IN (RE1) / FPGA_PROGRAM_B
 *  - FPGA side
 *    - FPGA Pin AA15: UIO4_07 / UIO3_01
 *    - FPGA Pin AF18: UIO4_11 / WDOG_OUT
 *
 * With these, the following pins have pull-up with SYS_3V3.
 *  - N/A
 *
 * And, the following pins have pull-up with VDD_3V3
 *  - FPGA_PROGRAM_B_IN
 *
 * And, the following pins have pull-down
 *  - N/A
 */
uint32_t uio4_07__wdog_out_test(void)
{
	return 0;
}

uint32_t hardware_options_test(void)
{
	uint32_t err_count = 0;

	info("* Start of Hardware Option Pin Tests: Initializing\n");

	if (!can_init(false)) {
		assert();
	}

	err_count += uio4_06__uio4_10_test();
	err_count += uio4_07__wdog_out_test();

	if (!can_terminate(false)) {
		assert();
	}

	info("* End of Hardware Option Pin Tests: err %d\n", err_count);

	return err_count;
}
