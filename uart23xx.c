/*------------------------------------------------------------------------/
  /  LPC23xx UART control module
  /-------------------------------------------------------------------------/
  /
  /  Copyright (C) 2013, ChaN, all right reserved.
  /
  / * This software is a free software and there is NO WARRANTY.
  / * No restriction on use. You can use, modify and redistribute it for
  /   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
  / * Redistributions of source code must retain the above copyright notice.
  /
  /-------------------------------------------------------------------------*/

#include "defines.h"
#include "uart23xx.h"
#include "interrupt.h"
#include "string.h"
#include "stdio.h"
#include "xprintf.h"
#include "LPC2300.h"
#include "usart_console.h" 

/* F_PCLK    8/16M  9/18M 10/20M 12/24M 12.5/25M 15/30M */
/* DIVADD       1     5      1      1      1       5    */
/* MULVAL      12     8     12     12      8       8    */
/* Error[%]   0.15  0.16   0.15   0.15   0.47    0.16   */

#define	F_CCLK		72000000	/* cclk frequency */
#define F_PCLK		18000000	/* pclk frequency for the UART modules */
#define	DIVADD		5			/* See below */
#define	MULVAL		8

#define	DLVAL0		((uint32_t)((double)F_PCLK / UART0_BPS / 16 / (1 + (double)DIVADD / MULVAL)))
#define	DLVAL1		((uint32_t)((double)F_PCLK / UART1_BPS / 16 / (1 + (double)DIVADD / MULVAL)))
#define	DLVAL2		((uint32_t)((double)F_PCLK / UART2_BPS / 16 / (1 + (double)DIVADD / MULVAL)))
#define	DLVAL3		((uint32_t)((double)F_PCLK / UART3_BPS / 16 / (1 + (double)DIVADD / MULVAL)))

#if F_PCLK * 1 == F_CCLK
#define PCLKDIV	PCLKDIV_1
#elif F_PCLK * 2 == F_CCLK
#define PCLKDIV	PCLKDIV_2
#elif F_PCLK * 4 == F_CCLK
#define PCLKDIV	PCLKDIV_4
#elif F_PCLK * 8 == F_CCLK
#define PCLKDIV	PCLKDIV_8
#else
#error Invalid frequency combination
#endif




char resiever[50], rec_len = 0;

#if USE_UART0

static volatile struct {
	uint16_t	ri, wi, ct, act;
	uint8_t		buff[UART0_TXB];
} TxBuff0;

static volatile struct {
	uint16_t	ri, wi, ct;
	uint8_t		buff[UART0_RXB];
} RxBuff0;

void Isr_UART0 (void)
{

	uint8_t iir, d;
	int i;

	iir = U0LSR;		/* 	[> [> Get interrupt ID <] <] */
	if(iir & 1 != 0)
	{	
		d = U0RBR;
		if (d == '\n')
		{
			resiever[rec_len++] = 0;	/* Make null-terminated string */
			process_command(resiever);
			rec_len = 0;
		}else{
			resiever[rec_len++] = d;
		}
	}else{
		d = U0RBR;
	}
	if (d == 'L')
	{
		FIO2SET = (1 << LED1) | (1 << LED2);
	}else if (d == 'O')
	{
		FIO2CLR = (1 << LED1) | (1 << LED2);

	}
	VICVectAddr = 0;

}


int uart0_test (void)
{
	return RxBuff0.ct;
}


uint8_t uart0_getc (void)
{
	uint8_t d;
	int i;

	/* Wait while Rx buffer is empty */
	while (!RxBuff0.ct) ;

	i = RxBuff0.ri;	/* Get a byte from Rx buffer */
	d = RxBuff0.buff[i++];
	RxBuff0.ri = i % UART0_RXB;
	U0IER = 0;		/* Disable interrupts */
	RxBuff0.ct--;
	U0IER = 0x07;	/* Reenable interrupt */

	return d;
}


