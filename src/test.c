#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "trch.h"
#include "fpga.h"
#include "tmp175.h"
#include "spi.h"
#include "mcp2517fd.h"
#include "utils.h"

uint8_t expect(char *name, bool expect, bool val)
{
        uint8_t ret = 0;
        printf("%s %s\n", name, expect == val ? "ok" : "ng");
        if (expect != val)
                ret = 1;
        return ret;
}

void fail(char *name)
{
        printf("%s ng", name);
}

uint8_t test_trch_r269(void)
{
        bool val = PORTCbits.RC2;

        return expect("r269", true, val);
}

uint8_t test_temp(void)
{
        struct tmp175_data t;
        int ret;
        uint8_t error = 0;

        t.master = 0;
        t.addr = 0x4c;

        ret = tmp175_data_read(&t, FPGA_STATE_POWER_OFF);
        if (ret < 0) {
                fail("temp");
                error = 1;
        }
        else {
                uint16_t data = ((t.data[1] | t.data[0]) >> 4);
                printf("temp %d ok\n", data);
        }

        return error;
}

/*
 * Target pins:           PIC    38 39 40 41
 * Adjacent pins:         PIC 37 ........... 42
 * Adjacent pin functions     spi            spi
 *
 * Disable IC18 by negating nCS, which is connected via SPICAN_CS_B on
 * RC1.
 *
 * I2C doesn't have CS.  It needs to be recovered by seding SCL pulses
 * after tests.
 *
 * I2C pins have pull-ups and are used as open-drains.  You shouldn't
 * drive HIGH.
 *
 * Test a target pin by drive adjacent pins LOW, make the pin HiZ by
 * settting dir IN, and read it.  If the pin is read HIGH, it's good.
 * Otherwise, has a bridge.
 */
uint8_t test_i2c_bridges()
{
        uint8_t ret = 0;

        /* Disable SPI */
        SPICAN_CS_B = PORT_DATA_HIGH;
        SPICAN_CS_B_DIR = PORT_DIR_OUT;

        /* TEST */
        /* 38 RD0 */
        PORTCbits.RC3 = PORT_DATA_LOW;
        TRISCbits.TRISC3 = PORT_DIR_OUT;
        PORTDbits.RD1 = PORT_DATA_LOW;
        TRISDbits.TRISD1 = PORT_DIR_OUT;

        TRISDbits.TRISD0 = PORT_DIR_IN;
        ret += expect("RD0", PORT_DATA_HIGH, PORTDbits.RD0);

        /* 39 RD1 */
        PORTDbits.RD0 = PORT_DATA_LOW;
        TRISDbits.TRISD0 = PORT_DIR_OUT;
        PORTDbits.RD2 = PORT_DATA_LOW;
        TRISDbits.TRISD2 = PORT_DIR_OUT;

        TRISDbits.TRISD1 = PORT_DIR_IN;
        ret += expect("RD1", PORT_DATA_HIGH, PORTDbits.RD1);

        /* 40 RD2 */
        PORTDbits.RD1 = PORT_DATA_LOW;
        TRISDbits.TRISD1 = PORT_DIR_OUT;
        PORTDbits.RD3 = PORT_DATA_LOW;
        TRISDbits.TRISD3 = PORT_DIR_OUT;

        TRISDbits.TRISD2 = PORT_DIR_IN;
        ret += expect("RD2", PORT_DATA_HIGH, PORTDbits.RD2);

        /* 41 RD3 */
        PORTDbits.RD2 = PORT_DATA_LOW;
        TRISDbits.TRISD2 = PORT_DIR_OUT;
        PORTCbits.RC4 = PORT_DATA_LOW;
        TRISCbits.TRISC4 = PORT_DIR_OUT;

        TRISDbits.TRISD3 = PORT_DIR_IN;
        ret += expect("RD3", PORT_DATA_HIGH, PORTDbits.RD3);

        /* Recover I2C and enable SPI */

        return ret;
}

