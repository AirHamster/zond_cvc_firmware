
#include <stdint.h>
void set_voltage(int8_t volts);
void adc_init(void);
void dac_init(void);
uint16_t adc_read_current(void);
uint16_t adc_read_voltage(void);
