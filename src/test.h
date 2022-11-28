#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ASCII_NUL  0
#define ASCII_SOH  1
#define ASCII_STX  2
#define ASCII_ETX  3
#define ASCII_EOT  4
#define ASCII_ENQ  5
#define ASCII_ACK  6

uint8_t test_trch_r269(void);
uint8_t test_temp(void);
uint8_t test_i2c_bridges(void);
void prepare_can_test(void);
uint8_t test_can(void);
void test_ok(bool ok);
void test_send(uint8_t val);