static void can_dump_regs(void)
{
        printf("C1CON %08lx\n", spi_read32(C1CON));
        printf("C1NBTCFG %08lx\n", spi_read32(C1NBTCFG));

        printf("C1FIFOCON1 %08lx\n", spi_read32(C1FIFOCON1));
        printf("C1FIFOSTA1 %08lx\n", spi_read32(C1FIFOSTA1));
        printf("C1FIFOUA1  %08lx\n", spi_read32(C1FIFOUA1));

        printf("C1FIFOCON2 %08lx\n", spi_read32(C1FIFOCON2));
        printf("C1FIFOSTA2 %08lx\n", spi_read32(C1FIFOSTA2));
        printf("C1FIFOUA2  %08lx\n", spi_read32(C1FIFOUA2));
}

static void can_reset(void)
{
        uint8_t buf[2];

        spi_get();
        buf[0] = spi_trans(0);
        buf[1] = spi_trans(0);
        spi_release();
}

static void can_set_bit_timing(void)
{
        /* Setup bit rate and baud rate */
        /* we want 1 MHz baud based on 20 MHz SYSCLK.  That means the
           Time Quanta (Tq) is 50 ns and 20 Tq in one Nominal Bit
           time.  We want 80 % Sampling point.

           - Sync-Seg:   1 Tq (req.)
           - Phase Seg2: 4 Tq (20 * 0.2)

           At this point we have 15 Tq for Prop Seg and Phase Seg1.

           For now, let's go with 10:5.

           To do that, we need to set

           - BRP:    1 (to get 20 MHz; Baud Rate Prescaler)
           - TSEG1: 15 (Propagation Segment + Phase Segment 1)
           - TSEG2:  4 (Phase Segment 2)
           - SJW:    4 (Synchronization Jump Width)
         */
        spi_write8(3, C1NBTCFG + 0); /* SJW */
        spi_write8(3, C1NBTCFG + 1); /* TSEG2 */
        spi_write8(14, C1NBTCFG + 2); /* TSEG1 */
        spi_write8(0, C1NBTCFG + 3); /* BRP */
}

static void can_enable_tx_fifo(void)
{
        /* Use FIFO 1 as Transimitting FIFO */
        spi_write8(0x80, C1FIFOCON1);

        printf("C1FIFOCON1 %08lx\n", spi_read32(C1FIFOCON1));
        printf("C1FIFOSTA1 %08lx\n", spi_read32(C1FIFOSTA1));
        printf("C1CON %08lx\n", spi_read32(C1CON));
}

#define RXIE BIT(17)
#define TFNRFNIE BIT(0)

static void can_enable_rx_fifo(void)
{
        /* Use FIFO 2 as Receiving FIFO and enable Receive FIFO Not Empty Interrupt */
        spi_write8(TFNRFNIE, C1FIFOCON2);

        /* Enable Recive FIFO Interrupt */
        spi_write32(RXIE, C1INT);

        printf("C1FIFOCON2 %08lx\n", spi_read32(C1FIFOCON2));
        printf("C1FIFOSTA2 %08lx\n", spi_read32(C1FIFOSTA2));
        printf("C1CON %08lx\n", spi_read32(C1CON));
        printf("C1INT %08lx\n", spi_read32(C1INT));
        printf("C1RXIF %08lx\n", spi_read32(C1RXIF));
}

#define FLTEN0 BIT(7)

#define CAN_ID_ANALYZER 'A' /* 0x41 */
#define CAN_ID_FPGA 'F' /* 0x46 */
#define CAN_ID_TRCH 'T' /* 0x54 */

#define CAN_MSID 0x7ff
#define CAN_MIDE BIT(30)

#define UINC BIT(0)
#define TXREQ BIT(1)

#define DLC_MASK (0xf)

#define ASCII_NUL  0
#define ASCII_SOH  1
#define ASCII_STX  2
#define ASCII_ETX  3
#define ASCII_EOT  4
#define ASCII_ENQ  5
#define ASCII_ACK  6

