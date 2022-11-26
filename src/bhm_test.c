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

#define I2C_DEV_DEFAULT_CVM_THREHOLD      (0x00007FF8)
#define I2C_DEV_DEFAULT_TEMP_LOW_THREHOLD (0x00004B00)
#define I2C_DEV_DEFAULT_TEMP_HI_THREHOLD  (0x00005000)
#define I2C_ALERT_MONITOR_REG (0x4FF00500)
#define I2C_DEV_CVM1 (0x00)
#define I2C_DEV_CVM2 (0x01)
#define I2C_DEV_TEMP1 (0x02)
#define I2C_DEV_TEMP2 (0x03)
#define I2C_DEV_TEMP3 (0x04)
#define I2C_DEV_ALERT_HI_ALL     (0x00000007)
#define I2C_DEV_ALERT_LOW_CVMCRT (0x00000006)
#define I2C_DEV_ALERT_LOW_CVMWAN (0x00000005)
#define I2C_DEV_ALERT_LOW_TEMP   (0x00000003)
#define I2C_DEV_TEMPALERT_MASK   (0x00040000)
#define I2C_DEV_CVMWARN_MASK     (0x00020000)
#define I2C_DEV_CVMCRIT_MASK     (0x00010000)
#define I2C_DEV_TEMPALERT_SHIFT  (18u)
#define I2C_DEV_CVMWARN_SHIFT    (17u)
#define I2C_DEV_CVMCRIT_SHIFT    (16u)

bool i2c_initalized = false;
bool i2c_sw_access_done = false;
uint32_t last_i2c_isr = 0;
extern bool is_exit;

static bool is_i2c_access_done(void)
{
	for (uint8_t i=0; i<10; i++) {
		if (i2c_sw_access_done) {
			i2c_sw_access_done = false;
			return true;
		}
		k_usleep(10);
	}

	return false;
}

static float convert_tmp(uint32_t raw_tmp)
{
	int8_t int_tmp = (raw_tmp & 0x0000FF00) >> 8;
	float point = ((raw_tmp & 0x000000F0) >> 4) * 0.0625;

	return int_tmp + point;
}

static float convert_xadc_tmp(uint32_t raw_tmp)
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

static uint32_t assert_tmp(void)
{
	uint32_t err_cnt = 0;
	uint32_t xadc_raw_tmp;
	uint32_t raw_tmp;
	float tmp;
	float xadc_tmp;
	uint32_t tmp_regs[] = {SCOBCA1_FPGA_SYSMON_BHM_TEMP1R,
							SCOBCA1_FPGA_SYSMON_BHM_TEMP2R,
							SCOBCA1_FPGA_SYSMON_BHM_TEMP3R};

	for (uint8_t i=0; i<ARRAY_SIZE(tmp_regs); i++) {
		raw_tmp = sys_read32(tmp_regs[i]);
		tmp = convert_tmp(raw_tmp);
		info("  Temperature Sensor %d : %.4f C (RAW:0x%08x)\n", i+1, tmp, raw_tmp);
		if (assert_i2c_access(raw_tmp)) {
			if ((tmp < SCOBCA1_TEMP_LIMIT_LOWER || tmp > SCOBCA1_TEMP_LIMIT_UPPER)) {
				err("  !!! Assertion failed: abnormal tmperature (Temperature Sensor %d)\n", i+1);
				write32(SCOBCA1_FPGA_SYSREG_PWRCYCLE, 0x5A5A0001);
				err_cnt++;
			}
		} else {
			err_cnt++;
		}
	}

	/* XADC Temperature */
	xadc_raw_tmp = sys_read32(SCOBCA1_FPGA_SYSMON_XADC_TEMP);
	xadc_tmp = convert_xadc_tmp(xadc_raw_tmp);
	info("  XADC Temperature    : %.4f C (RAW:0x%08x)\n", xadc_tmp, xadc_raw_tmp);
	if ((xadc_tmp < SCOBCA1_TEMP_LIMIT_LOWER || xadc_tmp > SCOBCA1_TEMP_LIMIT_UPPER)) {
		err("  !!! Assertion failed: abnormal tmperature (XADC Temperature)\n");
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

static bool cvm_critical_alert_test(uint8_t dev_no, uint16_t threshold)
{
	bool ret = true;
	uint32_t val;
	uint32_t ctrl = 0;

	/* Write threhsold value */
	val = threshold << 3;
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, val);

	/* I2C control */
	ctrl = 0x01000700 | (dev_no << 16);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl);

	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	/* Wait the alert interrupt */
	k_sleep(K_MSEC(10));

	/*  Verify latest ISR value */
	if (((last_i2c_isr & I2C_DEV_CVMCRIT_MASK) >> I2C_DEV_CVMCRIT_SHIFT) == 0) {
		err("  !!! Assertion failed: CVM Critical Alert is not detected\n");
		ret = false;
		goto end_of_test;
	}
	info("* CVM Critical Alert is detected, but it's expected behavior\n");

	if (!assert32(I2C_ALERT_MONITOR_REG, I2C_DEV_ALERT_LOW_CVMCRT, REG_READ_RETRY(0))) {
		ret = false;
		goto end_of_test;
	}

	/* Back to default threshold */
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, I2C_DEV_DEFAULT_CVM_THREHOLD);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl);
	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	k_sleep(K_MSEC(10));

	/* Verify Alert signal (All Hi) */
	if (!assert32(I2C_ALERT_MONITOR_REG, I2C_DEV_ALERT_HI_ALL, REG_READ_RETRY(0))) {
		ret = false;
		goto end_of_test;
	}

