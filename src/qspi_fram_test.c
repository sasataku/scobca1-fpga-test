/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "qspi_common.h"
#include "common.h"

#define QSPI_FRAM_MEM_ADDR_SIZE (3u)
#define QSPI_FRAM_MEM0 (0u)
#define QSPI_FRAM_MEM1 (1u)
#define QSPI_FRAM_MEM0_SS (0x01)
#define QSPI_FRAM_MEM1_SS (0x02)
#define QSPI_ASR_IDLE (0x00)
#define QSPI_ASR_BUSY (0x01)
#define QSPI_RX_FIFO_MAX_BYTE (16u)
#define QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT (2u)
#define QSPI_SPI_MODE_QUAD   (0x00020000)

static bool is_qspi_idle(void)
{
	printk("* Confirm QSPI Access Status is `Idle`\n");
	if (!assert32(SCOBCA1_FPGA_FRAM_QSPI_ASR, QSPI_ASR_IDLE,
			REG_READ_RETRY(10))) {
		printk("QSPI (FRAM) is busy, so exit test\n");
		return false;
	}

	return true;
}

static bool activate_spi_ss(uint32_t spi_mode)
{
	printk("* Activate SPI SS with %08x\n", spi_mode);
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, spi_mode);
	if (!is_qspi_idle()) {
		return false;
	}

	return true;
}

static bool inactivate_spi_ss(void)
{
	printk("* Inactivate SPI SS\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, 0x00000000);
	if (!is_qspi_idle()) {
		return false;
	}

	return true;
}

static void write_data_to_flash(uint32_t *write_data, size_t size)
{
	printk("* Write TX FIFO %d byte\n", size);
	for (uint8_t i=0; i<size; i++) {
		write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, write_data[i]);
	}
}

static bool send_dummy_cycle(uint8_t dummy_count)
{
	printk("* Send dummy cycle %d byte\n", dummy_count);
	for (uint8_t i=0; i<dummy_count; i++) {
		write32(SCOBCA1_FPGA_FRAM_QSPI_RDR, 0x00);
	}

	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Discard dummy data\n");
	for (uint8_t i=0; i<dummy_count; i++) {
		sys_read32(SCOBCA1_FPGA_FRAM_QSPI_RDR);
	}

	return true;
}

static bool read_and_verify_rx_data(size_t exp_size, uint32_t *exp_val)
{
	bool ret = true;

	printk("* Reqest RX FIFO %d byte\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		write32(SCOBCA1_FPGA_FRAM_QSPI_RDR, 0x00);
	}

	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Read RX FIFO %d byte and verify the value\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		if (!assert32(SCOBCA1_FPGA_FRAM_QSPI_RDR, exp_val[i],
						REG_READ_RETRY(1))) {
			ret = false;
		}
	}

	return ret;
}

static bool is_qspi_control_done(void)
{
	printk("* Confirm QSPI Interrupt Stauts is `SPI Control Done`\n");
	if (!assert32(SCOBCA1_FPGA_FRAM_QSPI_ISR, 0x01, REG_READ_RETRY(10))) {
		return false;
	}

	printk("* Clear QSPI Interrupt Stauts\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_ISR, 0x01);
	if (!assert32(SCOBCA1_FPGA_FRAM_QSPI_ISR, 0x00, REG_READ_RETRY(10))) {
		return false;
	}

	return true;
}

static bool verify_status_resisger1(uint32_t spi_ss, size_t exp_size, uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		return false;
	}

	printk("* Request Status Register 1\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x05);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read Memory data (1byte) adn Verify */
	ret = read_and_verify_rx_data(exp_size, exp_val);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		return false;
	}

	return ret;
}

static bool set_write_enable(uint32_t spi_ss, bool enable)
{
	uint32_t exp_write_disable[] = {0x00};
	uint32_t exp_write_enable[] = {0x02};

	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		return false;
	}

	if (enable) {
		printk("* Set `Write Enable` (Instructure:0x06) \n");
		write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x06);
	} else {
		printk("* Set `Write Disable` (Instructure:0x04) \n");
		write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x04);
	}

	if (!is_qspi_idle()) {
		return false;
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		return false;
	}

	if (enable) {
		if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_enable), exp_write_enable)) {
			return false;
		}
	} else {
		if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
			return false;
		}
	}

	return true;
}

static bool set_quad_io_mode(uint32_t spi_ss)
{
	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		return false;
	}

	printk("* Set QUAD I/O mode and dummy cycle (4) to configuration register\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x71);
	/*
	 * Configuraiton Resister
	 *   Non volatile: 0x000002
	 *   Volatile    : 0x070002
	 */
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x00);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x70);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x02);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x42);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Inactivate SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	/* Confirm SPI Control is Done */
	if(!is_qspi_control_done()) {
		return false;
	}

	return true;
}

static bool verify_config_register(uint32_t spi_ss, size_t exp_size, uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		return false;
	}

	printk("* Request Configuration Register\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x35);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read Memory data (1byte) adn Verify */
	ret = read_and_verify_rx_data(exp_size, exp_val);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		return false;
	}

	return ret;
}

static bool verify_quad_io_mode(uint32_t spi_ss)
{
	uint32_t exp_quad_mode[] = {0x42};

	if (!verify_config_register(spi_ss, ARRAY_SIZE(exp_quad_mode), exp_quad_mode)) {
		return false;
	}

	return true;
}

