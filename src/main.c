/*
 * Space Cubics OBC TRCH Software
 *
 * (C) Copyright 2021-2022 Space Cubics, LLC
 *
 */

#include <xc.h>
#include <pic.h>
#include <stdio.h>

#include "trch.h"
#include "utils.h"
#include "fpga.h"
#include "spi.h"
#include "i2c.h"
#include "tmp175.h"
#include "ina3221.h"
#include "usart.h"
#include "timer.h"
#include "interrupt.h"

#include "test.h"

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
        if (PIR1bits.RCIF) {
                usart_receive_msg_isr();
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
        if (IS_ENABLED(CONFIG_FPGA_PROGRAM_MODE)) {
                if (!FPGAPROG_MODE_B) {
                        FPGAPWR_EN = 1;
                        while (1) {
                                if (FPGA_CFG_MEM_SEL) {
                                        TRCH_CFG_MEM_SEL = 1;
                                }
                        }
                }
        }
}

bool do_cmd1(void)
{
        static bool v = true;

        v = !v;

        return v;
}

/* Port Get */
uint8_t get_port_a(void) { return PORTA; }
uint8_t get_port_b(void) { return PORTB; }
uint8_t get_port_c(void) { return PORTC; }
uint8_t get_port_d(void) { return PORTD; }
uint8_t get_port_e(void) { return PORTE; }

/* Port Set */
void set_port_a(uint8_t val) { /* PORTA = val; */ }
void set_port_b(uint8_t val) { /* PORTB = val; */ }
void set_port_c(uint8_t val) { /* PORTC = val; */ }
void set_port_d(uint8_t val) { /* PORTD = val; */ }
void set_port_e(uint8_t val) { /* PORTE = val; */ }

/* TRIS Get */
uint8_t get_tris_a(void) { return TRISA; }
uint8_t get_tris_b(void) { return TRISB; }
uint8_t get_tris_c(void) { return TRISC; }
uint8_t get_tris_d(void) { return TRISD; }
uint8_t get_tris_e(void) { return TRISE; }

/* TRIS Set */
void set_tris_a(uint8_t val) { /* TRISA = val; */ }
void set_tris_b(uint8_t val) { /* TRISB = val; */ }
void set_tris_c(uint8_t val) { /* TRISC = val; */ }
void set_tris_d(uint8_t val) { /* TRISD = val; */ }
void set_tris_e(uint8_t val) { /* TRISE = val; */ }

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
                        printf("Got cmd %d 0x%x\n", cmd[0], cmd[0]);
                if (r == 2)
                        printf("Got cmd %d 0x%x arg %d 0x%x\n",
                               cmd[0], cmd[0], cmd[1], cmd[1]);
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

                case 'A': set_port_a(cmd[1]); test_ok(true); break;
                case 'B': set_port_b(cmd[1]); test_ok(true); break;
                case 'C': set_port_c(cmd[1]); test_ok(true); break;
                case 'D': set_port_d(cmd[1]); test_ok(true); break;
                case 'E': set_port_e(cmd[1]); test_ok(true); break;

                case 't': val = get_tris_a(); test_send(val); break;
                case 'u': val = get_tris_b(); test_send(val); break;
                case 'v': val = get_tris_c(); test_send(val); break;
                case 'w': val = get_tris_d(); test_send(val); break;
                case 'x': val = get_tris_e(); test_send(val); break;

                case 'T': set_tris_a(cmd[1]); test_ok(true); break;
                case 'U': set_tris_b(cmd[1]); test_ok(true); break;
                case 'V': set_tris_c(cmd[1]); test_ok(true); break;
                case 'W': set_tris_d(cmd[1]); test_ok(true); break;
                case 'X': set_tris_e(cmd[1]); test_ok(true); break;

                default:
                        test_ok(false);
                        break;
                }
        }
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
        printf("SC OBC TRCH-FW for Quality Inspection\n");

        do_tests();

        while (1) {
                if (FPGA_PWR_CYCLE_REQ) {
                        activate_fpga = FPGA_SHUTDOWN;
                }

                fpga_state = fpga_state_control(activate_fpga, config_memory, boot_mode);
                switch(fpga_state) {
                case FPGA_STATE_POWER_DOWN:
                        puts("Down");
                        break;

                case FPGA_STATE_POWER_OFF:
                        puts("Off");
                        activate_fpga = FPGA_ACTIVATE;
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
                        puts("Active");
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
