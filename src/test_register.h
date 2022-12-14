/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCOBCA1_FPGA_TEST_REGISTER_H_
#define SCOBCA1_FPGA_TEST_REGISTER_H_

#include <zephyr/kernel.h>

/* SRAM mirror start */
#define SRAM_MIRROR_BASE (0x60000000)

/* Base address */
#define SCOBCA1_FPGA_TEST_REG_BASE_ADDR  (0x4FF00000)
#define TEST_REG_ADDR(offset) (SCOBCA1_FPGA_TEST_REG_BASE_ADDR + offset)

/* Test Register GPIO mode */
#define TEST_NOT_GPIO  0x00000000
#define TEST_GPIO_IN 0x00000001
#define TEST_GPIO_OUT_LOW 0x00000002
#define TEST_GPIO_OUT_HIGH 0x00000003 /* Hi Impedance also using this */

/* Control Registers */
// SRAM
#define TEST_CTRL_SRAM_A19 (0x0000)
#define TEST_CTRL_SRAM_A18 (0x0004)
#define TEST_CTRL_SRAM_A17 (0x0008)
#define TEST_CTRL_SRAM_A16 (0x000C)
#define TEST_CTRL_SRAM_A15 (0x0010)
#define TEST_CTRL_SRAM_A14 (0x0014)
#define TEST_CTRL_SRAM_A13 (0x0018)
#define TEST_CTRL_SRAM_A12 (0x001C)
#define TEST_CTRL_SRAM_A11 (0x0020)
#define TEST_CTRL_SRAM_A10 (0x0024)
#define TEST_CTRL_SRAM_A9 (0x0028)
#define TEST_CTRL_SRAM_A8 (0x002C)
#define TEST_CTRL_SRAM_A7 (0x0030)
#define TEST_CTRL_SRAM_A6 (0x0034)
#define TEST_CTRL_SRAM_A5 (0x0038)
#define TEST_CTRL_SRAM_A4 (0x003C)
#define TEST_CTRL_SRAM_A3 (0x0040)
#define TEST_CTRL_SRAM_A2 (0x0044)
#define TEST_CTRL_SRAM_A1 (0x0048)
#define TEST_CTRL_SRAM_A0 (0x004C)
#define TEST_CTRL_SRAM1_CE_B (0x0050)
#define TEST_CTRL_SRAM1_OE_B (0x0054)
#define TEST_CTRL_SRAM1_WE_B (0x0058)
#define TEST_CTRL_SRAM1_BHE_B (0x005C)
#define TEST_CTRL_SRAM1_BLE_B (0x0060)
#define TEST_CTRL_SRAM2_CE_B (0x0064)
#define TEST_CTRL_SRAM2_OE_B (0x0068)
#define TEST_CTRL_SRAM2_WE_B (0x006C)
#define TEST_CTRL_SRAM2_BHE_B (0x0070)
#define TEST_CTRL_SRAM2_BLE_B (0x0074)

// QSPI I/F(CFG)
#define TEST_CTRL_CFG_MEM_CS_B (0x0100)
#define TEST_CTRL_CFG_MEM_IO3 (0x0104)
#define TEST_CTRL_CFG_MEM_IO2 (0x0108)
#define TEST_CTRL_CFG_MEM_IO1 (0x010C)
#define TEST_CTRL_CFG_MEM_IO0 (0x0110)

