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
			read_volt();
			channel = 0;
			send_results();
		}else if (channel == 0)
		{
			read_curr();
			channel = 1;
		}
	}else{
		//UART0_send("\ngf\n", 4);
		if (conv_number == 101) {
			conv_number--;
			volts = adc_read_voltage();
		//UART0_send_byte(volts>>8);
		//UART0_send_byte(volts & 0xFF);
		/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte(CONF_REG_VAL, ADC);
		FIO1SET |= 1 << ADC;
		}else if (conv_number != 0){
			conv_number--;
			curr_big += adc_read_current();
		}else if (conv_number == 0){
			curr = (curr_big/100);
			conv_number = 101;
			getflag = 0;
			
					/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte((CONF_REG_VAL | 1), ADC);
		FIO1SET |= 1 << ADC;
		curr_big = 0;
		send_results();
		timer0_stop();
		led_clear(LED1);
		}
	}
	VICVectAddr = 0;
}
void read_volt(void)
{
		volts = adc_read_voltage();
		
		/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte(CONF_REG_VAL, ADC);
		FIO1SET |= 1 << ADC;
}
void read_curr(void){
			curr = adc_read_current();
		
		/* Need to select proper channel */
		FIO1CLR |= 1 << ADC;
		SPI0_send_1_byte(WRITE_CONF_REG, ADC);
		SPI0_send_2_byte((CONF_REG_VAL | 1), ADC);
		FIO1SET |= 1 << ADC;
}
void timer0_set_freq(uint8_t hz){
	T0TCR = 0;	/* Disable tim0 */
	if (hz == 100)
	{
		T0PR = 250;	/* Prescaler */
		//T0MR0 = 28800;	/* Top value (100 Hz) */
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

			fvolts = (volts - 9211)/1065.2;
			fcurr = (curr - 6037)/0.7927817;
			/* fvolts = (volts)/3.3; */
			/* fcurr = (curr - 1000); */

			//UART0_send("\n\nVoltage, V  : ",16 );
			UART0_send("V", 1);
			/* sprintf(volt_ascii, "%+f", fvolts); */
			ftoa(fvolts, volt_ascii);
			//my_ftoa(fvolts, volt_ascii);
			UART0_send(volt_ascii, 6);
			//UART0_send("\nCurrent, mkA: ",15 );
			UART0_send("C", 1);
			/* sprintf(curr_ascii, "+%f", fcurr); */
			ftoa(fcurr, curr_ascii);
			//ftoa2(fcurr, curr_ascii, 4);
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
/*
void ftoa(float num, char *str)
{
	int intpart = num;
	int intdecimal;
	int i;
	float decimal_part;
	char decimal[20];

	memset(str, 0x0, 20);
	itoa(num, str, 10);

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
*/
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