static void spi_send(uint32_t *buf, uint8_t len, uint16_t sid)
{
        uint16_t addr;
        uint32_t data;
        uint8_t *p = (uint8_t *)&data;

        printf("C1FIFOSTA1 %08lx\n", spi_read32(C1FIFOSTA1));

        addr = spi_read16(C1FIFOUA1);
        printf("C1FIFOUA1  %04x\n", addr);

        spi_write32(sid & CAN_MSID, (uint16_t)0x400 + addr);
        spi_write32(len & DLC_MASK, (uint16_t)0x404 + addr);
        spi_write32(*buf, (uint16_t)0x408 + addr);

        printf("msg %08lx\n", spi_read32(0x400 + addr));
        printf("msg %08lx\n", spi_read32(0x404 + addr));
        printf("msg %08lx\n", spi_read32(0x408 + addr));

        printf("C1FIFOCON1 %08lx\n", spi_read32(C1FIFOCON1));
        //spi_write8(0, C1FIFOCON1 + 2);

        /* send it */
        /* UINC and TXREQ of the CiFIFOCONm register must be set at
         * the same time after appending a message. -- DS20005678E */
        spi_write8(TXREQ | UINC, C1FIFOCON1 + 1);
}

void test_ok(bool ok)
{
        uint32_t buf;
        uint8_t *data = (uint8_t*)&buf;

        data[0] = ASCII_ACK;
        data[1] = ok;
        spi_send(&buf, 2, CAN_ID_FPGA);
}

void test_send(uint8_t val)
{
        uint32_t buf;
        uint8_t *data = (uint8_t*)&buf;

        data[0] = 'p';
        data[1] = val;
        spi_send(&buf, 2, CAN_ID_FPGA);
}

void test_send_to_analyzer(uint8_t val)
{
        uint32_t buf;
        uint8_t *data = (uint8_t*)&buf;

        data[0] = 'p';
        data[1] = val;
        spi_send(&buf, 2, CAN_ID_ANALYZER);
}

static void can_setup_filter(uint16_t sid, uint16_t sid_mask)
{
        /* Disable the filter */
        spi_write8(0, C1FLTCON0);

        /* Setup filter object and mask */
        spi_write32(sid & CAN_MSID, C1FLTOBJ0);
        spi_write32(CAN_MIDE | (sid_mask & CAN_MSID), C1MASK0);

        /* Enable the filter */
        spi_write8(0x2 | FLTEN0, C1FLTCON0);

        printf("C1FLTCON0 %08lx\n", spi_read32(C1FLTCON0));
        printf("C1FLTOBJ0 %08lx\n", spi_read32(C1FLTOBJ0));
        printf("C1MASK0   %08lx\n", spi_read32(C1MASK0));
}

#define TFNRFNIF BIT(0)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint8_t spican_error;

int spi_recv(uint8_t *buf, uint8_t len)
{
        uint16_t addr;
        uint8_t stat;
        uint8_t dlc;
        int ret = -1;
        uint8_t i;
        uint32_t val;

        if (spi_read8(C1FIFOSTA2) & TFNRFNIF) {

                if (SPICAN_INT_B == 1) {
                        spican_error = 1;
                        printf("SPICAN_INT_B ng");
                }
                printf("SPICAN_INT_B %d\n", SPICAN_INT_B);
                printf("C1RXIF     %08lx\n", spi_read32(C1RXIF));
                printf("C1INT      %08lx\n", spi_read32(C1INT));
                printf("C1FIFOCON2 %08lx\n", spi_read32(C1FIFOCON2));
                printf("C1FIFOSTA2 %08lx\n", spi_read32(C1FIFOSTA2));

                stat = spi_read8(C1FIFOSTA2);
                addr = spi_read16(C1FIFOUA2);
                printf("C1FIFOSTA2 %02x C1FIFOUA2 %04x\n", stat, addr);

                printf("recv obj 0 %08lx\n", spi_read32(0x400 + addr));
                printf("recv obj 1 %08lx\n", spi_read32(0x404 + addr));
                printf("recv obj 2 %08lx\n", spi_read32(0x408 + addr));

                dlc = spi_read32(0x404 + addr) & 0xf;
                dlc = MIN(dlc, len);
                for (i = 0; i < dlc; i++) {
                        if ((i % 4) == 0)
                                val = spi_read32(0x408 + addr + i);
                        buf[i] = (val >> (8 * (i % 4))) & 0xff;
                }

                /* ack */
                spi_write8(1, C1FIFOCON2 + 1);

                ret = dlc;
        }

        return ret;
}

