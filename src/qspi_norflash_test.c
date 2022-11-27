/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "system_reg.h"
#include "qspi_common.h"
#include "common.h"

#define QSPI_NOR_FLASH_MEM_ADDR_SIZE (3u)
#define QSPI_DATA_MEM0_SS (0x01)
#define QSPI_DATA_MEM1_SS (0x02)
#define QSPI_ASR_IDLE (0x00)
#define QSPI_ASR_BUSY (0x01)
#define QSPI_RX_FIFO_MAX_BYTE (16u)
#define QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT (4u)
#define QSPI_SPI_MODE_QUAD (0x00020000)

static bool qspi_select_mem(uint32_t base, uint8_t mem_no, uint32_t *spi_ss)
{
	if (base == SCOBCA1_FPGA_CFG_BASE_ADDR) {
		/* Config Memory is switched by CFGMEMSEL */
		if (mem_no == QSPI_DATA_MEM0) {
			debug("* [#0] Select Config Memory 0\n");
			write32(SCOBCA1_FPGA_SYSREG_CFGMEMCTL, 0x00);
			if (!assert32(SCOBCA1_FPGA_SYSREG_CFGMEMCTL, 0x00, REG_READ_RETRY(100000))) {
				err("  !!! Can not select Config Memory %d\n", mem_no);
				return false;
			}
		} else {
			debug("* [#0] Select Config Memory 1\n");
			write32(SCOBCA1_FPGA_SYSREG_CFGMEMCTL, 0x10);
			if (!assert32(SCOBCA1_FPGA_SYSREG_CFGMEMCTL, 0x30, REG_READ_RETRY(100000))) {
				err("  !!! Can not select Config Memory %d\n", mem_no);
				return false;
			}
		}
		*spi_ss = 0x01;
	} else {
		if (mem_no == QSPI_DATA_MEM0) {
			*spi_ss = QSPI_DATA_MEM0_SS;
		} else {
			*spi_ss = QSPI_DATA_MEM1_SS;
		}
	}

	return true;
}

static bool is_qspi_idle(uint32_t base)
{
	debug("* Confirm QSPI Access Status is `Idle`\n");
	if (!assert32(SCOBCA1_FPGA_NORFLASH_QSPI_ASR(base), QSPI_ASR_IDLE,
			REG_READ_RETRY(10))) {
		err("QSPI (Data Memory) is busy, so exit test\n");
		return false;
	}

	return true;
}

static bool activate_spi_ss(uint32_t base, uint32_t spi_mode)
{
	debug("* Activate SPI SS with %08x\n", spi_mode);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ACR(base), spi_mode);
	if (!is_qspi_idle(base)) {
		return false;
	}

	return true;
}

static bool inactivate_spi_ss(uint32_t base)
{
	debug("* Inactivate SPI SS\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ACR(base), 0x00000000);
	if (!is_qspi_idle(base)) {
		return false;
	}

	return true;
}

static void write_mem_addr_to_flash(uint32_t base, uint32_t mem_addr)
{
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), (mem_addr & 0x00FF0000) >> 16);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), (mem_addr & 0x0000FF00) >> 8);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), (mem_addr & 0x000000FF));
}

static void write_data_to_flash(uint32_t base, uint32_t *write_data, size_t size)
{
	debug("* Write TX FIFO %d byte\n", size);
	for (uint8_t i=0; i<size; i++) {
		write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), write_data[i]);
	}
}

static bool send_dummy_cycle(uint32_t base, uint8_t dummy_count)
{
	debug("* Send dummy cycle %d byte\n", dummy_count);
	for (uint8_t i=0; i<dummy_count; i++) {
		write32(SCOBCA1_FPGA_NORFLASH_QSPI_RDR(base), 0x00);
	}

	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	debug("* Discard dummy data\n");
	for (uint8_t i=0; i<dummy_count; i++) {
		sys_read32(SCOBCA1_FPGA_NORFLASH_QSPI_RDR(base));
	}

	return true;
}

