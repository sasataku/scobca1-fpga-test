/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "qspi_common.h"
#include "common.h"

#define QSPI_READ_RETRY(count) (count)
#define QSPI_READ_SIZE(size) (size)
#define QSPI_DATA_MEM_0 (0)
#define QSPI_DATA_MEM_1 (1)
#define QSPI_ASR_IDLE (0x00)
#define QSPI_ASR_BUSY (0x01)
#define QSPI_RX_FIFO_MAX_BYTE (16)
#define QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT (4)
#define QSPI_SPI_MODE_SINGLE (0x00000001)

static uint32_t exp_write_disable[] = {0x00, 0x00};
static uint32_t exp_write_enable[] = {0x02, 0x02};
static uint32_t exp_wip[] = {0x03, 0x03};
static uint32_t mem_addr[] = {0x80, 0x00, 0x00};

static bool is_qspi_idle(void)
{
	printk("* Confirm QSPI Access Status is `Idle`\n");
	if (!assert32(SCOBCA1_FPGA_DATA_QSPI_ASR, QSPI_ASR_IDLE,
			QSPI_READ_RETRY(10))) {
		printk("QSPI (Data Memory) is busy, so exit test\n");
		return false;
	}

	return true;
}

static bool activate_spi_ss(uint32_t spi_mode)
{
	printk("* Activate SPI SS with %08x\n", spi_mode);
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, spi_mode);
	if (!is_qspi_idle()) {
		return false;
	}

	return true;
}

static bool inactivate_spi_ss(void)
{
	printk("* Inactivate SPI SS\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, 0x00000000);
	if (!is_qspi_idle()) {
		return false;
	}

	return true;
}

static void write_data_to_flash(uint32_t *write_data, size_t size)
{
	printk("* Write TX FIFO %d byte\n", size);
	for (uint8_t i=0; i<size; i++) {
		write32(SCOBCA1_FPGA_DATA_QSPI_TDR, write_data[i]);
	}
}

static bool send_dummy_cycle(uint8_t dummy_count)
{
	printk("* Send dummy cycle %d byte\n", dummy_count);
	for (uint8_t i=0; i<dummy_count; i++) {
		write32(SCOBCA1_FPGA_DATA_QSPI_RDR, 0x00);
	}

	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Discard dummy data\n");
	for (uint8_t i=0; i<dummy_count; i++) {
		sys_read32(SCOBCA1_FPGA_DATA_QSPI_RDR);
	}

	return true;
}

static bool read_and_verify_rx_data(size_t size, uint32_t *exp_val)
{
	bool ret = true;

	printk("* Reqest RX FIFO %d byte\n", size);
	for (uint8_t i=0; i<size; i++) {
		write32(SCOBCA1_FPGA_DATA_QSPI_RDR, 0x00);
	}

	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Read RX FIFO %d byte and verify the value\n", size);
	for (uint8_t i=0; i<size; i++) {
		if (!assert32(SCOBCA1_FPGA_DATA_QSPI_RDR, exp_val[i],
						QSPI_READ_RETRY(1))) {
			ret = false;
		}
	}

	return ret;
}

static bool is_qspi_control_done(void)
{
	printk("* Confirm QSPI Interrupt Stauts is `SPI Control Done`\n");
	if (!assert32(SCOBCA1_FPGA_DATA_QSPI_ISR, 0x01, QSPI_READ_RETRY(10))) {
		return false;
	}

	printk("* Clear QSPI Interrupt Stauts\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_ISR, 0x01);
	if (!assert32(SCOBCA1_FPGA_DATA_QSPI_ISR, 0x00, QSPI_READ_RETRY(10))) {
		return false;
	}

	return true;
}

static bool verify_status_resisger1(uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(QSPI_SPI_MODE_SINGLE)) {
		return false;
	}

	printk("* Request Status Register 1\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x05);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read Memory data (2byte) adn Verify */
	ret = read_and_verify_rx_data(QSPI_READ_SIZE(2), exp_val);

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

static bool clear_status_register(void)
{
	/* Active SPI SS with SINGLE-IO */
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, 0x00000001);
	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Clear Status Register (Instructure:0x30) \n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x30);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		return false;
	}

	return true;
}

