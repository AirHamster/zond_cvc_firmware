#include <stdint.h>
#include "spi.h"
#include "LPC2300.h"



void SPI0_send_1_byte(uint8_t data, uint8_t slave)
{
	if(slave == DAC)
	{
		FIO1CLR |= 1 << DAC;
		S0SPDR = data;
		while(S0SPSR & 0x80 == 0);
		FIO1SET |= 1 << DAC;

	}else if (slave == ADC){
	
		FIO1CLR |= 1 << ADC;
		S0SPDR = data;
		while(S0SPSR & 0x80 == 0);
		FIO1SET |= 1 << ADC;
	}
	
}

void SPI0_send_2_byte(uint16_t data, uint8_t slave)
{
	if(slave == DAC)
	{
		//cs
		FIO1CLR |= 1 << DAC;
		while(S0SPSR & 0x80 == 0);
		S0SPDR = data >> 8;
		while(S0SPSR & 0x80 == 0);
		S0SPDR = data & 0xFF;
		FIO1SET |= 1 << DAC;
	}else if (slave == ADC){
		FIO1CLR |= 1 << ADC;
		while(S0SPSR & 0x80 == 0);
		S0SPDR = data >> 8;
		while(S0SPSR & 0x80 == 0);
		S0SPDR = data & 0xFF;
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
	PINSEL3 |= (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17);	/*MISO & MOSI pins */
	PINMODE3 |= (1 << 15) | (1 << 7);
}

uint8_t SPI0_read_1_byte(uint8_t slave)
{
	uint8_t data;
	if(slave == DAC)
	{
		FIO1CLR |= 1 << DAC;
		while(S0SPSR & 0x80 != 0x80);
		S0SPDR = 0;
		while(S0SPSR & 0x80 != 0x80);
		data = S0SPDR;
		FIO1SET |= 1 << DAC;
	}else if (slave == ADC){
		FIO1CLR |= 1 << ADC;
		while(S0SPSR & 0x80 != 0x80);
		S0SPDR = 0;
		while(S0SPSR & 0x80 != 0x80);
		data = S0SPDR;
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
		while(S0SPSR & 0x80 != 0x80);
		S0SPDR = 0;
		while(S0SPSR & 0x80 != 0x80);
		data = S0SPDR << 8;
		S0SPDR = 0;
		while(S0SPSR & 0x80 != 0x80);
		data |= S0SPDR;
		FIO1SET |= 1 << DAC;
	}else if (slave == ADC){
		FIO1CLR |= 1 << ADC;
		while(S0SPSR & 0x80 != 0x80);
		S0SPDR = 0;
		while(S0SPSR & 0x80 != 0x80);
		data = S0SPDR << 8;
		S0SPDR = 0;
		while(S0SPSR & 0x80 != 0x80);
		data |= S0SPDR;
		FIO1SET |= 1 << ADC;
	}
	
return data;
}
