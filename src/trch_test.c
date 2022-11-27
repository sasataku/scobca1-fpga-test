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

static int send_cmd_to_trch(uint8_t cmd, uint8_t arg, bool has_arg)
{
	uint16_t can_id = 'T';
	uint8_t can_data[2];
	int32_t timeout_us = 1000000;
	uint32_t recv_id;
	uint32_t recv_size;
	uint16_t res;
	uint8_t res_code;
	uint8_t res_val;

	can_data[0] = cmd;
	can_data[1] = arg;

	if (has_arg)
		debug("*  Sending  0x%02x 0x%02x to ID 0x%02x ('%c')\n", can_data[0], can_data[1], can_id, can_id);
	else
		debug("*  Sending  0x%02x to ID 0x%02x ('%c')\n", can_data[0], can_id, can_id);

	if (!can_send(can_id, can_data, has_arg ? 2 : 1)) {
		assert();
		return -1;
	}

	if (!is_can_rx_done(timeout_us)) {
		err("  !!! Assertion failed: CAN RX DONE timed out\n");
		assert();
		return -1;
	}

	/* Read CAN Packet size */
	recv_size = sys_read32(SCOBCA1_FPGA_CAN_RMR2);
	if (recv_size > 2) {
		err("  !!! Assertion failed: Invalid CAN Packet size %u, expecting <= 2\n", recv_size);
		assert();
		return -1;
	}

	/* Read CAN ID */
	recv_id = (sys_read32(SCOBCA1_FPGA_CAN_RMR1) & CAN_TXID1_BIT_MASK) >> CAN_TXID1_BIT_SHIFT;
	if (recv_id != 'F') {
		err("  !!! Assertion failed: Unexpected CAN ID %u, expecting 0x46", recv_id);
		assert();
		return -1;
	}

	/* Read CAN Packet data */
	res = sys_read32(SCOBCA1_FPGA_CAN_RMR3) >> 16;
	res_code = res >> 8;
	res_val = res & 0xff;
	debug("*  Received 0x%02x 0x%02x from FPGA 0x%02x\n", res_code, res_val, recv_id);

	return (int)res_val;
}

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

/* port 0011 1010 */
/* tris 0001 1000 */

/* RA0: TRCH_CFG_MEM_SEL OUT low */
/* RA1: FPGA_BOOT0       OUT HIGH */
/* RA2: FPGA_BOOT1       OUT low */
/* RA3: FPGA_PROGRAM_B   IN  HIGH */
/* RA4: FPGA_INIT_B      IN  HIGH */ /* Open Drain */
/* RA5: FPGAPWR_EN       OUT HIGH */

uint8_t get_porta(void) { return send_cmd_to_trch('a', 0, false); }
uint8_t get_portb(void) { return send_cmd_to_trch('b', 0, false); }
uint8_t get_portc(void) { return send_cmd_to_trch('c', 0, false); }
uint8_t get_portd(void) { return send_cmd_to_trch('d', 0, false); }
uint8_t get_porte(void) { return send_cmd_to_trch('e', 0, false); }

uint8_t get_trisa(void) { return send_cmd_to_trch('t', 0, false); }
uint8_t get_trisb(void) { return send_cmd_to_trch('u', 0, false); }
uint8_t get_trisc(void) { return send_cmd_to_trch('v', 0, false); }
uint8_t get_trisd(void) { return send_cmd_to_trch('w', 0, false); }
uint8_t get_trise(void) { return send_cmd_to_trch('x', 0, false); }

uint8_t set_porta(uint8_t val) { return send_cmd_to_trch('A', val, true); }
uint8_t set_portb(uint8_t val) { return send_cmd_to_trch('B', val, true); }
uint8_t set_portc(uint8_t val) { return send_cmd_to_trch('C', val, true); }
uint8_t set_portd(uint8_t val) { return send_cmd_to_trch('D', val, true); }
uint8_t set_porte(uint8_t val) { return send_cmd_to_trch('E', val, true); }

