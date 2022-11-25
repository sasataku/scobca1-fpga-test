/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "can.h"

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

#define CAN_PKT_SIZE (8u)
#define CAN_TXID1_BIT_MASK (0xFFE00000)
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

bool can_tx_done = false;
bool can_rx_done = false;
bool first_can_err_isr = false;

uint32_t can_get_idr(uint16_t can_id, uint32_t can_ext_id, bool extend)
{
	if (extend) {
		return (can_id << CAN_TXID1_BIT_SHIFT) +
				(CAN_TXSRTR_REMOTE << CAN_TXSRTR_BIT_SHIFT) +
				(CAN_TXIDE_EXT << CAN_TXIDE_BIT_SHIFT) +
				(can_ext_id << CAN_TXID2_BIT_SHIFT) +
				(CAN_TXERTR_DATA << CAN_TXRTR_BIT_SHIFT);
	} else {
		return (can_id << CAN_TXID1_BIT_SHIFT) +
				(CAN_TXSRTR_DATA << CAN_TXSRTR_BIT_SHIFT) +
				(CAN_TXIDE_NORMAL << CAN_TXIDE_BIT_SHIFT) +
				(can_ext_id << CAN_TXID2_BIT_SHIFT) +
				(CAN_TXERTR_DATA << CAN_TXRTR_BIT_SHIFT);
	}
}

void can_convert_can_data_to_word(uint8_t *can_data, uint8_t size, uint32_t *word1, uint32_t *word2)
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

bool is_can_tx_done(void)
{
	for (uint8_t i=0; i<10; i++) {
		if (can_tx_done) {
			can_tx_done = false;
			return true;
		}
		k_usleep(10);
	}

	return false;
}

bool is_can_rx_done(int32_t timeout_us)
{
	for (uint8_t i=0; i<10; i++) {
		if (can_rx_done) {
			can_rx_done = false;
			return true;
		}
		k_usleep(timeout_us);
	}

	return false;
}

bool can_init(bool test_mode)
{
	debug("* Set Clear ISR\n");
	write32(SCOBCA1_FPGA_CAN_ISR, 0xFFFFFFFF);

	debug("* Set Baudrate to 1Mbps\n");
	write32(SCOBCA1_FPGA_CAN_TQPR, 0x0001);
	write32(SCOBCA1_FPGA_CAN_BTSR, 0x01A7);

	if (test_mode) {
		debug("* Activate Test mode\n");
		write32(SCOBCA1_FPGA_CAN_STMCR, 0x01);
	}

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

bool can_terminate(bool test_mode)
{
	debug("* Disable CAN\n");
	write32(SCOBCA1_FPGA_CAN_ENR, 0x00);

	if (test_mode) {
		debug("* Disable Test mode\n");
		write32(SCOBCA1_FPGA_CAN_STMCR, 0x00);
	}

	debug("* Verify CAN Interrupt Register (No interupt: 0x00)\n");
	if (!assert32(SCOBCA1_FPGA_CAN_ISR, 0x00, REG_READ_RETRY(10))) {
		assert();
		return false;
	}

	return true;
}

bool can_send_full(uint16_t can_id, uint32_t can_ext_id, uint8_t *can_data, uint8_t size, bool extend)
{
	uint32_t data_word1;
	uint32_t data_word2;

	debug("* Send CAN ID\n");
	first_can_err_isr = false;
	write32(SCOBCA1_FPGA_CAN_TMR1, can_get_idr(can_id, can_ext_id, extend));

	debug("* Send CAN Packet size\n");
	write32(SCOBCA1_FPGA_CAN_TMR2, size);

	debug("* Send CAN Data %d byte\n", size);
	can_convert_can_data_to_word(can_data, size, &data_word1, &data_word2);
	write32(SCOBCA1_FPGA_CAN_TMR3, data_word1);
	write32(SCOBCA1_FPGA_CAN_TMR4, data_word2);

	if (!is_can_tx_done()) {
		err("  !!! Assertion failed: CAN TX DONE timed out\n");
		return false;
	}

	return true;
}
