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
		//op_amp enable
	led_set(LED2);
		timer0_start();
	}    

	/* Turn off amplifier */
	if(strncmp(cmd, "stop", 4) == 0)
	{
		//op_amp off
	led_set(LED1);
		timer0_stop();
	}
	/* Voltage setup  */
	if(strncmp(cmd, "set", 3) == 0)
	{
		set_voltage(cmd+4);
	}

	/* Manual  */
	if(strncmp(cmd, "help", 4) == 0)
		UART0_send(help_msg, sizeof(help_msg));

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


