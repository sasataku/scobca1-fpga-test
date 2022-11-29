/*
 * Copyright (c) 2022 Space Cubics, LLC.
 * Yasushi SHOJI <yashi@spacecubics.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "common.h"
#include "can.h"
#include "test_register.h"
#include "trch.h"

#define PORT_LEVEL_HIGH 1
#define PORT_LEVEL_LOW 0
#define PORT_DIR_OUT 0
#define PORT_DIR_IN 1

#define TRCH_CONFIG_MEM_SEL (0)
#define FPGA_BOOT0          (1)
#define FPGA_BOOT1          (2)
#define FPGA_PROGRAM_B      (3)
#define FPGA_INIT_B         (4)
#define FPGAPWR_EN          (5)

#define FPGA_PWR_CYCLE_REQ  (1)
#define FPGA_WATCHDOG       (4)

#define FPGA_RESERVE        (7)

uint8_t set_out (uint8_t val, uint8_t bit) { return val & ~(1 << bit); }
uint8_t set_in  (uint8_t val, uint8_t bit) { return val |  (1 << bit); }
uint8_t set_low (uint8_t val, uint8_t bit) { return val & ~(1 << bit); }
uint8_t set_high(uint8_t val, uint8_t bit) { return val |  (1 << bit); }
uint8_t get_bit (uint8_t val, uint8_t bit) { return val &  (1 << bit); }

void set_pin_input(uint32_t offset)
{
	sys_write32(TEST_GPIO_IN, TEST_REG_ADDR(offset));
}

void set_pin_output_low(uint32_t offset)
{
	sys_write32(TEST_GPIO_OUT_LOW, TEST_REG_ADDR(offset));
}

void set_pin_output_high(uint32_t offset)
{
	sys_write32(TEST_GPIO_OUT_HIGH, TEST_REG_ADDR(offset));
}

uint32_t get_pin(uint32_t offset, uint8_t bit)
{
	return !!(sys_read32(TEST_REG_ADDR(offset)) & (1 << bit));
}

static uint32_t test_fpga_boot0(void)
{
	uint8_t data;
	uint8_t tris;
	uint32_t first;
	uint32_t second;
	uint32_t reg;

	info("* FPGA_BOOT0: start\n");

	/* setup */
	data = trch_get_porta();
	tris = trch_get_trisa();
	reg = sys_read32(TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_BOOT0));

	/* test */
	set_pin_input(TEST_CTRL_TRCH_FPGA_BOOT0);
	trch_set_trisa(set_out(tris, FPGA_BOOT0));

	trch_set_porta(set_high(data, FPGA_BOOT0));
	first = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT0);

	trch_set_porta(set_low(data, FPGA_BOOT0));
	second = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT0);

	/* restore */
	trch_set_porta(data);
	trch_set_trisa(tris);
	sys_write32(reg, TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_BOOT0));

	info("* FPGA_BOOT0: error %d\n", first == second);

	return first == second;
}

static uint32_t test_fpga_boot1(void)
{
	uint8_t data;
	uint8_t tris;
	uint32_t first;
	uint32_t second;
	uint32_t reg;

	info("* FPGA_BOOT1: start\n");

	/* setup */
	data = trch_get_porta();
	tris = trch_get_trisa();
	reg = sys_read32(TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_BOOT1));

	/* test */
	set_pin_input(TEST_CTRL_TRCH_FPGA_BOOT1);
	trch_set_trisa(set_out(tris, FPGA_BOOT1));

	trch_set_porta(set_high(data, FPGA_BOOT1));
	first = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT1);

	trch_set_porta(set_low(data, FPGA_BOOT1));
	second = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT1);

	/* restore */
	trch_set_porta(data);
	trch_set_trisa(tris);
	sys_write32(reg, TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_BOOT1));

	info("* FPGA_BOOT1: error %d\n", first == second);

	return first == second;
}

/*
 * FPGA_PWR_CYCLE_REQ
 * PortB 1 / RB1
 * Dir: TRCH <- FPGA
 *
 * This signal is used to inform TRCH that we want to restart with a
 * power cycle.  The stock TRCH firmware will power cycle when this
 * signal get HIGH.  But for this system, the functionality is
 * removed.  Thus, no power cycle will happen regardless of this
 * singal level.
 */
