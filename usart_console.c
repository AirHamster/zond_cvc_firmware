#include "defines.h"
#include "string.h"
#include "stdio.h"
#include <string.h>
#include "xprintf.h"
#include "usart_console.h" 
#include "LPC2300.h"
#include "uart23xx.h"
#include "timers.h"
#include "spi.h"
/* #define GUI */
unsigned char RxCount,Index;
const char help_msg[] = "Plazma probe controller\n Usage:\n    start - start measurements\n    stop - finish measurements\n    set <voltage> - probe voltage setup\n";


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
#ifdef GUI
#else
	if(strncmp(cmd, "start", 5) == 0)
	{
		UART0_send("\nStarted\n", 9);
		gpio_set(OP_AMP_PORT, OP_AMP_PIN);
		led_set(LED2);
	/* FIO1PIN |= (1 << ADC_SCLK); */
	/* FIO1CLR |= 1 << ADC; */
	/* SPI0_send_1_byte(WRITE_CONF_REG, ADC); */
	/* SPI0_send_2_byte(CONF_REG_VAL, ADC); */
	/* FIO1SET |= 1 << ADC; */
	/* FIO1CLR |= 1 << ADC; */
	/* SPI0_send_1_byte(WRITE_MODE_REG, ADC); */
	/* SPI0_send_2_byte(MODE_REG_VAL, ADC); */
	/* FIO1SET |= 1 << ADC; */
		timer0_start();
	}    

	/* Turn off amplifier */
	if(strncmp(cmd, "stop", 4) == 0)
	{
		UART0_send("\nStopped\n", 9);
		gpio_clear(OP_AMP_PORT, OP_AMP_PIN);
		led_clear(LED2);
		timer0_stop();
	}
	/* Voltage setup  */
	if(strncmp(cmd, "set", 3) == 0)
	{
	
		lenth = strlen(cmd+4)-1;
		num = atoi(cmd + 4);
		UART0_send("\nOK\n", 4);
		/* UART0_send_byte(num >> 8); */
		/* UART0_send_byte(num); */
		dac_set_voltage(num);
	}

	/* Manual  */
	if(strncmp(cmd, "help", 4) == 0)
	{
		UART0_send(help_msg, sizeof(help_msg)-1);
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
