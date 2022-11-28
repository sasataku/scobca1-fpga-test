/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/console/console.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "irq.h"
#include "watchdog.h"
#include "i2c_test.h"
#include "hrmem_test.h"
#include "qspi_common.h"
#include "qspi_norflash_test.h"
#include "qspi_fram_test.h"
#include "usb_crack_test.h"
#include "pudc_crack_test.h"
#include "sys_clock_crack_test.h"
#include "user_io_crack_test.h"
#include "sram_addr_crack_test.h"
#include "sram_byte_crack_test.h"
#include "sram_err_crack_test.h"
#include "sram_data_crack_test.h"
#include "user_io_bridge_test.h"
#include "memory_bridge_test.h"
#include "can_test.h"
#include "bhm_test.h"
#include "system_reg.h"
#include "longrun_test.h"
#include "trch_test.h"
#include "pdi.h"

enum ScTestNo {
	SC_TEST_QSPI_INIT = 1,
	SC_TEST_LONG_RUN,
	SC_TEST_HRMEM,
	SC_TEST_QSPI_CFG_MEM,
	SC_TEST_QSPI_CFG_MEM_SECTOR,
	SC_TEST_QSPI_CFG_MEM_BLOCK,
	SC_TEST_QSPI_DATA_MEM,
	SC_TEST_QSPI_DATA_MEM_SECTOR,
	SC_TEST_QSPI_DATA_MEM_BLOCK,
	SC_TEST_QSPI_FRAM,
	SC_TEST_CAN,
	SC_TEST_CAN_SEND_CMD,
	SC_TEST_BOARD_HEALTH_MONITOR,
	SC_TEST_CRACK_PUDC,
	SC_TEST_CRACK_USER_IO,
	SC_TEST_CRACK_SYS_CLOCK,
	SC_TEST_CRACK_USB,
	SC_TEST_CRACK_SRAM_ADDR,
	SC_TEST_CRACK_SRAM_BYTE,
	SC_TEST_CRACK_SRAM_ERR,
	SC_TEST_CRACK_SRAM_DATA,
	SC_TEST_BRIDGE_USER_IO,
	SC_TEST_BRIDGE_MEMORY,
	SC_TEST_TRCH,
	SC_TEST_CRACK_CAN,
	SC_TEST_CRACK_I2C_INTERNAL,
	SC_TEST_TRCH_CFG_MEM_MONI,
	SC_TEST_PDI = 99,
};

bool is_exit;

// PIC16LF877A Configuration Bit Settings
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

static void __interrupt() isr(void) {
        if (PIR1bits.TMR2IF) {
                timer2_isr();
        }
}

static void trch_init (void) {
        ADCON1 = 0x07;
        PORTA = PORTA_INIT;
        TRISA = TRISA_INIT;
        PORTB = PORTB_INIT;
        TRISB = TRISB_INIT;
        PORTC = PORTC_INIT;
        TRISC = TRISC_INIT;
        PORTD = PORTD_INIT;
        TRISD = TRISD_INIT;
        PORTE = PORTE_INIT;
        TRISE = TRISE_INIT;
}