static bool read_and_verify_rx_data(uint32_t base, size_t exp_size, uint32_t *exp_val)
{
	bool ret = true;

	debug("* Reqest RX FIFO %d byte\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		write32(SCOBCA1_FPGA_NORFLASH_QSPI_RDR(base), 0x00);
	}

	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	debug("* Read RX FIFO %d byte and verify the value\n", exp_size);
	for (uint8_t i=0; i<exp_size; i++) {
		if (!assert32(SCOBCA1_FPGA_NORFLASH_QSPI_RDR(base), exp_val[i],
						REG_READ_RETRY(0))) {
			ret = false;
		}
	}

	return ret;
}

static bool is_qspi_control_done(uint32_t base)
{
	debug("* Confirm QSPI Interrupt Stauts is `SPI Control Done`\n");
	if (!assert32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base), 0x01, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	debug("* Clear QSPI Interrupt Stauts\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base), 0x01);
	if (!assert32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base), 0x00, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	return true;
}

static bool verify_status_resisger1(uint32_t base, uint32_t spi_ss, size_t exp_size, uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* Request Status Register 1\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x05);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	/* Read Memory data (2byte) adn Verify */
	ret = read_and_verify_rx_data(base, exp_size, exp_val);
	if (!ret) {
		assert();
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done(base)) {
		assert();
		return false;
	}

	return ret;
}

static bool clear_status_register(uint32_t base, uint32_t spi_ss)
{
	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	/* Clear All ISR */
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ISR(base), 0xFFFFFFFF);

	debug("* Clear Status Register (Instructure:0x30) \n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x30);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done(base)) {
		assert();
		return false;
	}

	return true;
}

static bool set_write_enable(uint32_t base, uint32_t spi_ss)
{
	uint32_t exp_write_enable[] = {0x02, 0x02};

	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* Set `Write Enable` (Instructure:0x06) \n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x06);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done(base)) {
		assert();
		return false;
	}

	if (!verify_status_resisger1(base, spi_ss, ARRAY_SIZE(exp_write_enable), exp_write_enable)) {
		assert();
		return false;
	}

	return true;
}

static bool set_quad_io_mode(uint32_t base, uint32_t spi_ss)
{
	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* Set QUAD I/O mode to configuration register\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x01);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x00);
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x02);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	/* Inactivate SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if(!is_qspi_control_done(base)) {
		assert();
		return false;
	}

	return true;
}

static bool verify_config_register(uint32_t base, uint32_t spi_ss, size_t exp_size, uint32_t *exp_val)
{
	bool ret;

	/* Activate SPI SS with SINGLE-IO */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* Request Configuration Register\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x35);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	/* Read Memory data (2byte) adn Verify */
	ret = read_and_verify_rx_data(base, exp_size, exp_val);

	/* Inactive SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	/* Confirm SPI Control is Done */
	if (!is_qspi_control_done(base)) {
		assert();
		return false;
	}

	return ret;
}

static bool verify_quad_io_mode(uint32_t base, uint32_t spi_ss)
{
	uint32_t exp_quad_mode[2] = {0x02, 0x02};

	if (!verify_config_register(base, spi_ss, ARRAY_SIZE(exp_quad_mode), exp_quad_mode)) {
		assert();
		return false;
	}

	return true;
}

