/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_SYSTEM_REG_H_
#define SCOBCA1_FPGA_TEST_SYSTEM_REG_H_

#include <zephyr/kernel.h>

#define SCOBCA1_FPGA_SYSREG_CODEMSEL  (0x4F000000) /* Code Memory Select Register */
#define SCOBCA1_FPGA_SYSREG_SYSCLKCTL (0x4F000004) /* System Clock Control Register */
#define SCOBCA1_FPGA_SYSREG_CFGMEMCTL (0x4F000010) /* Configuration Memory Register */
#define SCOBCA1_FPGA_SYSREG_PWRCYCLE  (0x4F000020) /* Power Cycle Register */
#define SCOBCA1_FPGA_SYSREG_SPAD1     (0x4F0000F0) /* Scratchpad 1 Register */
#define SCOBCA1_FPGA_SYSREG_SPAD2     (0x4F0000F4) /* Scratchpad 2 Register */
#define SCOBCA1_FPGA_SYSREG_SPAD3     (0x4F0000F8) /* Scratchpad 3 Register */
#define SCOBCA1_FPGA_SYSREG_SPAD4     (0x4F0000FC) /* Scratchpad 4 Register */
#define SCOBCA1_FPGA_SYSREG_VER       (0x4F00F000) /* System Register IP Version Register */
#define SCOBCA1_FPGA_SYSREG_BUILDINFO (0x4F00FF00) /* Build Infomation Register */
#define SCOBCA1_FPGA_SYSREG_DNA1      (0x4F00FF10) /* Device DNA 1 Register */
#define SCOBCA1_FPGA_SYSREG_DNA2      (0x4F00FF14) /* Device DNA 2 Register */

#endif /* SCOBCA1_FPGA_TEST_SYSTEM_REG_H_ */
