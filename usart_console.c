#include "string.h"
#include "stdio.h"
#include "xprintf.h"
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