static void fpga_program_maybe(void)
{
	info("[%d] QSPI Initialize\n", SC_TEST_QSPI_INIT);
	info("[%d] Long Run Test\n", SC_TEST_LONG_RUN);
	info("[%d] HRMEM Test\n", SC_TEST_HRMEM);
	info("[%d] QSPI Config Memory Test (only 16byte)\n", SC_TEST_QSPI_CFG_MEM);
	info("[%d] QSPI Config Memory Test (Sector)\n", SC_TEST_QSPI_CFG_MEM_SECTOR);
	info("[%d] QSPI Config Memory Test (Block)\n", SC_TEST_QSPI_CFG_MEM_BLOCK);
	info("[%d] QSPI Data Memory Test (only 16byte)\n", SC_TEST_QSPI_DATA_MEM);
	info("[%d] QSPI Data Memory Test (Sector)\n", SC_TEST_QSPI_DATA_MEM_SECTOR);
	info("[%d] QSPI Data Memory Test (Block)\n", SC_TEST_QSPI_DATA_MEM_BLOCK);
	info("[%d] QSPI FRAM Test\n", SC_TEST_QSPI_FRAM);
	info("[%d] CAN Test\n", SC_TEST_CAN);
	info("[%d] CAN send command to TRCH\n", SC_TEST_CAN_SEND_CMD);
	info("[%d] Board Health Monitor Test\n", SC_TEST_BOARD_HEALTH_MONITOR);

	/* for crack and bridge testing */
	info("[%d] PUDC crack Test\n", SC_TEST_CRACK_PUDC);
	info("[%d] User IO crack Test\n", SC_TEST_CRACK_USER_IO);
	info("[%d] System Clock crack Test\n", SC_TEST_CRACK_SYS_CLOCK);
	info("[%d] USB crack Test\n", SC_TEST_CRACK_USB);
	info("[%d] SRAM addr crack Test\n", SC_TEST_CRACK_SRAM_ADDR);
	info("[%d] SRAM byte crack Test\n", SC_TEST_CRACK_SRAM_BYTE);
	info("[%d] SRAM err crack Test\n", SC_TEST_CRACK_SRAM_ERR);
	info("[%d] SRAM data crack Test\n", SC_TEST_CRACK_SRAM_DATA);
	info("[%d] User IO Bridge Test\n", SC_TEST_BRIDGE_USER_IO);
	info("[%d] Memory Bridge Test\n", SC_TEST_BRIDGE_MEMORY);

	info("[%d] Shared Pin Test\n", SC_TEST_TRCH);
	info("[%d] CAN crack Test\n", SC_TEST_CRACK_CAN);
	info("[%d] Internal I2C crack Test\n", SC_TEST_CRACK_I2C_INTERNAL);
	info("[%d] Config Memory TRCH_CFG_MEM_MONI Test\n", SC_TEST_TRCH_CFG_MEM_MONI);
	info("[%d] Pre Delivery Inspection\n", SC_TEST_PDI);
}

