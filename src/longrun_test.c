/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "longrun_test.h"
#include "common.h"
#include "general_timer_reg.h"
#include "bhm_test.h"
#include "hrmem_test.h"
#include "can_test.h"
#include "qspi_common.h"
#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"

#define LONGRUN_STACK_SIZE (2048u)
#define THREAD_PRIORITY (7u)
#define FRAM_TEST_SIZE (KB(16))

K_THREAD_STACK_DEFINE(_longrun_thread_stack, LONGRUN_STACK_SIZE);
static struct k_thread _k_thread_data;
static uint32_t last_erase_time = 0;
static uint8_t cfg_write_val_0 = 0x00;
static uint8_t cfg_write_val_1 = 0x10;
static uint8_t data_write_val_0 = 0x20;
static uint8_t data_write_val_1 = 0x30;
static uint8_t fram_write_val_0 = 0x40;
static uint8_t fram_write_val_1 = 0x50;
static uint8_t cfg_read_val_0;
static uint8_t cfg_read_val_1;
static uint8_t data_read_val_0;
static uint8_t data_read_val_1;
static uint8_t fram_read_val_0;
static uint8_t fram_read_val_1;
static uint32_t cfg_mem_addr_0 = 0x00A00000;
static uint32_t cfg_mem_addr_1 = 0x00B00000;
static uint32_t data_mem_addr_0 = 0x00000000;
static uint32_t data_mem_addr_1 = 0x00100000;
static uint32_t fram_mem_addr_0 = 0x000000;
static int32_t fram_mem_addr_1 = 0x001000;

extern bool is_exit;

enum NorflashState
{
	NORFLASH_STATE_ERASED,
	NORFLASH_STATE_WROTE,
	NORFLASH_STATE_IDLE,
};

static enum NorflashState norflash_state = NORFLASH_STATE_IDLE;

static uint32_t get_obc_uptime(void)
{
	return (sys_read32(SCOBCA1_FPGA_GPTMR_GTR) & 0xFFFFF0) >> 4;
}

static bool check_norflash_erase_cycle(void)
{
	uint32_t current_time = get_obc_uptime();

	if (last_erase_time == 0) {
		last_erase_time = current_time;
		norflash_state = NORFLASH_STATE_ERASED;
		return true;
	} else if((current_time - last_erase_time) > 60) {
		last_erase_time = current_time;
		norflash_state = NORFLASH_STATE_ERASED;
		return true;
	}

	return false;
}

static bool check_norflash_write_cycle(void)
{
	uint32_t current_time = get_obc_uptime();

	if (norflash_state == NORFLASH_STATE_ERASED &&
			(current_time - last_erase_time) > 1) {
		norflash_state = NORFLASH_STATE_WROTE;
		return true;
	}

	return false;
}

static bool check_norflash_read_cycle(void)
{
	if (norflash_state == NORFLASH_STATE_WROTE) {
		return true;
	}

	return false;
}

static uint32_t config_memory_erase(void)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_CFG_BASE_ADDR;

	/* Block Erase on Config Memory 0 */
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM0, QSPI_ERASE_BLOCK,
							cfg_mem_addr_0, false)) {
		err_cnt++;
	}

	/* Block Erase on Config Memory 1 */
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM1, QSPI_ERASE_BLOCK,
							cfg_mem_addr_1, false)) {
		err_cnt++;
	}

	return err_cnt;
}

static uint32_t data_memory_erase(void)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_DATA_BASE_ADDR;

	/* Block Erase on Config Memory 0 */
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM0, QSPI_ERASE_BLOCK,
							data_mem_addr_0, false)) {
		err_cnt++;
	}

	/* Block Erase on Config Memory 1 */
	if (!qspi_norflash_erase(base, QSPI_DATA_MEM1, QSPI_ERASE_BLOCK,
							data_mem_addr_1, false)) {
		err_cnt++;
	}

	return err_cnt;
}

static uint32_t config_memory_write(void)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_CFG_BASE_ADDR;

	/* Block Write to Config Memory 0 */
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM0, cfg_mem_addr_0,
									QSPI_NOR_FLASH_BLOCK_BYTE, cfg_write_val_0)) {
		err_cnt++;
	}
	cfg_read_val_0 = cfg_write_val_0;
	cfg_write_val_0++;

	/* Block Write to Config Memory 1 */
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM1, cfg_mem_addr_1,
									QSPI_NOR_FLASH_BLOCK_BYTE, cfg_write_val_1)) {
		err_cnt++;
	}
	cfg_read_val_1 = cfg_write_val_1;
	cfg_write_val_1++;

	return err_cnt;
}

static uint32_t data_memory_write(void)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_DATA_BASE_ADDR;

	/* Block Write to Data Memory 0 */
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM0, data_mem_addr_0,
									QSPI_NOR_FLASH_BLOCK_BYTE, data_write_val_0)) {
		err_cnt++;
	}
	data_read_val_0 = data_write_val_0;
	data_write_val_0++;

	/* Block Write to Data Memory 1 */
	if (!qspi_norflash_multi_write(base, QSPI_DATA_MEM1, data_mem_addr_1,
									QSPI_NOR_FLASH_BLOCK_BYTE, data_write_val_1)) {
		err_cnt++;
	}
	data_read_val_1 = data_write_val_1;
	data_write_val_1++;

	return err_cnt;
}

