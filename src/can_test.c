/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/console/console.h>
#include <stdlib.h>
#include "can_test.h"
#include "common.h"
#include "can.h"

static bool can_recv_test(uint16_t can_id, uint32_t can_ext_id, uint8_t *exp_can_data,
							uint8_t size, bool extend, uint32_t timeout_us)
{
	bool ret = true;

	uint32_t data_word1;
	uint32_t data_word2;

	if (!is_can_rx_done(timeout_us)) {
		err("  !!! Assertion failed: CAN RX DONE timed out\n");
		return false;
	}

	debug("* Read CAN ID and Verify\n");
	if (!assert32(SCOBCA1_FPGA_CAN_RMR1, can_get_idr(can_id, can_ext_id, extend), REG_READ_RETRY(1))) {
		assert();
		ret = false;
	}

	debug("* Read CAN Packet size and Verify\n");
	if (!assert32(SCOBCA1_FPGA_CAN_RMR2, size, REG_READ_RETRY(1))) {
		assert();
		ret = false;
	}

	debug("* ReadCAN Data and Verify\n");
	can_convert_can_data_to_word(exp_can_data, size, &data_word1, &data_word2);
	if (!assert32(SCOBCA1_FPGA_CAN_RMR3, data_word1, REG_READ_RETRY(1))) {
		assert();
		ret = false;
	}
	if (!assert32(SCOBCA1_FPGA_CAN_RMR4, data_word2, REG_READ_RETRY(1))) {
		assert();
		ret = false;
	}

	return ret;
}

static bool can_send_cmd_to_trch(uint8_t cmd_code)
{
	uint16_t can_id = 0x46;
	uint32_t can_ext_id = 0x00;
	uint8_t can_data[2];
	bool extend = false;
	int32_t timeout_us = 1000000;
	uint32_t recv_id;
	uint32_t recv_size;
	uint32_t recv_word1, recv_word2;

	debug("* [#1] Start CAN Test Initializing\n");
	if (!can_init(false)) {
		assert();
	}

	debug("* [#2] Start CAN command Sending\n");
	can_data[0] = 0x5A;
	can_data[1] = cmd_code;
	first_can_err_isr = false;
	info("* Send CAN Data (ID: 0x%02x) Data: 0x%02x 0x%02x\n",
			can_id, can_data[0], can_data[1]);
	if (!can_send_full(can_id, can_ext_id, can_data, ARRAY_SIZE(can_data), extend)) {
		assert();
		return false;
	}

	if (!is_can_rx_done(timeout_us)) {
		err("  !!! Assertion failed: CAN RX DONE timed out\n");
		return false;
	}

	debug("* Read CAN ID\n");
	recv_id = sys_read32(SCOBCA1_FPGA_CAN_RMR1);
	info("* Received CAN ID: 0x%x\n", (recv_id & CAN_TXID1_BIT_MASK) >> CAN_TXID1_BIT_SHIFT);

	debug("* Read CAN Packet size\n");
	recv_size = sys_read32(SCOBCA1_FPGA_CAN_RMR2);
	info("* Recevied Data Size: %d byte\n", recv_size);

	debug("* Read CAN Packet data\n");
	recv_word1 = sys_read32(SCOBCA1_FPGA_CAN_RMR3);
	recv_word2 = sys_read32(SCOBCA1_FPGA_CAN_RMR4);
	if (recv_size > 8) {
		err("  !!! Assertion failed: Invalid CAN Packet size\n");
		return false;
	}

	info("* Received Data:");
	for (uint8_t i=0; i<recv_size; i++) {
		if (i<4) {
			info(" 0x%02x", (recv_word1 >> 8*(3-i) & 0xFF));
		} else {
			info(" 0x%02x", (recv_word2 >> 8*(3-(i-4)) & 0xFF));
		}
	}
	info("\n");

	debug("* [#] CAN Terminating\n");
	if (!can_terminate(false)) {
		return false;
	}

	return true;
}

