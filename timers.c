#include <stdint.h>
#include "LPC2300.h"
#include "timers.h"
#include "interrupt.h"
#include "defines.h"
#include "uart23xx.h"
#include "adc_dac.h"
#include "usart_console.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define _FTOA_TOO_LARGE 1
#define _FTOA_TOO_SMALL 2
#define PRECISION 6
/* #define NATIVE */
#define ASCII
extern void gpio_set(uint8_t port, uint8_t pin);
extern void gpio_clear(uint8_t port, uint8_t pin);
extern uint8_t native, getflag, conv_number;
extern uint8_t channel;
extern uint64_t curr_big;
extern uint16_t volts, curr;
extern uint16_t curr_array[CONV_NUMBER];
extern uint16_t volt_array[CONV_NUMBER];
void Isr_TIM0(void)
{
	uint16_t dat;
	T0IR = 0x3F;
	uint8_t data;
	uint8_t num = 5;
	char *volt_ascii[50];
	char *curr_ascii[50];
	FIO1PIN |= (1 << ADC_SCLK);
	if (getflag == 0){
		if (channel == 1)
		{
			//read_volt();
		//	channel = 0;
		//	send_results();
		}else if (channel == 0)
		{
		//	read_curr();
		//	channel = 1;
		}
	}else{
		//UART0_send("\ngf\n", 4);
		if (conv_number == CONV_NUMBER + 1) {
			conv_number--;
			volts = read_volt();
			//read_volt();
		/* Need to select proper channel */
		//FIO1CLR |= 1 << ADC;
		//SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		//SPI0_send_2_byte(CONF_REG_VAL, ADC);
		//FIO1SET |= 1 << ADC;
		}else if (conv_number != 0){
			conv_number--;
			//curr_big += adc_read_current();
			curr_array[conv_number] = read_curr();
			volt_array[conv_number] = read_volt();
			//volts = read_volt();
		}else if (conv_number == 0){
			//curr = (curr_big/CONV_NUMBER);
			curr = process_array(curr_array);
			//volts = process_array(volt_array);
			conv_number = CONV_NUMBER + 1;
			getflag = 0;
			
					/* Need to select proper channel */
					read_curr();
		//FIO1CLR |= 1 << ADC;
		//SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		//SPI0_send_2_byte((CONF_REG_VAL | 1), ADC);
		//FIO1SET |= 1 << ADC;
		curr_big = 0;
		send_results();
		timer0_stop();
		led_clear(LED1);
		}
	}
	VICVectAddr = 0;
}
uint16_t read_volt(void)
{
		uint16_t v = adc_read_voltage();
		
		/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte(CONF_REG_VAL, ADC);
		FIO1SET |= 1 << ADC;
		
		return v;
}
uint16_t read_curr(void){
		
		uint16_t c = adc_read_current();
		
		/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte((CONF_REG_VAL | 1), ADC);
		FIO1SET |= 1 << ADC;
		
		return c;
}
void timer0_set_freq(uint8_t hz){
	T0TCR = 0;	/* Disable tim0 */
	if (hz == 100)
	{
		T0PR = 250;	/* Prescaler */
		T0MR0 = 720;	/* Top value (100 Hz) */
	}else{
		//T0MR0 = 72000;	/* Top value (40 Hz) */
		UART0_send("100hz\n", 6);
		T0PR = 250;	/* Prescaler */
		T0MR0 = 36000;	/* Top value (1 Hz) */
	}
}
void send_results(void){
	uint16_t dat;
	uint8_t data;
	uint8_t num = 5;
	char *volt_ascii[50];
	char *curr_ascii[50];
	if (native == 1)
		{
			/* UART0_send("\nOutput voltage: ",17 ); */
			UART0_send("\n\nVoltage, V  : ",16 );

			if (volts < 10000)
				num = 4;
			if (volts < 1000)
				num = 3;
			if (volts < 100)
				num = 2;
			if (volts < 10)
				num = 1;
			UART0_send(itoa(volts, volt_ascii,10), num);
			/* UART0_send("\nCurrent: ",10 ); */
			UART0_send("\nCurrent, mkA: ",15 );
			if (curr < 10000)
				num = 4;
			if (curr < 1000)
				num = 3;
			if (curr < 100)
				num = 2;
			if (curr < 10)
				num = 1;
			UART0_send(itoa(curr, curr_ascii,10), num);
		}else{
			uint8_t status;
			float fvolts;
			float fcurr;
			uint16_t volt_ascii_pointer;
			uint16_t curr_ascii_pointer;

			fvolts = (volts - 22267)/807.0;
			fcurr = (curr - 6037)/0.7927817;
			/* fvolts = (volts)/3.3; */
			/* fcurr = (curr - 1000); */


			UART0_send("V", 1);

			ftoa(fvolts, volt_ascii);

			UART0_send(volt_ascii, 6);

			UART0_send("C", 1);
			ftoa(fcurr, curr_ascii);
			UART0_send(curr_ascii, 6);
			UART0_send("\n", 1);

		}
}
void timer0_init(void)
{
	PCONP |= (1 << 1);	/* Power on tim0 */
	T0TCR = 0;	/* Disable tim0 */

	T0IR = (1 << 0);	/* Channel 0 match interrupt */
	T0MCR = 3;
	T0CTCR = 0;
	T0PR = 25;	/* Prescaler */
	T0MR0 = 72000;	/* Top value (5 Hz) */
	RegisterIrq(TIMER0_IRQn, (void *)Isr_TIM0, PRI_LOWEST);
}

void timer0_start(void)
{
	T0TCR |= 1;
	gpio_set(OP_AMP_PORT, OP_AMP_PIN);
}

void timer0_stop(void)
{
	T0TCR &= ~1;
	gpio_clear(OP_AMP_PORT, OP_AMP_PIN);
}

void ftoa(float num, char *str)
{
    int intpart = num;
    int intdecimal;
    int i;
    float decimal_part;
    char decimal[20];

    memset(str, 0x0, 20);
    if (num > (-1) && num < (0))
    {
        strcat(str, "-");
        itoa(num, str+1, 10);
    }else{
        itoa(num, str, 10);
    }
    strcat(str, ".");

    decimal_part = num - intpart;
    intdecimal = decimal_part * 1000000;

    if(intdecimal < 0)
    {
        intdecimal = -intdecimal;
    }
    itoa(intdecimal, decimal, 10);
    for(i =0;i < (PRECISION - strlen(decimal));i++)
    {
        strcat(str, "0");
    }
    strcat(str, decimal);
}

float calculateSD(uint16_t *data)
{
    float sum = 0.0, mean, standardDeviation = 0.0;

    int i;

    for(i=0; i<CONV_NUMBER; ++i)
    {
        sum += data[i];
    }

    mean = sum/CONV_NUMBER;

    for(i=0; i<CONV_NUMBER; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    return sqrt(standardDeviation/CONV_NUMBER);
}

float process_array(uint16_t *array)
{
    float result = 0.0, median = 0.0, median2 = 0.0, sd = 0.0;
    int i, counter = 0;
    
    for (i = 0; i < CONV_NUMBER; i++)
    {
       median += array[i];
    }
    median = median / CONV_NUMBER;
    
    sd = calculateSD(array);
    
    for (i = 0; i < CONV_NUMBER; i++)
    {
       if (!((array[i] < (median - sd)) || (array[i] > (median + sd))))
       {
           median2 += array[i];
           counter++;
       }
    }
    if (counter != 0)
    {
      return (median2 / counter);
    }else{
        return 0;
    }
}