static uint32_t config_memory_read(void)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_CFG_BASE_ADDR;

	/* Read Block data (64KB) from Config Memory 0 */
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM0, cfg_mem_addr_0,
								QSPI_NOR_FLASH_BLOCK_BYTE, cfg_read_val_0, false)) {
		err_cnt++;
	}

	/* Read Block data (64KB) from Config Memory 1 */
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM1, cfg_mem_addr_1,
								QSPI_NOR_FLASH_BLOCK_BYTE, cfg_read_val_1, false)) {
		err_cnt++;
	}

	return err_cnt;
}

static uint32_t data_memory_read(void)
{
	uint32_t err_cnt = 0;
	uint32_t base = SCOBCA1_FPGA_DATA_BASE_ADDR;

	/* Read Block data (64KB) from Data Memory 0 */
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM0, data_mem_addr_0,
								QSPI_NOR_FLASH_BLOCK_BYTE, data_read_val_0, false)) {
		err_cnt++;
	}

	/* Read Block data (64KB) from Data Memory 1 */
	if (!qspi_norflash_multi_read(base, QSPI_DATA_MEM1, data_mem_addr_1,
								QSPI_NOR_FLASH_BLOCK_BYTE, data_read_val_1, false)) {
		err_cnt++;
	}

	return err_cnt;
}

static uint32_t fram_write(void)
{
	uint32_t err_cnt = 0;

	/* Write data (64KB) from Data Memory 0 */
	if (!qspi_fram_multi_write(QSPI_DATA_MEM0, fram_mem_addr_0, FRAM_TEST_SIZE, fram_write_val_0)) {
		err_cnt++;
	}
	fram_read_val_0 = fram_write_val_0;
	fram_write_val_0++;

	/* Write data (64KB) from Data Memory 1 */
	if (!qspi_fram_multi_write(QSPI_DATA_MEM1, fram_mem_addr_1, FRAM_TEST_SIZE, fram_write_val_1)) {
		err_cnt++;
	}
	fram_read_val_1 = fram_write_val_1;
	fram_write_val_1++;

	return err_cnt;
}

static uint32_t fram_read(void)
{
	uint32_t err_cnt = 0;

	/* Read data (64KB) from Data Memory 0 */
	if (!qspi_fram_multi_read(QSPI_DATA_MEM0, fram_mem_addr_0, FRAM_TEST_SIZE, fram_read_val_0)) {
		err_cnt++;
	}

	/* Read data (64KB) from Data Memory 1 */
	if (!qspi_fram_multi_read(QSPI_DATA_MEM1, fram_mem_addr_1, FRAM_TEST_SIZE, fram_read_val_1)) {
		err_cnt++;
	}

	return err_cnt;
}

static void start_longrun_test(void *p1, void *p2, void *p3)
{
	bool ret;
	uint32_t err_cnt = 0;
	uint16_t loop_count = 0;
	uint32_t hrmem_start_val = 0x00;
	uint32_t hrmem_next_val;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* Enable Board Health Monitoring */
	for (uint8_t i=0; i<5; i++) {
		info("* [#] Enable Board Health Monitoring\n");
		ret = bhm_enable();
		if (ret) {
			break;
		}
	}

	if (!ret) {
		err("* Abort. Can not enable Board Health Moniting\n");
		return;
	}

	while (true) {
		loop_count++;

		if (check_norflash_erase_cycle()) {
			/* Block Erase NOR flash */
			info("* [#] Erase Config Memory\n");
			err_cnt += config_memory_erase();
			info("* [#] Erase Data Memory\n");
			err_cnt += data_memory_erase();
			norflash_state = NORFLASH_STATE_ERASED;
		}

		/* Dump Board Halth Monitoring */
		info("* [#] Dump Board Halth Monitoring\n");
		err_cnt += bhm_read_sensor_data();

		/* FRAM Write Read Test */
		info("* [#] Start Write/Read FRAM Test\n");
		err_cnt += fram_write();
		err_cnt += fram_read();

		/* HRMEM Write/Read (1Mbyte) */
		info("* [#] Start HRMEM Test\n");
		err_cnt += hrmem_rw(MB(1), hrmem_start_val, &hrmem_next_val);
		hrmem_start_val = hrmem_next_val;

		/* CAN Loop back Test */
		info("* [#] Start CAN Loop back Test\n");
		err_cnt += can_loopback();

		if (check_norflash_write_cycle()) {
			/* Write Config Memory Test */
			info("* [#] Start Write Config Memory Test\n");
			err_cnt += config_memory_write();

			/* Write Data Memory Test */
			info("* [#] Start Write Data Memory Test\n");
			err_cnt += data_memory_write();
		}

		if (check_norflash_read_cycle()) {
			/* Read Config Memory Test */
			info("* [#] Start Read Config Memory Test\n");
			err_cnt += config_memory_read();

			/* Read Data Memory Test */
			info("* [#] Start Read Data Memory Test\n");
			err_cnt += data_memory_read();
		}

		info("* Loop [%d][%d] Total assertion: %d\n", loop_count, get_obc_uptime(), err_cnt);

		if (is_exit) {
			printk("* Stop Long Run Test\n");
			is_exit = false;
			break;
		}
	}
}

uint32_t longrun_test(uint32_t test_no)
{
	k_tid_t tid;

	tid = k_thread_create(&_k_thread_data, _longrun_thread_stack, LONGRUN_STACK_SIZE,
					start_longrun_test, NULL, NULL, NULL,
					THREAD_PRIORITY, 0, K_NO_WAIT);
	printk("* [%d] Start Long Run Test [tid:%d]\n", test_no, (unsigned int)tid);

	return 0;
}
