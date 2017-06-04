#include <stdint.h>
#include <stdint.h>
#include "LPC2300.h"
#include "defines.h"
#include "usart_console.h"
#include "spi.h"
#define WAIT_ON_SPIF         while (spi_readStatus() == 0) {} 

void dac_set_voltage(uint16_t voltage);
void adc_init(void);
uint16_t adc_read_current(void);
uint16_t adc_read_voltage(void);