void prepare_can_test(void)
{
        uint8_t buf;

        spi_init();
        can_reset();
        can_dump_regs();
        can_set_bit_timing();

        printf("C1NBTCFG %08lx\n", spi_read32(C1NBTCFG));

        /* Have one and only one TX FIFO for now */
        can_enable_tx_fifo();

        /* Have one and only one TX FIFO for now */
        can_enable_rx_fifo();

        /* with RTXAT = 1: Restricted retransmission attempts,
         * CiFIFOCONm.TXAT is used. */
        spi_write8(0x19, C1CON + 2);

        /* move to CAN 2.0 mode */
        spi_write8(0x6, C1CON + 3);
        printf("C1CON %08lx\n", spi_read32(C1CON));

        can_setup_filter(CAN_ID_TRCH, CAN_MSID);
}

uint8_t test_can(void)
{
        uint8_t ret = 0;
        uint8_t counter = 0;

        /* Test for SPICAN_INT_B */
        {
                uint8_t buf;
                int r;

                if (SPICAN_INT_B == 0) {
                        ret++;
                        printf("SPICAN_INT_B ng");
                }

                /* send a frame and wait for a frame.  spi_recv() will
                 * check SPICAN_INT_B and increment spican_error if
                 * error. */
                test_send_to_analyzer(counter++);
                do {
                        r = spi_recv(&buf, 1);
                } while (r < 0);
                ret += spican_error;
        }

        /* Test for TRCH_CAN_SLEEP_EN */
        {
                uint8_t tx_error1;
                uint8_t tx_error2;
                uint8_t tx_error3;
                uint16_t error_free1;
                uint16_t error_free2;
                uint16_t error_free3;
                bool success;

                /* Disable CAN phy and send a frame.  We expect error
                 * counters go up and error free trans counter stays.
                 * After re-enable the phy, we expect error counter to
                 * stay and error-free counter goes up. */
                __delay_ms(1);
                tx_error1 = spi_read8(C1BDIAG0+1);
                error_free1 = spi_read16(C1BDIAG1);

                TRCH_CAN_SLEEP_EN = PORT_DATA_HIGH;
                __delay_ms(1);

                test_send_to_analyzer(counter++);
                __delay_ms(1);

                tx_error2 = spi_read8(C1BDIAG0+1);
                error_free2 = spi_read16(C1BDIAG1);

                TRCH_CAN_SLEEP_EN = PORT_DATA_LOW;
                __delay_ms(1);

                test_send_to_analyzer(counter++);
                __delay_ms(1);

                tx_error3 = spi_read8(C1BDIAG0+1);
                error_free3 = spi_read16(C1BDIAG1);

                printf("1 %x %x\n", tx_error1, error_free1);
                printf("2 %x %x\n", tx_error2, error_free2);
                printf("3 %x %x\n", tx_error3, error_free3);


                success = (((tx_error1 < tx_error2) && (tx_error2 == tx_error3)) &&
                           ((error_free1 == error_free2) && (error_free2 < error_free3)));

                printf("TRCH_CAN_SLEEP_EN %s\n", success ? "ok" : "ng");

                if (!success) {
                        ret++;
                }
        }

        return ret;
}
