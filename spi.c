#include "spi.h"
/* #define DEBUG_SPI */


uint8_t SPI0_send_1_byte(uint8_t data, uint8_t slave)
{
	uint8_t dat = 0;
	/* S0SPDR = data; */
	/* while (spi_readStatus() == 0);  */
	/* data = S0SPDR; */
	if (slave == ADC )
	{
	dat = SPI_ADC_data_transfers_8bit(data);
	}else if (slave == DAC)
	{
	dat = SPI_DAC_data_transfers_8bit(data);
	}
	return data;
}

uint16_t SPI0_send_2_byte(uint16_t data, uint8_t slave)
{
	uint16_t dat;
	/* S0SPDR = (data >> 8) & 0xFF; */
	/* while (spi_readStatus() == 0);  */
	/* dat = S0SPDR << 8; */
	/* S0SPDR = data & 0xFF; */
	/* while (spi_readStatus() == 0);  */
	if (slave == ADC )
	{
	dat = SPI_ADC_data_transfers_16bit(data);
	}else if (slave == DAC)
	{
	dat = SPI_DAC_data_transfers_16bit(data);
	}
	return dat;
}

void SPI0_init(void)
{
	PCONP |= (1 << 8);
	PCLKSEL0 |= (1<<17) | (1<<16);//=72Mhz/8 
	S0SPCR |= (1 << 5);	/*   Master mode*/
	S0SPCCR = 0x12; 	/* SPI0 perif clock divided by 18 to reach 500kHz */
	PINSEL3 |= ((1 << 9) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 16) | (1 << 17));/* 	[>MISO0, MOSI0, CLK0 as SPI pins<] */
	/* PINMODE3 |= (1 << 15) | (1 << 7); */
}

uint16_t SPI_ADC_data_transfers_16bit (uint16_t data)
{
	uint16_t dat = 0;
	uint16_t mask;
	char i;

	//Write
	mask = 0x8000;
	for(i = 16;  i > 0 ; i--)
	{
		//SCLK
		FIO1PIN &= ~(1 << ADC_SCLK);
		if(data & mask)
			FIO1PIN |= 1 << ADC_DIN;
		else
			FIO1PIN &= ~(1 << ADC_DIN);

		mask = mask >> 1;

		//read
		if(FIO1PIN & (1 << (ADC_DOUT)))
			dat |= 1 << (i - 1);
		else
			dat &= ~(1 << (i - 1));
		FIO1PIN |= 1 << ADC_SCLK;

	}  
	return dat;
}

unsigned char SPI_ADC_data_transfers_8bit (unsigned char data)
{
	unsigned char dat = 0;
	char i;

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

		//read
		if(FIO1PIN & (1 << (ADC_DOUT)))
			dat |= 1 << (i - 1);
		else
			dat &= ~(1 << (i - 1));

		FIO1PIN |= 1 << ADC_SCLK;
	}  
	return dat;
}
uint8_t spi_readStatus (void) {

	uint8_t SPIStatus;
	uint8_t abrt, modf, rovr, wcol, spif;

	SPIStatus = S0SPSR;
	abrt = (SPIStatus & 0x8 ) >> 3;
	modf = (SPIStatus & 0x10) >> 4;
	rovr = (SPIStatus & 0x20) >> 5;
	wcol = (SPIStatus & 0x40) >> 6;
	spif = (SPIStatus & 0x80) >> 7;
#ifdef DEBUG_SPI
	UART0_send("\nS0SPSR: ", 9);
	UART0_send(&SPIStatus, 1);

	if(abrt==1) { 
		UART0_send("Slave Abort occurred\n", sizeof("Slave Abort occurred\n"));
	}

	if(modf==1) { 
		UART0_send("Mode Fault occurred\n", sizeof("Mode Fault occurred\n"));
	}

	if(rovr==1) { 
		UART0_send("Read Overrun occurred\n", sizeof("Read Overrun occurred\n"));
	}

	if(wcol==1) { 
		UART0_send("Write Collision occurred\n", sizeof("Write Collision occurred\n"));
	}

	if(spif==1) { 
		UART0_send("\nSPIF is 1 - clearing\n", sizeof("\nSPIF is 1 - clearing\n")-1);
	}
#endif

	S0SPCR = S0SPCR; // write cr register to clear status bit
	return(spif);
}
uint16_t SPI_DAC_data_transfers_16bit (uint16_t data)
{
	uint16_t dat = 0;
	uint16_t mask;
	char i;

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
