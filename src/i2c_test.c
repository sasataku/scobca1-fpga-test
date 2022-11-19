/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "i2c_test.h"
#include "common.h"

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
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01, REG_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x00, REG_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x00);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x4b, REG_READ_RETRY(1));
	if (!ret) {
		err_cnt++;
	}

	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x00, REG_READ_RETRY(1));
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
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01, REG_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x00, REG_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x00);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, (slave_addr << 1) + I2C_RX_MODE);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_TXFIFOR, 0x0101);
	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01, REG_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x01);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_ISR, 0x00, REG_READ_RETRY(10));
	if (!ret) {
		err_cnt++;
	}

	write32(SCOBCA1_FPGA_INTERNAL_I2CM_ENR, 0x00);
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x71, REG_READ_RETRY(1));
	if (!ret) {
		err_cnt++;
	}
	ret = assert32(SCOBCA1_FPGA_INTERNAL_I2CM_RXFIFOR, 0x27, REG_READ_RETRY(1));
	if (!ret) {
		err_cnt++;
	}

	return err_cnt;
}

uint32_t internal_i2c_test(uint32_t test_no)
{
	uint32_t ret;
	uint32_t err_cnt = 0;

	printk("* [%d] Start Intearnal I2C Test\n", test_no);

	printk("* [%d-1] Start Tempature Sensor 1 Test\n", test_no);
	ret = internal_i2c_temp_test(0x4C);
	err_cnt += ret;

	printk("* [%d-2] Start Tempature Sensor 2 Test\n", test_no);
	ret = internal_i2c_temp_test(0x4D);
	err_cnt += ret;

	printk("* [%d-3] Start Tempature Sensor 3 Test\n", test_no);
	ret = internal_i2c_temp_test(0x4E);
	err_cnt += ret;

	printk("* [%d-4] Start Current/Voltage Monitor 1 Test\n", test_no);
	ret = internal_i2c_cvm_test(0x40);
	err_cnt += ret;

	printk("* [%d-5] Start Current/Voltage Monitor 2 Test\n", test_no);
	ret = internal_i2c_cvm_test(0x41);
	err_cnt += ret;

	print_result(test_no, err_cnt);

	return err_cnt;
}
