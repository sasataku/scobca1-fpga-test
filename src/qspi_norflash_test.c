/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "qspi_common.h"
#include "common.h"

#define QSPI_NOR_FLASH_MEM_ADDR_SIZE (3u)
#define QSPI_DATA_MEM0 (0u)
#define QSPI_DATA_MEM1 (1u)
#define QSPI_DATA_MEM0_SS (0x01)
#define QSPI_DATA_MEM1_SS (0x02)
#define QSPI_ASR_IDLE (0x00)
#define QSPI_ASR_BUSY (0x01)
#define QSPI_RX_FIFO_MAX_BYTE (16u)
#define QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT (4u)
#define QSPI_SPI_MODE_QUAD (0x00020000)
#define QSPI_NOR_FLASH_SECTOR (4*1024)

enum QspiEraseType
{
	QSPI_ERASE_SECTOR,		/* 4KB */
	QSPI_ERASE_HALF_BLOCK,	/* 32KB */
	QSPI_ERASE_BLOCK,		/* 64KB */
};

static bool is_qspi_idle(void)
{
	printk("* Confirm QSPI Access Status is `Idle`\n");
	if (!assert32(SCOBCA1_FPGA_DATA_QSPI_ASR, QSPI_ASR_IDLE,
			REG_READ_RETRY(10))) {
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

static void write_mem_addr_to_flash(uint32_t mem_addr)
{
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, (mem_addr & 0x00FF0000) >> 16);
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, (mem_addr & 0x0000FF00) >> 8);
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, (mem_addr & 0x000000FF));
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

static bool read_and_verify_rx_data(size_t exp_size, uint32_t *exp_val)
{
	bool ret = true;

	printk("* Reqest RX FIFO %d byte\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		write32(SCOBCA1_FPGA_DATA_QSPI_RDR, 0x00);
	}

	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Read RX FIFO %d byte and verify the value\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		if (!assert32(SCOBCA1_FPGA_DATA_QSPI_RDR, exp_val[i],
						REG_READ_RETRY(1))) {
			ret = false;
		}
	}

	return ret;
}

static bool is_qspi_control_done(void)
{
	printk("* Confirm QSPI Interrupt Stauts is `SPI Control Done`\n");
	if (!assert32(SCOBCA1_FPGA_DATA_QSPI_ISR, 0x01, REG_READ_RETRY(10))) {
		return false;
	}

	printk("* Clear QSPI Interrupt Stauts\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_ISR, 0x01);
	if (!assert32(SCOBCA1_FPGA_DATA_QSPI_ISR, 0x00, REG_READ_RETRY(10))) {
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
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x05);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read Memory data (2byte) adn Verify */
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

static bool clear_status_register(uint32_t spi_ss)
{
	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
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

static bool set_write_enable(uint32_t spi_ss)
{
	uint32_t exp_write_enable[] = {0x02, 0x02};

	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
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

	if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_enable), exp_write_enable)) {
		return false;
	}

	return true;
}

static bool set_quad_io_mode(uint32_t spi_ss)
{
	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
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

static bool verify_config_register(uint32_t spi_ss, size_t exp_size, uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		return false;
	}

	printk("* Request Configuration Register\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x35);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Read Memory data (2byte) adn Verify */
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
	uint32_t exp_quad_mode[2] = {0x02, 0x02};

	if (!verify_config_register(spi_ss, ARRAY_SIZE(exp_quad_mode), exp_quad_mode)) {
		return false;
	}

	return true;
}

static bool qspi_data_memory_init(uint8_t mem_no)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_DATA_MEM0) {
		spi_ss = QSPI_DATA_MEM0_SS;
	} else {
		spi_ss = QSPI_DATA_MEM1_SS;
	}

	printk("* [#1] Clear Status Register\n");
	if (!clear_status_register(spi_ss)) {
		return false;
	}

	printk("* [#2] Set to `Write Enable'\n");
	if (!set_write_enable(spi_ss)) {
		return false;
	}

	printk("* [#3] Set to `QUAD I/O modee'\n");
	if (!set_quad_io_mode(spi_ss)) {
		return false;
	}

	/* Wait 1 sec */
	k_sleep(K_MSEC(1000));

	printk("* [#4] Verify Configuration Register is QUAD I/O mode (0x02)\n");
	if (!verify_quad_io_mode(spi_ss)) {
		return false;
	}

	return true;
}

bool qspi_memory_data_erase(uint32_t spi_ss, enum QspiEraseType type, uint32_t mem_addr)
{
	/* Activate SPI SS */
	if (!activate_spi_ss(spi_ss) ) {
		return false;
	}

	switch (type) {
	case QSPI_ERASE_SECTOR:
		printk("* Send Sector (4KB) Erase instruction\n");
		write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x20);
		break;
	case QSPI_ERASE_HALF_BLOCK:
		printk("* Send Sector (32KB) Erase instruction\n");
		write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x52);
		break;
	case QSPI_ERASE_BLOCK:
		printk("* Send Block (64KB) Erase instruction\n");
		write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0xD8);
		break;
	default:
		printk("   Invalid Erase Type %d\n", type);
		return false;
	}

	printk("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(mem_addr);
	if (!is_qspi_idle()) {
		return false;
	}

	/* Inactive the SPI SS */
	if (!inactivate_spi_ss() ) {
		return false;
	}

	return true;
}

