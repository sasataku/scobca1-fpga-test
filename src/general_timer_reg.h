/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_GENERAL_TIMER_REG_H_
#define SCOBCA1_FPGA_TEST_GENERAL_TIMER_REG_H_

#include <zephyr/kernel.h>

#define SCOBCA1_FPGA_GPTMR_GTR     (0x4F050000) /* Global Timer Register */
#define SCOBCA1_FPGA_GPTMR_TECR    (0x4F050004) /* Timer Enable Control Register */
#define SCOBCA1_FPGA_GPTMR_SITRR   (0x4F050008) /* Software Interrupt Timer Remaining Register */
#define SCOBCA1_FPGA_GPTMR_HITRR   (0x4F05000C) /* Hardware Interrupt Timer Remaining Register */
#define SCOBCA1_FPGA_GPTMR_GTSR    (0x4F050010) /* Global Timer Interrupt Status Register */
#define SCOBCA1_FPGA_GPTMR_GTER    (0x4F050014) /* Global Timer Interrupt Enable Register */
#define SCOBCA1_FPGA_GPTMR_GTOCR1  (0x4F050020) /* Global Timer Output Compare Register 1 */
#define SCOBCA1_FPGA_GPTMR_GTOCR2  (0x4F050024) /* Global Timer Output Compare Register 2 */
#define SCOBCA1_FPGA_GPTMR_GTOCR3  (0x4F050028) /* Global Timer Output Compare Register 3 */
#define SCOBCA1_FPGA_GPTMR_GTOCR4  (0x4F05002C) /* Global Timer Output Compare Register 4 */
#define SCOBCA1_FPGA_GPTMR_SITCR   (0x4F050100) /* Software Interrupt Timer Control Register */
#define SCOBCA1_FPGA_GPTMR_SITPR   (0x4F050104) /* Software Interrupt Timer Prescaler Register */
#define SCOBCA1_FPGA_GPTMR_SITSR   (0x4F050108) /* Software Interrupt Timer Status Register */
#define SCOBCA1_FPGA_GPTMR_SITER   (0x4F05010C) /* Software Interrupt Timer Enable Register */
#define SCOBCA1_FPGA_GPTMR_SITOCR1 (0x4F050110) /* Software Interrupt Timer Output Compare Register 1 */
#define SCOBCA1_FPGA_GPTMR_SITOCR2 (0x4F050114) /* Software Interrupt Timer Output Compare Register 2 */
#define SCOBCA1_FPGA_GPTMR_SITOCR3 (0x4F050118) /* Software Interrupt Timer Output Compare Register 3 */
#define SCOBCA1_FPGA_GPTMR_SITOCR4 (0x4F05011C) /* Software Interrupt Timer Output Compare Register 4 */
#define SCOBCA1_FPGA_GPTMR_SITOCR5 (0x4F050120) /* Software Interrupt Timer Output Compare Register 5 */
#define SCOBCA1_FPGA_GPTMR_SITOCR6 (0x4F050124) /* Software Interrupt Timer Output Compare Register 6 */
#define SCOBCA1_FPGA_GPTMR_SITOCR7 (0x4F050128) /* Software Interrupt Timer Output Compare Register 7 */
#define SCOBCA1_FPGA_GPTMR_SITOCR8 (0x4F05012C) /* Software Interrupt Timer Output Compare Register 8 */
#define SCOBCA1_FPGA_GPTMR_HITCR   (0x4F050200) /* Hardware Interrupt Timer Control Register */
#define SCOBCA1_FPGA_GPTMR_HITPR   (0x4F050204) /* Hardware Interrupt Timer Prescaler Register */
#define SCOBCA1_FPGA_GPTMR_HITOCR1 (0x4F050210) /* Hardware Interrupt Timer Output Compare Register 1 */
#define SCOBCA1_FPGA_GPTMR_HITOCR2 (0x4F050214) /* Hardware Interrupt Timer Output Compare Register 2 */
#define SCOBCA1_FPGA_GPTMR_HITOCR3 (0x4F050218) /* Hardware Interrupt Timer Output Compare Register 3 */
#define SCOBCA1_FPGA_GPTMR_HITOCR4 (0x4F05021C) /* Hardware Interrupt Timer Output Compare Register 4 */
#define SCOBCA1_FPGA_GPTMR_HITOCR5 (0x4F050220) /* Hardware Interrupt Timer Output Compare Register 5 */
#define SCOBCA1_FPGA_GPTMR_HITOCR6 (0x4F050224) /* Hardware Interrupt Timer Output Compare Register 6 */
#define SCOBCA1_FPGA_GPTMR_HITOCR7 (0x4F050228) /* Hardware Interrupt Timer Output Compare Register 7 */
#define SCOBCA1_FPGA_GPTMR_HITOCR8 (0x4F05022C) /* Hardware Interrupt Timer Output Compare Register 8 */
#define SCOBCA1_FPGA_GPTMR_VER     (0x4F05F000) /* General Purpose Timer IP Version Register */

#endif /* SCOBCA1_FPGA_TEST_GENERAL_TIMER_REG_H_ */
