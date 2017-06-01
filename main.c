
#include <string.h>
#include "uart23xx.h"
/* #include "usart_console.h"  */
#include "xprintf.h"
#include "interrupt.h"
#define DEBUG1
#define LED1 0
#define LED2 1
/*---------------------------------------------------------*/
/* 1000Hz timer interrupt generated by TIMER0              */
/*---------------------------------------------------------*/
void Isr_TIMER0 (void)
{
	T0IR = 1;			/* Clear irq flag */

	/* Timer++;			[> Performance timer <] */
	/* TmrFrm += 1000;		[> Video frame timer (disp.c) <] */

	/* MCI_timerproc();	[> Disk timer process <] */

} 

void GPIOInit (void)
{
	/* FIO2MASK = 0; */
	/* Leds to output */
	FIO2DIR0 |= (1 << LED1) | (1 << LED2);
	/* Switch on leds */
	FIO2SET0 = (1 << LED1) | (1 << LED2);
	SCS |= 1;
}

void IoInit1(void)
{
// 1. Init OSC
	SCS = (1 << 5);
	/* SCS_bit.OSCRANGE = 0; */
	/* SCS_bit.OSCEN = 1; */
	// 2.  Wait for OSC ready
	/* while(!SCS_bit.OSCSTAT); */
  while (!(SCS & 0x40)); //	[>Osc stabilization procedure<] 
	// 3. Disconnect PLL
	PLLCON = 1; 
	/* PLLCON_bit.PLLC = 0; */
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 4. Disable PLL
	PLLCON = 0;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 5. Select source clock for PLL
	CLKSRCSEL = 1;			/* Select external as the PLL clock source */
	/* CLKSRCSEL_bit.CLKSRC = 1; // Selects the main oscillator as a PLL clock source. */
	// 6. Set PLL settings 288 MHz
	PLLCFG = ((2 - 1) << 16) | (24 - 1);	/* Re-configure PLL */
	/* PLLCFG_bit.MSEL = 24-1; */
	/* PLLCFG_bit.NSEL = 2-1; */
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 7. Enable PLL
	/* PLLCON_bit.PLLE = 1; */
	PLLCON |= 1 << 0; 
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 8. Wait for the PLL to achieve lock
	/* while(!PLLSTAT_bit.PLOCK); */
	while ((PLLSTAT & (1 << 26)) == 0);	/* Wait for PLL locked */
	// 9. Set clk divider settings
	CCLKCFG   = 4-1;            // 1/4 Fpll - 72 MHz; 1/5 - 60
	//USBCLKCFG = 6-1;            // 1/6 Fpll - 48 MHz
	PCLKSEL0 = PCLKSEL1 = 0;    // other peripherals


	PCLKSEL1 = 1 << 2;	//GPIO: 0 - 1/4; 1 - 1; 2 - 1/2; 3 - 1/8
	/* PCLKSEL0_bit.PCLK_TIMER0 = 1;	//0 - 1/4; 1 - 1; 2 - 1/2; 3 - 1/8 */
	/* PCLKSEL0_bit.PCLK_UART0 = 0;	//UART0: 0 - 1/4; 1 - 1; 2 - 1/2; 3 - 1/8 */
        
	// 10. Connect the PLL
	PLLCON |= 1 << 1;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	ClearVector();			/* Initialie VIC */
	GPIOInit();
	IrqEnable();			/* Enable Irq */

}
static
void IoInit (void)
{
#define PLL_N		2UL
#define PLL_M		72UL
#define CCLK_DIV	4

	if ( PLLSTAT & (1 << 25) ) {
		PLLCON = 1;				/* Disconnect PLL output if PLL is in use */
		PLLFEED = 0xAA; PLLFEED = 0x55;
	}
	PLLCON = 0;				/* Disable PLL */
	PLLFEED = 0xAA; PLLFEED = 0x55;
/* SCS |= (1<<4); 	[> Set ext oscillator range <]
 * SCS |= (1<<5); [> External osc enable <]
 * while (!(SCS & 0x40)); 	[>Osc stabilization procedure<] */
	CLKSRCSEL = 0;			/* Select external as the PLL clock source */

	PLLCFG = ((PLL_N - 1) << 16) | (PLL_M - 1);	/* Re-configure PLL */
	PLLFEED = 0xAA; PLLFEED = 0x55;
	PLLCON = 1;				/* Enable PLL */
	PLLFEED = 0xAA; PLLFEED = 0x55;

	while ((PLLSTAT & (1 << 26)) == 0);	/* Wait for PLL locked */

	CCLKCFG = CCLK_DIV-1;	/* Select CCLK frequency (divide ratio of hclk) */
	PLLCON = 3;				/* Connect PLL output to the sysclk */
	PLLFEED = 0xAA; PLLFEED = 0x55;

	MAMCR = 0;				/* Configure MAM with 0 wait operation */
	MAMTIM = 3;
	MAMCR = 2;

	PCLKSEL0 = 0x00000000;	/* Initialize peripheral clock to default */
	PCLKSEL1 = 0x00000000;

	ClearVector();			/* Initialie VIC */
	GPIOInit();
	SCS |= 1;				/* Enable FIO0 and FIO1 */

	/* Initialize Timer0 as 1kHz interval timer */
	RegisterIrq(TIMER0_IRQn, Isr_TIMER0, PRI_LOWEST);
	T0CTCR = 0;
	T0MR0 = 18000 - 1;		/* 18M / 1k = 18000 */
	T0MCR = 0x3;			/* Clear TC and Interrupt on MR0 match */
	T0TCR = 1;

	IrqEnable();			/* Enable Irq */
}



int main (void)
{
	
	GPIOInit();
	IoInit1();			/* 	[> Initialize PLL, VIC and timer <] */
	 uart0_init();		/* 	[> Initialize UART and join it to the console <] <] */
	UART0_send("LPC initialized\n", 16);
	/* spi_init(); */
	/* xputs("\nLPC initialized\n"); */
	/* xdev_in(uart0_getc); */
	/* xdev_out(uart0_putc); */
	
	while(1);
	return 0;
}
