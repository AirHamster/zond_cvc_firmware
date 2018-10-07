#define CONV_NUMBER 50

void timer0_init(void);
void timer0_start(void);
void timer0_stop(void);
void timer0_set_freq(uint8_t hz);
void Isr_TIM0(void);
void ftoa(float num, char *str);
void read_volt(void);
void read_curr(void);
void send_results(void);
char * my_ftoa(float value, char *result);
int ftoaEngine(float value, char *buffer, int presc);
float process_array(uint16_t *array);
float calculateSD(uint16_t *data);
