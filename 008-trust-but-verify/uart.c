/* Copyright (C) 2015 Goswin von Brederlow <goswin-v-b@web.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Uart related functions
 */

#include "uart.h"
#include "arch_info.h"
#include "gpio.h"
#include "assert.h"
#include "init_priorities.h"
#include "peripherals.h"

enum UART0_Reg {
    UART0_DR     = 0x00, // 0x??201000 data register
    UART0_RSRECR = 0x04, // 0x??201004 Receive Status Reg / Error Clear Reg
    UART0_FR     = 0x18, // 0x??201018 flag register
    // UART0_ILPR   = 0x20, // 0x??201020 disabled IrDA register
    UART0_IBRD   = 0x24, // 0x??201024 Integer Baud rate divisor
    UART0_FBRD   = 0x28, // 0x??201028 Fractional Baud rate divisor
    UART0_LCRH   = 0x2C, // 0x??20102C Line Control register
    UART0_CR     = 0x30, // 0x??201030 Control register
    UART0_IFLS   = 0x34, // 0x??201034 Interrupt FIFO Level Select Register
    UART0_IMSC   = 0x38, // 0x??201038 Interupt Mask Set Clear Register
    UART0_RIS    = 0x3C, // 0x??20103C Raw Interupt Status Register
    UART0_MIS    = 0x40, // 0x??201040 Masked Interupt Status Register
    UART0_ICR    = 0x44, // 0x??201044 Interupt Clear Register
    // UART0_DMACR  = 0x48, // 0x??201048 disabled DMA Control Register
    // UART0_ITCR   = 0x80, // 0x??201080 Test Control register
    // UART0_ITIP   = 0x84, // 0x??201084 Integration test input reg
    // UART0_ITOP   = 0x88, // 0x??201088 Integration test output reg
    // UART0_TDR    = 0x8C, // 0x??20108C Test Data reg
};

PERIPHERAL_BASE(UART0)

enum {
    // Data register bits
    DR_OE = 1 << 11, // Overrun error
    DR_BE = 1 << 10, // Break error
    DR_PE = 1 <<  9, // Parity error
    DR_FE = 1 <<  8, // Framing error

    // Receive Status Register / Error Clear Register
    RSRECR_OE = 1 << 3, // Overrun error
    RSRECR_BE = 1 << 2, // Break error
    RSRECR_PE = 1 << 1, // Parity error
    RSRECR_FE = 1 << 0, // Framing error

    // Flag Register (depends on LCRH.FEN)
    FR_TXFE = 1 << 7, // Transmit FIFO empty
    FR_RXFF = 1 << 6, // Receive FIFO full
    FR_TXFF = 1 << 5, // Transmit FIFO full
    FR_RXFE = 1 << 4, // Receive FIFO empty
    FR_BUSY = 1 << 3, // BUSY transmitting data
    FR_CTS  = 1 << 0, // Clear To Send

    // Line Control Register
    LCRH_SPS   = 1 << 7, // sticky parity selected
    LCRH_WLEN  = 3 << 5, // word length (5, 6, 7 or 8 bit)
    LCRH_WLEN5 = 0 << 5, // word length 5 bit
    qLCRH_WLEN6 = 1 << 5, // word length 6 bit
    LCRH_WLEN7 = 2 << 5, // word length 7 bit
    LCRH_WLEN8 = 3 << 5, // word length 8 bit
    LCRH_FEN   = 1 << 4, // Enable FIFOs
    LCRH_STP2  = 1 << 3, // Two stop bits select
    LCRH_EPS   = 1 << 2, // Even Parity Select
    LCRH_PEN   = 1 << 1, // Parity enable
    LCRH_BRK   = 1 << 0, // send break

    // Control Register
    CR_CTSEN  = 1 << 15, // CTS hardware flow control
    CR_RTSEN  = 1 << 14, // RTS hardware flow control
    CR_RTS    = 1 << 11, // (not) Request to send
    CR_RXE    = 1 <<  9, // Receive enable
    CR_TXW    = 1 <<  8, // Transmit enable
    CR_LBE    = 1 <<  7, // Loopback enable
    CR_UARTEN = 1 <<  0, // UART enable

    // Interrupts (IMSC / RIS / MIS / ICR)
    INT_OER   = 1 << 10, // Overrun error interrupt
    INT_BER   = 1 <<  9, // Break error interrupt
    INT_PER   = 1 <<  8, // Parity error interrupt
    INT_FER   = 1 <<  7, // Framing error interrupt
    INT_RTR   = 1 <<  6, // Receive timeout interrupt
    INT_TXR   = 1 <<  5, // Transmit interrupt
    INT_RXR   = 1 <<  4, // Receive interrupt
    INT_DSRRM = 1 <<  3, // unsupported / write zero
    INT_DCDRM = 1 <<  2, // unsupported / write zero
    INT_CTSRM = 1 <<  1, // nUARTCTS modem interrupt
    INT_RIRM  = 1 <<  0, // unsupported / write zero
    INT_ALL = 0x7F2,

