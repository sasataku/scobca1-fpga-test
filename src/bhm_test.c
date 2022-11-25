/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "system_reg.h"
#include "system_monitor_reg.h"
#include "general_timer_reg.h"
#include "bhm_test.h"

bool i2c_initalized = false;
extern bool is_exit;

static float convert_temp(uint32_t raw_tmp)
{
	float conv_tmp;

	conv_tmp = ((raw_tmp & 0x0000FF00) >> 8) +
				(((raw_tmp & 0x000000F0) >> 4) * 0.0625);

	return conv_tmp;
}

static float convert_xadc_temp(uint32_t raw_tmp)
{
	float conv_tmp;

	conv_tmp = ((((raw_tmp & 0x0000FFF0) >> 4) * 503.975) / 4096) - 273.15;

	return conv_tmp;
}

static uint32_t convert_cv_shunt(uint32_t raw_snt)
{
	return ((raw_snt >> 3) * 40);
}

static uint32_t convert_cv_bus(uint32_t raw_bus)
{
	return ((raw_bus >> 3) * 8);
}

static float convert_cv_xadc(uint32_t raw_xadc)
{
	float conv_cv;

	conv_cv = (raw_xadc & 0x0000FFF0) >> 4;

	return conv_cv / 4096 * 3;
}

static bool assert_i2c_access(uint32_t raw_val)
{
	if ((raw_val & 0x80000000) == 0) {
		return true;
	} else {
		err("  !!! Assertion failed: I2C error occurred\n");
		return false;
	}
}

static uint32_t assert_temp(void)
{
	uint32_t err_cnt = 0;
	uint32_t xadc_raw_temp;
	uint32_t raw_tmp;
	float tmp;
	float xadc_tmp;
	uint32_t tmp_regs[] = {SCOBCA1_FPGA_SYSMON_BHM_TEMP1R,
							SCOBCA1_FPGA_SYSMON_BHM_TEMP2R,
							SCOBCA1_FPGA_SYSMON_BHM_TEMP3R};

	for (uint8_t i=0; i<ARRAY_SIZE(tmp_regs); i++) {
		raw_tmp = sys_read32(tmp_regs[i]);
		tmp = convert_temp(raw_tmp);
		info("  Temperature Sensor %d : %.4f C (RAW:0x%08x)\n", i+1, tmp, raw_tmp);
		if (assert_i2c_access(raw_tmp)) {
			if ((tmp < SCOBCA1_TEMP_LIMIT_LOWER || tmp > SCOBCA1_TEMP_LIMIT_UPPER)) {
				err("  !!! Assertion failed: abnormal temperature (Temperature Sensor %d)\n", i+1);
				write32(SCOBCA1_FPGA_SYSREG_PWRCYCLE, 0x5A5A0001);
				err_cnt++;
			}
		} else {
			err_cnt++;
		}
	}

	/* XADC Temperature */
	xadc_raw_temp = sys_read32(SCOBCA1_FPGA_SYSMON_XADC_TEMP);
	xadc_tmp = convert_xadc_temp(xadc_raw_temp);
	info("  XADC Temperature    : %.4f C (RAW:0x%08x)\n", xadc_tmp, xadc_raw_temp);
	if ((xadc_tmp < SCOBCA1_TEMP_LIMIT_LOWER || xadc_tmp > SCOBCA1_TEMP_LIMIT_UPPER)) {
		err("  !!! Assertion failed: abnormal temperature (XADC Temperature)\n");
		write32(SCOBCA1_FPGA_SYSREG_PWRCYCLE, 0x5A5A0001);
		err_cnt++;
	}

	return err_cnt;
}

