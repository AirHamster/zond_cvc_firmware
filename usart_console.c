#include "string.h"
#include "stdio.h"
#include "xprintf.h"
#include "usart_console.h" 
#include "LPC2300.h"
#include "uart23xx.h"
//UART0
//unsigned short UART0Status;
unsigned char UART0Count = 0;
unsigned char UART0TxEmpty = 1;
unsigned char UART0Buffer[BUFSIZE];
unsigned char UART0Str[MESSAGE_LENGTH];
unsigned char UART_TxStr[MESSAGE_LENGTH];

unsigned char RxMessage_Dev_ID;
unsigned char RxMessage_Command;
unsigned short RxMessage_Addr;
unsigned short RxMessage_Data;
unsigned short RxMessage_CRC;

unsigned char TxMessage_Dev_ID = DeviceID;
unsigned char TxMessage_Command = 0x19;
unsigned short TxMessage_Addr = 0xFAFA;
unsigned short TxMessage_Data = 0xCCBB;
unsigned short TxMessage_CRC = 0xD199;

unsigned char RxCount,Index;

void processCommand(char *cmd)
{
#ifdef DEBUG1
	xputs(cmd);
#endif
	char answer[20]="";
	if(strncmp(cmd, "start", 5) == 0)
	{
	}    

	/* Turn off amplifier */
	if(strncmp(cmd, "stop", 4) == 0)
	{
	}
	/* Voltage setup  */
	if(strncmp(cmd, "set", 8) == 0)
	{
	}

	/* Manual  */
	if(strncmp(cmd, "help", 4) == 0)
		xputs("Plazma probe controller:\n \tUsage:\n \t\tstart - start measurements\n  \t\tstop - finish measurements\n  \t\tset <voltage> - probe voltage setup\n");

}
void UART0_send(unsigned char *BufferPtr, unsigned short Length )
{
  U0IER &= ~(1 << RBR_Enable );     // Disable RBR
  
  while ( Length != 0 )
  {
    // THRE status, contain valid data 
    while (U0LSR & (1 << 5) ==0 );//ждём пока освободиться регистр THR
    UART0TxEmpty = 0;
    U0THR = *BufferPtr;// в сдвиговый регистр положить данные
     
    BufferPtr++;
    Length--;
  }
  
  UART0Count = 0;
  U0IER |= (1 << RBR_Enable );      // Re-enable RBR 
  
  return;
}