static bool qspi_data_memory_set_quad_read_mode(uint32_t spi_ss)
{
	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
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

static bool qspi_data_memory_quad_read_data(uint32_t spi_ss, uint32_t mem_addr, uint8_t read_size, uint32_t *exp_vals)
{
	bool ret;

	printk("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, QSPI_SPI_MODE_QUAD + spi_ss);

	printk("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(mem_addr);
	
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

static bool qspi_memory_data_quad_write(uint32_t spi_ss, uint32_t mem_addr, uint8_t write_size, uint32_t *write_data)
{
	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(spi_ss)) {
		return false;
	}

	printk("* Snd QUAD Page program instruction\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_TDR, 0x32);

	printk("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(mem_addr);
	if (!is_qspi_idle()) {
		return false;
	}

	printk("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_DATA_QSPI_ACR, QSPI_SPI_MODE_QUAD + spi_ss);

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

static bool qspi_data_memory_erase(uint8_t mem_no, uint32_t mem_addr, bool is_wait_idle)
{
	uint32_t spi_ss;
	uint32_t exp_write_disable[] = {0x00, 0x00};

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_DATA_MEM0) {
		spi_ss = QSPI_DATA_MEM0_SS;
	} else {
		spi_ss = QSPI_DATA_MEM1_SS;
	}

	printk("* [#1] Set to `Write Enable'\n");
	if (!set_write_enable(spi_ss)) {
		return false;
	}

	printk("* [#2] Sector (4KB) Erase\n");
	if (!qspi_memory_data_erase(spi_ss, QSPI_ERASE_SECTOR, mem_addr)) {
		return false;
	}

	if (is_wait_idle) {
		/* Wait 250 msec */
		k_sleep(K_MSEC(250));
		if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
			return false;
		}
	}

	return true;
}

bool qspi_data_memory_read(uint8_t mem_no, uint32_t mem_addr, uint8_t read_size, uint32_t *exp_vals)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_DATA_MEM0) {
		spi_ss = QSPI_DATA_MEM0_SS;
	} else {
		spi_ss = QSPI_DATA_MEM1_SS;
	}

	printk("* [#1] Set QUAD-IO Read Mode\n");
	if (!qspi_data_memory_set_quad_read_mode(spi_ss)) {
		return false;
	}

	printk("* [#2] Read Data (QUAD-IO Mode) \n");
	if (!qspi_data_memory_quad_read_data(spi_ss, mem_addr, read_size, exp_vals)) {
		return false;
	}

	return true;
}

bool qspi_data_memory_read_sector(uint8_t mem_no, uint32_t mem_addr, uint8_t start_val, bool is_init)
{
	bool ret = true;
	uint32_t spi_ss;
	uint32_t exp_vals[QSPI_RX_FIFO_MAX_BYTE];
	uint16_t loop_count;

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_DATA_MEM0) {
		spi_ss = QSPI_DATA_MEM0_SS;
	} else {
		spi_ss = QSPI_DATA_MEM1_SS;
	}

	loop_count = QSPI_NOR_FLASH_SECTOR/QSPI_RX_FIFO_MAX_BYTE;
	for (uint16_t i=0; i<loop_count; i++) {

		printk("* [#1] Set QUAD-IO Read Mode\n");
		if (!qspi_data_memory_set_quad_read_mode(spi_ss)) {
			return false;
		}

		printk("* [#2] Read Data (QUAD-IO Mode) \n");
		if (is_init) {
			qspi_create_fifo_data(0xFF, exp_vals, QSPI_RX_FIFO_MAX_BYTE, true);
		} else {
			start_val = qspi_create_fifo_data(start_val, exp_vals, QSPI_RX_FIFO_MAX_BYTE, false);
		}

		if (!qspi_data_memory_quad_read_data(spi_ss, mem_addr, QSPI_RX_FIFO_MAX_BYTE, exp_vals)) {
			ret = false;
		}
		mem_addr += QSPI_RX_FIFO_MAX_BYTE;
	}

	return ret;
}

bool qspi_data_memory_write(uint8_t mem_no, uint32_t mem_addr, uint8_t write_size, uint32_t *write_data)
{
	uint32_t spi_ss;
	uint32_t exp_write_disable[] = {0x00, 0x00};

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_DATA_MEM0) {
		spi_ss = QSPI_DATA_MEM0_SS;
	} else {
		spi_ss = QSPI_DATA_MEM1_SS;
	}

	printk("* [#1] Set to `Write Enable'\n");
	if (!set_write_enable(spi_ss)) {
		return false;
	}

	printk("* [#2] Write Data (QUAD Mode)\n");
	if (!qspi_memory_data_quad_write(spi_ss, mem_addr, write_size, write_data)) {
		return false;
	}

	printk("* [#3] Verify Status Register (WEL=0)\n");
	if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
		return false;
	}

	return true;
}

