/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "can_test.h"
#include "common.h"

#define CAN_READ_RETRY(count) (count)
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
	printk("**** [1] Set Baudrate to 1Mbps\n");
	write32(SCOBCA1_FPGA_CAN_TQPR, 0x0001);
	write32(SCOBCA1_FPGA_CAN_BTSR, 0x01A7);

	printk("**** [2] Activate Test mode\n");
	write32(SCOBCA1_FPGA_CAN_STMCR, 0x01);

	printk("**** [3] Enable CAN\n");
	write32(SCOBCA1_FPGA_CAN_ENR, 0x01);

	printk("*** [4] Verify CAN Status Registe (Error Active: 0x04)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_STSR, 0x04, CAN_READ_RETRY(10))) {
		return false;
	}

	printk("*** [5] Verify CAN Interrupt Registe (No interupt: 0x00)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x00, CAN_READ_RETRY(10))) {
		return false;
	}

	printk("*** [6] Clear All FIFO\n");
	write32(SCOBCA1_FPGA_CAN_FIFORR, 0xFFFFFFFF);

	return true;
}

static bool can_loopback_test_finish(void)
{
	printk("**** [1] Disable CAN\n");
	write32(SCOBCA1_FPGA_CAN_ENR, 0x00);

	printk("**** [2] Disable Test mode\n");
	write32(SCOBCA1_FPGA_CAN_STMCR, 0x00);

	printk("**** [3] Verify CAN Interrupt Register (No interupt: 0x00)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x00, CAN_READ_RETRY(10))) {
		return false;
	}

	return true;
}

static bool can_send_test(uint16_t can_id, uint32_t can_ext_id, uint8_t *can_data, uint8_t size)
{
	uint32_t data_word1;
	uint32_t data_word2;

	printk("**** [1] Send CAN ID\n");
	write32(SCOBCA1_FPGA_CAN_TMR1, get_idr(can_id, can_ext_id));

	printk("**** [2] Send CAN Packet size\n");
	write32(SCOBCA1_FPGA_CAN_TMR2, size);

	printk("**** [3] Send CAN Data %d byte\n", size);
	convert_can_data_to_word(can_data, size, &data_word1, &data_word2);
	write32(SCOBCA1_FPGA_CAN_TMR3, data_word1);
	write32(SCOBCA1_FPGA_CAN_TMR4, data_word2);

	printk("*** [4] Verify CAN Interrupt Registe (CAN_TRNSDN/CAN_RCVDN/CAN_RXFVAL)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x31, CAN_READ_RETRY(10))) {
		return false;
	}

	printk("*** [5] Clear CAN Interrupt Register and Verify\n");
	write32(SCOBCA1_FPGA_CAN_ISR, 0x31);
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x00, CAN_READ_RETRY(10))) {
		return false;
	}

	return true;
}

static bool can_recv_test(uint16_t can_id, uint32_t can_ext_id, uint8_t *exp_can_data, uint8_t size)
{
	bool ret = true;

	uint32_t data_word1;
	uint32_t data_word2;

	printk("**** [1] Read CAN ID and Verify\n");
	if (!assert32(SCOBCA1_FPGA_CAN_RMR1, get_idr(can_id, can_ext_id), CAN_READ_RETRY(1))) {
		ret = false;
	}

	printk("**** [2] Read CAN Packet size and Verify\n");
	if (!assert32(SCOBCA1_FPGA_CAN_RMR2, size, CAN_READ_RETRY(1))) {
		ret = false;
	}

	printk("**** [3] ReadCAN Data and Verify\n");
	convert_can_data_to_word(exp_can_data, size, &data_word1, &data_word2);
	if (!assert32(SCOBCA1_FPGA_CAN_RMR3, data_word1, CAN_READ_RETRY(1))) {
		ret = false;
	}
	if (!assert32(SCOBCA1_FPGA_CAN_RMR4, data_word2, CAN_READ_RETRY(1))) {
		ret = false;
	}

	return ret;
}

static uint32_t can_loop_back_test()
{
	uint32_t err_cnt = 0;
	uint16_t can_id = 0x0123;
	uint32_t can_ext_id = 0x35678;
	uint8_t can_data[CAN_PKT_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1};

	printk("*** [1] Start CAN Test Init (for Loop back)\n");
	if (!can_loopback_test_init()) {
		err_cnt++;
		goto end_of_test;
	}

	printk("*** [2] Start CAN Send Test\n");
	if (!can_send_test(can_id, can_ext_id, can_data, CAN_PKT_SIZE)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("*** [3] Start CAN Recv Test\n");
	if (!can_recv_test(can_id, can_ext_id, can_data, CAN_PKT_SIZE)) {
		err_cnt++;
		goto end_of_test;
	}

	printk("*** [4] Start CAN Test Finish\n");
	if (!can_loopback_test_finish()) {
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

	printk("* Start CAN Test\n");

	printk("** Start CAN Loop back Test\n");
	ret = can_loop_back_test();
	err_cnt += ret;

	if (err_cnt == 0) {
		printk("* Test Result: Passed\n");
	} else {
		printk("* Test Result: Failed (Assertion count: %d)\n", err_cnt);
	}

	return err_cnt;
}