end_of_test:
	last_i2c_isr = 0;
	return true;
}

static bool cvm_warn_alert_test(uint8_t dev_no, uint16_t threshold)
{
	bool ret = true;
	uint32_t val;
	uint32_t ctrl = 0;

	/* Write threhsold value */
	val = threshold << 3;
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, val);

	/* I2C control */
	ctrl = 0x01000800 | (dev_no << 16);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl);

	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	/* Wait the alert interrupt */
	k_sleep(K_MSEC(10));

	/*  Verify latest ISR value */
	if (((last_i2c_isr & I2C_DEV_CVMWARN_MASK) >> I2C_DEV_CVMWARN_SHIFT) == 0) {
		err("  !!! Assertion failed: CVM Critical Alert is not detected\n");
		ret = false;
		goto end_of_test;
	}
	info("* CVM Warning Alert is detected, but it's expected behavior\n");

	if (!assert32(I2C_ALERT_MONITOR_REG, I2C_DEV_ALERT_LOW_CVMWAN, REG_READ_RETRY(0))) {
		ret = false;
		goto end_of_test;
	}

	/* Back to default threshold */
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, I2C_DEV_DEFAULT_CVM_THREHOLD);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl);
	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	k_sleep(K_MSEC(10));

	/* Verify Alert signal (All Hi) */
	if (!assert32(I2C_ALERT_MONITOR_REG, I2C_DEV_ALERT_HI_ALL, REG_READ_RETRY(0))) {
		ret = false;
		goto end_of_test;
	}

end_of_test:
	last_i2c_isr = 0;
	return ret;
}

