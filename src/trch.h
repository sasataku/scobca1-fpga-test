/*
 * Copyright (c) 2022 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

static inline uint8_t set_out (uint8_t val, uint8_t bit) { return val & ~(1 << bit); }
static inline uint8_t set_in  (uint8_t val, uint8_t bit) { return val |  (1 << bit); }
static inline uint8_t set_low (uint8_t val, uint8_t bit) { return val & ~(1 << bit); }
static inline uint8_t set_high(uint8_t val, uint8_t bit) { return val |  (1 << bit); }
static inline uint8_t get_bit (uint8_t val, uint8_t bit) { return val &  (1 << bit); }

static inline uint8_t trch_get_porta(void) { return can_send_cmd_to_trch('a', 0, false); }
static inline uint8_t trch_get_portb(void) { return can_send_cmd_to_trch('b', 0, false); }
static inline uint8_t trch_get_portc(void) { return can_send_cmd_to_trch('c', 0, false); }
static inline uint8_t trch_get_portd(void) { return can_send_cmd_to_trch('d', 0, false); }
static inline uint8_t trch_get_porte(void) { return can_send_cmd_to_trch('e', 0, false); }

static inline uint8_t trch_get_trisa(void) { return can_send_cmd_to_trch('t', 0, false); }
static inline uint8_t trch_get_trisb(void) { return can_send_cmd_to_trch('u', 0, false); }
static inline uint8_t trch_get_trisc(void) { return can_send_cmd_to_trch('v', 0, false); }
static inline uint8_t trch_get_trisd(void) { return can_send_cmd_to_trch('w', 0, false); }
static inline uint8_t trch_get_trise(void) { return can_send_cmd_to_trch('x', 0, false); }

static inline uint8_t trch_set_porta(uint8_t val) { return can_send_cmd_to_trch('A', val, true); }
static inline uint8_t trch_set_portb(uint8_t val) { return can_send_cmd_to_trch('B', val, true); }
static inline uint8_t trch_set_portc(uint8_t val) { return can_send_cmd_to_trch('C', val, true); }
static inline uint8_t trch_set_portd(uint8_t val) { return can_send_cmd_to_trch('D', val, true); }
static inline uint8_t trch_set_porte(uint8_t val) { return can_send_cmd_to_trch('E', val, true); }

static inline uint8_t trch_set_trisa(uint8_t val) { return can_send_cmd_to_trch('T', val, true); }
static inline uint8_t trch_set_trisb(uint8_t val) { return can_send_cmd_to_trch('U', val, true); }
static inline uint8_t trch_set_trisc(uint8_t val) { return can_send_cmd_to_trch('V', val, true); }
static inline uint8_t trch_set_trisd(uint8_t val) { return can_send_cmd_to_trch('W', val, true); }
static inline uint8_t trch_set_trise(uint8_t val) { return can_send_cmd_to_trch('X', val, true); }