// QSPI I/F(DATA)
#define TEST_CTRL_DATA_MEM1_CS_B (0x0200)
#define TEST_CTRL_DATA_MEM1_IO3 (0x0204)
#define TEST_CTRL_DATA_MEM1_IO2 (0x0208)
#define TEST_CTRL_DATA_MEM1_IO1 (0x020C)
#define TEST_CTRL_DATA_MEM1_IO0 (0x0210)
#define TEST_CTRL_DATA_MEM2_CS_B (0x0214)
#define TEST_CTRL_DATA_MEM2_IO3 (0x0218)
#define TEST_CTRL_DATA_MEM2_IO2 (0x021C)
#define TEST_CTRL_DATA_MEM2_IO1 (0x0220)
#define TEST_CTRL_DATA_MEM2_IO0 (0x0224)
// QSPI I/F(FRAM)
#define TEST_CTRL_FRAM1_CS_B (0x0300)
#define TEST_CTRL_FRAM1_IO3 (0x0304)
#define TEST_CTRL_FRAM1_IO2 (0x0308)
#define TEST_CTRL_FRAM1_IO1 (0x030C)
#define TEST_CTRL_FRAM1_IO0 (0x0310)
#define TEST_CTRL_FRAM2_CS_B (0x0314)
#define TEST_CTRL_FRAM2_IO3 (0x0318)
#define TEST_CTRL_FRAM2_IO2 (0x031C)
#define TEST_CTRL_FRAM2_IO1 (0x0320)
#define TEST_CTRL_FRAM2_IO0 (0x0324)
// EXT I2C I/F
#define TEST_CTRL_I2C_EXT_SCL (0x0600)
#define TEST_CTRL_I2C_EXT_SDA (0x0604)
// TRCH I/F
#define TEST_CTRL_TRCH_FPGA_WATCHDOG (0x0704)
#define TEST_CTRL_TRCH_FPGA_PWR_CYCLE_REQ (0x0708)
#define TEST_CTRL_TRCH_FPGA_RESERVE (0x070C)
#define TEST_CTRL_TRCH_FPGA_BOOT0 (0x0720)
#define TEST_CTRL_TRCH_FPGA_BOOT1 (0x0724)
// define ULPI I/F
#define TEST_CTRL_ULPI_RESET_B (0x0804)
#define TEST_CTRL_ULPI_CS (0x0808)
// User IO1 I/F
#define TEST_CTRL_UIO1_00 (0x0C00)
#define TEST_CTRL_UIO1_01 (0x0C04)
#define TEST_CTRL_UIO1_02 (0x0C08)
#define TEST_CTRL_UIO1_03 (0x0C0C)
#define TEST_CTRL_UIO1_04 (0x0C10)
#define TEST_CTRL_UIO1_05 (0x0C14)
#define TEST_CTRL_UIO1_06 (0x0C18)
#define TEST_CTRL_UIO1_07 (0x0C1C)
#define TEST_CTRL_UIO1_08 (0x0C20)
#define TEST_CTRL_UIO1_09 (0x0C24)
#define TEST_CTRL_UIO1_10 (0x0C28)
#define TEST_CTRL_UIO1_11 (0x0C2C)
#define TEST_CTRL_UIO1_12 (0x0C30)
#define TEST_CTRL_UIO1_13 (0x0C34)
#define TEST_CTRL_UIO1_14 (0x0C38)
#define TEST_CTRL_UIO1_15 (0x0C3C)
// User IO2 I/F
#define TEST_CTRL_UIO2_00 (0x0D00)
#define TEST_CTRL_UIO2_01 (0x0D04)
#define TEST_CTRL_UIO2_02 (0x0D08)
#define TEST_CTRL_UIO2_03 (0x0D0C)
#define TEST_CTRL_UIO2_04 (0x0D10)
#define TEST_CTRL_UIO2_05 (0x0D14)
#define TEST_CTRL_UIO2_06 (0x0D18)
#define TEST_CTRL_UIO2_07 (0x0D1C)
#define TEST_CTRL_UIO2_08 (0x0D20)
#define TEST_CTRL_UIO2_09 (0x0D24)
#define TEST_CTRL_UIO2_10 (0x0D28)
#define TEST_CTRL_UIO2_11 (0x0D2C)
#define TEST_CTRL_UIO2_12 (0x0D30)
#define TEST_CTRL_UIO2_13 (0x0D34)
#define TEST_CTRL_UIO2_14 (0x0D38)
#define TEST_CTRL_UIO2_15 (0x0D3C)
// User IO4 I/F
#define TEST_CTRL_UIO4_06 (0x0E00)
#define TEST_CTRL_UIO4_07 (0x0E04)
#define TEST_CTRL_UIO4_08 (0x0E08)
#define TEST_CTRL_UIO4_09 (0x0E0C)
#define TEST_CTRL_UIO4_10 (0x0E10)
#define TEST_CTRL_UIO4_11 (0x0E14)
// Reserve (unused)
#define TEST_CTRL_RSV_00 (0x0F00)
#define TEST_CTRL_RSV_01 (0x0F04)
#define TEST_CTRL_RSV_02 (0x0F08)
#define TEST_CTRL_RSV_03 (0x0F0C)
#define TEST_CTRL_RSV_04 (0x0F10)
#define TEST_CTRL_RSV_05 (0x0F14)
#define TEST_CTRL_RSV_06 (0x0F18)
#define TEST_CTRL_RSV_07 (0x0F1C)
#define TEST_CTRL_RSV_08 (0x0F20)
#define TEST_CTRL_RSV_09 (0x0F24)
#define TEST_CTRL_RSV_10 (0x0F28)
#define TEST_CTRL_RSV_11 (0x0F2C)
#define TEST_CTRL_RSV_12 (0x0F30)
#define TEST_CTRL_RSV_13 (0x0F34)
#define TEST_CTRL_RSV_14 (0x0F38)
#define TEST_CTRL_RSV_15 (0x0F3C)

