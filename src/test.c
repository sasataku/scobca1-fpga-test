#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "trch.h"
#include "fpga.h"
#include "tmp175.h"

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

void do_tests(void)
{
        /* TRCH */
        test_trch_r269();

        /* I2C */
        test_temp();
        test_i2c_bridges();
}
