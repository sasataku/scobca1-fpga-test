#pragma once

void expect(char *name, bool expect, bool val);
uint8_t test_trch_r269(void);
uint8_t test_temp(void);
uint8_t test_i2c_bridges(void);
void prepare_can_test(void);
uint8_t test_can(void);
void test_ok(bool ok);
void test_send(uint8_t val);