static bool can_crack_loopback_test(void)
{
	uint16_t can_id = 0x01;
	uint32_t can_ext_id = 0x00;
	uint8_t can_data[CAN_PKT_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
	bool extend = false;
	int32_t timeout_us = 1000000;
	uint16_t recv_can_id = 0x02;
	uint32_t recv_can_ext_id = 0x00;

	debug("* [#1] Start CAN Test Initializing\n");
	if (!can_init(false)) {
		assert();
	}

	debug("* [#2] Start CAN command Sending\n");
	first_can_err_isr = false;
	if (!can_send_full(can_id, can_ext_id, can_data, CAN_PKT_SIZE, extend)) {
		assert();
		return false;
	}

	debug("* [#3] Start CAN Recv Test\n");
	if (!can_recv_test(recv_can_id, recv_can_ext_id, can_data, CAN_PKT_SIZE, extend, timeout_us)) {
		assert();
		return true;
	}

	debug("* [#4] Start CAN Test Terminating\n");
	if (!can_terminate(false)) {
		assert();
		return true;
	}

	return true;
}

static bool can_crack_sleep_en_test(void)
{
	uint16_t can_id = 0x01;
	uint32_t can_ext_id = 0x00;
	uint8_t can_data[CAN_PKT_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
	bool extend = false;
	uint32_t data_word1;
	uint32_t data_word2;

	debug("* [#1] Enable CAN PHY Sleep Mode\n");
	write32(SCOBCA1_FPGA_CAN_PSLMCR, 0x01);

	debug("* [#2] Start CAN Test Initializing\n");
	if (!can_init(false)) {
		assert();
		return false;
	}

	debug("* [#3] Send CAN ID\n");
	first_can_err_isr = false;
	write32(SCOBCA1_FPGA_CAN_TMR1, can_get_idr(can_id, can_ext_id, extend));

	debug("* [#4] Send CAN Packet size\n");
	write32(SCOBCA1_FPGA_CAN_TMR2, CAN_PKT_SIZE);

	debug("* Send CAN Data %d byte\n", CAN_PKT_SIZE);
	can_convert_can_data_to_word(can_data, CAN_PKT_SIZE, &data_word1, &data_word2);
	write32(SCOBCA1_FPGA_CAN_TMR3, data_word1);
	write32(SCOBCA1_FPGA_CAN_TMR4, data_word2);

	if (is_can_tx_done()) {
		err("  !!! Assertion failed: even if CAN PHY Sleep, but CAN TX Done,\n");
		return false;
	}
	info("*** CAN TX Done timeout, but it's expected behavior\n");

	debug("* [#4] Start CAN Test Terminating)\n");
	if (!can_terminate(false)) {
		assert();
		return false;
	}

	return true;
}

uint32_t can_loopback(void)
{
	uint32_t err_cnt = 0;
	uint16_t can_id = 0x0123;
	uint32_t can_ext_id = 0x35678;
	uint8_t can_data[CAN_PKT_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1};
	uint32_t timeout_us = 10;
	bool extend = true;

	debug("* [#1] Start CAN Test Initializing (for Test Mode)\n");
	if (!can_init(true)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	debug("* [#2] Start CAN Send Test\n");
	if (!can_send_full(can_id, can_ext_id, can_data, CAN_PKT_SIZE, extend)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	debug("* [#3] Start CAN Recv Test\n");
	if (!can_recv_test(can_id, can_ext_id, can_data, CAN_PKT_SIZE, extend, timeout_us)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	debug("* [#4] Start CAN Test Terminating (for Test Mode)\n");
	if (!can_terminate(true)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

end_of_test:
	return err_cnt;
}

uint32_t can_test(uint32_t test_no)
{
	uint32_t ret;
	uint32_t err_cnt = 0;

	info("* [%d] Start CAN Test\n", test_no);

	info("* [%d-1] Start CAN Loop back Test\n", test_no);
	ret = can_loopback();
	err_cnt += ret;

	print_result(test_no, err_cnt);

	return err_cnt;
}

uint32_t can_send_cmd(uint32_t test_no)
{
	uint32_t err_cnt = 0;
	uint8_t cmd_code;
	char *s;

	info("Please input command code (0-255)\n");
	info("> ");

	s = console_getline();
	cmd_code = strtol(s, NULL, 10);

	if (!can_send_cmd_to_trch(cmd_code)) {
		err_cnt++;
	}

	print_result(test_no, err_cnt);
	return err_cnt;
}

uint32_t can_crack_test(uint32_t test_no)
{
	uint32_t err_num = 0;

	info("*** System Clock crack test starts ***\n");

	info("*** [#1] Start CAN Loop back Test\n");
	if (!can_crack_loopback_test()) {
		err_num++;
	}

	info("*** [#2] Start CAN SLEEP Enable Test\n");
	if (!can_crack_sleep_en_test()) {
		err_num++;
	}

	info("*** test done, error count: %d ***\n", err_num);

	return err_num;
}
