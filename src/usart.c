/*
 * Space Cubics OBC TRCH Software
 *  USART Driver
 *
 * (C) Copyright 2021-2022
 *         Space Cubics, LLC
 *
 */

#include "usart.h"

#include <xc.h>
#include <pic.h>
#include <string.h>

#include "interrupt.h"
#include "trch.h"

/* Variables shared with main */
struct usart_tx_msg tx_msg;
static struct usart_rx_msg rx_msg;

/*
 * USART Initialize
 *  - Asynchronouse mode
 *  - 8 bit transmission
 *  - No parity bit
 *  - boardrate 9600 [bps]
 */
void usart_init (void) {
        TXSTAbits.TX9 = 0;
        TXSTAbits.SYNC = 0;
        TXSTAbits.BRGH = 1;
        TXSTAbits.TX9D = 0;
        TXSTAbits.TXEN = 1;
        RCSTAbits.SPEN = 1;
        RCSTAbits.RX9 = 0;
        RCSTAbits.ADDEN = 0;
        RCSTAbits.RX9D = 0;
        SPBRG = 0x19;
        tx_msg.active = 0;
        rx_msg.active = 0;
        rx_msg.addr = 0;
}

void putch(char ch)
{
        if (ch == '\n') {
                while (!PIR1bits.TXIF);
                TXREG = '\r';
                /* AN774: There is a delay of one instruction cycle
                 * after writing to TXREG, before TXIF gets
                 * cleared. */
                NOP();
        }
        while (!PIR1bits.TXIF);
        TXREG = ch;
}
