/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "can_test.h"
#include "common.h"

#define CAN_PKT_SIZE (8u)
#define CAN_TXID1_BIT_SHIFT  (21u)
#define CAN_TXSRTR_BIT_SHIFT (20u)
#define CAN_TXIDE_BIT_SHIFT  (19u)
#define CAN_TXID2_BIT_SHIFT  (1u)
#define CAN_TXRTR_BIT_SHIFT  (0u)
#define CAN_TXSRTR_DATA   (0u)
#define CAN_TXSRTR_REMOTE (1u)
#define CAN_TXIDE_NORMAL  (0u)
#define CAN_TXIDE_EXT     (1u)
#define CAN_TXERTR_DATA   (0u)
#define CAN_TXERTR_REMOTE (1u)

static uint32_t get_idr(uint16_t can_id, uint32_t can_ext_id)
{
	return (can_id << CAN_TXID1_BIT_SHIFT) +
			(CAN_TXSRTR_REMOTE << CAN_TXSRTR_BIT_SHIFT) +
			(CAN_TXIDE_EXT << CAN_TXIDE_BIT_SHIFT) +
			(can_ext_id << CAN_TXID2_BIT_SHIFT) +
			(CAN_TXERTR_DATA << CAN_TXRTR_BIT_SHIFT);
}

static void convert_can_data_to_word(uint8_t *can_data, uint8_t size, uint32_t *word1, uint32_t *word2)
{
	*word1 = 0;
	*word2 = 0;

	for (uint8_t i=0; i<size; i++) {
		if (i < 4) {
			*word1 += can_data[i] << (3*8-(i*8));
		} else {
			*word2 += can_data[i] << (3*8-(i-4)*8);
		}
	}
}

static bool can_loopback_test_init(void)
{
	debug("* Set Clear ISR\n");
	write32(SCOBCA1_FPGA_CAN_ISR, 0xFFFFFFFF);

	debug("* Set Baudrate to 1Mbps\n");
	write32(SCOBCA1_FPGA_CAN_TQPR, 0x0001);
	write32(SCOBCA1_FPGA_CAN_BTSR, 0x01A7);

	debug("* Activate Test mode\n");
	write32(SCOBCA1_FPGA_CAN_STMCR, 0x01);

	debug("* Enable CAN\n");
	write32(SCOBCA1_FPGA_CAN_ENR, 0x01);

	k_usleep(10);

	debug("* Verify CAN Status Registe (Error Active: 0x04)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_STSR, 0x04, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	debug("* Verify CAN Interrupt Registe (No interupt: 0x00)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x00, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	debug("* Clear All FIFO\n");
	write32(SCOBCA1_FPGA_CAN_FIFORR, 0xFFFFFFFF);

	return true;
}

static bool can_loopback_test_terminate(void)
{
	debug("* Disable CAN\n");
	write32(SCOBCA1_FPGA_CAN_ENR, 0x00);

	debug("* Disable Test mode\n");
	write32(SCOBCA1_FPGA_CAN_STMCR, 0x00);

	debug("* Verify CAN Interrupt Register (No interupt: 0x00)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x00, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	return true;
}

static bool can_send_test(uint16_t can_id, uint32_t can_ext_id, uint8_t *can_data, uint8_t size)
{
	uint32_t data_word1;
	uint32_t data_word2;

	debug("* Send CAN ID\n");
	write32(SCOBCA1_FPGA_CAN_TMR1, get_idr(can_id, can_ext_id));

	debug("* Send CAN Packet size\n");
	write32(SCOBCA1_FPGA_CAN_TMR2, size);

	debug("* Send CAN Data %d byte\n", size);
	convert_can_data_to_word(can_data, size, &data_word1, &data_word2);
	write32(SCOBCA1_FPGA_CAN_TMR3, data_word1);
	write32(SCOBCA1_FPGA_CAN_TMR4, data_word2);

	k_usleep(10);

	debug("* Verify CAN Interrupt Registe (CAN_TRNSDN/CAN_RCVDN/CAN_RXFVAL)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x31, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	debug("* Clear CAN Interrupt Register and Verify\n");
	write32(SCOBCA1_FPGA_CAN_ISR, 0x31);
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x00, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	return true;
}

static bool can_recv_test(uint16_t can_id, uint32_t can_ext_id, uint8_t *exp_can_data, uint8_t size)
{
	bool ret = true;

	uint32_t data_word1;
	uint32_t data_word2;

	debug("* Read CAN ID and Verify\n");
	if (!assert32(SCOBCA1_FPGA_CAN_RMR1, get_idr(can_id, can_ext_id), REG_READ_RETRY(1))) {
		assert();
		ret = false;
	}

	debug("* Read CAN Packet size and Verify\n");
	if (!assert32(SCOBCA1_FPGA_CAN_RMR2, size, REG_READ_RETRY(1))) {
		assert();
		ret = false;
	}

	debug("* ReadCAN Data and Verify\n");
	convert_can_data_to_word(exp_can_data, size, &data_word1, &data_word2);
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

uint32_t can_loopback(void)
{
	uint32_t err_cnt = 0;
	uint16_t can_id = 0x0123;
	uint32_t can_ext_id = 0x35678;
	uint8_t can_data[CAN_PKT_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1};

	debug("* [#1] Start CAN Test Initializing (for Loop back)\n");
	if (!can_loopback_test_init()) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	debug("* [#2] Start CAN Send Test\n");
	if (!can_send_test(can_id, can_ext_id, can_data, CAN_PKT_SIZE)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	debug("* [#3] Start CAN Recv Test\n");
	if (!can_recv_test(can_id, can_ext_id, can_data, CAN_PKT_SIZE)) {
		assert();
		err_cnt++;
		goto end_of_test;
	}

	debug("* [#4] Start CAN Test Terminating\n");
	if (!can_loopback_test_terminate()) {
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
