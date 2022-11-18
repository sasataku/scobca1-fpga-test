/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "i2c_test.h"
#include "common.h"

#define I2C_READ_RETRY(count) (count)
#define I2C_TX_MODE (0)
#define I2C_RX_MODE (1)

static uint32_t internal_i2c_temp_test(uint32_t slave_addr)
{
	bool ret;
	uint32_t err_cnt = 0;

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, (slave_addr << 1) + I2C_TX_MODE);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, 0x0102);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, (slave_addr << 1) + I2C_RX_MODE);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, 0x0101);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01, I2C_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x00, I2C_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x00);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x4b, I2C_READ_RETRY(1));
	if (!ret) {
		err_cnt++;
	}

	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x00, I2C_READ_RETRY(1));
	if (!ret) {
		err_cnt++;
	}

	return err_cnt;
}

static uint32_t internal_i2c_cvm_test(uint32_t slave_addr)
{
	uint32_t ret;
	uint32_t err_cnt = 0;

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, (slave_addr << 1) + I2C_TX_MODE);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, 0x0100);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01, I2C_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x00, I2C_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x00);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, (slave_addr << 1) + I2C_RX_MODE);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, 0x0101);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01, I2C_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x00, I2C_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x00);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x71, I2C_READ_RETRY(1));
	if (!ret) {
		err_cnt++;
	}
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x27, I2C_READ_RETRY(1));
	if (!ret) {
		err_cnt++;
	}

	return err_cnt;
}

uint32_t internal_i2c_test(uint32_t test_no)
{
	uint32_t ret;
	uint32_t err_cnt = 0;

	printk("* Start Intearnal I2C Test\n");

	printk("** Start Tempature Sensor 1 Test\n");
	ret = internal_i2c_temp_test(0x4C);
	err_cnt += ret;

	printk("** Start Tempature Sensor 2 Test\n");
	ret = internal_i2c_temp_test(0x4D);
	err_cnt += ret;

	printk("** Start Tempature Sensor 3 Test\n");
	ret = internal_i2c_temp_test(0x4E);
	err_cnt += ret;

	printk("** Start Current/Voltage Monitor 1 Test\n");
	ret = internal_i2c_cvm_test(0x40);
	err_cnt += ret;

	printk("** Start Current/Voltage Monitor 2 Test\n");
	ret = internal_i2c_cvm_test(0x41);
	err_cnt += ret;

	if (err_cnt == 0) {
		printk("* Test Result: Passed\n");
	} else {
		printk("* Test Result: Failed (Assertion count: %d)\n", err_cnt);
	}

	return err_cnt;
}
