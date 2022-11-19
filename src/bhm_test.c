/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"
#include "system_monitor_reg.h"
#include "general_timer_reg.h"
#include "bhm_test.h"

static float convert_temp(uint32_t raw_tmp)
{
	float conv_tmp;

	conv_tmp = ((raw_tmp & 0x0000FF00) >> 8) +
				(((raw_tmp & 0x000000F0) >> 4) * 0.0625);

	return conv_tmp;
}

static float convert_cv_shunt(uint32_t raw_snt)
{
	return ((raw_snt >> 3) * 40);
}

static uint32_t convert_cv_bus(uint32_t raw_bus)
{
	return ((raw_bus >> 3) * 8);
}

static bool assert_i2c_access(uint32_t raw_val)
{
	if ((raw_val & 0x80000000) == 0) {
		return true;
	} else {
		printk("  !!! Assertion failed: I2C error occurred\n");
		return false;
	}
}

static uint32_t assert_temp(void)
{
	uint32_t err_cnt = 0;
	uint32_t raw_tmp;
	float tmp;
	uint32_t tmp_regs[] = {SCOBCA1_FPGA_SYSMON_BHM_TEMP1R,
							SCOBCA1_FPGA_SYSMON_BHM_TEMP2R,
							SCOBCA1_FPGA_SYSMON_BHM_TEMP3R};

	for (uint8_t i=0; i<ARRAY_SIZE(tmp_regs); i++) {
		raw_tmp = sys_read32(tmp_regs[i]);
		tmp = convert_temp(raw_tmp);
		printk("  Tempature Sensor %d  : %.4f C (RAW:0x%08x)\n", i+1, tmp, raw_tmp);
		if (assert_i2c_access(raw_tmp)) {
			if ((tmp < SCOBCA1_TEMP_LIMIT_LOWER || tmp > SCOBCA1_TEMP_LIMIT_UPPER)) {
				printk("  !!! Assertion failed: abnormal tempature (Tempature Sensor %d)\n", i+1);
				err_cnt++;
			}
		} else {
			err_cnt++;
		}
	}

	return err_cnt;
}

static uint32_t assert_cv(void)
{
	uint32_t err_cnt = 0;
	uint32_t raw_snt, raw_bus;
	uint32_t snt, bus;
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
	const char vdd_chars[][16] = {"VDD 1V0      ",
									"VDD 1V8      ",
									"VDD 3V3      ",
									"VDD 3V3 SYS-A",
									"VDD 3V3 SYS-B",
									"VDD 3V3 IO   "};

	for (uint8_t i=0; i<ARRAY_SIZE(snt_regs); i++) {
		raw_snt = sys_read32(snt_regs[i]);
		snt = convert_cv_shunt(raw_snt);
		printk("  %s Shunt : %d uv (RAW:0x%08x)\n", vdd_chars[i], snt, raw_snt); 
		if (!assert_i2c_access(raw_snt)) {
			err_cnt++;
		}
		raw_bus = sys_read32(bus_regs[i]);
		bus = convert_cv_bus(raw_bus);
		printk("  %s Bus   : %d mv (RAW:0x%08x)\n", vdd_chars[i], bus, raw_bus); 
		if (!assert_i2c_access(raw_bus)) {
			err_cnt++;
		}
	}

	return err_cnt;
}

uint32_t bhm_enable(void)
{
	uint32_t err_cnt = 0;

	printk("* [#1] Enable Board Health Interrupt\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_IER, 0x00073F03);
	printk("SCOBCA1_FPGA_SYSMON_BHM_IER: %08x\n", sys_read32(SCOBCA1_FPGA_SYSMON_BHM_IER));

	printk("* [#2] Set I2C Access Count Setting to 0 (No retry)\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_I2CACCCNTR, 0x00);
	printk("SCOBCA1_FPGA_SYSMON_BHM_I2CACCCNTR: %08x\n", sys_read32(SCOBCA1_FPGA_SYSMON_BHM_I2CACCCNTR));

	printk("* [#3] Enable all sensor device initialization\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_INICTLR, 0x0001001F);
	printk("SCOBCA1_FPGA_SYSMON_BHM_INICTLR: %08x\n", sys_read32(SCOBCA1_FPGA_SYSMON_BHM_INICTLR));

	printk("* [#4] Verify initialization and clear\n");
	if (!assert32(SCOBCA1_FPGA_SYSMON_BHM_ISR, 0x01, REG_READ_RETRY(10))) {
		err_cnt++;
		goto end_of_test;
	}
	write32(SCOBCA1_FPGA_SYSMON_BHM_ISR, 0x01);
	if (!assert32(SCOBCA1_FPGA_SYSMON_BHM_ISR, 0x00, REG_READ_RETRY(10))) {
		err_cnt++;
		goto end_of_test;
	}

	printk("* [#5] Set monitoring timing\n");
	/*
		I2C access timing
		Current Voltage: 0.1 ms
		Tempature      : 1.7 ms
	*/
	write32(SCOBCA1_FPGA_GPTMR_HITCR, 0x00280000);
	write32(SCOBCA1_FPGA_GPTMR_HITPR, 0x095F);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR1, 0x0014);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR2, 0x0001);
	write32(SCOBCA1_FPGA_GPTMR_HITOCR3, 0x0011);

	printk("* [#6] Enable all sensor device monitoring\n");
	write32(SCOBCA1_FPGA_SYSMON_BHM_MONCTLR, 0x1F);

	printk("* [#7] Enable hardware interrupt timer\n");
	write32(SCOBCA1_FPGA_GPTMR_TECR, 0x02);

end_of_test:
	return err_cnt;
}

uint32_t bhm_read_sensor_data(void)
{
	uint32_t err_cnt = 0;

	printk("* [#1] Read sensor data and verify\n");
	err_cnt += assert_cv();
	err_cnt += assert_temp();

	return err_cnt;
}

uint32_t bhm_test(uint32_t test_no)
{
	uint32_t ret;
	uint32_t err_cnt = 0;

	printk("* [%d] Start Board Health Monitor Test\n", test_no);

	printk("* [%d-1] Start Enable Board Health Monitor\n", test_no);
	ret = bhm_enable();
	err_cnt += ret;

	k_sleep(K_MSEC(5));

	printk("* [%d-2] Start Read seonsor data test\n", test_no);
	ret = bhm_read_sensor_data();
	err_cnt += ret;

	print_result(test_no, err_cnt);

	return err_cnt;
}
