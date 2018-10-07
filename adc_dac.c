#include "adc_dac.h"
void Delay(int value) //Задержка на value микросекунд
{
	int i, j;
	for(j = 0; j < value; j++)
		for(i = 0; i < 10; i++)
			i = i;
}
void adc_init(void)
{
	uint16_t dat = 0;
	/* ADC needs hight level at sclk in idle */
	FIO1PIN |= (1 << ADC_SCLK);
	
	/* Reset ADC */
	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(0xFF, ADC);
	SPI0_send_1_byte(0xFF, ADC);
	SPI0_send_1_byte(0xFF, ADC);
	SPI0_send_1_byte(0xFF, ADC);
	FIO1SET |= 1 << ADC;

	/* Delay because adc needs 500us after reset sequence */
	for (dat = 0; dat < 2000; dat++);

	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_CONF_REG, ADC);
	SPI0_send_2_byte((CONF_REG_VAL | 1), ADC);
	FIO1SET |= 1 << ADC;

	for (dat = 0; dat < 2000; dat++);

	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_MODE_REG, ADC);
	SPI0_send_2_byte(MODE_REG_VAL, ADC);
	FIO1SET |= 1 << ADC;

	FIO1CLR |= 1 << DAC;
	SPI0_send_2_byte((0x1000 | 578), DAC);
	FIO1SET |= 1 << DAC;


	/* Switch on green led to indicate successful  initialization */
	//led_set(LED1);

}

void dac_set_voltage(uint16_t voltage)
{
	/* SPI0_send_2_byte((voltage | DAC_LOAD_CMD)); */
	FIO1CLR |= 1 << DAC;
	SPI0_send_2_byte((0x1000 | voltage), DAC);
	FIO1SET |= 1 << DAC;
}

uint16_t adc_read_current(void)
{
	uint16_t current;
	/* Need to select proper channel */
	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_CONF_REG, ADC);
	SPI0_send_2_byte(CONF_REG_VAL, ADC);
	FIO1SET |= 1 << ADC;

	for (current = 0; current < 200; current++);
	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(READ_DATA_REG, ADC);
	current = SPI0_send_2_byte(0xFFFF, ADC);
	FIO1SET |= 1 << ADC;

	return current;
}

uint16_t adc_read_voltage(void)
{
	uint16_t voltage;
	/* Need to select proper channel */
	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_CONF_REG, ADC);
	SPI0_send_2_byte((CONF_REG_VAL | 1), ADC);	// | 1 - select 2 channel
	FIO1SET |= 1 << ADC;
	for (voltage = 0; voltage < 200; voltage ++);

	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(READ_DATA_REG, ADC);
	voltage = SPI0_send_2_byte(0xFFFF, ADC);
	FIO1SET |= 1 << ADC;

	return voltage;

}

