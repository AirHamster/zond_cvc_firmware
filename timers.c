#include <stdint.h>
#include "LPC2300.h"
#include "timers.h"
#include "interrupt.h"
#include "defines.h"
#include "uart23xx.h"
#include "adc_dac.h"
#include "usart_console.h"
extern void gpio_set(uint8_t port, uint8_t pin);
extern void gpio_clear(uint8_t port, uint8_t pin);
void Isr_TIM0(void)
{
	uint16_t dat;
	T0IR = 0x3F;
	uint16_t volts, curr;
	uint8_t data;
	FIO1PIN |= (1 << ADC_SCLK);

	curr = adc_read_current();
        dat = 40 * 5;
	/* volts = adc_read_voltage(); */
	FIO1CLR |= 1 << ADC;

	SPI0_send_1_byte((WRITE_CONF_REG | (1 << 6)), ADC);
	dat = SPI0_send_2_byte(0xF1, ADC);

	FIO1SET |= 1 << ADC;

	UART0_send("\nSPI_recieved: ", 15);
	UART0_send_byte(dat >> 8);
        UART0_send_byte(dat); 
	/* UART0_send("\nOutput voltage: ",17 ); */
	/* UART0_send_byte(volts >> 8); */
	/* UART0_send_byte(volts); */
	UART0_send("\nCurrent: ",10 );
	UART0_send_byte(curr >> 8);
	UART0_send_byte(curr); 

/*         dat = 40 * 5;
 *         FIO1CLR |= 1 << ADC;
 * 
 *         SPI0_send_1_byte((WRITE_CONF_REG | (1 << 6)), ADC);
 *         dat = SPI0_send_2_byte(0xF1, ADC);
 * 
 *         FIO1SET |= 1 << ADC;
 * 
 *         UART0_send("\nSPI_recieved: ", 15);
 *         UART0_send_byte(dat >> 8);
 *         UART0_send_byte(dat); */
/*         FIO1CLR |= 1 << ADC;
 * 
 *         SPI0_send_1_byte(READ_ID_REG, ADC);
 *         dat = SPI0_send_1_byte(0xFF, ADC);
 * 
 *         FIO1SET |= 1 << ADC;
 * 
 *         UART0_send("\nSPI_recieved: ", 15);
 *         UART0_send_byte(dat); */
	VICVectAddr = 0;
}
void timer0_init(void)
{
	PCONP |= (1 << 1);	/* Power on tim0 */
	T0TCR = 0;	/* Disable tim0 */

	T0IR = (1 << 0);	/* Channel 0 match interrupt */
	T0MCR = 3;
	T0CTCR = 0;
	T0PR = 200;	/* Prescaler */
	T0MR0 = 72000;	/* Top value (5 Hz) */
	RegisterIrq(TIMER0_IRQn, (void *)Isr_TIM0, PRI_LOWEST);
}

void timer0_start(void)
{
	T0TCR |= 1;
	gpio_set(OP_AMP_PORT, OP_AMP_PIN);
}

void timer0_stop(void)
{
	T0TCR &= ~1;
	gpio_clear(OP_AMP_PORT, OP_AMP_PIN);
}