void uart0_putc (uint8_t d)
{
	int i;

	/* Wait for Tx buffer ready */
	while (TxBuff0.ct >= UART0_TXB) ;

	U0IER = 0x05;		/* Disable Tx Interrupt */
	if (TxBuff0.act) {
		i = TxBuff0.wi;	/* Put a byte into Tx byffer */
		TxBuff0.buff[i++] = d;
		TxBuff0.wi = i % UART0_TXB;
		TxBuff0.ct++;
	} else {
		U0THR = d;		/* Trigger Tx sequense */
		TxBuff0.act = 1;
	}
	U0IER = 0x07;		/* Reenable Tx Interrupt */
}


void uart0_init (void)
{
	//UART0
	PCONP |= 1 << PCUART0; // Питание на UART0
	//PCLKSEL0 |= 1 << PINSEL_UART0_0;      // PCLK = CCLK

	//8 bit lenght word,1 stop bit,disable parity generation,disable breake transmission, enable access to Divisor Latches
	U0LCR |= (1 << word_len_0)|(1 << word_len_1)|(1 << DLAB);
	//f = 18 mGz,Baud = 115200.
	U0FDR = 0xC1;
	U0DLL = 0x09;
	U0DLM = 0x00;
	U0LCR &= ~(1 << DLAB);//DLAB = 0

	//UART FIFO Нужно ли оно?
	U0FCR |= ((1 << FIFO_Enable )|(1 << RX_FIFO_Reset)|(1 << TX_FIFO_Reset));//Enable and reset TX and RX FIFO

	//Настройка ножек мк: P0(2) - TxD,P0(3) - RxD.
	//P0.02,P0.03 - pull-up mode
	PINSEL0 |= (1 << 4)|(1 << 6);

	//Interrupts
	/* InstallIRQ( UART0_INT, (void *)UART0_INT_Handler, 0x0E); */
	/* U0IER |= ((1 << RBR_Enable )|(1 << THRE_Enable)|(1 << RLS_Enable));[> Enable UART0 interrupt <] */
	U0IER |= (1 << RBR_Enable );/* Enable UART0 interrupt */
	/* [> Enable Tx/Rx/Error interrupts <] */
	RegisterIrq(UART0_IRQn, (void *)Isr_UART0, PRI_LOWEST);

}

#endif	/* USE_UART0 */



#if USE_UART1

static volatile struct {
	uint16_t	ri, wi, ct, act;
	uint8_t		buff[UART1_TXB];
} TxBuff1;

static volatile struct {
	uint16_t	ri, wi, ct;
	uint8_t		buff[UART1_RXB];
} RxBuff1;

void Isr_UART1 (void)
{
	uint8_t iir, d;
	int i, cnt;


	for (;;) {
		iir = U1IIR;			/* Get interrupt ID */
		if (iir & 1) break;		/* Exit if there is no interrupt */
		switch (iir & 7) {
		case 4:			/* Rx FIFO is half filled or timeout occured */
			i = RxBuff1.wi;
			cnt = RxBuff1.ct;
			while (U1LSR & 0x01) {	/* Get all data in the Rx FIFO */
				d = U1RBR;
				if (cnt < UART1_RXB) {	/* Store data if Rx buffer is not full */
					RxBuff1.buff[i++] = d;
					i %= UART1_RXB;
					cnt++;
				}
			}
			RxBuff1.wi = i;
			RxBuff1.ct = cnt;
			break;

		case 2:			/* Tx FIFO empty */
			cnt = TxBuff1.ct;
			if (cnt) {		/* There is one or more byte to send */
				i = TxBuff1.ri;
				for (d = 16; d && cnt; d--, cnt--) {	/* Fill Tx FIFO */
					U1THR = TxBuff1.buff[i++];
					i %= UART1_TXB;
				}
				TxBuff1.ri = i;
				TxBuff1.ct = cnt;
			} else {
				TxBuff1.act = 0; /* When no data to send, next putc must trigger Tx sequense */
			}
			break;

		default:		/* Data error or break detected */
			U1LSR;
			U1RBR;
			break;
		}
	}
}


