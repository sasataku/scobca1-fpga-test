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
uint32_t uio4_06__uio4_10_test(void)
{
	return 0;
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

/*
 * uio4_08__i2c_fpga_ext_scl_tests
 *
 * We have UIO4_08 to I2C_FPGA_EXT_SCL loop.
 *
 * Related pins are,
 *  - B2B pin 27 side
 *    - B2B Pin 26: UIO3_01 / UIO4_07
 *    - B2B Pin 27: UIO3_03 / UIO4_08
 *    - B2B Pin 28: UIO4_00
 *  - B2B pin 36 side
 *    - B2B Pin 33: GND
 *    - B2B Pin 34: I2C_EXT_SCL / I2C_FPGA_EXT_SCL
 *    - B2B Pin 35: I2C_EXT_SDA / I2C_FPGA_EXT_SDA
 *  - TRCH side
 *    - TRCH Pin  3: UIO3_01      (RD5) / UIO4_07
 *    - TRCH Pin  4: UIO3_02      (RD6) / UIO4_08
 *    - TRCH Pin  5: FPGA_RESERVE (RD7)
 *    - TRCH Pin 39: I2C_INT_SDA  (RD1) / I2C_FPGA_INT_SDA
 *    - TRCH Pin 40: I2C_EXT_SCL  (RD2) / I2C_FPGA_EXT_SCL
 *    - TRCH Pin 41: I2C_EXT_SDA  (RD3) / I2C_FPGA_EXT_SDA
 *  - FPGA side
 *    - FPGA Pin  W18: UIO4_08          / UIO3_02
 *    - FPGA Pin AE22: I2C_FPGA_EXT_SCL / I2C_EXT_SCL
 *
 * With these, the following pins have pull-up with SYS_3V3.
 *  - I2C_INT_SDA
 *  - I2C_EXT_SCL
 *  - I2C_EXT_SDA
 *  - I2C_FPGA_EXT_SCL
 *
 * And, the following pins have pull-up with VDD_3V3
 *  - N/A
 *
 * And, the following pins have pull-down
 *  - N/A
 */
uint32_t uio4_08__i2c_fpga_ext_scl_tests(void)
{
	return 0;
}

/*
 * uio4_09__i2c_fpga_ext_sda_tests
 *
 * We have UIO4_09 to I2C_FPGA_EXT_SDA loop.
 *
 * Related pins are,
 *  - B2B pin 30 side
 *    - B2B Pin 29: GND
 *    - B2B Pin 30: TRCH_UART_TX / UIO4_09
 *    - B2B Pin 31: TRCH_UART_RX / UIO4_10
 *  - B2B pin 35 side
 *    - B2B Pin 34: I2C_EXT_SCL / I2C_FPGA_EXT_SCL
 *    - B2B Pin 35: I2C_EXT_SDL / I2C_FPGA_EXT_SDA
 *    - B2B Pin 36: WDOG_OUT    / muxed UIO4_11)
 *  - TRCH side
 *    - TRCH Pin 40: I2C_EXT_SCL  (RD2) / I2C_FPGA_EXT_SCL
 *    - TRCH Pin 41: I2C_EXT_SDA  (RD3) / I2C_FPGA_EXT_SDA
 *    - TRCH Pin 42: SPICAN_MISO  (RC4)
 *    - TRCH Pin 43: SPICAN_MOSI  (RC5)
 *    - TRCH Pin 44: TRCH_UART_TX (RC6) / UIO4_09
 *  - FPGA side
 *    - FPGA Pin  W15: UIO4_09          / TRCH_UART_TX
 *    - FPGA Pin AF22: I2C_FPGA_EXT_SDA / I2C_EXT_SDA
 *
 * With these, the following pins have pull-up with SYS_3V3.
 *  - I2C_EXT_SCL
 *  - I2C_EXT_SDA
 *
 * And, the following pins have pull-up with VDD_3V3
 *  - TRCH_UART_RX
 *
 * And, the following pins have pull-down
 *  - SPICAN_MISO
 */

/* RC6 */
#define TRCH_UART_TX (6)
/* RD3 */
#define TRCH_I2C_EXT_SDA (3)

uint32_t uio4_09__i2c_fpga_ext_sda_tests(void)
{
	uint32_t err_count = 0;
	uint8_t portc, portd;
	uint8_t trisc, trisd;
	uint32_t uio409_moni, i2csda_moni;
	uint32_t prev_trch_uart_tx, trch_uart_tx;
	uint32_t trch_i2c_sda;
	uint32_t fpga_uio_409;
	uint32_t fpga_i2c_sda;

	info("* Start of UIO4_09 to I2C_FPGA_EXT_SDA loop test\n");

	/* Save current settings */
	portc = trch_get_portc();
	trisc = trch_get_trisc();
	portd = trch_get_portd();
	trisd = trch_get_trisd();
	uio409_moni = sys_read32(TEST_REG_ADDR(TEST_CTRL_UIO4_09));
	i2csda_moni = sys_read32(TEST_REG_ADDR(TEST_CTRL_I2C_EXT_SDA));

	/* TTRCH_UART_TX is not connected to any pins, so set the input
	 * and save the status */
	trch_set_trisc(set_in(trisc, TRCH_UART_TX));
	prev_trch_uart_tx = get_bit(trch_get_portc(), TRCH_UART_TX);

	info("* [#1] UIO4_09 Hi\n");
	/* Change to input all pin expected UIO4_09 */
	set_pin_input(TEST_CTRL_I2C_EXT_SDA);
	trch_set_trisd(set_in(trisd, TRCH_I2C_EXT_SDA));

	/* Change output hi to UIO4_09 */
	set_pin_output_high(TEST_CTRL_UIO4_09);

	/*
	 * Check all pin level (expected All Hi without UART TX)
	 */
	fpga_uio_409 = get_pin(TEST_MONI_USER_IO4, MONI_BIT_UIO4_09);
	fpga_i2c_sda = get_pin(TEST_MONI_I2C_EXT, MONI_BIT_I2C_EXT_SDA);
	trch_uart_tx = get_bit(trch_get_portc(), TRCH_UART_TX);
	printk("uart tris 0x%x\n", trch_get_trisc());
	trch_i2c_sda = get_bit(trch_get_portd(), TRCH_I2C_EXT_SDA);
	printk("sda tris 0x%x\n", trch_get_trisd());
	if (!(fpga_uio_409 && fpga_i2c_sda &&
		trch_uart_tx == prev_trch_uart_tx && trch_i2c_sda)) {
		err("  !!! Assertion failed: UIO4_09 (Hi) to I2C_FPGA_EXT_SDA loop failed\n");
		err("fpga_uio_409: %x, fpga_i2c_sda: %x, trch_uart_tx: %d\n", fpga_uio_409, fpga_i2c_sda, trch_uart_tx);
		err("prev_trch_uart_tx: %x, trch_i2c_sda: %x\n", prev_trch_uart_tx, trch_i2c_sda);
		err_count++;
	}
	goto end_of_test;

	info("* [#2] UIO4_09 Low\n");
	/* Change output hi to UIO4_09 */
	set_pin_output_low(TEST_CTRL_UIO4_09);

	/*
	 * Check all pin level
	 */
	fpga_uio_409 = get_pin(TEST_MONI_USER_IO4, MONI_BIT_UIO4_09);
	fpga_i2c_sda = get_pin(TEST_MONI_I2C_EXT, MONI_BIT_I2C_EXT_SDA);
	trch_uart_tx = get_bit(trch_get_portc(), TRCH_UART_TX);
	trch_i2c_sda = get_bit(trch_get_portd(), TRCH_I2C_EXT_SDA);
	if (!(!fpga_uio_409 && !fpga_i2c_sda &&
		trch_uart_tx == prev_trch_uart_tx && !trch_i2c_sda)) {
		err("  !!! Assertion failed: UIO4_09 (Low) to I2C_FPGA_EXT_SDA loop failed\n");
		err("fpga_uio_409: %x, fpga_i2c_sda: %x, trch_uart_tx: %d\n", fpga_uio_409, fpga_i2c_sda, trch_uart_tx);
		err("prev_trch_uart_tx: %x, trch_i2c_sda: %x\n", prev_trch_uart_tx, trch_i2c_sda);
		err_count++;
	}

	/* Restore current settings */
	trch_set_portc(portc);
	trch_set_trisc(trisc);
	trch_set_portd(portd);
	trch_set_trisd(trisd);
	sys_write32(uio409_moni, TEST_REG_ADDR(TEST_MONI_USER_IO4));
	sys_write32(i2csda_moni, TEST_REG_ADDR(TEST_MONI_I2C_EXT));

end_of_test:
	return err_count;
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
	err_count += uio4_08__i2c_fpga_ext_scl_tests();
	err_count += uio4_09__i2c_fpga_ext_sda_tests();

	if (!can_terminate(false)) {
		assert();
	}

	info("* End of Hardware Option Pin Tests: err %d\n", err_count);

	while(1);

	return err_count;
}
