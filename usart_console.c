#include "defines.h"
#include "string.h"
#include "stdio.h"
#include "xprintf.h"
#include "usart_console.h" 
#include "LPC2300.h"
#include "uart23xx.h"
unsigned char RxCount,Index;
const char help_msg[] = "Plazma probe controller\n Usage:\n    start - start measurements\n    stop - finish measurements\n    set <voltage> - probe voltage setup\n";
void process_command(char *cmd)
{
	char answer[20]="";
	if(strncmp(cmd, "start", 5) == 0)
	{
		UART0_send("\nStarted\n", 9);
		gpio_set(OP_AMP_PORT, OP_AMP_PIN);
		led_set(LED2);
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
		dac_set_voltage(cmd+4);
	}

	/* Manual  */
	if(strncmp(cmd, "help", 4) == 0)
	{
		UART0_send(help_msg, sizeof(help_msg));
		/* UART0_send(S0SPCR, 1);
		 * UART0_send("\n", 1);
		 * UART0_send(S0PSR, 1);
		 * UART0_send("\n", 1);
		 * UART0_send(S0SPCCR, 1);
		 * UART0_send("\n", 1); */
	}
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