/* Monitor Registers */
#define TEST_MONI_SRAM        (0x0078)
#define TEST_MONI_SRAM_IO     (0x007C)
#define TEST_MONI_SRAM_ERR    (0x0080)
#define TEST_MONI_CFG_MEM     (0x0114)
#define TEST_MONI_DATA_MEM    (0x0228)
#define TEST_MONI_FRAM        (0x0328)
#define TEST_MONI_SYSCLK      (0x0400)
#define TEST_MONI_CV_TEMP     (0x0500)
#define TEST_MONI_I2C_EXT     (0x0608)
#define TEST_MONI_FPGA_BOOT   (0x0700)
#define TEST_MONI_TRCH        (0x0710)
#define TEST_MONI_ULPI_CLOCK  (0x0800)
#define TEST_MONI_ULPI_IF     (0x080C)
#define TEST_MONI_FPGA_CFG    (0x0B00)
#define TEST_MONI_USER_IO1    (0x0C40)
#define TEST_MONI_USER_IO2    (0x0D40)
#define TEST_MONI_USER_IO4    (0x0E18)
#define TEST_MONI_FPGA_RESERVE (0x0F40)

/* Monitor bit position */
#define MONI_BIT_SRAM_A19 (29)
#define MONI_BIT_SRAM_A18 (28)
#define MONI_BIT_SRAM_A17 (27)
#define MONI_BIT_SRAM_A16 (26)
#define MONI_BIT_SRAM_A15 (25)
#define MONI_BIT_SRAM_A14 (24)
#define MONI_BIT_SRAM_A13 (23)
#define MONI_BIT_SRAM_A12 (22)
#define MONI_BIT_SRAM_A11 (21)
#define MONI_BIT_SRAM_A10 (20)
#define MONI_BIT_SRAM_A9 (19)
#define MONI_BIT_SRAM_A8 (18)
#define MONI_BIT_SRAM_A7 (17)
#define MONI_BIT_SRAM_A6 (16)
#define MONI_BIT_SRAM_A5 (15)
#define MONI_BIT_SRAM_A4 (14)
#define MONI_BIT_SRAM_A3 (13)
#define MONI_BIT_SRAM_A2 (12)
#define MONI_BIT_SRAM_A1 (11)
#define MONI_BIT_SRAM_A0 (10)
#define MONI_BIT_SRAM1_CE_B (9)
#define MONI_BIT_SRAM1_OE_B (8)
#define MONI_BIT_SRAM1_WE_B (7)
#define MONI_BIT_SRAM1_BHE_B (6)
#define MONI_BIT_SRAM1_BLE_B (5)
#define MONI_BIT_SRAM2_CE_B (4)
#define MONI_BIT_SRAM2_OE_B (3)
#define MONI_BIT_SRAM2_WE_B (2)
#define MONI_BIT_SRAM2_BHE_B (1)
#define MONI_BIT_SRAM2_BLE_B (0)

// SRAM ECC error
#define MONI_BIT_SRAM1_ERR   (0)
#define MONI_BIT_SRAM2_ERR   (1)

// SRAM IO
#define MONI_BIT_SRAM1_ERR   (0)
#define MONI_BIT_SRAM2_ERR   (1)

// QSPI I/F(CFG)
#define MONI_BIT_CFG_MEM_CS_B (4)
#define MONI_BIT_CFG_MEM_IO3 (3)
#define MONI_BIT_CFG_MEM_IO2 (2)
#define MONI_BIT_CFG_MEM_IO1 (1)
#define MONI_BIT_CFG_MEM_IO0 (0)

