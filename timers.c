#include <stdint.h>
#include "LPC2300.h"
#include "timers.h"
#include "interrupt.h"
#include "defines.h"
#include "uart23xx.h"
#include "adc_dac.h"
#include "usart_console.h"
#include <stdlib.h>
extern void gpio_set(uint8_t port, uint8_t pin);
extern void gpio_clear(uint8_t port, uint8_t pin);
extern uint8_t channel;
extern uint16_t volts, curr;
void Isr_TIM0(void)
{
	uint16_t dat;
	T0IR = 0x3F;
	uint8_t data;
	uint8_t num = 5;
	char *volt_ascii[5];
	char *curr_ascii[5];
	FIO1PIN |= (1 << ADC_SCLK);
	if (channel == 1)
	{
		volts = adc_read_voltage();
		UART0_send("\nOutput voltage: ",17 );

		if (volts < 10000)
			num = 4;
		if (volts < 1000)
			num = 3;
		if (volts < 100)
			num = 2;
		if (volts < 10)
			num = 1;
		UART0_send(itoa(volts, volt_ascii,10), num);
		UART0_send("\nCurrent: ",10 );
		if (curr < 10000)
			num = 4;
		if (curr < 1000)
			num = 3;
		if (curr < 100)
			num = 2;
		if (curr < 10)
			num = 1;
		UART0_send(itoa(curr, curr_ascii,10), num);
		channel = 0;
		/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte(CONF_REG_VAL, ADC);
		FIO1SET |= 1 << ADC;
	}else if (channel == 0)
	{
		curr = adc_read_current();
		channel = 1;
		/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte((CONF_REG_VAL | 1), ADC);
		FIO1SET |= 1 << ADC;
	}
	VICVectAddr = 0;
}
void timer0_init(void)
{
	PCONP |= (1 << 1);	/* Power on tim0 */
	T0TCR = 0;	/* Disable tim0 */

	T0IR = (1 << 0);	/* Channel 0 match interrupt */
	T0MCR = 3;
	T0CTCR = 0;
	T0PR = 25;	/* Prescaler */
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