static uint32_t assert_cv(void)
{
	uint32_t err_cnt = 0;
	uint32_t raw_snt, raw_bus, raw_xadc;
	uint32_t snt, bus;
	float xadc;
	uint32_t snt_regs[] = {SCOBCA1_FPGA_SYSMON_BHM_1V0SNTVR,
							SCOBCA1_FPGA_SYSMON_BHM_1V8SNTVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3SNTVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3SYSASNTVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3SYSBSNTVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3IOSNTVR};
	uint32_t bus_regs[] = {SCOBCA1_FPGA_SYSMON_BHM_1V0BUSVR,
							SCOBCA1_FPGA_SYSMON_BHM_1V8BUSVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3BUSVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3SYSABUSVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3SYSBBUSVR,
							SCOBCA1_FPGA_SYSMON_BHM_3V3IOBUSVR};
	uint32_t xadc_regs[] = {SCOBCA1_FPGA_SYSMON_XADC_VCCINT,
							SCOBCA1_FPGA_SYSMON_XADC_VCCAUX,
							SCOBCA1_FPGA_SYSMON_XADC_VCCBRAM};
	const char vdd_chars[][16] = {"VDD 1V0      ",
									"VDD 1V8      ",
									"VDD 3V3      ",
									"VDD 3V3 SYS-A",
									"VDD 3V3 SYS-B",
									"VDD 3V3 IO   "};
	const char xadc_chars[][24] = {"XADC VCCINT 1V0 ",
									"XADC VCCAUX 1V8 ",
									"XADC VCCBRAM 1V0"};

	for (uint8_t i=0; i<ARRAY_SIZE(snt_regs); i++) {
		raw_snt = sys_read32(snt_regs[i]);
		snt = convert_cv_shunt(raw_snt);
		info("  %s Shunt : %d uv (RAW:0x%08x)\n", vdd_chars[i], snt, raw_snt);
		if (!assert_i2c_access(raw_snt)) {
			err_cnt++;
		}
		raw_bus = sys_read32(bus_regs[i]);
		bus = convert_cv_bus(raw_bus);
		info("  %s Bus   : %d mv (RAW:0x%08x)\n", vdd_chars[i], bus, raw_bus);
		if (!assert_i2c_access(raw_bus)) {
			err_cnt++;
		}
	}

	/* XADC Voltage */
	for (uint8_t i=0; i<ARRAY_SIZE(xadc_regs); i++) {
		raw_xadc = sys_read32(xadc_regs[i]);
		xadc = convert_cv_xadc(raw_xadc);
		info("  %s    : %.4f v (RAW:0x%08x)\n", xadc_chars[i], xadc, raw_xadc);
	}

	return err_cnt;
}

bool bhm_enable(void)
{
	debug("* [#1] Clear Board Health Interrupt\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_ISR, 0xFFFFFFFF);

	debug("* [#2] Enable Board Health Interrupt (Without Tempature Alert)\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_IER, 0x00033F03);

	debug("* [#3] Set I2C Access Count Setting to 0 (No retry)\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_I2CACCCNTR, 0x00);

	debug("* [#4] Enable all sensor device initialization\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_INICTLR, 0x0001001F);

	debug("* [#5] Verify initialization\n");
	for (int8_t i=0; i<10; i++) {
		if (i2c_initalized) {
			break;
		}
		k_sleep(K_MSEC(10));
	}

	if (!i2c_initalized) {
		err("  !!! Assertion failed: I2C sensor init timed out\n");
		return false;
	}

	debug("* [#6] Set monitoring timing\n");
	/*
		I2C access timing
		Current Voltage: 0.1 ms
		Temperature      : 1.7 ms
	*/
	write32(SCOBCA1_FPGA_GPTMR_HITCR, 0x00280000);
	write32(SCOBCA1_FPGA_GPTMR_HITPR, 0x095F);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR1, 0x0014);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR2, 0x0001);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR3, 0x0011);

	debug("* [#7] Enable all sensor device monitoring\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_MONCTLR, 0x1F);

	debug("* [#8] Enable hardware interrupt timer\n");
	write32(SCOBCA1_FPGA_GPTMR_TECR, 0x02);

	return true;
}

uint32_t bhm_read_sensor_data(void)
{
	uint32_t err_cnt = 0;

	debug("* [#1] Read sensor data and verify\n");
	err_cnt += assert_cv();
	err_cnt += assert_temp();

	return err_cnt;
}

uint32_t bhm_test(uint32_t test_no)
{
	uint32_t err_cnt = 0;

	info("* [%d] Start Board Health Monitor Test\n", test_no);

	info("* [%d-1] Start Enable Board Health Monitor\n", test_no);
	if (!bhm_enable()) {
		err_cnt++;
		goto end_of_test;
	}

	k_sleep(K_MSEC(5));

	info("* [%d-2] Start Read seonsor data test\n", test_no);
	err_cnt += bhm_read_sensor_data();

end_of_test:
	print_result(test_no, err_cnt);

	return err_cnt;
}