// QSPI I/F(DATA)
#define MONI_BIT_DATA_MEM1_CS_B (9)
#define MONI_BIT_DATA_MEM1_IO3 (8)
#define MONI_BIT_DATA_MEM1_IO2 (7)
#define MONI_BIT_DATA_MEM1_IO1 (6)
#define MONI_BIT_DATA_MEM1_IO0 (5)
#define MONI_BIT_DATA_MEM2_CS_B (4)
#define MONI_BIT_DATA_MEM2_IO3 (3)
#define MONI_BIT_DATA_MEM2_IO2 (2)
#define MONI_BIT_DATA_MEM2_IO1 (1)
#define MONI_BIT_DATA_MEM2_IO0 (0)
// QSPI I/F(FRAM)
#define MONI_BIT_FRAM1_CS_B (9)
#define MONI_BIT_FRAM1_IO3 (8)
#define MONI_BIT_FRAM1_IO2 (7)
#define MONI_BIT_FRAM1_IO1 (6)
#define MONI_BIT_FRAM1_IO0 (5)
#define MONI_BIT_FRAM2_CS_B (4)
#define MONI_BIT_FRAM2_IO3 (3)
#define MONI_BIT_FRAM2_IO2 (2)
#define MONI_BIT_FRAM2_IO1 (1)
#define MONI_BIT_FRAM2_IO0 (0)
// SYSTEM CLOCK
#define MONI_BIT_SYSCLK2   (1)
#define MONI_BIT_SYSCLK1   (0)
// CV / TEMP (I2C INT I/F)
#define MONI_BIT_TEMP_ALART  (2)
#define MONI_BIT_CV_WARN     (1)
#define MONI_BIT_CV_CRIT     (0)
// EXT I2C I/F
#define MONI_BIT_I2C_EXT_SCL (1)
#define MONI_BIT_I2C_EXT_SDA (0)
// TRCH (FPGA BOOT) * not subject to bridge testing? *
#define MONI_BIT_FPGA_BOOT_32 (31)
#define MONI_BIT_FPGA_BOOT_31 (30)
#define MONI_BIT_FPGA_BOOT_30 (29)
#define MONI_BIT_FPGA_BOOT_29 (28)
#define MONI_BIT_FPGA_BOOT_28 (27)
#define MONI_BIT_FPGA_BOOT_27 (26)
#define MONI_BIT_FPGA_BOOT_26 (25)
#define MONI_BIT_FPGA_BOOT_25 (24)
#define MONI_BIT_FPGA_BOOT_24 (23)
#define MONI_BIT_FPGA_BOOT_23 (22)
#define MONI_BIT_FPGA_BOOT_22 (21)
#define MONI_BIT_FPGA_BOOT_21 (20)
#define MONI_BIT_FPGA_BOOT_20 (19)
#define MONI_BIT_FPGA_BOOT_19 (18)
#define MONI_BIT_FPGA_BOOT_18 (17)
#define MONI_BIT_FPGA_BOOT_17 (16)
#define MONI_BIT_FPGA_BOOT_16 (15)
#define MONI_BIT_FPGA_BOOT_15 (14)
#define MONI_BIT_FPGA_BOOT_14 (13)
#define MONI_BIT_FPGA_BOOT_13 (12)
#define MONI_BIT_FPGA_BOOT_12 (11)
#define MONI_BIT_FPGA_BOOT_11 (10)
#define MONI_BIT_FPGA_BOOT_10 (9)
#define MONI_BIT_FPGA_BOOT_9  (8)
#define MONI_BIT_FPGA_BOOT_8  (7)
#define MONI_BIT_FPGA_BOOT_7  (6)
#define MONI_BIT_FPGA_BOOT_6  (5)
#define MONI_BIT_FPGA_BOOT_5  (4)
#define MONI_BIT_FPGA_BOOT_4  (3)
#define MONI_BIT_FPGA_BOOT_3  (2)
#define MONI_BIT_FPGA_BOOT_2  (1)
#define MONI_BIT_FPGA_BOOT_1  (0)
// TRCH I/F
#define MONI_BIT_TRCH_FPGA_BOOT1 (4)
#define MONI_BIT_TRCH_FPGA_BOOT0 (3)
#define MONI_BIT_TRCH_FPGA_WATCHDOG (2)
#define MONI_BIT_TRCH_FPGA_PWR_CYCLE_REQ (1)
#define MONI_BIT_TRCH_FPGA_RESERVE (0)
// ULPI Clock
#define MONI_BIT_ULPI_CLOCK (0)
// ULPI I/F
#define MONI_BIT_ULPI_RESET_B (1)
#define MONI_BIT_ULPI_CS (0)
// FPGA Config I/F
#define MONI_BIT_FPGA_CFG (0)
// User IO1 I/F
#define MONI_BIT_UIO1_00 (0)
#define MONI_BIT_UIO1_01 (1)
#define MONI_BIT_UIO1_02 (2)
#define MONI_BIT_UIO1_03 (3)
#define MONI_BIT_UIO1_04 (4)
#define MONI_BIT_UIO1_05 (5)
#define MONI_BIT_UIO1_06 (6)
#define MONI_BIT_UIO1_07 (7)
#define MONI_BIT_UIO1_08 (8)
#define MONI_BIT_UIO1_09 (9)
#define MONI_BIT_UIO1_10 (10)
#define MONI_BIT_UIO1_11 (11)
#define MONI_BIT_UIO1_12 (12)
#define MONI_BIT_UIO1_13 (13)
#define MONI_BIT_UIO1_14 (14)
#define MONI_BIT_UIO1_15 (15)
// User IO2 I/F
#define MONI_BIT_UIO2_00 (0)
#define MONI_BIT_UIO2_01 (1)
#define MONI_BIT_UIO2_02 (2)
#define MONI_BIT_UIO2_03 (3)
#define MONI_BIT_UIO2_04 (4)
#define MONI_BIT_UIO2_05 (5)
#define MONI_BIT_UIO2_06 (6)
#define MONI_BIT_UIO2_07 (7)
#define MONI_BIT_UIO2_08 (8)
#define MONI_BIT_UIO2_09 (9)
#define MONI_BIT_UIO2_10 (10)
#define MONI_BIT_UIO2_11 (11)
#define MONI_BIT_UIO2_12 (12)
#define MONI_BIT_UIO2_13 (13)
#define MONI_BIT_UIO2_14 (14)
#define MONI_BIT_UIO2_15 (15)
// User IO4 I/F
#define MONI_BIT_UIO4_06 (0)
#define MONI_BIT_UIO4_07 (1)
#define MONI_BIT_UIO4_08 (2)
#define MONI_BIT_UIO4_09 (3)
#define MONI_BIT_UIO4_10 (4)
#define MONI_BIT_UIO4_11 (5)
// Reserve (unused)
#define MONI_BIT_RSV_00 (0)
#define MONI_BIT_RSV_01 (1)
#define MONI_BIT_RSV_02 (2)
#define MONI_BIT_RSV_03 (3)
#define MONI_BIT_RSV_04 (4)
#define MONI_BIT_RSV_05 (5)
#define MONI_BIT_RSV_06 (6)
#define MONI_BIT_RSV_07 (7)
#define MONI_BIT_RSV_08 (8)
#define MONI_BIT_RSV_09 (9)
#define MONI_BIT_RSV_10 (10)
#define MONI_BIT_RSV_11 (11)
#define MONI_BIT_RSV_12 (12)
#define MONI_BIT_RSV_13 (13)
#define MONI_BIT_RSV_14 (14)
#define MONI_BIT_RSV_15 (15)

