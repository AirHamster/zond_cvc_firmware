#include <stdint.h>
void dac_set_voltage(int8_t voltage);
void adc_init(void);
uint16_t adc_read_current(void);
uint16_t adc_read_voltage(void);
