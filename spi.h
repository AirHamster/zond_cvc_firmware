#include <stdint.h>
#define ADC_1CH 0x1010
#define ADC_2CH 0x1011

void SPI0_send_1_byte(uint8_t data, uint8_t slave);
void SPI0_send_2_byte(uint16_t data, uint8_t slave);
void SPI0_init(void);
uint8_t SPI0_read_1_byte(uint8_t slave);
uint16_t SPI0_read_2_byte(uint8_t slave);
