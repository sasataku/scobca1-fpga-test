/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_SYSTEM_MONITOR_REG_H_
#define SCOBCA1_FPGA_TEST_SYSTEM_MONITOR_REG_H_

#include <zephyr/kernel.h>

#define SCOBCA1_FPGA_SYSMON_WDOG_CTRL        (0x4F040000) /* Watchdog Control Register */
#define SCOBCA1_FPGA_SYSMON_WDOG_SIVAL       (0x4F040010) /* Watchdog Signal Interval Register */
#define SCOBCA1_FPGA_SYSMON_INT_STATUS       (0x4F040030) /* System Monitor Interrupt Status Register */
#define SCOBCA1_FPGA_SYSMON_INT_ENABLE       (0x4F040034) /* System Monitor Interrupt Enable Register */
#define SCOBCA1_FPGA_SYSMON_SEM_STATE        (0x4F040040) /* SEM Controller State Register */
#define SCOBCA1_FPGA_SYSMON_SEM_ECCOUNT      (0x4F040044) /* SEM Error Correction Count Register */
#define SCOBCA1_FPGA_SYSMON_SEM_HTIMEOUT     (0x4F040048) /* SEM Heartbeat Timeout Register */
#define SCOBCA1_FPGA_SYSMON_SEM_EINJECT1     (0x4F040050) /* SEM Error Injection Command Register */
#define SCOBCA1_FPGA_SYSMON_SEM_EINJECT2     (0x4F040054) /* SEM Error Injection Command Register */
#define SCOBCA1_FPGA_SYSMON_XADC_TEMP        (0x4F041000) /* XADC Tempature */
#define SCOBCA1_FPGA_SYSMON_XADC_VCCINT      (0x4F041010) /* XADC VCCINT 1.0V */
#define SCOBCA1_FPGA_SYSMON_XADC_VCCAUX      (0x4F041020) /* XADC VCCAUX 1.8V */
#define SCOBCA1_FPGA_SYSMON_XADC_VCCBRAM     (0x4F041060) /* XADC VCCBRAM 1.0V */
#define SCOBCA1_FPGA_SYSMON_BHM_INICTLR      (0x4F042000) /* Board Health Initialization Access Control Register */
#define SCOBCA1_FPGA_SYSMON_BHM_MONCTLR      (0x4F042004) /* Board Health Monitoring Access Control Register */
#define SCOBCA1_FPGA_SYSMON_BHM_ISR          (0x4F042010) /* Board Health Interrupt Status Register */
#define SCOBCA1_FPGA_SYSMON_BHM_IER          (0x4F042014) /* Board Health Interrupt Enable Register */
#define SCOBCA1_FPGA_SYSMON_BHM_1V0SNTVR     (0x4F042020) /* Board Health VDD_1V0 Shunt Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_1V0BUSVR     (0x4F042024) /* Board Health VDD_1V0 Bus Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_1V8SNTVR     (0x4F042028) /* Board Health VDD_1V8 Shunt Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_1V8BUSVR     (0x4F04202C) /* Board Health VDD_1V8 Bus Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3SNTVR     (0x4F042030) /* Board Health VDD_3V3 Shunt Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3BUSVR     (0x4F042034) /* Board Health VDD_3V3 Bus Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3SYSASNTVR (0x4F042038) /* Board Health VDD_3V3_SYS_A Shunt Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3SYSABUSVR (0x4F04203C) /* Board Health VDD_3V3_SYS_A Bus Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3SYSBSNTVR (0x4F042040) /* Board Health VDD_3V3_SYS_B Shunt Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3SYSBBUSVR (0x4F042044) /* Board Health VDD_3V3_SYS_B Bus Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3IOSNTVR   (0x4F042048) /* Board Health VDD_3V3_IO Shunt Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_3V3IOBUSVR   (0x4F04204C) /* Board Health VDD_3V3_IO Bus Voltage Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_TEMP1R       (0x4F042050) /* Board Health Temperature1 Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_TEMP2R       (0x4F042054) /* Board Health Temperature2 Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_TEMP3R       (0x4F042058) /* Board Health Temperature3 Monitor Register */
#define SCOBCA1_FPGA_SYSMON_BHM_SWCTLR       (0x4F042060) /* Board Health Software Access Control Register */
#define SCOBCA1_FPGA_SYSMON_BHM_SWWDTR       (0x4F042064) /* Board Health Software Access Write Data Register */
#define SCOBCA1_FPGA_SYSMON_BHM_SWRDTR       (0x4F042068) /* Board Health Software Access Read Data Register */
#define SCOBCA1_FPGA_SYSMON_BHM_I2CPSCR      (0x4F042080) /* Board Health I2C Prescale Setting Register */
#define SCOBCA1_FPGA_SYSMON_BHM_I2CACCCNTR   (0x4F042084) /* Board Health I2C Access Count Setting Register */
#define SCOBCA1_FPGA_SYSMON_BHM_ASR          (0x4F0420C0) /* Board Health Access Status Register */

#endif /* SCOBCA1_FPGA_TEST_SYSTEM_MONITOR_REG_H_ */
