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
#define QSPI_FIFO_MAX_BYTE (16u)
#define QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT (2u)
#define QSPI_SPI_MODE_QUAD   (0x00020000)

static bool is_qspi_idle(void)
{
	debug("* Confirm QSPI Access Status is `Idle`\n");
	if (!assert32(SCOBCA1_FPGA_FRAM_QSPI_ASR, QSPI_ASR_IDLE,
			REG_READ_RETRY(10))) {
		err("QSPI (FRAM) is busy, so exit test\n");
		return false;
	}

	return true;
}

static bool activate_spi_ss(uint32_t spi_mode)
{
	debug("* Activate SPI SS with %08x\n", spi_mode);
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, spi_mode);
	if (!is_qspi_idle()) {
		return false;
	}

	return true;
}

static bool inactivate_spi_ss(void)
{
	debug("* Inactivate SPI SS\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, 0x00000000);
	if (!is_qspi_idle()) {
		return false;
	}

	return true;
}

static void write_data_to_flash(uint32_t *write_data, size_t size)
{
	debug("* Write TX FIFO %d byte\n", size);
	for (uint8_t i=0; i<size; i++) {
		write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, write_data[i]);
	}
}

static void write_mem_addr_to_flash(uint32_t mem_addr)
{
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, (mem_addr & 0x00FF0000) >> 16);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, (mem_addr & 0x0000FF00) >> 8);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, (mem_addr & 0x000000FF));
}

static bool send_dummy_cycle(uint8_t dummy_count)
{
	debug("* Send dummy cycle %d byte\n", dummy_count);
	for (uint8_t i=0; i<dummy_count; i++) {
		write32(SCOBCA1_FPGA_FRAM_QSPI_RDR, 0x00);
	}

	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	debug("* Discard dummy data\n");
	for (uint8_t i=0; i<dummy_count; i++) {
		sys_read32(SCOBCA1_FPGA_FRAM_QSPI_RDR);
	}

	return true;
}

static bool read_and_verify_rx_data(size_t exp_size, uint32_t *exp_val)
{
	bool ret = true;

	debug("* Reqest RX FIFO %d byte\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		write32(SCOBCA1_FPGA_FRAM_QSPI_RDR, 0x00);
	}

	if (!is_qspi_idle()) {
		return false;
	}

	debug("* Read RX FIFO %d byte and verify the value\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		if (!assert32(SCOBCA1_FPGA_FRAM_QSPI_RDR, exp_val[i],
						REG_READ_RETRY(0))) {
			ret = false;
		}
	}

	return ret;
}

static bool is_qspi_control_done(void)
{
	debug("* Confirm QSPI Interrupt Stauts is `SPI Control Done`\n");
	if (!assert32(SCOBCA1_FPGA_FRAM_QSPI_ISR, 0x01, REG_READ_RETRY(10))) {
		return false;
	}

	debug("* Clear QSPI Interrupt Stauts\n");
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
		assert();
		return false;
	}

	debug("* Request Status Register 1\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x05);
	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	/* Read Memory data (1byte) adn Verify */
	ret = read_and_verify_rx_data(exp_size, exp_val);
	if (!ret) {
		assert();
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		assert();
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
		assert();
		return false;
	}

	if (enable) {
		debug("* Set `Write Enable` (Instructure:0x06) \n");
		write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x06);
	} else {
		debug("* Set `Write Disable` (Instructure:0x04) \n");
		write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x04);
	}

	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		assert();
		return false;
	}

	if (enable) {
		if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_enable), exp_write_enable)) {
			assert();
			return false;
		}
	} else {
		if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
			assert();
			return false;
		}
	}

	return true;
}

static bool set_quad_io_mode(uint32_t spi_ss)
{
	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		assert();
		return false;
	}

	debug("* Set QUAD I/O mode and dummy cycle (4) to configuration register\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x71);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x00);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x00);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x02);
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x42);
	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	/* Inactivate SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	/* Confirm SPI Control is Done */
	if(!is_qspi_control_done()) {
		assert();
		return false;
	}

	return true;
}

