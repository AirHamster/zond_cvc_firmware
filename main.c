#define DEBUG1

#include "defines.h"
#include <stdint.h>
#include <string.h>
#include "uart23xx.h"
#include "usart_console.h" 
#include "xprintf.h"
#include "interrupt.h"
#include "spi.h"
#include "timers.h"
#include "adc_dac.h"
uint8_t channel = 1;	//adc channel
uint8_t native = 0;
uint8_t conv_number = CONV_NUMBER + 1;
uint8_t getflag =0;
uint64_t curr_big;
uint16_t volts, curr;
uint16_t curr_array[CONV_NUMBER];
uint16_t volt_array[CONV_NUMBER];
void gpio_init (void)
{	
	SCS |= 1;
	FIO2MASK = 0;
	/* Leds to output */
	FIO2DIR |= (1 << LED1) | (1 << LED2) | (1 << OP_AMP_PIN);

	/* FIO1MASK = 0xFFFFFFFF & (0 << DAC) | (0 << ADC); */
	FIO1MASK = 0;
	FIO1DIR |=  (1 << DAC) | (1 << ADC) | (1 << ADC_DIN) | (1 << ADC_SCLK);	/*  Slave select pins  */
	FIO1DIR &= ~(1 << ADC_DOUT);
	/* FIO1DIR |=  (1 << DAC) | (1 << ADC);[> 	Slave select pins  <] */
	FIO1SET |= (1 << ADC) | (1 << DAC) | (1 << ADC_SCLK);	 /*  Set hight level  */
}
void led_set(uint8_t led)
{
	FIO2SET |= (1 << led);
}
void led_clear(uint8_t led)
{
	FIO2CLR |= (1 << led);
}
void gpio_set(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case 0:
			FIO0SET |= (1 << pin);
			break;
		case 1:
			FIO1SET |= (1 << pin);
			break;
		case 2:
			FIO2SET |= (1 << pin);
			break;
		default:
			break;	
	}
}
void gpio_clear(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case 0:
			FIO0CLR |= (1 << pin);
			break;
		case 1:
			FIO1CLR |= (1 << pin);
			break;
		case 2:
			FIO2CLR |= (1 << pin);
			break;
		default:
			break;	
	}
}
void pll_init(void)
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
	/* GPIOInit(); */
	IrqEnable();			/* Enable Irq */

}


int main (void)
{
	uint16_t i, j, rise; 
	pll_init();
	uart0_init();	
	/* SPI0_init(); */
	timer0_init();
	gpio_init();
	adc_init();
	FIO1CLR |= 1 << DAC;
	SPI0_send_2_byte((0x1000 | 1382), DAC);
	FIO1SET |= 1 << DAC;
	UART0_send("\nLPC initialized\n", 17);

	while(1)
	{
	}
	return 0;
}