int uart1_test (void)
{
	return RxBuff1.ct;
}


uint8_t uart1_getc (void)
{
	uint8_t d;
	int i;

	/* Wait while Rx buffer is empty */
	while (!RxBuff1.ct) ;

	i = RxBuff1.ri;	/* Get a byte from Rx buffer */
	d = RxBuff1.buff[i++];
	RxBuff1.ri = i % UART1_RXB;
	U1IER = 0;		/* Disable interrupts */
	RxBuff1.ct--;
	U1IER = 0x07;	/* Reenable interrupt */

	return d;
}


void uart1_putc (uint8_t d)
{
	int i;

	/* Wait for Tx buffer ready */
	while (TxBuff1.ct >= UART1_TXB) ;

	U1IER = 0x05;		/* Disable Tx Interrupt */
	if (TxBuff1.act) {
		i = TxBuff1.wi;	/* Put a byte into Tx byffer */
		TxBuff1.buff[i++] = d;
		TxBuff1.wi = i % UART1_TXB;
		TxBuff1.ct++;
	} else {
		U1THR = d;		/* Trigger Tx sequense */
		TxBuff1.act = 1;
	}
	U1IER = 0x07;		/* Enable Tx Interrupt */
}


void uart1_init (void)
{
	/* Enable UART1 module */
	__set_PCONP(PCUART1);
	__set_PCLKSEL(PCLK_UART1, PCLKDIV);

	/* Initialize UART */
	U1IER = 0x00;			/* Disable interrupt */
	U1LCR = 0x83;			/* Select baud rate divisor latch */
	U1DLM = DLVAL1 / 256;	/* Set BRG dividers */
	U1DLL = DLVAL1 % 256;
	U1FDR = (MULVAL << 4) | DIVADD;
	U1LCR = 0x03;			/* Set serial format N81 and deselect divisor latch */
	U1FCR = 0x87;			/* Enable FIFO */
	U1TER = 0x80;			/* Enable Tansmission */

	/* Clear Tx/Rx buffers */
	TxBuff1.ri = 0; TxBuff1.wi = 0; TxBuff1.ct = 0; TxBuff1.act = 0;
	RxBuff1.ri = 0; RxBuff1.wi = 0; RxBuff1.ct = 0;

	/* Attach UART1 to I/O pad */
	__set_PINSEL(0, 16, 1);	/* P0.16 - RXD1 */
	__set_PINSEL(0, 15, 1);	/* P0.15 - TXD1 */

	/* Enable Tx/Rx/Error interrupts */
	RegisterIrq(UART1_IRQn, Isr_UART1, PRI_LOWEST);
	U1IER = 0x07;
}

#endif	/* USE_UART1 */



#if USE_UART2

static volatile struct {
	uint16_t	ri, wi, ct, act;
	uint8_t		buff[UART2_TXB];
} TxBuff2;

static volatile struct {
	uint16_t	ri, wi, ct;
	uint8_t		buff[UART2_RXB];
} RxBuff2;

void Isr_UART2 (void)
{
	uint8_t iir, d;
	int i, cnt;


	for (;;) {
		iir = U2IIR;		/* Get interrupt ID */
		if (iir & 1) break;	/* Exit if there is no interrupt */
		switch (iir & 7) {
		case 4:			/* Rx FIFO is half filled or timeout occured */
			i = RxBuff2.wi;
			cnt = RxBuff2.ct;
			while (U2LSR & 0x01) {	/* Get all data in the Rx FIFO */
				d = U2RBR;
				if (cnt < UART2_RXB) {	/* Store data if Rx buffer is not full */
					RxBuff2.buff[i++] = d;
					i %= UART2_RXB;
					cnt++;
				}
			}
			RxBuff2.wi = i;
			RxBuff2.ct = cnt;
			break;

		case 2:			/* Tx FIFO empty */
			cnt = TxBuff2.ct;
			if (cnt) {		/* There is one or more byte to send */
				i = TxBuff2.ri;
				for (d = 16; d && cnt; d--, cnt--) {	/* Fill Tx FIFO */
					U2THR = TxBuff2.buff[i++];
					i %= UART2_TXB;
				}
				TxBuff2.ri = i;
				TxBuff2.ct = cnt;
			} else {
				TxBuff2.act = 0; /* When no data to send, next putc must trigger Tx sequense */
			}
			break;

		default:		/* Data error or break detected */
			U2LSR;
			U2RBR;
			break;
		}
	}
}