/*
 * Data structure for loop back test
 *
 * in_ctrl_reg: control register to set as INPUT
 * out_ctrl_reg: control register to change HI/LOW
 * in_moni_reg: monitor register to read INPUT status
 * moni_bitpos: bit position in monitor register
 */
struct loopback_test_regs
{
	uint32_t in_ctrl_reg;
	uint32_t out_ctrl_reg;
	uint32_t in_moni_reg;
	uint8_t moni_bitpos;
};

void set_test_gpio_mode(uint32_t addr, uint32_t mode);
uint32_t get_test_gpio_mode(uint32_t addr);
uint32_t get_test_moni_status(uint32_t addr, uint8_t bitpos);
bool test_moni_status_high(uint32_t addr, uint8_t bitpos);
bool test_moni_status_low(uint32_t addr, uint8_t bitpos);
bool check_test_moni_status(uint32_t addr, uint8_t bitpos, uint32_t exp);
uint32_t test_paired_pins_connection(struct loopback_test_regs *target);


static inline void set_pin_input(uint32_t offset)
{
	sys_write32(TEST_GPIO_IN, TEST_REG_ADDR(offset));
}

static inline void set_pin_output_low(uint32_t offset)
{
	sys_write32(TEST_GPIO_OUT_LOW, TEST_REG_ADDR(offset));
}

static inline void set_pin_output_high(uint32_t offset)
{
	sys_write32(TEST_GPIO_OUT_HIGH, TEST_REG_ADDR(offset));
}

static inline uint32_t get_pin(uint32_t offset, uint8_t bit)
{
	return !!(sys_read32(TEST_REG_ADDR(offset)) & (1 << bit));
}

#endif /* SCOBCA1_FPGA_TEST_REGISTER_H_ */
