#include "spi.h"
/* #define DEBUG_SPI */


uint8_t SPI0_send_1_byte(uint8_t data, uint8_t slave)
{
	uint8_t dat = 0;
	if (slave == ADC )
	{
	dat = SPI_ADC_data_transfers_8bit(data);
	}else if (slave == DAC)
	{
	dat = SPI_DAC_data_transfers_8bit(data);
	}
	return dat;
}

uint16_t SPI0_send_2_byte(uint16_t data, uint8_t slave)
{
	uint16_t dat;
	if (slave == ADC )
	{
	dat = SPI_ADC_data_transfers_16bit(data);
	}else if (slave == DAC)
	{
	dat = SPI_DAC_data_transfers_16bit(data);
	}
	return dat;
}

uint16_t SPI_ADC_data_transfers_16bit (uint16_t data)
{
	uint16_t dat = 0;
	uint16_t mask;
	char i, j;

	//Write
	mask = 0x8000;
	for(i = 16;  i > 0 ; i--)
	{
		//SCLK
		FIO1PIN &= ~(1 << ADC_SCLK);
		/* if(data & mask) */
		if(data & (1 << (i - 1)))
			FIO1PIN |= 1 << ADC_DIN;
		else
			FIO1PIN &= ~(1 << ADC_DIN);

		mask = mask >> 1;

		for (j = 0; j < 6; j++);
		FIO1PIN |= 1 << ADC_SCLK;
		//read
		if(FIO1PIN & (1 << (ADC_DOUT)))
			dat |= 1 << (i - 1);
		else
			dat &= ~(1 << (i - 1));

	}  
	return dat;
}

unsigned char SPI_ADC_data_transfers_8bit (unsigned char data)
{
	unsigned char dat = 0;
	char i, j;

	uint16_t mask = 0x8000;
	//Write
	for(i = 8;  i > 0 ; i--)
	{
		//SCLK
		FIO1PIN &= ~(1 << ADC_SCLK);
		if(data & (1 << (i - 1)))
		/* if(data & mask) */
			FIO1PIN |= 1 << ADC_DIN;
		else
			FIO1PIN &= ~(1 << ADC_DIN);

		mask = mask >> 1;

		for (j = 0; j < 6; j++);
		FIO1PIN |= 1 << ADC_SCLK;
		//read
		if(FIO1PIN & (1 << (ADC_DOUT)))
			dat |= 1 << (i - 1);
		else
			dat &= ~(1 << (i - 1));

	}  
	return dat;
}
uint16_t SPI_DAC_data_transfers_16bit (uint16_t data)
{
	uint16_t dat = 0;
	uint16_t mask;
	char i, j;

	//Write
	mask = 0x8000;
	for(i = 16;  i > 0 ; i--)
	{
		FIO1PIN |= 1 << ADC_SCLK;
		if(data & mask)
			FIO1PIN |= 1 << ADC_DIN;
		else
			FIO1PIN &= ~(1 << ADC_DIN);

		//SCLK
		mask = mask >> 1;
		for (j = 0; j < 6; j++);
		FIO1PIN &= ~(1 << ADC_SCLK);

		//read
		if(FIO1PIN & (1 << (ADC_DOUT)))
			dat |= 1 << (i - 1);
		else
			dat &= ~(1 << (i - 1));

	}  
	return dat;
}

unsigned char SPI_DAC_data_transfers_8bit (unsigned char data)
{
	unsigned char dat = 0;
	char i, j;

	//Write
	for(i = 8;  i > 0 ; i--)
	{
		if(data & (1 << (i - 1)))
			FIO1PIN |= 1 << ADC_DIN;
		else
			FIO1PIN &= ~(1 << ADC_DIN);

		//SCLK
		FIO1PIN |= 1 << ADC_SCLK;
		for (j = 0; j < 6; j++);
		FIO1PIN &= ~(1 << ADC_SCLK);

		//read
		if(FIO1PIN & (1 << (ADC_DOUT)))
			dat |= 1 << (i - 1);
		else
			dat &= ~(1 << (i - 1));

	}  
	return dat;
}