static uint32_t test_fpga_pwr_cycle_req(void)
{
	uint8_t data;
	uint8_t tris;
	uint32_t first;
	uint32_t second;
	uint32_t reg;

	info("* FPGA_PWR_CYCLE_REQ: start\n");

	/* setup */
	data = trch_get_portb();
	tris = trch_get_trisb();
	reg = sys_read32(TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_PWR_CYCLE_REQ));

	/* test */
	trch_set_trisb(set_in(tris, FPGA_PWR_CYCLE_REQ));

	set_pin_output_high(TEST_CTRL_TRCH_FPGA_PWR_CYCLE_REQ);
	first = get_bit(trch_get_portb(), FPGA_PWR_CYCLE_REQ);

	set_pin_output_low(TEST_CTRL_TRCH_FPGA_PWR_CYCLE_REQ);
	second = get_bit(trch_get_portb(), FPGA_PWR_CYCLE_REQ);

	/* restore */
	trch_set_portb(data);
	trch_set_trisb(tris);
	sys_write32(reg, TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_PWR_CYCLE_REQ));

	info("* FPGA_PWR_CYCLE_REQ: error %d\n", first == second);

	return first == second;
}

/*
 * FPGA_WATCHDOG
 * PortB 4 / RB4
 * Dir: TRCH <- FPGA
 *
 * FPGA sends live pluses on the line.  Read the signal at TRCH side
 * multiple times and see the signal is changing.
 *
 * The interval is default to 500 ms.  We have some delay at CAN and
 * TRCH side so 500 ms is good value to wait.
 *
 * No setup should be needed for both sides.
 */
static uint32_t test_fpga_watchdog(void)
{
	uint8_t first;
	uint8_t second;
	uint8_t third;
	uint8_t fourth;

	info("* FPGA_WATCHDOG: start\n");

	/* setup: None */

	/* test */
	first = get_bit(trch_get_portb(), FPGA_WATCHDOG);
	k_msleep(500);
	second = get_bit(trch_get_portb(), FPGA_WATCHDOG);
	k_msleep(500);
	third = get_bit(trch_get_portb(), FPGA_WATCHDOG);
	k_msleep(500);
	fourth = get_bit(trch_get_portb(), FPGA_WATCHDOG);

	debug("*  FPGA_WATCHDOG: %x %x %x %x\n", first, second, third, fourth);

	/* restore: None */

	info("* FPGA_WATCHDOG: error %d\n", (first == second) && (first == third) && (first == fourth));

	return (first == second) && (first == third) && (first == fourth);
}

/*
 * FPGA_RESERVE
 * PortD 7 / RD7
 * Dir: TRCH <-> FPGA
 *
 * This is reserved signal. No functionarity is assigned to it.
 *
 * To test it, we'll make the pin INPUT for FPGA and drive it from
 * TRCH.
 */
static uint32_t test_fpga_reserve(void)
{
	uint8_t data;
	uint8_t tris;
	uint32_t first;
	uint32_t second;
	uint32_t reg;

	info("* FPGA_RESERVE: start\n");

	/* setup */
	data = trch_get_portd();
	tris = trch_get_trisd();
	reg = sys_read32(TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_RESERVE));

	/* test */
	set_pin_input(TEST_CTRL_TRCH_FPGA_RESERVE);
	trch_set_trisd(set_out(tris, FPGA_RESERVE));

	trch_set_portd(set_high(data, FPGA_RESERVE));
	first = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_RESERVE);

	trch_set_portd(set_low(data, FPGA_RESERVE));
	second = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_RESERVE);

	/* restore */
	trch_set_portd(data);
	trch_set_trisd(tris);
	sys_write32(reg, TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_RESERVE));

	info("* FPGA_RESERVE: error %d\n", first == second);

	return first == second;
}

static uint32_t open_circuit_tests(void)
{
	uint32_t err_count = 0;

	err_count += test_fpga_boot0();
	err_count += test_fpga_boot1();
	err_count += test_fpga_pwr_cycle_req();
	err_count += test_fpga_watchdog();
	err_count += test_fpga_reserve();

	return err_count;
}

static uint32_t short_circuit_tests(void)
{
	uint32_t err_count = 0;
	return err_count;
}

uint32_t trch_test(void)
{
	uint32_t err_count = 0;

	info("* Start of Shared Pin Tests: Initializing\n");

	if (!can_init(false)) {
		assert();
	}

	err_count += open_circuit_tests();
	err_count += short_circuit_tests();

	if (!can_terminate(false)) {
		assert();
	}

	info("* End of Shared Pin Tests: err %d\n", err_count);

	return err_count;
}