static bool set_write_enable(void)
{
	if (!verify_status_resisger1(exp_write_disable)) {
		return false;
	}

	/* Active SPI SS with SINGLE-IO */
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, 0x00000001);
	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Set `Write Enable` (Instructure:0x06) \n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x06);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done()) {
		return false;
	}

	if (!verify_status_resisger1(exp_write_enable)) {
		return false;
	}

	return true;
}

static bool set_quad_io_mode(void)
{
	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(QSPI_SPI_MODE_SINGLE)) {
		return false;
	}

	printk("* Set QUAD I/O mode to configuration register\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x01);
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x00);
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x02);
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

static bool verify_config_register(uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(QSPI_SPI_MODE_SINGLE)) {
		return false;
	}

	printk("* Request Configuration Register\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x35);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read Memory data (2byte) adn Verify */
	ret = read_and_verify_rx_data(QSPI_READ_SIZE(2), exp_val);

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

static bool verify_quad_io_mode(void)
{
	uint32_t exp_quad_mode[2] = {0x02, 0x02};

	if (!verify_config_register(exp_quad_mode)) {
		return false;
	}

	return true;
}

static bool qspi_data_memory_init(uint8_t mem_no)
{
	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	/* TODO: Select Memory number 0 or 1 */

	printk("* [#1] Clear Status Register\n");
	if (!clear_status_register()) {
		return false;
	}

	printk("* [#2] Set to `Write Enable'\n");
	if (!set_write_enable()) {
		return false;
	}

	printk("* [#3] Set to `QUAD I/O modee'\n");
	if (!set_quad_io_mode()) {
		return false;
	}

	/* Wait 1 sec */
	k_sleep(K_MSEC(1000));

	printk("* [#4] Verify Configuration Register is QUAD I/O mode (0x02)\n");
	if (!verify_quad_io_mode()) {
		return false;
	}

	return true;
}

static bool qspi_block_erase(void)
{
	/* Activate SPI SS */
	if (!activate_spi_ss(QSPI_SPI_MODE_SINGLE) ) {
		return false;
	}

	printk("* Send Block Erase instruction\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0xD8);

	printk("* Send Memory Address (3byte)\n");
	write_data_to_flash(mem_addr, ARRAY_SIZE(mem_addr));
	if (!is_qspi_idle()) {
		return false;
	}

	/* Inactive the SPI SS */
	if (!inactivate_spi_ss() ) {
		return false;
	}

	return true;
}

static bool qspi_data_memory_set_quad_read_mode(void)
{
	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(QSPI_SPI_MODE_SINGLE) ) {
		return false;
	}

	printk("* Set QUAD-IO read mode\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0xEB);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Keep SPI SS for QUAD Read */
	return true;
}

static bool qspi_data_memory_quad_read_data(uint8_t read_size, uint32_t *exp_vals)
{
	bool ret;

	printk("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, 0x00020001);

	printk("* Send Memory Address (3byte)\n");
	write_data_to_flash(mem_addr, ARRAY_SIZE(mem_addr));
	
	printk("* Send Mode (0x00)\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x00);

	/* Send Dummy Cycle */
	send_dummy_cycle(QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read Initial RX data and verify */
	ret = read_and_verify_rx_data(read_size, exp_vals);

	/* Inactive the SPI SS */
	if (!inactivate_spi_ss()) {
		return false;
	}

	return ret;
}

static bool qspi_memory_data_write_data(uint8_t write_size, uint32_t *write_data)
{
	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(QSPI_SPI_MODE_SINGLE) ) {
		return false;
	}

	printk("* Snd QUAD Page program instruction\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x32);

	printk("* Send Memory Address (3byte)\n");
	write_data_to_flash(mem_addr, ARRAY_SIZE(mem_addr));
	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, 0x00020001);

	/* Write data */
	write_data_to_flash(write_data, write_size);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss() ) {
		return false;
	}

	return true;
}

static bool qspi_data_memeory_erase_test(uint8_t mem_no)
{
	printk("* [#1] Set to `Write Enable'\n");
	if (!set_write_enable()) {
		return false;
	}

	printk("* [#2] Block Erase\n");
	if (!qspi_block_erase()) {
		return false;
	}

	printk("* [#3] Verify Status Register (WEL/WIP:0x03)\n");
	if (!verify_status_resisger1(exp_wip)) {
		return false;
	}

	/* Wait 1 sec */
	k_sleep(K_MSEC(1000));

	printk("* [#4] Verify Status Register (WEL=0)\n");
	if (!verify_status_resisger1(exp_write_disable)) {
		return false;
	}

	return true;
}

static bool qspi_data_memory_read_data_test(uint8_t mem_no, uint8_t read_size, uint32_t *exp_vals)
{
	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	/* TODO: Select Memory number 0 or 1 */

	printk("* [#1] Set QUAD-IO Read Mode\n");
	if (!qspi_data_memory_set_quad_read_mode()) {
		return false;
	}

	printk("* [#2] Read Data (QUAD-IO Mode) \n");
	if (!qspi_data_memory_quad_read_data(read_size, exp_vals)) {
		return false;
	}

	return true;
}

static bool qspi_data_memory_write_data_test(uint8_t mem_no, uint8_t write_size, uint32_t *write_data)
{
	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	printk("* [#1] Set to `Write Enable'\n");
	if (!set_write_enable()) {
		return false;
	}

	printk("* [#2] Write Data (QUAD Mode)\n");
	if (!qspi_memory_data_write_data(write_size, write_data)) {
		return false;
	}

	printk("* [#3] Verify Status Register (WEL=0)\n");
	if (!verify_status_resisger1(exp_write_disable)) {
		return false;
	}

	return true;
}

uint32_t qspi_norflash_initialize(uint32_t test_no)
{
	uint32_t err_cnt = 0;

	printk("* [%d-1] Start QSPI Data Memory: Initialize\n", test_no);
	if (!qspi_data_memory_init(QSPI_DATA_MEM_0)) {
		err_cnt++;
	}

	return err_cnt;
}

uint32_t qspi_data_memory_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t exp_init_data[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t write_data[QSPI_RX_FIFO_MAX_BYTE] =
			{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf1, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01};

	/* Set expected initial data (all 0xFF) */
	for (uint8_t i=0; i<QSPI_RX_FIFO_MAX_BYTE; i++) {
		exp_init_data[i] = 0xFF;
	}

	printk("* [%d] Start QSPI Data Memory Test\n", test_no);

	printk("* [%d-1] Start QSPI Data Memory: Erase Test \n", test_no);
	if (!qspi_data_memeory_erase_test(QSPI_DATA_MEM_0)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-2] Start QSPI Data Memory: Read initial data Test \n", test_no);
	if (!qspi_data_memory_read_data_test(QSPI_DATA_MEM_0, QSPI_RX_FIFO_MAX_BYTE, exp_init_data)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-3] Start QSPI Data Memory: Write data Test \n", test_no);
	if (!qspi_data_memory_write_data_test(QSPI_DATA_MEM_0, QSPI_RX_FIFO_MAX_BYTE, write_data)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-4] Start QSPI Data Memory: Read data Test \n", test_no);
	if (!qspi_data_memory_read_data_test(QSPI_DATA_MEM_0, QSPI_RX_FIFO_MAX_BYTE, write_data)) {
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	print_result(test_no, err_cnt);

	return err_cnt;
}