static bool qspi_norflash_init(uint32_t base, uint8_t mem_no)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		err("   !!! Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (!qspi_select_mem(base, mem_no, &spi_ss)) {
		return false;
	}

	debug("* [#1] Clear Status Register\n");
	if (!clear_status_register(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* [#2] Set to `Write Enable'\n");
	if (!set_write_enable(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* [#3] Set to `QUAD I/O modee'\n");
	if (!set_quad_io_mode(base, spi_ss)) {
		assert();
		return false;
	}

	/* Wait 1 sec */
	k_sleep(K_MSEC(1000));

	debug("* [#4] Verify Configuration Register is QUAD I/O mode (0x02)\n");
	if (!verify_quad_io_mode(base, spi_ss)) {
		assert();
		return false;
	}

	return true;
}

bool qspi_memory_data_erase(uint32_t base, uint32_t spi_ss, enum QspiEraseType type, uint32_t mem_addr)
{
	/* Activate SPI SS */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	switch (type) {
	case QSPI_ERASE_SECTOR:
		debug("* Send Sector (4KB) Erase instruction\n");
		write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x20);
		break;
	case QSPI_ERASE_HALF_BLOCK:
		debug("* Send Sector (32KB) Erase instruction\n");
		write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x52);
		break;
	case QSPI_ERASE_BLOCK:
		debug("* Send Block (64KB) Erase instruction\n");
		write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0xD8);
		break;
	default:
		assert();
		err("   Invalid Erase Type %d\n", type);
		return false;
	}

	debug("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(base, mem_addr);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	/* Inactive the SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	return true;
}

static bool qspi_norflash_set_quad_read_mode(uint32_t base, uint32_t spi_ss)
{
	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* Set QUAD-IO read mode\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0xEB);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	/* Keep SPI SS for QUAD Read */
	return true;
}

static bool qspi_norflash_quad_read_data(uint32_t base, uint32_t spi_ss, uint32_t mem_addr,
										uint8_t read_size, uint32_t *exp_vals)
{
	bool ret;

	debug("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ACR(base), QSPI_SPI_MODE_QUAD + spi_ss);

	debug("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(base, mem_addr);
	
	debug("* Send Mode (0x00)\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x00);

	/* Send Dummy Cycle */
	send_dummy_cycle(base, QSPI_NOR_FLASH_DUMMY_CYCLE_COUNT);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	/* Read Initial RX data and verify */
	ret = read_and_verify_rx_data(base, read_size, exp_vals);
	if (!ret) {
		assert();
	}

	/* Inactive the SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	return ret;
}

static bool qspi_memory_data_quad_write(uint32_t base, uint32_t spi_ss, uint32_t mem_addr,
										uint8_t write_size, uint32_t *write_data)
{
	/* Active SPI SS with SINGLE-IO */
	if (!activate_spi_ss(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* Snd QUAD Page program instruction\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_TDR(base), 0x32);

	debug("* Send Memory Address (3byte)\n");
	write_mem_addr_to_flash(base, mem_addr);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	debug("* Activate SPI SS with Quad-IO SPI Mode\n");
	write32(SCOBCA1_FPGA_NORFLASH_QSPI_ACR(base), QSPI_SPI_MODE_QUAD + spi_ss);

	/* Write data */
	write_data_to_flash(base, write_data, write_size);
	if (!is_qspi_idle(base)) {
		assert();
		return false;
	}

	/* Inactive SPI SS */
	if (!inactivate_spi_ss(base)) {
		assert();
		return false;
	}

	return true;
}

bool qspi_norflash_erase(uint32_t base, uint8_t mem_no, enum QspiEraseType type,
								uint32_t mem_addr, bool is_wait_idle)
{
	uint32_t spi_ss;
	uint32_t exp_write_disable[] = {0x00, 0x00};

	if (mem_no > 1) {
		err("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (!qspi_select_mem(base, mem_no, &spi_ss)) {
		return false;
	}

	debug("* [#1] Clear Status Register\n");
	if (!clear_status_register(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* [#2] Set to `Write Enable'\n");
	if (!set_write_enable(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* [#3] Erase\n");
	if (!qspi_memory_data_erase(base, spi_ss, type, mem_addr)) {
		assert();
		return false;
	}

	if (!is_wait_idle) {
		return true;
	}

	switch (type) {
	case QSPI_ERASE_SECTOR:
		k_sleep(K_MSEC(300));
		break;
	case QSPI_ERASE_HALF_BLOCK:
		k_sleep(K_MSEC(400));
		break;
	case QSPI_ERASE_BLOCK:
		k_sleep(K_MSEC(800));
		break;
	default:
		break;
	}

	if (!verify_status_resisger1(base, spi_ss,
					ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
		assert();
		return false;
	}

	return true;
}

bool qspi_norflash_read(uint32_t base, uint8_t mem_no, uint32_t mem_addr, uint8_t read_size, uint32_t *exp_vals)
{
	uint32_t spi_ss;

	if (mem_no > 1) {
		err("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (!qspi_select_mem(base, mem_no, &spi_ss)) {
		return false;
	}

	debug("* [#1] Set QUAD-IO Read Mode\n");
	if (!qspi_norflash_set_quad_read_mode(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* [#2] Read Data (QUAD-IO Mode) \n");
	if (!qspi_norflash_quad_read_data(base, spi_ss, mem_addr, read_size, exp_vals)) {
		assert();
		return false;
	}

	return true;
}

bool qspi_norflash_multi_read(uint32_t base, uint8_t mem_no, uint32_t mem_addr, uint32_t size,
								uint8_t start_val, bool is_init)
{
	bool ret = true;
	uint32_t spi_ss;
	uint32_t exp_vals[QSPI_RX_FIFO_MAX_BYTE];
	uint16_t loop_count;

	if (mem_no > 1) {
		err("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (!qspi_select_mem(base, mem_no, &spi_ss)) {
		return false;
	}

	debug("* [#1] Clear Status Register\n");
	if (!clear_status_register(base, spi_ss)) {
		assert();
		return false;
	}

	loop_count = size/QSPI_RX_FIFO_MAX_BYTE;
	for (uint16_t i=0; i<loop_count; i++) {

		debug("* [#2] Set QUAD-IO Read Mode\n");
		if (!qspi_norflash_set_quad_read_mode(base, spi_ss)) {
			assert();
			return false;
		}

		debug("* [#3] Read Data (QUAD-IO Mode) \n");
		if (is_init) {
			qspi_create_fifo_data(0xFF, exp_vals, QSPI_RX_FIFO_MAX_BYTE, true);
		} else {
			start_val = qspi_create_fifo_data(start_val, exp_vals, QSPI_RX_FIFO_MAX_BYTE, false);
		}

		if (!qspi_norflash_quad_read_data(base, spi_ss, mem_addr, QSPI_RX_FIFO_MAX_BYTE, exp_vals)) {
			ret = false;
		}
		mem_addr += QSPI_RX_FIFO_MAX_BYTE;
	}

	return ret;
}

bool qspi_norflash_write(uint32_t base, uint8_t mem_no, uint32_t mem_addr,
						uint8_t write_size, uint32_t *write_data)
{
	uint32_t spi_ss;
	uint32_t exp_write_disable[] = {0x00, 0x00};

	if (mem_no > 1) {
		debug("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (!qspi_select_mem(base, mem_no, &spi_ss)) {
		return false;
	}

	debug("* [#1] Clear Status Register\n");
	if (!clear_status_register(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* [#2] Set to `Write Enable'\n");
	if (!set_write_enable(base, spi_ss)) {
		assert();
		return false;
	}

	debug("* [#3] Write Data (QUAD Mode)\n");
	if (!qspi_memory_data_quad_write(base, spi_ss, mem_addr, write_size, write_data)) {
		assert();
		return false;
	}

	k_usleep(10);

	debug("* [#4] Verify Status Register (WEL=0)\n");
	if (!verify_status_resisger1(base, spi_ss, ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
		assert();
		return false;
	}

	return true;
}

bool qspi_norflash_multi_write(uint32_t base, uint8_t mem_no, uint32_t mem_addr, uint32_t size, uint8_t start_val)
{
	uint32_t spi_ss;
	uint32_t write_data[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t exp_write_disable[] = {0x00, 0x00};
	uint16_t loop_count;

	if (mem_no > 1) {
		err("Invalid Mem number %d (expected 0 or 1)\n", mem_no);
		return false;
	}

	if (!qspi_select_mem(base, mem_no, &spi_ss)) {
		return false;
	}

	debug("* [#1] Clear Status Register\n");
	if (!clear_status_register(base, spi_ss)) {
		assert();
		return false;
	}

	loop_count = size/QSPI_RX_FIFO_MAX_BYTE;
	for (uint16_t i=0; i<loop_count; i++) {

		debug("* [#2] Set to `Write Enable'\n");
		if (!set_write_enable(base, spi_ss)) {
			assert();
			return false;
		}

		debug("* [#3] Write Data (QUAD Mode)\n");
		start_val = qspi_create_fifo_data(start_val, write_data, QSPI_RX_FIFO_MAX_BYTE, false);
		if (!qspi_memory_data_quad_write(base, spi_ss, mem_addr, QSPI_RX_FIFO_MAX_BYTE, write_data)) {
			assert();
			return false;
		}
		mem_addr += QSPI_RX_FIFO_MAX_BYTE;

		k_usleep(10);

		debug("* [#4] Verify Status Register (WEL=0)\n");
		if (!verify_status_resisger1(base, spi_ss, ARRAY_SIZE(exp_write_disable), exp_write_disable)) {
			assert();
			return false;
		}
	}

	return true;
}

uint32_t qspi_norflash_initialize(uint32_t test_no)
{
	uint32_t err_cnt = 0;

	info("* [%d-1] Start QSPI Config Memory [0]: Initialize\n", test_no);
	if (!qspi_norflash_init(SCOBCA1_FPGA_CFG_BASE_ADDR, QSPI_DATA_MEM0)) {
		err_cnt++;
	}

	info("* [%d-2] Start QSPI Config Memory [1]: Initialize\n", test_no);
	if (!qspi_norflash_init(SCOBCA1_FPGA_CFG_BASE_ADDR, QSPI_DATA_MEM1)) {
		err_cnt++;
	}

	info("* [%d-3] Start QSPI Data Memory [0]: Initialize\n", test_no);
	if (!qspi_norflash_init(SCOBCA1_FPGA_DATA_BASE_ADDR, QSPI_DATA_MEM0)) {
		err_cnt++;
	}

	info("* [%d-4] Start QSPI Data Memory [1]: Initialize\n", test_no);
	if (!qspi_norflash_init(SCOBCA1_FPGA_DATA_BASE_ADDR, QSPI_DATA_MEM1)) {
		err_cnt++;
	}

	return err_cnt;
}

/*
 *   1. Erase Memory 0 (Sector)
 *   2. Erase Memory 1 (Sector)
 *   3. Read Memory 0  (all 0xFF)
 *   4. Read Memory 1  (all 0xFF)
 *   5. Write Memory 0 (only 16 byte)
 *   6. Write Memory 1 (only 16 byte)
 *   7. Read Memory 0
 *   8. Read Memory 1
 */
static uint32_t qspi_norflash_test(uint32_t test_no, uint32_t base)
{
	uint32_t err_cnt = 0;
	uint32_t exp_init_data[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t write_data_0[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t write_data_1[QSPI_RX_FIFO_MAX_BYTE];
	uint32_t mem_addr_0 = 0x00800000;
	uint32_t mem_addr_1 = 0x00900000;
	uint8_t start_val_0 = 0x00;
	uint8_t start_val_1 = 0x10;
	bool is_wait_idle = true;

	/* Create test data */
	for (uint8_t i=0; i<QSPI_RX_FIFO_MAX_BYTE; i++) {
		exp_init_data[i] = 0xFF;
	}
	start_val_0 = qspi_create_fifo_data(start_val_0, write_data_0, QSPI_RX_FIFO_MAX_BYTE, false);
	start_val_1 = qspi_create_fifo_data(start_val_1, write_data_1, QSPI_RX_FIFO_MAX_BYTE, false);

	info("* [%d-1] Start QSPI Memory [0]: Erase Test (Sector)\n", test_no);
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM0, QSPI_ERASE_SECTOR,
							mem_addr_0, is_wait_idle)) {
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-2] Start QSPI Memory [1]: Erase Test (Sector)\n", test_no);
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM1, QSPI_ERASE_SECTOR,
							mem_addr_1, is_wait_idle)) {
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-3] Start QSPI Memory [0]: Read initial data Test\n", test_no);
	if (!qspi_norflash_read(base, QSPI_DATA_MEM0, mem_addr_0,
							QSPI_RX_FIFO_MAX_BYTE, exp_init_data)) {
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-4] Start QSPI Memory [1]: Read initial data Test\n", test_no);
	if (!qspi_norflash_read(base, QSPI_DATA_MEM1, mem_addr_1,
							QSPI_RX_FIFO_MAX_BYTE, exp_init_data)) {
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-5] Start QSPI Memory [0]: Write data Test \n", test_no);
	if (!qspi_norflash_write(base, QSPI_DATA_MEM0, mem_addr_0,
							QSPI_RX_FIFO_MAX_BYTE, write_data_0)) {
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-6] Start QSPI Memory [1]: Write data Test \n", test_no);
	if (!qspi_norflash_write(base, QSPI_DATA_MEM1, mem_addr_1,
							QSPI_RX_FIFO_MAX_BYTE, write_data_1)) {
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-7] Start QSPI Memory [0]: Read data Test \n", test_no);
	if (!qspi_norflash_read(base, QSPI_DATA_MEM0, mem_addr_0,
							QSPI_RX_FIFO_MAX_BYTE, write_data_0)) {
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-8] Start QSPI Memory [1]: Read data Test \n", test_no);
	if (!qspi_norflash_read(base, QSPI_DATA_MEM1, mem_addr_1,
							QSPI_RX_FIFO_MAX_BYTE, write_data_1)) {
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	return err_cnt;
}

/*
 *   1. Erase Memory 0 (Sector)
 *   2. Erase Memory 1 (Sector)
 *   3. Read Memory 0  (all 0xFF)
 *   4. Read Memory 1  (all 0xFF)
 *   5. Write Memory 0 (Sector:4KB)
 *   6. Write Memory 1 (Sector:4KB)
 *   7. Read Memory 0
 *   8. Read Memory 1
 */
static uint32_t qspi_norflash_sector_test(uint32_t test_no, uint32_t base)
{
	uint32_t err_cnt = 0;
	uint32_t mem_addr_0 = 0x00800000;
	uint32_t mem_addr_1 = 0x00900000;
	uint8_t start_val_0 = 0x00;
	uint8_t start_val_1 = 0x10;
	bool is_wait_idle = true;

	info("* [%d-1] Start QSPI Memory [0]: Erase Test (Sector)\n", test_no);
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM0, QSPI_ERASE_SECTOR, mem_addr_0, is_wait_idle)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-2] Start QSPI Memory [1]: Erase Test (Sector)\n", test_no);
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM1, QSPI_ERASE_SECTOR, mem_addr_1, is_wait_idle)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-3] Start QSPI Memory [0]: Read initial data Test (Sector:4KB)\n", test_no);
	is_wait_idle = true;
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM0, mem_addr_0,
								QSPI_NOR_FLASH_SECTOR_BYTE, start_val_0, true)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-4] Start QSPI Memory [1]: Read initial data Test (Sector:4KB)\n", test_no);
	is_wait_idle = true;
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM1, mem_addr_1,
								QSPI_NOR_FLASH_SECTOR_BYTE, start_val_1, true)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-5] Start QSPI Memory [0]: Write data Test (Sector:4KB)\n", test_no);
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM0, mem_addr_0,
									QSPI_NOR_FLASH_SECTOR_BYTE, start_val_0)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-6] Start QSPI Memory [1]: Write data Test (Sector:4KB)\n", test_no);
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM1, mem_addr_1,
									QSPI_NOR_FLASH_SECTOR_BYTE, start_val_1)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-7] Start QSPI Memory [0]: Read data Test (Sector:4KB)\n", test_no);
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM0, mem_addr_0,
								QSPI_NOR_FLASH_SECTOR_BYTE, start_val_0, false)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-8] Start QSPI Memory [1]: Read data Test (Sector:4KB)\n", test_no);
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM1, mem_addr_1,
								QSPI_NOR_FLASH_SECTOR_BYTE, start_val_1, false)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	return err_cnt;
}

/*
 *   1. Erase Memory 0 (Sector)
 *   2. Erase Memory 1 (Sector)
 *   3. Read Memory 0  (all 0xFF)
 *   4. Read Memory 1  (all 0xFF)
 *   5. Write Memory 0 (Block:64KB)
 *   6. Write Memory 1 (Block:64KB)
 *   7. Read Memory 0
 *   8. Read Memory 1
 */
static uint32_t qspi_norflash_block_test(uint32_t test_no, uint32_t base)
{
	uint32_t err_cnt = 0;
	uint32_t mem_addr_0 = 0x00A00000;
	uint32_t mem_addr_1 = 0x00B00000;
	uint8_t start_val_0 = 0x60;
	uint8_t start_val_1 = 0x70;
	bool is_wait_idle = true;

	info("* [%d-1] Start QSPI Memory [0]: Erase Test (Block)\n", test_no);
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM0, QSPI_ERASE_BLOCK, mem_addr_0, is_wait_idle)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-2] Start QSPI Memory [1]: Erase Test (Block)\n", test_no);
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM1, QSPI_ERASE_BLOCK, mem_addr_1, is_wait_idle)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-3] Start QSPI Memory [0]: Read initial data Test (Block:64KB)\n", test_no);
	is_wait_idle = true;
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM0, mem_addr_0,
								QSPI_NOR_FLASH_BLOCK_BYTE, start_val_0, true)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-4] Start QSPI Memory [1]: Read initial data Test (Block:64KB)\n", test_no);
	is_wait_idle = true;
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM1, mem_addr_1,
								QSPI_NOR_FLASH_BLOCK_BYTE, start_val_1, true)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-5] Start QSPI Memory [0]: Write data Test (Block:64KB)\n", test_no);
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM0, mem_addr_0,
								QSPI_NOR_FLASH_BLOCK_BYTE, start_val_0)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-6] Start QSPI Memory [1]: Write data Test (Block:64KB)\n", test_no);
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM1, mem_addr_1,
								QSPI_NOR_FLASH_BLOCK_BYTE, start_val_1)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	info("* [%d-7] Start QSPI Memory [0]: Read data Test (Block:64KB)\n", test_no);
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM0, mem_addr_0,
								QSPI_NOR_FLASH_BLOCK_BYTE, start_val_0, false)) {
		assert();
		err_cnt++;
	}

	info("* [%d-8] Start QSPI Memory [1]: Read data Test (Block:64KB)\n", test_no);
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM1, mem_addr_1,
								QSPI_NOR_FLASH_BLOCK_BYTE, start_val_1, false)) {
		assert();
		err_cnt++;
	}

