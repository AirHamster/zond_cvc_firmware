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
	T0IR = 0x3F;
	uint16_t volts, curr;
	uint8_t data;
	volts = adc_read_voltage();
	curr = adc_read_current();
	UART0_send("\nOutput voltage: ",17 );
	data = volts >> 8;
	UART0_send_byte(data);
	data = volts & 0xFF;
	UART0_send_byte(data);

	UART0_send("\nCurrent: ",10 );
	data = curr >> 8;
	UART0_send_byte(data);
	data = curr & 0xFF;
	UART0_send_byte(data);
	/* UART0_send(&data, 1); */
	/* UART0_send("Tim0\n", 5); */
	VICVectAddr = 0;
}
void timer0_init(void)
{
	PCONP |= (1 << 1);	/* Power on tim0 */
	T0TCR = 0;	/* Disable tim0 */

	T0IR = (1 << 0);	/* Channel 0 match interrupt */
	T0MCR = 3;
	T0CTCR = 0;
	T0PR = 400;	/* Prescaler */
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