uint8_t set_trisa(uint8_t val) { return send_cmd_to_trch('T', val, true); }
uint8_t set_trisb(uint8_t val) { return send_cmd_to_trch('U', val, true); }
uint8_t set_trisc(uint8_t val) { return send_cmd_to_trch('V', val, true); }
uint8_t set_trisd(uint8_t val) { return send_cmd_to_trch('W', val, true); }
uint8_t set_trise(uint8_t val) { return send_cmd_to_trch('X', val, true); }

uint8_t set_out (uint8_t val, uint8_t bit) { return val & ~(1 << bit); }
uint8_t set_in  (uint8_t val, uint8_t bit) { return val |  (1 << bit); }
uint8_t set_low (uint8_t val, uint8_t bit) { return val & ~(1 << bit); }
uint8_t set_high(uint8_t val, uint8_t bit) { return val |  (1 << bit); }

/*
 * TRCH_CFG_MEM_SEL
 * PortA 0 / RA0
 * Dir: TRCH -> FPGA
 *
 * This is a special signal.  TRCH actively picks up FPGA_CFG_MEM_SEL
 * and assert it to TRCH_CFG_MEM_SEL.  IOW, we don't need to anything
 * but change FPGA_CFG_MEM_SEL and compare them.
 */
static uint32_t test_trch_cfg_mem_sel(void)
{
	uint32_t err_count = 0;

	/* set FPGA_CFG_MEM_SEL HIGH
	 * get TRCH_CFG_MEM_SEL
	 * compare them
	 *
	 * set FPGA_CFG_MEM_SEL LOW
	 * get TRCH_CFG_MEM_SEL again
	 * compare them
	 */

	return err_count;
}

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

	/* setup */
	data = get_porta();
	tris = get_trisa();
	reg = sys_read32(TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_BOOT0));

	/* test */
	set_pin_input(TEST_CTRL_TRCH_FPGA_BOOT0);
	set_trisa(set_out(tris, FPGA_BOOT0));

	set_porta(set_high(data, FPGA_BOOT0));
	first = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT0);

	set_porta(set_low(data, FPGA_BOOT0));
	second = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT0);

	/* restore */
	set_porta(data);
	set_trisa(tris);
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

	/* setup */
	data = get_porta();
	tris = get_trisa();
	reg = sys_read32(TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_BOOT1));

	/* test */
	set_pin_input(TEST_CTRL_TRCH_FPGA_BOOT1);
	set_trisa(set_out(tris, FPGA_BOOT1));

	set_porta(set_high(data, FPGA_BOOT1));
	first = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT1);

	set_porta(set_low(data, FPGA_BOOT1));
	second = get_pin(TEST_MONI_TRCH, MONI_BIT_TRCH_FPGA_BOOT1);

	/* restore */
	set_porta(data);
	set_trisa(tris);
	sys_write32(reg, TEST_REG_ADDR(TEST_CTRL_TRCH_FPGA_BOOT1));

	info("* FPGA_BOOT1: error %d\n", first == second);

	return first == second;
}

static uint32_t test_fpga_program_b(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_fpga_init_b(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_fpga_pwr_cycle_req(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_trch_cfg_mem_moni(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_fpga_watchdog(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_fpga_cfg_mem_sel(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_fpga_reserve(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_fpga_program_b_in(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t test_fpga_init_b_in(void)
{
	uint32_t err_count = 0;
	return err_count;
}

static uint32_t open_circuit_tests(void)
{
	uint32_t err_count = 0;

	err_count += test_trch_cfg_mem_sel();
	err_count += test_fpga_boot0();
	err_count += test_fpga_boot1();
	err_count += test_fpga_program_b();
	err_count += test_fpga_init_b();
	err_count += test_fpga_pwr_cycle_req();
	err_count += test_trch_cfg_mem_moni();
	err_count += test_fpga_watchdog();
	err_count += test_fpga_cfg_mem_sel();
	err_count += test_fpga_reserve();
	err_count += test_fpga_program_b_in();
	err_count += test_fpga_init_b_in();

	return err_count;
}

static uint32_t short_circuit_tests(void)
{
	uint32_t err_count = 0;
	return err_count;
}

uint32_t trch_test(void)
{
	int err_count = 0;

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