static bool verify_config_register(uint32_t spi_ss, size_t exp_size, uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		assert();
		return false;
	}

	debug("* Request Configuration Register\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x35);
	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	/* Read Memory data (1byte) adn Verify */
	ret = read_and_verify_rx_data(exp_size, exp_val);
	if (!ret) {
		assert();
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		assert();
		return false;
	}

	return ret;
}

static bool verify_quad_io_mode(uint32_t spi_ss)
{
	uint32_t exp_quad_mode[] = {0x42};

	if (!verify_config_register(spi_ss, ARRAY_SIZE(exp_quad_mode), exp_quad_mode)) {
		assert();
		return false;
	}

	return true;
}

static bool qspi_fram_init(uint8_t mem_no)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		debug("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_FRAM_MEM0) {
		spi_ss = QSPI_FRAM_MEM0_SS;
	} else {
		spi_ss = QSPI_FRAM_MEM1_SS;
	}

	debug("* [#1] Set to `Write Enable'\n");
	if (!set_write_enable(spi_ss, true)) {
		assert();
		return false;
	}

	debug("* [#2] Set to `QUAD I/O modee'\n");
	if (!set_quad_io_mode(spi_ss)) {
		assert();
		return false;
	}

	/* Wait 1 sec */
	k_sleep(K_MSEC(1000));

	debug("* [#3] Verify Configuration Register is QUAD I/O mode (0x02)\n");
	if (!verify_quad_io_mode(spi_ss)) {
		assert();
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

	debug("* Set QUAD-IO read mode\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0xEB);
	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	/* Keep SPI SS for Quad Read */

	return true;
}

static bool qspi_fram_quad_read_data(uint32_t spi_ss, uint8_t read_size, uint32_t *exp_vals, uint32_t mem_addr)
{
	bool ret;

	debug("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, QSPI_SPI_MODE_QUAD + spi_ss);

	debug("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(mem_addr);
	
	debug("* Send Mode (0x00)\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x00);

	/* Send Dummy Cycle */
	send_dummy_cycle(QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT);
	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	/* Read RX data and Verify */
	ret = read_and_verify_rx_data(read_size, exp_vals);
	if (!ret) {
		assert();
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		assert();
		return false;
	}

	return ret;
}

static bool qspi_fram_quad_write_data(uint32_t spi_ss, uint8_t write_size, uint32_t *write_data, uint32_t mem_addr)
{
	if (!activate_spi_ss(spi_ss) ) {
		return false;
	}

	debug("* Snd QUAD I/O Write instruction\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0xD2);
	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	debug("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_ACR, QSPI_SPI_MODE_QUAD + spi_ss);

	debug("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(mem_addr);

	debug("* Send Mode (0x00)\n");
	write32(SCOBCA1_FPGA_FRAM_QSPI_TDR, 0x00);

	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	/* Write data */
	write_data_to_flash(write_data, write_size);
	if (!is_qspi_idle()) {
		assert();
		return false;
	}

	if (!inactivate_spi_ss() ) {
		assert();
		return false;
	}

	return true;
}

bool qspi_fram_multi_write(uint8_t mem_no, uint32_t mem_addr, uint32_t size, uint8_t start_val)
{
	uint32_t spi_ss;
	uint32_t write_data[QSPI_FIFO_MAX_BYTE];
	uint16_t loop_count;

	if (mem_no > 1) {
		err("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_FRAM_MEM0) {
		spi_ss = QSPI_FRAM_MEM0_SS;
	} else {
		spi_ss = QSPI_FRAM_MEM1_SS;
	}

	loop_count = size/QSPI_FIFO_MAX_BYTE;
	for (uint16_t i=0; i<loop_count; i++) {

		debug("* [#1] Set to `Write Enable'\n");
		if (!set_write_enable(spi_ss, true)) {
			assert();
			return false;
		}

		debug("* [#2] Write Data (QUAD Mode)\n");
		start_val = qspi_create_fifo_data(start_val, write_data, QSPI_FIFO_MAX_BYTE, false);
		if (!qspi_fram_quad_write_data(spi_ss, QSPI_FIFO_MAX_BYTE, write_data, mem_addr)) {
			assert();
			return false;
		}

		debug("* [#3] Set to `Write Disable'\n");
		if (!set_write_enable(spi_ss, false)) {
			assert();
			return false;
		}
		mem_addr += QSPI_FIFO_MAX_BYTE;
	}

	return true;
}

bool qspi_fram_multi_read(uint8_t mem_no, uint32_t mem_addr, uint32_t size, uint8_t start_val)
{
	bool ret = true;
	uint32_t spi_ss;
	uint32_t exp_vals[QSPI_FIFO_MAX_BYTE];
	uint16_t loop_count;

	if (mem_no > 1) {
		err("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_FRAM_MEM0) {
		spi_ss = QSPI_FRAM_MEM0_SS;
	} else {
		spi_ss = QSPI_FRAM_MEM1_SS;
	}

	loop_count = size/QSPI_FIFO_MAX_BYTE;
	for (uint16_t i=0; i<loop_count; i++) {

		debug("* [#1] Set QUAD-IO Read Mode\n");
		if (!qspi_fram_set_quad_read_mode(spi_ss)) {
			assert();
			ret = false;
		}

		debug("* [#2] Read Data (QUAD-IO Mode) \n");
		start_val = qspi_create_fifo_data(start_val, exp_vals, QSPI_FIFO_MAX_BYTE, false);
		if (!qspi_fram_quad_read_data(spi_ss, QSPI_FIFO_MAX_BYTE, exp_vals, mem_addr)) {
			ret = false;
		}
		mem_addr += QSPI_FIFO_MAX_BYTE;
	}

	return ret;
}

uint32_t qspi_fram_initialize(uint32_t test_no)
{
	uint32_t err_cnt = 0;

	info("* [%d] Start QSPI FRAM [MEM0]: Initialize\n", test_no);
	if (!qspi_fram_init(QSPI_FRAM_MEM0)) {
		err_cnt++;
	}

	info("* [%d] Start QSPI FRAM [MEM1]: Initialize\n", test_no);
	if (!qspi_fram_init(QSPI_FRAM_MEM1)) {
		err_cnt++;
	}

	return err_cnt;
}

uint32_t qspi_fram_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t write_data_0[QSPI_FIFO_MAX_BYTE] = {0x00};
	uint32_t write_data_1[QSPI_FIFO_MAX_BYTE] = {0x00};
	uint32_t mem_addr_0 = 0x000000;
	uint32_t mem_addr_1 = 0x001000;
	uint8_t start_val_0 = 0x20;
	uint8_t start_val_1 = 0x30;

	info("* [%d] Start QSPI FRAM Test\n", test_no);

	qspi_create_fifo_data(start_val_0, write_data_0, ARRAY_SIZE(write_data_0), false);
	qspi_create_fifo_data(start_val_1, write_data_1, ARRAY_SIZE(write_data_1), false);

	info("* [%d-1] Start QSPI FRAM [0]: Write data Test\n", test_no);
	if (!qspi_fram_multi_write(QSPI_FRAM_MEM0, mem_addr_0, QSPI_FIFO_MAX_BYTE, start_val_0)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-2] Start QSPI FRAM [1]: Write data Test\n", test_no);
	if (!qspi_fram_multi_write(QSPI_FRAM_MEM1, mem_addr_1, QSPI_FIFO_MAX_BYTE, start_val_1)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-3] Start QSPI FRAM [0]: Read data Test\n", test_no);
	if (!qspi_fram_multi_read(QSPI_FRAM_MEM0, mem_addr_0, QSPI_FIFO_MAX_BYTE, start_val_0)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-4] Start QSPI FRAM [1]: Read data Test\n", test_no);
	if (!qspi_fram_multi_read(QSPI_FRAM_MEM1, mem_addr_1, QSPI_FIFO_MAX_BYTE, start_val_1)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	print_result(test_no, err_cnt);

	return err_cnt;
}