static void print_ids(void)
{
	info("\n");
	info("* System Register IP Version : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_VER));
	info("* Build Information          : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_BUILDINFO));
	info("* Device DNA 1               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA1));
	info("* Device DNA 2               : %08x\n", sys_read32(SCOBCA1_FPGA_SYSREG_DNA2));
	info("\n");
}

#ifndef CONFIG_AUTO_RUN_TEST_NUMBER
#define AUTO_RUN_TEST_NUMBER (-1)
#else
#define AUTO_RUN_TEST_NUMBER (CONFIG_AUTO_RUN_TEST_NUMBER)
#endif

void main(void)
{
	char *s;
	uint32_t test_no;

	start_kick_wdt_thread();
	irq_init();
	console_getline_init();

	info("This is the FPGA test program for SC-OBC-A1\n");
	print_ids();
	info("Please input `h` to show the test program menu\n");
	info("\n");

	while (true) {

		if (IS_ENABLED(CONFIG_AUTO_RUN)) {
			test_no = AUTO_RUN_TEST_NUMBER;
		}
		else {
			info("> ");

			s = console_getline();
			if (strcmp(s, "h") == 0) {
				print_menu();
				continue;
			} else if (strcmp(s, "q") == 0) {
				is_exit = true;
				continue;
			}

			test_no = strtol(s, NULL, 10);
		}

		switch (test_no) {
		case SC_TEST_QSPI_INIT:
			qspi_init(test_no);
			break;
		case SC_TEST_LONG_RUN:
			longrun_test(test_no);
			break;
		case SC_TEST_HRMEM:
			hrmem_test(test_no);
			break;
		case SC_TEST_QSPI_CFG_MEM:
			qspi_config_memory_test(test_no);
			break;
		case SC_TEST_QSPI_CFG_MEM_SECTOR:
			qspi_config_memory_sector_test(test_no);
			break;
		case SC_TEST_QSPI_CFG_MEM_BLOCK:
			qspi_config_memory_block_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM:
			qspi_data_memory_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM_SECTOR:
			qspi_data_memory_sector_test(test_no);
			break;
		case SC_TEST_QSPI_DATA_MEM_BLOCK:
			qspi_data_memory_block_test(test_no);
			break;
		case SC_TEST_QSPI_FRAM:
			qspi_fram_test(test_no);
			break;
		case SC_TEST_CAN:
			can_test(test_no);
			break;
		case SC_TEST_CAN_SEND_CMD:
			can_send_cmd(test_no);
			break;
		case SC_TEST_BOARD_HEALTH_MONITOR:
			bhm_test(test_no);
			break;
		case SC_TEST_BRIDGE_USER_IO:
			user_io_bridge_test(test_no);
			break;
		case SC_TEST_BRIDGE_MEMORY:
			memory_bridge_test(test_no);
			break;
		case SC_TEST_CRACK_USB:
			usb_crack_test(test_no);
			break;
		case SC_TEST_CRACK_PUDC:
			pudc_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SYS_CLOCK:
			sys_clock_crack_test(test_no);
			break;
		case SC_TEST_CRACK_USER_IO:
			user_io_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SRAM_ADDR:
			sram_addr_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SRAM_BYTE:
			sram_byte_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SRAM_ERR:
			sram_err_crack_test(test_no);
			break;
		case SC_TEST_CRACK_SRAM_DATA:
			sram_data_crack_test(test_no);
			break;
		case SC_TEST_TRCH:
			trch_test();
			break;
		case SC_TEST_CRACK_CAN:
			can_crack_test(test_no);
			break;
		case SC_TEST_CRACK_I2C_INTERNAL:
			i2c_internal_crack_test(test_no);
			break;
		case SC_TEST_TRCH_CFG_MEM_MONI:
			qspi_config_memory_trch_moni_test(test_no);
			break;
		case SC_TEST_PDI:
			start_pdi(test_no);
			print_ids();
			/* Exit after Pre Delivery Inspection */
			return;
		default:
			break;
		}
	}
        return v;
}

/* Port Get */
uint8_t get_port_a(void) { return PORTA; }
uint8_t get_port_b(void) { return PORTB; }
uint8_t get_port_c(void) { return PORTC; }
uint8_t get_port_d(void) { return PORTD; }
uint8_t get_port_e(void) { return PORTE; }

/* Port Set */
uint8_t set_port_a(uint8_t val) { PORTA = val; return PORTA; }
uint8_t set_port_b(uint8_t val) { PORTB = val; return PORTB; }
uint8_t set_port_c(uint8_t val) { PORTC = val; return PORTC; }
uint8_t set_port_d(uint8_t val) { PORTD = val; return PORTD; }
uint8_t set_port_e(uint8_t val) { PORTE = val; return PORTE; }

/* TRIS Get */
uint8_t get_tris_a(void) { return TRISA; }
uint8_t get_tris_b(void) { return TRISB; }
uint8_t get_tris_c(void) { return TRISC; }
uint8_t get_tris_d(void) { return TRISD; }
uint8_t get_tris_e(void) { return TRISE; }

/* TRIS Set */
uint8_t set_tris_a(uint8_t val) { TRISA = val; return TRISA; }
uint8_t set_tris_b(uint8_t val) { TRISB = val; return TRISB; }
uint8_t set_tris_c(uint8_t val) { TRISC = val; return TRISC; }
uint8_t set_tris_d(uint8_t val) { TRISD = val; return TRISD; }
uint8_t set_tris_e(uint8_t val) { TRISE = val; return TRISE; }

int spi_recv(uint8_t *buf, uint8_t len);
void test_ok(bool ok);
void test_send(uint8_t val);

void do_active(void)
{
        int r;
        uint8_t cmd[2];
        bool ok;
        uint8_t val;

        r = spi_recv(cmd, 2);
        if (r > 0) {
                if (r == 1)
                        printf("Got cmd 0x%x\n", cmd[0]);
                if (r == 2)
                        printf("Got cmd 0x%x arg 0x%x\n", cmd[0], cmd[1]);
                switch (cmd[0]) {
                case 0:
                        ok = do_cmd1();
                        test_ok(ok);
                        break;
                case 'a': val = get_port_a(); test_send(val); break;
                case 'b': val = get_port_b(); test_send(val); break;
                case 'c': val = get_port_c(); test_send(val); break;
                case 'd': val = get_port_d(); test_send(val); break;
                case 'e': val = get_port_e(); test_send(val); break;

                case 'A': val = set_port_a(cmd[1]); test_send(val); break;
                case 'B': val = set_port_b(cmd[1]); test_send(val); break;
                case 'C': val = set_port_c(cmd[1]); test_send(val); break;
                case 'D': val = set_port_d(cmd[1]); test_send(val); break;
                case 'E': val = set_port_e(cmd[1]); test_send(val); break;

                case 't': val = get_tris_a(); test_send(val); break;
                case 'u': val = get_tris_b(); test_send(val); break;
                case 'v': val = get_tris_c(); test_send(val); break;
                case 'w': val = get_tris_d(); test_send(val); break;
                case 'x': val = get_tris_e(); test_send(val); break;

                case 'T': val = set_tris_a(cmd[1]); test_send(val); break;
                case 'U': val = set_tris_b(cmd[1]); test_send(val); break;
                case 'V': val = set_tris_c(cmd[1]); test_send(val); break;
                case 'W': val = set_tris_d(cmd[1]); test_send(val); break;
                case 'X': val = set_tris_e(cmd[1]); test_send(val); break;

                default:
                        test_ok(false);
                        break;
                }
        }
}

#ifndef CONFIG_FPGAPWR_HOLD_SEC
#define CONFIG_FPGAPWR_HOLD_SEC (3)
#endif
#define FPGAPWR_HOLD_TIME (SEC_TO_TICKS(CONFIG_FPGAPWR_HOLD_SEC))

enum FpgaGoal hold_fpgapwr_en(void)
{
        enum FpgaGoal ret = FPGA_SHUTDOWN;
        static bool first_time = true;
        static uint16_t first_time_tick;
        uint16_t current_tick;

        current_tick = (uint16_t)timer_get_ticks();

        if (first_time) {
                first_time = false;
                first_time_tick = current_tick;
        }
        else {
                if ((current_tick - first_time_tick) >= FPGAPWR_HOLD_TIME) {
                        first_time = true;
                        ret = FPGA_ACTIVATE;
                }
        }

        return ret;
}

void main (void)
{
        enum FpgaState fpga_state;
        enum FpgaGoal activate_fpga = FPGA_SHUTDOWN;
        int config_memory = 0;
        int boot_mode = FPGA_BOOT_48MHZ;

        // Initialize trch-firmware
        trch_init();
        fpga_program_maybe();
        fpga_state = fpga_init();

        usart_init();
        timer2_init();
        timer2_ctrl(1);
        interrupt_enable();

        /*
         * Space Cubics OBC TRCH-Firmware Main
         */
        SRS3_UART_DE = 1;
        SRS3_UART_RE_B = 0;
        __delay_ms(100);
        printf("SC OBC TRCH-FW for Quality Inspection\n");

        /* Tests */
        /*  - TRCH */
        test_trch_r269();
        /*  - I2C */
        test_temp();
        test_i2c_bridges();
        /*  - CAN */
        prepare_can_test();
        test_can();

        while (1) {
                fpga_state = fpga_state_control(activate_fpga, config_memory, boot_mode);
                switch(fpga_state) {
                case FPGA_STATE_POWER_DOWN:
                        puts("Down");
                        break;

                case FPGA_STATE_POWER_OFF:
                        puts("Off");
                        activate_fpga = hold_fpgapwr_en();
                        __delay_ms(100);
                        break;

                case FPGA_STATE_POWER_UP:
                        puts("Up");
                        break;

                case FPGA_STATE_READY:
                        puts("Ready");
                        break;

                case FPGA_STATE_CONFIG:
                        puts("Config");
                        __delay_ms(100);
                        break;

                case FPGA_STATE_ACTIVE:
                        printf("Active %02x %02x %02x %02x %02x\n", PORTA, PORTB, PORTC, PORTD, PORTE);
                        do_active();
                        __delay_ms(500);
                        break;

                case FPGA_STATE_ERROR:
                        puts("Error");
                        activate_fpga = FPGA_SHUTDOWN;
                        /* activate_fpga = FPGA_RECONFIGURE; */
                        config_memory = !config_memory;
                        break;

                default:
                        break;
                }
        }
        return;
}
