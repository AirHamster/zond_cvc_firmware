#include <stdint.h>
#include "LPC2300.h"
#include "timers.h"
#include "interrupt.h"
void Isr_TIM0(void)
{
	
}
void timer0_init(void)
{
	PCON |= (1 << 1);	/* Power on tim0 */
	T0TCR = 0;	/* Disable tim0 */

	T0IR = (1 << 0);	/* Channel 0 match interrupt */
	T0CTCR = 0;
	T0PC = 200;	/* Prescaler */
	T0MR0 = 72000;	/* Top value */
	RegisterIrq(TIMER0_IRQn, (void *)Isr_TIM0, PRI_LOWEST);
}

void timer0_start(void)
{

}

void timer0_stop(void)
{

}