end_of_test:
	return err_cnt;
}

uint32_t qspi_config_memory_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_CFG_BASE_ADDR;

	info("* [%d] Start QSPI Config Memory Test (only 16byte)\n", test_no);

	err_cnt = qspi_norflash_test(test_no, base);

	print_result(test_no, err_cnt);

	return err_cnt;
}

uint32_t qspi_data_memory_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_DATA_BASE_ADDR;

	info("* [%d] Start QSPI Data Memory Test (only 16byte)\n", test_no);

	err_cnt = qspi_norflash_test(test_no, base);

	print_result(test_no, err_cnt);

	return err_cnt;
}

uint32_t qspi_config_memory_sector_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_CFG_BASE_ADDR;

	info("* [%d] Start QSPI Config Memory Test (Sector)\n", test_no);

	err_cnt = qspi_norflash_sector_test(test_no, base);

	print_result(test_no, err_cnt);

	return err_cnt;
}

uint32_t qspi_data_memory_sector_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_DATA_BASE_ADDR;

	info("* [%d] Start QSPI Data Memory Test (Sector)\n", test_no);

	err_cnt = qspi_norflash_sector_test(test_no, base);

	print_result(test_no, err_cnt);

	return err_cnt;
}

uint32_t qspi_config_memory_block_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_CFG_BASE_ADDR;

	info("* [%d] Start QSPI Config Memory Test (Block)\n", test_no);

	err_cnt = qspi_norflash_block_test(test_no, base);

	print_result(test_no, err_cnt);

	return err_cnt;
}

uint32_t qspi_data_memory_block_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_DATA_BASE_ADDR;

	info("* [%d] Start QSPI Data Memory Test (Block)\n", test_no);

	err_cnt = qspi_norflash_block_test(test_no, base);

	print_result(test_no, err_cnt);

	return err_cnt;
}
