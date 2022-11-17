#include <stdbool.h>
#include <stdio.h>

#include "test.h"
#include "trch.h"


void test_r269(void)
{
        bool val = PORTCbits.RC2;

        expect("r269", true, val);
}

void do_test_trch(void)
{
        test_r269();
}
