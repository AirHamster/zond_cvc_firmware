#ifndef _UART_DEF
#define _UART_DEF

#include "LPC2300.h"

#define USE_UART0	1		/* Enable UART0 function */
#define UART0_BPS 	230400	/* UART0 bit rate */
#define UART0_TXB	128		/* Size of Tx buffer */
#define UART0_RXB	128		/* Size of Rx buffer */

// #define USE_UART1	0		[> Enable UART1 function <]
#define UART1_BPS 	38400
#define UART1_TXB	128
#define UART1_RXB	128

// #define USE_UART2	0		[> Enable UART2 function <]
#define UART2_BPS 	38400
#define UART2_TXB	128
#define UART2_RXB	128

// #define USE_UART3	0		[> Enable UART3 function <]
#define UART3_BPS 	38400
#define UART3_TXB	128
#define UART3_RXB	128

// UART interrupt
// U0IIR register
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01

// U0LSR register
#define LSR_RDR         0x01
#define LSR_OE          0x02
#define LSR_PE          0x04
#define LSR_FE          0x08
#define LSR_BI          0x10
#define LSR_THRE        0x20
#define LSR_TEMT        0x40
#define LSR_RXFE        0x80

#define BUFSIZE         16
#define MESSAGE_LENGTH  8

//UART
// bit PCONP register
#define PCUART0         3

// bit PCLKSEL0 register
#define PINSEL_UART0_0  6         
#define PINSEL_UART0_1  7 

// bit U0LCR register
#define word_len_0      0
#define word_len_1      1
#define DLAB            7

//bit U0FCR register
#define FIFO_Enable     0
#define RX_FIFO_Reset   1
#define TX_FIFO_Reset   2

//bit U0IER register
#define RBR_Enable      0
#define THRE_Enable     1
#define RLS_Enable      2

//UART message
#define DeviceID        0xAA

#define STATUS_OK       1
#define STATUS_DATA     2
#define STATUS_RTR      3

#define STATUS_ERROR    -1
#define STATUS_FULL     -2
#define STATUS_EMPTY    -3                                                             
#define STATUS_BUSY     -4

#if USE_UART0
void uart0_init (void);
int uart0_test (void);
void uart0_putc (uint8_t);
uint8_t uart0_getc (void);
#endif
#if USE_UART1
void uart1_init (void);
int uart1_test (void);
void uart1_putc (uint8_t);
uint8_t uart1_getc (void);
#endif
#if USE_UART2
void uart2_init (void);
int uart2_test (void);
void uart2_putc (uint8_t);
uint8_t uart2_getc (void);
#endif
#if USE_UART3
void uart3_init (void);
int uart3_test (void);
void uart3_putc (uint8_t);
uint8_t uart3_getc (void);
#endif

#endif