    IFSL_RXIFLSEL = 7 << 3,     // Receive interrupt FIFO level select
    IFSL_RX_1_8   = 0b000 << 3, // Receive FIFO 1/8 full
    IFSL_RX_1_4   = 0b001 << 3, // Receive FIFO 1/4 full
    IFSL_RX_1_2   = 0b010 << 3, // Receive FIFO 1/2 full
    IFSL_RX_3_4   = 0b011 << 3, // Receive FIFO 3/4 full
    IFSL_RX_7_8   = 0b100 << 3, // Receive FIFO 7/8 full
    IFSL_TXIFLSEL = 7 << 0,     // Transmit interrupt FIFO level select
    IFSL_TX_1_8   = 0b000 << 0, // Transmit FIFO 1/8 full
    IFSL_TX_1_4   = 0b001 << 0, // Transmit FIFO 1/4 full
    IFSL_TX_1_2   = 0b010 << 0, // Transmit FIFO 1/2 full
    IFSL_TX_3_4   = 0b011 << 0, // Transmit FIFO 3/4 full
    IFSL_TX_7_8   = 0b100 << 0, // Transmit FIFO 7/8 full
};

CONSTRUCTOR(UART, 1002) {
    PERIPHERAL_ENTER(lock, NULL, UART0_BASE);

    volatile uint32_t *cr = UART0_reg(UART0_CR);
    volatile uint32_t *fr = UART0_reg(UART0_FR);
    volatile uint32_t *lcrh = UART0_reg(UART0_LCRH);
    volatile uint32_t *icr = UART0_reg(UART0_ICR);
    volatile uint32_t *ibrd = UART0_reg(UART0_IBRD);
    volatile uint32_t *fbrd = UART0_reg(UART0_FBRD);
    volatile uint32_t *imsc = UART0_reg(UART0_IMSC);
    volatile uint32_t *ifls = UART0_reg(UART0_IFLS);

    // wait for end of transmission
    while(*fr & FR_BUSY) { }

    // Disable UART0
    *cr = 0;

    // flush transmit FIFO
    *lcrh &= ~LCRH_FEN;
	
    // select function 0 and disable pull up/down for pins 14, 15
    gpio_configure(&lock, 14, FN0, OFF);
    gpio_configure(&lock, 15, FN0, OFF);

    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.

    // Divider = 3000000/(16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    *ibrd = 1;
    *fbrd = 40;

    // Enable FIFO & 8 bit data transmission (1 stop bit, no parity)
    *lcrh = LCRH_FEN | LCRH_WLEN8;

    // set FIFO interrupts to fire at half full
    *ifls = IFSL_RX_1_2 | IFSL_TX_1_2;
    
    // Clear pending interrupts.
    *icr = INT_ALL;

    // Mask all interrupts.
    *imsc = INT_ALL;

    // Enable UART0, receive & transfer part of UART.
    *cr = CR_UARTEN | CR_TXW | CR_RXE;

    PERIPHERAL_LEAVE(lock);
} CONSTRUCTOR_END

void putc(char c) {
    uart_putc(NULL, c);
}

char getc(void) {
    return uart_getc(NULL);
}

void puts(const char *str) {
    uart_puts(NULL, str);
}

void uart_putc(const PeripheralLock *prev, char c) {
    PERIPHERAL_ENTER(lock, prev, UART0_BASE);

    // wait for space in the transmit FIFO
    while(*UART0_reg(UART0_FR) & FR_TXFF) { }

    // add char to transmit FIFO
    *UART0_reg(UART0_DR) = c;

    PERIPHERAL_LEAVE(lock);
}

char uart_getc(const PeripheralLock *prev) {
    char c;
    PERIPHERAL_ENTER(lock, prev, UART0_BASE);

    // wait for data in the receive FIFO
    while(*UART0_reg(UART0_FR) & FR_RXFE) { }

    // extract char from receive FIFO
    c = *UART0_reg(UART0_DR);

    PERIPHERAL_LEAVE(lock);
    return c;
}

void uart_puts(const PeripheralLock *prev, const char *str) {
    PERIPHERAL_ENTER(lock, prev, UART0_BASE);

    // putc until 0 byte
    while (*str) putc(*str++);

    PERIPHERAL_LEAVE(lock);
}
