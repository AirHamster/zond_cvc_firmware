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
	uint8_t dat = 0;
	
	FIO1CLR |= 1 << ADC;

	SPI0_send_1_byte(READ_ID_REG);
	dat = SPI0_send_1_byte(0xFF);

	FIO1SET |= 1 << ADC;

	UART0_send("\nSPI_recieved: ", 15);
	UART0_send_byte(dat);

	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_CONF_REG);
	SPI0_send_2_byte(CONF_REG_VAL);
	FIO1SET |= 1 << ADC;
	
	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_MODE_REG);
	SPI0_send_2_byte(MODE_REG_VAL);
	FIO1SET |= 1 << ADC;

	/* SPI0_send_1_byte(WRITE_OFFSET_REG); */
	/* SPI0_send_2_byte(OFFSET_REG_VAL); */
	led_set(LED1);

}

void dac_set_voltage(uint16_t voltage)
{
	/* SPI0_send_2_byte((voltage | DAC_LOAD_CMD)); */
	FIO1CLR |= 1 << DAC;
	SPI0_send_2_byte((voltage));
	FIO1SET |= 1 << DAC;
}

uint16_t adc_read_current(void)
{
	uint16_t current;
	/* Need to select proper channel */
	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_MODE_REG);
	SPI0_send_2_byte(MODE_REG_VAL);
	FIO1SET |= 1 << ADC;

	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(READ_DATA_REG);
	current = SPI0_send_2_byte(0xFFFF);
	FIO1SET |= 1 << ADC;

	return current;
}

uint16_t adc_read_voltage(void)
{
	uint16_t voltage;
	/* Need to select proper channel */
	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(WRITE_MODE_REG);
	SPI0_send_2_byte((MODE_REG_VAL | 1));	// | 1 - select 2 channel
	FIO1SET |= 1 << ADC;

	FIO1CLR |= 1 << ADC;
	SPI0_send_1_byte(READ_DATA_REG);
	voltage = SPI0_send_2_byte(0xFFFF);
	FIO1SET |= 1 << ADC;

	return voltage;

}

