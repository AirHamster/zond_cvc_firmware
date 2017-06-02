#include "adc_dac.h"
#include <stdint.h>
#include "LPC2300.h"
#include "defines.h"
void adc_init(void)
{
	SPI0_send_1_byte(WRITE_CONF_REG, ADC);
	SPI0_send_2_byte(CONF_REG_VAL, ADC);
	
	SPI0_send_1_byte(WRITE_MODE_REG, ADC);
	SPI0_send_2_byte(MODE_REG_VAL, ADC);

	SPI0_send_1_byte(WRITE_OFFSET_REG, ADC);
	SPI0_send_2_byte(OFFSET_REG_VAL, ADC);
	led_set(LED1);

}

void dac_set_voltage(uint16_t voltage)
{
	/* SPI0_send_2_byte((voltage | DAC_LOAD_CMD), DAC); */
	SPI0_send_2_byte((voltage), DAC);
}

uint16_t adc_read_current(void)
{
	uint16_t current;
	/* Need to select proper channel */
	SPI0_send_1_byte(WRITE_MODE_REG, ADC);
	SPI0_send_2_byte(MODE_REG_VAL, ADC);

	SPI0_send_1_byte(READ_DATA_REG, ADC);
	current = SPI0_read_2_byte(ADC);

	return current;
}

uint16_t adc_read_voltage(void)
{
	uint16_t voltage;
	/* Need to select proper channel */
	SPI0_send_1_byte(WRITE_MODE_REG, ADC);
	SPI0_send_2_byte((MODE_REG_VAL | 1), ADC);	// | 1 - select 2 channel

	SPI0_send_1_byte(READ_DATA_REG, ADC);
	voltage = SPI0_read_2_byte(ADC);

	return voltage;

}