static bool qspi_fram_init(uint8_t mem_no)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_FRAM_MEM0) {
		spi_ss = QSPI_FRAM_MEM0_SS;
	} else {
		spi_ss = QSPI_FRAM_MEM1_SS;
	}

	printk("* [#1] Set to `Write Enable'\n");
	if (!set_write_enable(spi_ss, true)) {
		return false;
	}

	printk("* [#2] Set to `QUAD I/O modee'\n");
	if (!set_quad_io_mode(spi_ss)) {
		return false;
	}

	/* Wait 1 sec */
	k_sleep(K_MSEC(1000));

	printk("* [#3] Verify Configuration Register is QUAD I/O mode (0x02)\n");
	if (!verify_quad_io_mode(spi_ss)) {
		return false;
	}

	return true;
}

static bool qspi_fram_set_quad_read_mode(uint32_t spi_ss)
{
	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		return false;
	}

	printk("* Set QUAD-IO read mode\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0xEB);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Keep SPI SS for Quad Read */

	return true;
}

static bool qspi_fram_quad_read_data(uint32_t spi_ss, uint8_t read_size, uint32_t *exp_vals, uint32_t *mem_addr)
{
	bool ret;

	printk("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, QSPI_SPI_MODE_QUAD + spi_ss);

	printk("* Send Memory Address (3byte)\n");
	write_data_to_flash(mem_addr, QSPI_FRAM_MEM_ADDR_SIZE);
	
	printk("* Send Mode (0x00)\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x00);

	/* Send Dummy Cycle */
	send_dummy_cycle(QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read RX data and Verify */
	ret = read_and_verify_rx_data(read_size, exp_vals);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	return ret;
}

static bool qspi_fram_quad_write_data(uint32_t spi_ss, uint8_t write_size, uint32_t *write_data, uint32_t *mem_addr)
{
	if (!activate_spi_ss(spi_ss) ) {
		return false;
	}

	printk("* Snd QUAD I/O Write instruction\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0xD2);
	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, QSPI_SPI_MODE_QUAD + spi_ss);

	printk("* Send Memory Address (3byte)\n");
	write_data_to_flash(mem_addr, QSPI_FRAM_MEM_ADDR_SIZE);

	printk("* Send Mode (0x00)\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x00);

	if (!is_qspi_idle()) {
		return false;
	}

	/* Write data */
	write_data_to_flash(write_data, write_size);
	if (!is_qspi_idle()) {
		return false;
	}

	if (!inactivate_spi_ss() ) {
		return false;
	}

	return true;
}

static bool qspi_fram_write_data(uint8_t mem_no, uint8_t write_size, uint32_t *write_data, uint32_t *mem_addr)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_FRAM_MEM0) {
		spi_ss = QSPI_FRAM_MEM0_SS;
	} else {
		spi_ss = QSPI_FRAM_MEM1_SS;
	}

	printk("* [#1] Set to `Write Enable'\n");
	if (!set_write_enable(spi_ss, true)) {
		return false;
	}

	printk("* [#2] Write Data (QUAD Mode)\n");
	if (!qspi_fram_quad_write_data(spi_ss, write_size, write_data, mem_addr)) {
		return false;
	}

	printk("* [#3] Set to `Write Disable'\n");
	if (!set_write_enable(spi_ss, false)) {
		return false;
	}

	return true;
}

static bool qspi_fram_read_data(uint8_t mem_no, uint8_t read_size, uint32_t *exp_vals, uint32_t *mem_addr)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_FRAM_MEM0) {
		spi_ss = QSPI_FRAM_MEM0_SS;
	} else {
		spi_ss = QSPI_FRAM_MEM1_SS;
	}

	printk("* [#1] Set QUAD-IO Read Mode\n");
	if (!qspi_fram_set_quad_read_mode(spi_ss)) {
		return false;
	}

	printk("* [#2] Read Data (QUAD-IO Mode) \n");
	if (!qspi_fram_quad_read_data(spi_ss, read_size, exp_vals, mem_addr)) {
		return false;
	}

	return true;
}

uint32_t qspi_fram_initialize(uint32_t test_no)
{
	uint32_t err_cnt = 0;

	printk("* [%d] Start QSPI FRAM [MEM1]: Initialize\n", test_no);
	if (!qspi_fram_init(QSPI_FRAM_MEM1)) {
		err_cnt++;
	}

	return err_cnt;
}

uint32_t qspi_fram_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t write_data[QSPI_RX_FIFO_MAX_BYTE] = {0x00};
	uint32_t mem_addr[QSPI_FRAM_MEM_ADDR_SIZE] = {0x00, 0x00, 0x00};
	uint8_t start_val = 0x00;

	printk("* [%d] Start QSPI FRAM Test\n", test_no);

	start_val = qspi_create_fifo_data(start_val, write_data, ARRAY_SIZE(write_data), false);

	printk("* [%d-1] Start QSPI FRAM [1]: Write data Test\n", test_no);
	if (!qspi_fram_write_data(QSPI_FRAM_MEM1, QSPI_RX_FIFO_MAX_BYTE, write_data, mem_addr)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-2] Start QSPI FRAM [1]: Read data Test\n", test_no);
	if (!qspi_fram_read_data(QSPI_FRAM_MEM1, QSPI_RX_FIFO_MAX_BYTE, write_data, mem_addr)) {
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	print_result(test_no, err_cnt);

	return err_cnt;
}
