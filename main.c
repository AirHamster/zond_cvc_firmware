#define DEBUG1

#include "defines.h"
#include <stdint.h>
#include <string.h>
#include "uart23xx.h"
#include "usart_console.h" 
#include "xprintf.h"
#include "interrupt.h"
#include "spi.h"
void GPIOInit (void)
{
	SCS |= 1;
	FIO2MASK = 0;
	/* Leds to output */
	FIO2DIR |= (1 << LED1) | (1 << LED2);
	/* Switch on leds */
	/* FIO2SET = (1 << LED1) | (1 << LED2); */
	/* ADC and DAC chip select pins setup */
	FIO1MASK = 0;
	FIO1DIR |= (1 << ADC) | (1 << DAC);	/* Slave select pins */
	FIO1SET |= (1 << ADC) | (1 << DAC);	/* Set hight level */
}
void led_set(uint8_t led)
{
	FIO2SET |= (1 << led);
}
void led_clear(uint8_t led)
{
	FIO2CLR |= (1 << led);
}
void IoInit(void)
{
// 1. Init OSC
	SCS = (1 << 5);
	// 2.  Wait for OSC ready
  while (!(SCS & 0x40)); //	[>Osc stabilization procedure<] 
	// 3. Disconnect PLL
	PLLCON = 1; 
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 4. Disable PLL
	PLLCON = 0;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 5. Select source clock for PLL
	CLKSRCSEL = 1;			/* Select external as the PLL clock source */
	// 6. Set PLL settings 288 MHz
	PLLCFG = ((2 - 1) << 16) | (24 - 1);	/* Re-configure PLL */
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 7. Enable PLL
	PLLCON |= 1 << 0; 
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// 8. Wait for the PLL to achieve lock
	while ((PLLSTAT & (1 << 26)) == 0);	/* Wait for PLL locked */
	// 9. Set clk divider settings
	CCLKCFG   = 4-1;            // 1/4 Fpll - 72 MHz; 1/5 - 60
	//USBCLKCFG = 6-1;            // 1/6 Fpll - 48 MHz
	PCLKSEL0 = PCLKSEL1 = 0;    // other peripherals


	PCLKSEL1 = 1 << 2;	//GPIO: 0 - 1/4; 1 - 1; 2 - 1/2; 3 - 1/8
        
	// 10. Connect the PLL
	PLLCON |= 1 << 1;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	ClearVector();			/* Initialie VIC */
	GPIOInit();
	IrqEnable();			/* Enable Irq */

}


int main (void)
{
	uint16_t d;
	GPIOInit();
	IoInit();			/* 	[> Initialize PLL, VIC and timer <] */
	uart0_init();		/* 	[> Initialize UART and join it to the console <] <] */
	SPI0_init();
	UART0_send("LPC initialized\n", 16);
	led_set(LED1);
	/* led_clear(LED2); */
	while(1)
	{
		/* S0SPDR = 0x55; */
		/* SPI0_send_1_byte(0xF0, ADC); */
		d = SPI0_read_2_byte(ADC);
		/* UART0_send(d, 1); */
	}
	return 0;
}