static bool tmp_alert_test(uint8_t dev_no, uint16_t low, uint16_t hi)
{
	bool ret = true;
	uint32_t val;
	uint32_t ctrl_low = 0;
	uint32_t ctrl_hi = 0;

	/* Write threhsold value (Tlow) */
	val = low << 4;
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, val);

	/* I2C control */
	ctrl_low = 0x01000200 | (dev_no << 16);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl_low);

	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	/* Write threhsold value (THight) */
	val = hi << 4;
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, val);

	/* I2C control */
	ctrl_hi = 0x01000300 | (dev_no << 16);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl_hi);

	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	/* Wait the alert interrupt */
	k_sleep(K_MSEC(100));

	/*  Verify latest ISR value */
	if (((last_i2c_isr & I2C_DEV_TEMPALERT_MASK) >> I2C_DEV_TEMPALERT_SHIFT) == 0)  {
		err("  !!! Assertion failed: Temperature Alert is not detected\n");
		ret = false;
	} else {
		info("* Temperature Alert is detected, but it's expected behavior\n");
	}

	if (!assert32(I2C_ALERT_MONITOR_REG, I2C_DEV_ALERT_LOW_TEMP, REG_READ_RETRY(10))) {
		ret = false;
	}

	/* Back to default threshold */
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, I2C_DEV_DEFAULT_TEMP_LOW_THREHOLD);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl_low);
	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	write32(SCOBCA1_FPGA_SYSMON_BHM_SWWDTR, I2C_DEV_DEFAULT_TEMP_HI_THREHOLD);
	write32(SCOBCA1_FPGA_SYSMON_BHM_SWCTLR, ctrl_hi);
	if (!is_i2c_access_done()) {
		err("  !!! Assertion failed: I2C SW Access Done timed out\n");
		ret = false;
		goto end_of_test;
	}

	k_sleep(K_MSEC(100));

	/* Verify Alert signal (All Hi) */
	if (!assert32(I2C_ALERT_MONITOR_REG, I2C_DEV_ALERT_HI_ALL, REG_READ_RETRY(0))) {
		ret = false;
		goto end_of_test;
	}

end_of_test:
	last_i2c_isr = 0;
	return ret;
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
		Current Voltage  : 0.1 ms
		Temperature      : 6.4 ms
	*/
	write32(SCOBCA1_FPGA_GPTMR_HITCR, 0x00280000);
	write32(SCOBCA1_FPGA_GPTMR_HITPR, 0x095F);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR1, 0x0050);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR2, 0x0001);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR3, 0x0040);

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
	err_cnt += assert_tmp();

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

uint32_t i2c_internal_crack_test(uint32_t test_no)
{
	uint32_t err_num = 0;
	uint16_t cv_threshold = 0x01;
	uint16_t tmp_low_threshold = 0x01;
	uint16_t tmp_hi_threshold = 0x02;

	info("*** Internal I2C Alert crack test starts ***\n");

	info("* [#1] Enable Board Health Interrupt\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_IER, 0x00073F03);

	info("* [#2] Verify Alert signal (All Hi)\n");
	if (!assert32(I2C_ALERT_MONITOR_REG, I2C_DEV_ALERT_HI_ALL, REG_READ_RETRY(0))) {
		err_num++;
		goto end_of_test;
	}

	info("* [#3] CVM1 critical alert test\n");
	if (!cvm_critical_alert_test(I2C_DEV_CVM1, cv_threshold)) {
		err_num++;
	}

	info("* [#4] CVM2 critical alert test\n");
	if (!cvm_critical_alert_test(I2C_DEV_CVM2, cv_threshold)) {
		err_num++;
	}

	info("* [#5] CVM1 warning alert test\n");
	if (!cvm_warn_alert_test(I2C_DEV_CVM1, cv_threshold)) {
		err_num++;
	}

	info("* [#6] CVM2 warning alert test\n");
	if (!cvm_warn_alert_test(I2C_DEV_CVM1, cv_threshold)) {
		err_num++;
	}

	info("* [#7] Temperature 1 alert test\n");
	if (!tmp_alert_test(I2C_DEV_TEMP1, tmp_low_threshold, tmp_hi_threshold)) {
		err_num++;
	}

	info("* [#8] Temperature 2 alert test\n");
	if (!tmp_alert_test(I2C_DEV_TEMP2, tmp_low_threshold, tmp_hi_threshold)) {
		err_num++;
	}

	info("* [#9] Temperature 3 alert test\n");
	if (!tmp_alert_test(I2C_DEV_TEMP2, tmp_low_threshold, tmp_hi_threshold)) {
		err_num++;
	}

	info("*** test done, error count: %d ***\n", err_num);

end_of_test:
	return err_num;
}
