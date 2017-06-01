#include "adc_dac.h"
#include <stdint.h>
#include "LPC2300.h"
void set_voltage(int8_t volts)
{
	//adc_spi_setup
	UART0_send("Voltage set\n", 12);

}
void adc_init(void)
{

}

void dac_init(void)
{

}

uint16_t adc_read_current(void)
{

}

uint16_t adc_read_voltage(void)
{

}

