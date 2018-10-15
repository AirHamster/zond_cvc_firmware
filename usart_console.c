#include "defines.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xprintf.h"
#include "usart_console.h" 
#include "LPC2300.h"
#include "uart23xx.h"
#include "timers.h"
#include "spi.h"
/* #define GUI */
#define isdigit(c) (c >= '0' && c <= '9')
unsigned char RxCount,Index;
extern uint8_t native;
extern uint16_t volts, curr;
extern uint8_t getflag;
const char help_msg[] = "Plazma probe controller\n Usage:\n    start - start measurements\n    stop - finish measurements\n    set <voltage> - probe voltage setup\n    native - non-formated output\n    ascii - output in ascii presentation\n";


uint16_t hex_to_int(uint8_t c){
	uint16_t first = c / 16 - 3;
	uint16_t second = c % 16;
	uint16_t result = first*10 + second;
	if(result > 9) result--;
	return result;
}

uint16_t hex_to_ascii(uint16_t c){
	uint16_t high = hex_to_int(c >> 8) * 16;
	uint16_t low = hex_to_int(c & 0xFF);
	return high+low;
}
void process_command(char *cmd)
{
	uint16_t num;
	uint8_t lenth;
	double input;
#ifdef GUI
#else
		if(strncmp(cmd, "Z?", 2) == 0)
	{
		UART0_send("z\n", 2);
		//gpio_set(OP_AMP_PORT, OP_AMP_PIN);
		led_set(LED2);
	//	timer0_start();
	}    
	
	if(strncmp(cmd, "start", 5) == 0)
	{
		UART0_send("\nStarted\n", 9);
		gpio_set(OP_AMP_PORT, OP_AMP_PIN);
		led_set(LED1);
		read_curr();
		timer0_set_freq(1);
		timer0_start();
	}    

	/* Turn off amplifier */
	if(strncmp(cmd, "stop", 4) == 0)
	{
		//UART0_send("\nStopped\n", 9);
		gpio_clear(OP_AMP_PORT, OP_AMP_PIN);
		led_clear(LED1);
		timer0_stop();
		FIO1CLR |= 1 << DAC;
		SPI0_send_2_byte((0x1000 | 1382), DAC);
		FIO1SET |= 1 << DAC;
		
		adc_init();
	}
	/* Voltage setup  */
	if(strncmp(cmd, "set", 3) == 0)
	{

		lenth = strlen(cmd+4)-1;
		if (native == 1)
		{
		num = atoi(cmd + 4);
		}else{
		input = atof(cmd + 4);
		input = input/0.01846 + 1382;	
		num = (int)input;
		}
		UART0_send("\nOK\n", 4);
		UART0_send_byte(num >> 8);
		UART0_send_byte(num);
		dac_set_voltage(num);
	}
	if(strncmp(cmd, "get", 3) == 0)
	{
		//read_volt();
		lenth = strlen(cmd+4)-1;
		if (native == 1)
		{
		num = atoi(cmd + 4);
		}else{
		input = atof(cmd + 4);
		input = input/0.01846 + 1382;	
		num = (int)input;
		}
		dac_set_voltage(num);
		//UART0_send_byte(num >> 8);
		//UART0_send_byte(num);
		getflag = 1;
		
		
		//UART0_send_byte(volts>>8);
		//UART0_send_byte(volts & 0xFF);
		timer0_set_freq(100);
		led_set(LED1);
		
		timer0_start();
		//UART0_send("\nOK\n", 4);
		
	}
	/* Manual  */
	if(strncmp(cmd, "help", 4) == 0)
	{
		UART0_send(help_msg, sizeof(help_msg)-1);
	}
	/* Switching between output value presentation */
	if (strncmp(cmd, "native", 6) == 0)
	{
		native = 1;
	}
	if (strncmp(cmd, "ascii", 5) == 0)
	{
		native = 0;
	}
#endif
}
void UART0_send_byte(uint8_t byte)
{

	U0IER &= ~(1 << RBR_Enable );     // Disable RBR
	while ((U0LSR & (1 << 5)) == 0);//ждём пока освободиться регистр THR
	U0THR = byte;
	U0IER |= (1 << RBR_Enable );      // Re-enable RBR 
}
void UART0_send(unsigned char *BufferPtr, unsigned short Length )
{
	U0IER &= ~(1 << RBR_Enable );     // Disable RBR

	while ( Length != 0 )
	{
		// THRE status, contain valid data 
		while ((U0LSR & (1 << 5)) == 0);//ждём пока освободиться регистр THR
		U0THR = *BufferPtr;// в сдвиговый регистр положить данные
		BufferPtr++;
		Length--;
	}
	U0IER |= (1 << RBR_Enable );      // Re-enable RBR 

	return;
}



double atof(const char *s)
{
	// This function stolen from either Rolf Neugebauer or Andrew Tolmach. 
	// Probably Rolf.
	double a = 0.0;
	int e = 0;
	int c;
	uint8_t neg_flag = 0;
	if ((c = *s) == '-')
	{
		neg_flag = 1;
		*s++;
	}
	while ((c = *s++) != '\0' && isdigit(c)) {
		a = a*10.0 + (c - '0');
	}
	if (c == '.') {
		while ((c = *s++) != '\0' && isdigit(c)) {
			a = a*10.0 + (c - '0');
			e = e-1;
		}
	}
	if (c == 'e' || c == 'E') {
		int sign = 1;
		int i = 0;
		c = *s++;
		if (c == '+')
			c = *s++;
		else if (c == '-') {
			c = *s++;
			sign = -1;
		}
		while (isdigit(c)) {
			i = i*10 + (c - '0');
			c = *s++;
		}
		e += i*sign;
	}
	while (e > 0) {
		a *= 10.0;
		e--;
	}
	while (e < 0) {
		a *= 0.1;
		e++;
	}
	if (neg_flag == 1)
		a = a*(-1);
	return a;
}

