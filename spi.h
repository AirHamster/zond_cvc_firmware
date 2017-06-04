#include <stdint.h>
#include <stdint.h>
#include "LPC2300.h"
#include "defines.h"
#include "usart_console.h"

#define ADC_1CH 0x1010
#define ADC_2CH 0x1011
#define ADC_DIN 24
#define ADC_DOUT 23
#define ADC_SCLK 20

uint8_t SPI0_send_1_byte(uint8_t data);
uint16_t SPI0_send_2_byte(uint16_t data);
void SPI0_init(void);
void Delay(int value);
uint8_t spi_readStatus (void); 
unsigned char SPI_ADC_data_transfers_16bit (unsigned short data);

unsigned char SPI_ADC_data_transfers_8bit (unsigned char data);