bool qspi_data_memory_write_sector(uint8_t mem_no, uint32_t mem_addr, uint8_t *start_val)
{
	uint32_t spi_ss;
	uint32_t write_data[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t exp_write_disable[] = {0x00, 0x00};
	uint16_t loop_count;

	if (mem_no > 1) {
		printk("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (mem_no == QSPI_DATA_MEM0) {
		spi_ss = QSPI_DATA_MEM0_SS;
	} else {
		spi_ss = QSPI_DATA_MEM1_SS;
	}

	loop_count = QSPI_NOR_FLASH_SECTOR/QSPI_RX_FIFO_MAX_BYTE;
	for (uint16_t i=0; i<loop_count; i++) {

		printk("* [#1] Set to `Write Enable'\n");
		if (!set_write_enable(spi_ss)) {
			return false;
		}

		printk("* [#2] Write Data (QUAD Mode)\n");
		*start_val = qspi_create_fifo_data(*start_val, write_data, QSPI_RX_FIFO_MAX_BYTE, false);
		if (!qspi_memory_data_quad_write(spi_ss, mem_addr, QSPI_RX_FIFO_MAX_BYTE, write_data)) {
			return false;
		}
		mem_addr += QSPI_RX_FIFO_MAX_BYTE;

		printk("* [#3] Verify Status Register (WEL=0)\n");
		if (!verify_status_resisger1(spi_ss, ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
			return false;
		}
	}

	return true;
}

uint32_t qspi_norflash_initialize(uint32_t test_no)
{
	uint32_t err_cnt = 0;

	printk("* [%d-1] Start QSPI Data Memory [0]: Initialize\n", test_no);
	if (!qspi_data_memory_init(QSPI_DATA_MEM0)) {
		err_cnt++;
	}

	printk("* [%d-1] Start QSPI Data Memory [1]: Initialize\n", test_no);
	if (!qspi_data_memory_init(QSPI_DATA_MEM1)) {
		err_cnt++;
	}

	return err_cnt;
}

uint32_t qspi_data_memory_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t exp_init_data[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t write_data[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t mem_addr = 0x00800000;
	uint8_t start_val = 0x00;
	bool is_wait_idle = true;

	/* Create test data */
	for (uint8_t i=0; i<QSPI_RX_FIFO_MAX_BYTE; i++) {
		exp_init_data[i] = 0xFF;
	}
	start_val = qspi_create_fifo_data(start_val, write_data, QSPI_RX_FIFO_MAX_BYTE, false);

	printk("* [%d] Start QSPI Data Memory Test\n", test_no);

	printk("* [%d-1] Start QSPI Data Memory: Erase Test (Sector)\n", test_no);
	if (!qspi_data_memory_erase(QSPI_DATA_MEM0, mem_addr, is_wait_idle)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-2] Start QSPI Data Memory: Read initial data Test (Sector:4KB)\n", test_no);
	if (!qspi_data_memory_read(QSPI_DATA_MEM0, mem_addr, QSPI_RX_FIFO_MAX_BYTE, exp_init_data)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-3] Start QSPI Data Memory: Write data Test \n", test_no);
	if (!qspi_data_memory_write(QSPI_DATA_MEM0, mem_addr, QSPI_RX_FIFO_MAX_BYTE, write_data)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-4] Start QSPI Data Memory: Read data Test \n", test_no);
	if (!qspi_data_memory_read(QSPI_DATA_MEM0, mem_addr, QSPI_RX_FIFO_MAX_BYTE, write_data)) {
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	print_result(test_no, err_cnt);

	return err_cnt;
}

uint32_t qspi_data_memory_sector_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t mem_addr = 0x00800000;
	uint8_t start_val = 0x00;
	bool is_wait_idle = true;

	printk("* [%d] Start QSPI Data Memory Test (Sector)\n", test_no);
	printk("* [%d-1] Start QSPI Data Memory: Erase Test (Sector)\n", test_no);
	if (!qspi_data_memory_erase(QSPI_DATA_MEM0, mem_addr, is_wait_idle)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-2] Start QSPI Data Memory: Read initial data Test (Sector:4KB)\n", test_no);
	is_wait_idle = true;
	if (!qspi_data_memory_read_sector(QSPI_DATA_MEM0, mem_addr, start_val, is_wait_idle)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-3] Start QSPI Data Memory: Write data Test (Sector:4KB)\n", test_no);
	if (!qspi_data_memory_write_sector(QSPI_DATA_MEM0, mem_addr, &start_val)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [%d-2] Start QSPI Data Memory: Read initial data Test (Sector:4KB)\n", test_no);
	if (!qspi_data_memory_read_sector(QSPI_DATA_MEM0, mem_addr, start_val, false)) {
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	print_result(test_no, err_cnt);

	return err_cnt;
}
