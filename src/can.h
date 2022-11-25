/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/* Base address */
#define SCOBCA1_FPGA_CAN_BASE_ADDR  (0x40400000)

/* Offset */
#define CAN_ENR_OFFSET    (0x0000) /* CAN Enable Register */
#define CAN_TQPR_OFFSET   (0x0008) /* CAN Time Quantum Prescaler Register */
#define CAN_BTSR_OFFSET   (0x000C) /* CAN Bit Timing Setting Register */
#define CAN_ECNTR_OFFSET  (0x0010) /* CAN Error Count Register */
#define CAN_STSR_OFFSET   (0x0018) /* CAN Status Register */
#define CAN_ISR_OFFSET    (0x0020) /* CAN Interrupt Status Register */
#define CAN_IER_OFFSET    (0x0024) /* CAN Interrupt Enable Register */
#define CAN_TMR1_OFFSET   (0x0030) /* CAN TX Message Register1 */
#define CAN_TMR2_OFFSET   (0x0034) /* CAN TX Message Register2 */
#define CAN_TMR3_OFFSET   (0x0038) /* CAN TX Message Register3 */
#define CAN_TMR4_OFFSET   (0x003C) /* CAN TX Message Register4 */
#define CAN_THPMR1_OFFSET (0x0040) /* CAN TX High Priority Message Register1 */
#define CAN_THPMR2_OFFSET (0x0044) /* CAN TX High Priority Message Register2 */
#define CAN_THPMR3_OFFSET (0x0048) /* CAN TX High Priority Message Register3 */
#define CAN_THPMR4_OFFSET (0x004C) /* CAN TX High Priority Message Register4 */
#define CAN_RMR1_OFFSET   (0x0050) /* CAN RX Message Register1 */
#define CAN_RMR2_OFFSET   (0x0054) /* CAN RX Message Register2 */
#define CAN_RMR3_OFFSET   (0x0058) /* CAN RX Message Register3 */
#define CAN_RMR4_OFFSET   (0x005C) /* CAN RX Message Register4 */
#define CAN_AFER_OFFSET   (0x0060) /* CAN Acceptance Filter Enable Register */
#define CAN_AFIMR1_OFFSET (0x0070) /* CAN Acceptance Filter ID Mask Register1 */
#define CAN_AFIVR1_OFFSET (0x0074) /* CAN Acceptance Filter ID Value Register1 */
#define CAN_AFIMR2_OFFSET (0x0090) /* CAN Acceptance Filter ID Mask Register2 */
#define CAN_AFIVR2_OFFSET (0x0094) /* CAN Acceptance Filter ID Value Register2 */
#define CAN_AFIMR3_OFFSET (0x00B0) /* CAN Acceptance Filter ID Mask Register3 */
#define CAN_AFIVR3_OFFSET (0x00B4) /* CAN Acceptance Filter ID Value Register3 */
#define CAN_AFIMR4_OFFSET (0x00D0) /* CAN Acceptance Filter ID Mask Register4 */
#define CAN_AFIVR4_OFFSET (0x00D4) /* CAN Acceptance Filter ID Value Register4 */
#define CAN_FIFORR_OFFSET (0x00F0) /* CAN FIFO and Buffer Reset Register */
#define CAN_STMCR_OFFSET  (0x0100) /* CAN Self Test Mode Control Register */
#define CAN_PSLMCR_OFFSET (0x0200) /* CAN PHY Sleep Mode Control Register */
#define CAN_VER_OFFSET    (0xF000) /* CAN Controller IP Version Register */

/* CAN Controller Register */
#define SCOBCA1_FPGA_CAN_ENR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_ENR_OFFSET)
#define SCOBCA1_FPGA_CAN_TQPR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_TQPR_OFFSET)
#define SCOBCA1_FPGA_CAN_BTSR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_BTSR_OFFSET)
#define SCOBCA1_FPGA_CAN_ECNTR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_ECNTR_OFFSET)
#define SCOBCA1_FPGA_CAN_STSR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_STSR_OFFSET)
#define SCOBCA1_FPGA_CAN_ISR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_ISR_OFFSET)
#define SCOBCA1_FPGA_CAN_IER (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_IER_OFFSET)
#define SCOBCA1_FPGA_CAN_TMR1 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_TMR1_OFFSET)
#define SCOBCA1_FPGA_CAN_TMR2 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_TMR2_OFFSET)
#define SCOBCA1_FPGA_CAN_TMR3 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_TMR3_OFFSET)
#define SCOBCA1_FPGA_CAN_TMR4 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_TMR4_OFFSET)
#define SCOBCA1_FPGA_CAN_THPMR1 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_THPMR1_OFFSET)
#define SCOBCA1_FPGA_CAN_THPMR2 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_THPMR2_OFFSET)
#define SCOBCA1_FPGA_CAN_THPMR3 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_THPMR3_OFFSET)
#define SCOBCA1_FPGA_CAN_THPMR4 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_THPMR4_OFFSET)
#define SCOBCA1_FPGA_CAN_RMR1 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_RMR1_OFFSET)
#define SCOBCA1_FPGA_CAN_RMR2 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_RMR2_OFFSET)
#define SCOBCA1_FPGA_CAN_RMR2 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_RMR2_OFFSET)
#define SCOBCA1_FPGA_CAN_RMR3 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_RMR3_OFFSET)
#define SCOBCA1_FPGA_CAN_RMR4 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_RMR4_OFFSET)
#define SCOBCA1_FPGA_CAN_AFER (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFER_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIMR1 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIMR1_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIVR1 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIVR1_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIMR2 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIMR2_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIVR2 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIVR2_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIMR3 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIMR3_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIVR3 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIVR3_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIMR4 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIMR4_OFFSET)
#define SCOBCA1_FPGA_CAN_AFIVR4 (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_AFIVR4_OFFSET)
#define SCOBCA1_FPGA_CAN_FIFORR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_FIFORR_OFFSET)
#define SCOBCA1_FPGA_CAN_STMCR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_STMCR_OFFSET)
#define SCOBCA1_FPGA_CAN_PSLMCR (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_PSLMCR_OFFSET)
#define SCOBCA1_FPGA_CAN_VER (SCOBCA1_FPGA_CAN_BASE_ADDR + CAN_VER_OFFSET)

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

extern bool can_tx_done;
extern bool can_rx_done;
extern bool first_can_err_isr;

bool can_init(bool test_mode);
bool can_terminate(bool test_mode);
uint32_t can_get_idr(uint16_t can_id, uint32_t can_ext_id, bool extend);
bool can_send_full(uint16_t can_id, uint32_t can_ext_id, uint8_t *can_data, uint8_t size, bool extend);
bool is_can_rx_done(int32_t timeout_us);
bool is_can_tx_done(void);
void can_convert_can_data_to_word(uint8_t *can_data, uint8_t size, uint32_t *word1, uint32_t *word2);

static inline bool can_send(uint16_t can_id, uint8_t *can_data, uint8_t size)
{
	return can_send_full(can_id, 0, can_data, size, false);
}