int uart2_test (void)
{
	return RxBuff2.ct;
}


uint8_t uart2_getc (void)
{
	uint8_t d;
	int i;

	/* Wait while Rx buffer is empty */
	while (!RxBuff2.ct) ;

	i = RxBuff2.ri;	/* Get a byte from Rx buffer */
	d = RxBuff2.buff[i++];
	RxBuff2.ri = i % UART2_RXB;
	U2IER = 0;		/* Disable interrupts */
	RxBuff2.ct--;
	U2IER = 0x07;	/* Reenable interrupt */

	return d;
}


void uart2_putc (uint8_t d)
{
	int i;

	/* Wait for Tx buffer ready */
	while (TxBuff2.ct >= UART2_TXB) ;

	U2IER = 0x05;		/* Disable Tx Interrupt */
	if (TxBuff2.act) {
		i = TxBuff2.wi;	/* Put a byte into Tx byffer */
		TxBuff2.buff[i++] = d;
		TxBuff2.wi = i % UART2_TXB;
		TxBuff2.ct++;
	} else {
		U2THR = d;		/* Trigger Tx sequense */
		TxBuff2.act = 1;
	}
	U2IER = 0x07;		/* Enable Tx Interrupt */
}


void uart2_init (void)
{
	/* Enable UART2 module */
	__set_PCONP(PCUART2);
	__set_PCLKSEL(PCLK_UART2, PCLKDIV);

	/* Initialize UART */
	U2IER = 0x00;			/* Disable interrupt */
	U2LCR = 0x83;			/* Select baud rate divisor latch */
	U2DLM = DLVAL2 / 256;	/* Set BRG dividers */
	U2DLL = DLVAL2 % 256;
	U2FDR = (MULVAL << 4) | DIVADD;
	U2LCR = 0x03;			/* Set serial format N81 and deselect divisor latch */
	U2FCR = 0x87;			/* Enable FIFO */
	U2TER = 0x80;			/* Enable Tansmission */

	/* Clear Tx/Rx buffers */
	TxBuff2.ri = 0; TxBuff2.wi = 0; TxBuff2.ct = 0; TxBuff2.act = 0;
	RxBuff2.ri = 0; RxBuff2.wi = 0; RxBuff2.ct = 0;

	/* Attach UART2 to I/O pad */
	__set_PINSEL(0, 11, 1);	/* P0.11 - TXD2 */
	__set_PINSEL(0, 10, 1);	/* P0.10 - RXD2 */

	/* Enable Tx/Rx/Error interrupts */
	RegisterIrq(UART2_IRQn, Isr_UART2, PRI_LOWEST);
	U2IER = 0x07;
}

#endif	/* USE_UART2 */



#if USE_UART3

static volatile struct {
	uint16_t	ri, wi, ct, act;
	uint8_t		buff[UART3_TXB];
} TxBuff3;

static volatile struct {
	uint16_t	ri, wi, ct;
	uint8_t		buff[UART3_RXB];
} RxBuff3;

