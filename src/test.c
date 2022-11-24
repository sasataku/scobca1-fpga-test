#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "trch.h"
#include "fpga.h"
#include "tmp175.h"
#include "spi.h"
#include "mcp2517fd.h"
#include "utils.h"

void expect(char *name, bool expect, bool val)
{
        printf("%s %s\n", name, expect == val ? "ok" : "ng");
}

void fail(char *name)
{
        printf("%s ng", name);
}

void test_trch_r269(void)
{
        bool val = PORTCbits.RC2;

        expect("r269", true, val);
}

void test_temp(void)
{
        struct tmp175_data t;
        int ret;

        t.master = 0;
        t.addr = 0x4c;

        ret = tmp175_data_read(&t, FPGA_STATE_POWER_OFF);
        if (ret < 0) {
                fail("temp");
        }
        else {
                uint16_t data = ((t.data[1] | t.data[0]) >> 4);
                printf("temp %d ok\n", data);
        }

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
void test_i2c_bridges()
{
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
        expect("RD0", PORT_DATA_HIGH, PORTDbits.RD0);

        /* 39 RD1 */
        PORTDbits.RD0 = PORT_DATA_LOW;
        TRISDbits.TRISD0 = PORT_DIR_OUT;
        PORTDbits.RD2 = PORT_DATA_LOW;
        TRISDbits.TRISD2 = PORT_DIR_OUT;

        TRISDbits.TRISD1 = PORT_DIR_IN;
        expect("RD1", PORT_DATA_HIGH, PORTDbits.RD1);

        /* 40 RD2 */
        PORTDbits.RD1 = PORT_DATA_LOW;
        TRISDbits.TRISD1 = PORT_DIR_OUT;
        PORTDbits.RD3 = PORT_DATA_LOW;
        TRISDbits.TRISD3 = PORT_DIR_OUT;

        TRISDbits.TRISD2 = PORT_DIR_IN;
        expect("RD2", PORT_DATA_HIGH, PORTDbits.RD2);

        /* 41 RD3 */
        PORTDbits.RD2 = PORT_DATA_LOW;
        TRISDbits.TRISD2 = PORT_DIR_OUT;
        PORTCbits.RC4 = PORT_DATA_LOW;
        TRISCbits.TRISC4 = PORT_DIR_OUT;

        TRISDbits.TRISD3 = PORT_DIR_IN;
        expect("RD3", PORT_DATA_HIGH, PORTDbits.RD3);

        /* Recover I2C and enable SPI */
}

static void spi_dump_regs(void)
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

static void spi_reset(void)
{
        uint8_t buf[2];

        spi_get();
        buf[0] = spi_trans(0);
        buf[1] = spi_trans(0);
        spi_release();
}

static void spi_set_bit_timing(void)
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

static void spi_enable_tx_fifo(void)
{
        /* Use FIFO 1 as Transimitting FIFO */
        spi_write8(0x80, C1FIFOCON1);

        printf("C1FIFOCON1 %08lx\n", spi_read32(C1FIFOCON1));
        printf("C1FIFOSTA1 %08lx\n", spi_read32(C1FIFOSTA1));
        printf("C1CON %08lx\n", spi_read32(C1CON));
}

static void spi_enable_rx_fifo(void)
{
        /* Use FIFO 2 as Receiving FIFO */
        spi_write8(0x00, C1FIFOCON2);

        printf("C1FIFOCON2 %08lx\n", spi_read32(C1FIFOCON2));
        printf("C1FIFOSTA2 %08lx\n", spi_read32(C1FIFOSTA2));
        printf("C1CON %08lx\n", spi_read32(C1CON));
}

#define FLTEN0 BIT(7)

#define CAN_ID_FPGA 'F' /* 0x46 */
#define CAN_ID_TRCH 'T' /* 0x54 */

#define CAN_MSID 0x7ff
#define CAN_MIDE BIT(30)

#define UINC BIT(0)
#define TXREQ BIT(1)

#define DLC_MASK (0xf)

static void spi_send(uint8_t *buf, uint8_t len, uint16_t sid)
{
        uint16_t addr;

        /*
          C1FIFOSTA1 を読み出し、FIFOに空きがあることを確認(TFNRFNIF==1)
          C1FIFOUA1 を読み出し、送信FIFOのアドレスを取得
          0x400 + アドレス + 0 にCAN IDを書き込み
          0x400 + アドレス + 4 にDLCなどを書き込み
          0x400 + アドレス + 8 以降に送信データを書き込み
          C1FIFOCON2 のTXREQとUINCを1にして書き込み(転送を開始し、FIFOを進める)
        */

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
        spi_write8(TXREQ | UINC, C1FIFOCON1 + 1);
}

void test_ok(bool ok)
{
        uint8_t buf = ok;
        spi_send(&buf, 1, CAN_ID_FPGA);
}

void test_send(uint8_t val)
{
        spi_send(&val, 1, CAN_ID_FPGA);
}

static void spi_setup_filter(uint16_t sid, uint16_t sid_mask)
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

int spi_recv(uint8_t *buf, uint8_t len)
{
        uint16_t addr;
        uint8_t stat;
        uint8_t dlc;
        int ret = -1;
        uint8_t i;
        uint32_t val;

        if (spi_read8(C1FIFOSTA2) & TFNRFNIF) {

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

static void prepare_can_test(void)
{
        uint8_t buf;

        spi_init();
        spi_reset();
        spi_dump_regs();
        spi_set_bit_timing();

        printf("C1NBTCFG %08lx\n", spi_read32(C1NBTCFG));

        /* Have one and only one TX FIFO for now */
        spi_enable_tx_fifo();

        /* Have one and only one TX FIFO for now */
        spi_enable_rx_fifo();

        /* with RTXAT = 1: Restricted retransmission attempts,
         * CiFIFOCONm.TXAT is used. */
        spi_write8(0x19, C1CON + 2);

        /* move to CAN 2.0 mode */
        spi_write8(0x6, C1CON + 3);
        printf("C1CON %08lx\n", spi_read32(C1CON));

        spi_setup_filter(CAN_ID_TRCH, CAN_MSID);
}

void do_tests(void)
{
        /* TRCH */
        test_trch_r269();

        /* I2C */
        test_temp();
        test_i2c_bridges();

        /* CAN */
        prepare_can_test();
}
