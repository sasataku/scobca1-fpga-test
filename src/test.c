#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "trch.h"

void expect(char *name, bool expect, bool val)
{
        printf("%s %s\n", name, expect == val ? "ok" : "ng");
}

void test_trch_r269(void)
{
        bool val = PORTCbits.RC2;

        expect("r269", true, val);
}

void do_tests(void)
{
        /* TRCH */
        test_trch_r269();
}