void Isr_UART3 (void)
{
	uint8_t iir, d;
	int i, cnt;


	for (;;) {
		iir = U3IIR;		/* Get interrupt ID */
		if (iir & 1) break;	/* Exit if there is no interrupt */
		switch (iir & 7) {
		case 4:			/* Rx FIFO is half filled or timeout occured */
			i = RxBuff3.wi;
			cnt = RxBuff3.ct;
			while (U3LSR & 0x01) {	/* Get all data in the Rx FIFO */
				d = U3RBR;
				if (cnt < UART3_RXB) {	/* Store data if Rx buffer is not full */
					RxBuff3.buff[i++] = d;
					i %= UART3_RXB;
					cnt++;
				}
			}
			RxBuff3.wi = i;
			RxBuff3.ct = cnt;
			break;

		case 2:			/* Tx FIFO empty */
			cnt = TxBuff3.ct;
			if (cnt) {		/* There is one or more byte to send */
				i = TxBuff3.ri;
				for (d = 16; d && cnt; d--, cnt--) {	/* Fill Tx FIFO */
					U3THR = TxBuff3.buff[i++];
					i %= UART3_TXB;
				}
				TxBuff3.ri = i;
				TxBuff3.ct = cnt;
			} else {
				TxBuff3.act = 0; /* When no data to send, next putc must trigger Tx sequense */
			}
			break;

		default:		/* Data error or break detected */
			U3LSR;
			U3RBR;
			break;
		}
	}
}


int uart3_test (void)
{
	return RxBuff3.ct;
}


uint8_t uart3_getc (void)
{
	uint8_t d;
	int i;

	/* Wait while Rx buffer is empty */
	while (!RxBuff3.ct) ;

	i = RxBuff3.ri;	/* Get a byte from Rx buffer */
	d = RxBuff3.buff[i++];
	RxBuff3.ri = i % UART3_RXB;
	U3IER = 0;		/* Disable interrupts */
	RxBuff3.ct--;
	U3IER = 0x07;	/* Reenable interrupt */

	return d;
}


void uart3_putc (uint8_t d)
{
	int i;

	/* Wait for Tx buffer ready */
	while (TxBuff3.ct >= UART3_TXB) ;

	U3IER = 0x05;		/* Disable Tx Interrupt */
	if (TxBuff3.act) {
		i = TxBuff3.wi;	/* Put a byte into Tx byffer */
		TxBuff3.buff[i++] = d;
		TxBuff3.wi = i % UART3_TXB;
		TxBuff3.ct++;
	} else {
		U3THR = d;		/* Trigger Tx sequense */
		TxBuff3.act = 1;
	}
	U3IER = 0x07;		/* Enable Tx Interrupt */
}


void uart3_init (void)
{
	/* Enable UART3 module */
	__set_PCONP(PCUART3);
	__set_PCLKSEL(PCLK_UART3, PCLKDIV);

	/* Initialize UART */
	U3IER = 0x00;			/* Disable interrupt */
	U3LCR = 0x83;			/* Select baud rate divisor latch */
	U3DLM = DLVAL3 / 256;	/* Set BRG dividers */
	U3DLL = DLVAL3 % 256;
	U3FDR = (MULVAL << 4) | DIVADD;
	U3LCR = 0x03;			/* Set serial format N81 and deselect divisor latch */
	U3FCR = 0x87;			/* Enable FIFO */
	U3TER = 0x80;			/* Enable Tansmission */

	/* Clear Tx/Rx buffers */
	TxBuff3.ri = 0; TxBuff3.wi = 0; TxBuff3.ct = 0; TxBuff3.act = 0;
	RxBuff3.ri = 0; RxBuff3.wi = 0; RxBuff3.ct = 0;

	/* Attach UART3 to I/O pad */
	__set_PINSEL(0, 1, 2);	/* P0.1 - TXD3 */
	__set_PINSEL(0, 0, 2);	/* P0.0 - RXD3 */

	/* Enable Tx/Rx/Error interrupts */
	RegisterIrq(UART3_IRQn, Isr_UART3, PRI_LOWEST);
	U3IER = 0x07;
}

#endif	/* USE_UART3 */




