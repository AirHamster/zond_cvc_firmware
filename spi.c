#include <stdint.h>
#include "spi.h"
#include "LPC2300.h"
#include "defines.h"


void Delay(int value) //Задержка на value микросекунд
{
	int i, j;
	for(j = 0; j < value; j++)
		for(i = 0; i < 10; i++)
			i = i;
}

void SPI0_send_1_byte(uint8_t data, uint8_t slave)
{
	if(slave == DAC)
	{
		FIO1CLR |= 1 << DAC;
		SPI_ADC_data_transfers_8bit(data);
		FIO1SET |= 1 << DAC;

	}else if (slave == ADC){

		FIO1CLR |= 1 << ADC;
		SPI_ADC_data_transfers_8bit(data);
		FIO1SET |= 1 << ADC;
	}

}

void SPI0_send_2_byte(uint16_t data, uint8_t slave)
{
	if(slave == DAC)
	{
		//cs
		FIO1CLR |= 1 << DAC;
		/* while((S0SPSR & 1<<7) != 1<<7); */
		/* S0SPDR = data >> 8; */
		SPI_ADC_data_transfers_16bit(data); 
		/* while((S0SPSR & 1<<7) != 1<<7); */
		/* S0SPDR = data & 0xFF; */
		FIO1SET |= 1 << DAC;
	}else if (slave == ADC){
		FIO1CLR |= 1 << ADC;
		SPI_ADC_data_transfers_16bit(data); 
		FIO1SET |= 1 << ADC;
	}

}

void SPI0_init(void)
{
	PCLKSEL0 |= (1<<17) | (1<<16);//=72Mhz/8 
	PCON |= (1 << 8);
	S0SPCR |= (1 << 3) | (1 << 5);	/*   Master mode & CPHA */
	S0SPCCR = 0xFF; 	/* SPI0 perif clock divided by 256 */
	/* PINSEL3 |= (3 << 14) | (3 < 16);	[>MISO & MOSI pins <] */
	PINSEL3 |= ((1 << 9) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17));	/*MISO & MOSI pins */
	/* PINMODE3 |= (1 << 15) | (1 << 7); */
}

uint8_t SPI0_read_1_byte(uint8_t slave)
{
	uint8_t data;
	if(slave == DAC)
	{
		FIO1CLR |= 1 << DAC;
		data = SPI_ADC_data_transfers_8bit(0);
		FIO1SET |= 1 << DAC;
	}else if (slave == ADC){
		FIO1CLR |= 1 << ADC;
		data = SPI_ADC_data_transfers_8bit(0);
		FIO1SET |= 1 << ADC;
	}

	return data;
}
uint16_t SPI0_read_2_byte(uint8_t slave)
{
	uint16_t data;
	if(slave == DAC)
	{
		FIO1CLR |= 1 << DAC;
		data = SPI_ADC_data_transfers_16bit(0);
		FIO1SET |= 1 << DAC;
	}else if (slave == ADC){
		FIO1CLR |= 1 << ADC;
		data = SPI_ADC_data_transfers_16bit(0);
		FIO1SET |= 1 << ADC;
	}

	return data;
}

unsigned char SPI_ADC_data_transfers_16bit (unsigned short data)
{
	unsigned char dat = 0;
	char i;

	//Write
	for(i = 16;  i > 0 ; i--)
	{
		if(data & (1 << (i - 1)))
			FIO1PIN |= 1 << ADC_DIN;
		else
			FIO1PIN &= ~(1 << ADC_DIN);

		//SCLK
		FIO1PIN |= 1 << ADC_SCLK;
		FIO1PIN &= ~(1 << ADC_SCLK);

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
	char i;

	//Write
	for(i = 8;  i > 0 ; i--)
	{
		if(data & (1 << (i - 1)))
			FIO1PIN |= 1 << ADC_DIN;
		else
			FIO1PIN &= ~(1 << ADC_DIN);

		//SCLK
		FIO1PIN |= 1 << ADC_SCLK;
		FIO1PIN &= ~(1 << ADC_SCLK);

		//read
		if(FIO1PIN & (1 << (ADC_DOUT)))
			dat |= 1 << (i - 1);
		else
			dat &= ~(1 << (i - 1));

	}  
	return dat;
}
