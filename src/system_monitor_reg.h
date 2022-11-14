/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_SYSTEM_MONITOR_REG_H_
#define SCOBCA1_FPGA_TEST_SYSTEM_MONITOR_REG_H_

#include <zephyr/kernel.h>

#define SCOBCA1_FPGA_SYSMON_WDOG_CTRL    (0x4F040000) /* Watchdog Control Register */
#define SCOBCA1_FPGA_SYSMON_WDOG_SIVAL   (0x4F040010) /* Watchdog Signal Interval Register */
#define SCOBCA1_FPGA_SYSMON_INT_STATUS   (0x4F040030) /* System Monitor Interrupt Status Register */
#define SCOBCA1_FPGA_SYSMON_INT_ENABLE   (0x4F040034) /* System Monitor Interrupt Enable Register */
#define SCOBCA1_FPGA_SYSMON_SEM_STATE    (0x4F040040) /* SEM Controller State Register */
#define SCOBCA1_FPGA_SYSMON_SEM_ECCOUNT  (0x4F040044) /* SEM Error Correction Count Register */
#define SCOBCA1_FPGA_SYSMON_SEM_HTIMEOUT (0x4F040048) /* SEM Heartbeat Timeout Register */
#define SCOBCA1_FPGA_SYSMON_SEM_EINJECT1 (0x4F040050) /* SEM Error Injection Command Register 1 */
#define SCOBCA1_FPGA_SYSMON_SEM_EINJECT2 (0x4F040054) /* SEM Error Injection Command Register 2 */
#define SCOBCA1_FPGA_SYSMON_XADC_REG     (0x4F041000) /* XADC Register Window */

#endif /* SCOBCA1_FPGA_TEST_SYSTEM_MONITOR_REG_H_ */
