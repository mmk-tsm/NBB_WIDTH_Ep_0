//////////////////////////////////////////////////////
// P.Smith                          17/6/08
// Telnet functionality removed from main and transferred into this file
//
// P.Smith                          21/6/08
// close serial port before redirecting stdio
//
//
// P.Smith                          14/11/08
// write out the correct port when telnet connected.
// show username prompt when connected.
// there is a problem with connecting putty, it dump 21 characters into the read
// buffer before anything is entered by the user.
// this is corrected by setting g_nTelNetSecondsCounter to 2 and checking if it has gone to 
// zero when the data is read in, if it is not then it is ignored until
// the counters shows 0.
//
// P.Smith                          15/1/09
// added better messages for Telnet
// 
// P.Smith                          18/5/09
// remove check for g_nTelNetSecondsCounter, this was stopping the log in 
// from progressing occasionally.
//
// P.Smith                          2/7/09
// added RepeatedCommand to check for up arrow key.
// this repeats the command again.
// if the command is repeated, dump it out again
//////////////////////////////////////////////////////


#include "BatchCalibrationFunctions.h"
#include    "BatVars.h"
#include    "BatVars2.h"
#include "ucos.h"
#include "predef.h" 
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h> 
#include <htmlfiles.h>
#include <cfinter.h>
#include <smarttrap.h>
#include <tcp.h>
#include "InitComms.h"
#include <serial.h>
#include "TelnetCommands.h"
#include "TimeDate.h"
#include "Telnet.h"

extern 	char 	temp_string[];
extern 	char 	temp_string_copy[];


static DWORD   TelnetTaskStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );


#define TCP_LISTEN_PORT (23)     //Telnet port.

//////////////////////////////////////////////////////
// TelnetTask()          
//////////////////////////////////////////////////////

void TelnetTask( void *pd)  
//void TCPTask
{
#define TELNET_RX_BUFSIZE (80)
char TelnetRxBuffer[TELNET_RX_BUFSIZE];
char    cBuffer[200];
char    tBuffer[200];
    
int fdlisten = -1;
   fdTelnet = -1;
   while (1)
   {
      // Set up the listening TCP socket
      fdlisten = listen(INADDR_ANY, TCP_LISTEN_PORT, 5);
      if (fdlisten > 0)
      {
         IPADDR client_addr;
         WORD port;
         while(1)
         {
/* Wait to accept incoming connection. Display debug messages to
serial port. 
When a connection is terminated, the application will come back to
this accept() call and wait for another connection.
*/
//         iprintf("Wainting for connection on port %d...\n", TCP_LISTEN_PORT);
         
         fdTelnet = accept(fdlisten, &client_addr, &port, 0);
         // Display sign-on message to connecting client
         cBuffer[0] = '\0';
         sprintf(tBuffer,"\nTSM Telnet Session Started ");
         strcat( cBuffer, tBuffer );
         sprintf( tBuffer, "%02d/%02d/%04d\t%02d:%02d:%02d\t\n",g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
         strcat( cBuffer, tBuffer );
    
         char s[20];
            s[0] = 'a';
            s[1] = '\0';
         SerialClose(NBB_PANEL_COMMS_PORT);
         ReplaceStdio( 0, fdTelnet ); // in
         ReplaceStdio( 1, fdTelnet ); // out
         ReplaceStdio( 2, fdTelnet ); // err
         iprintf("%s\r\n",cBuffer);  // nbb--todo-- check length of buffer.
/* Loop while connection is valid. The read() function will return
0 or a negative number if the client closes the connection, so we
test the return value in the loop.
*/
         ShowUserNamePrompt();
         g_nTelNetSecondsCounter = 2;
         temp_string[0] = '\0';
         TelnetRxBuffer[0] = '\0';
         iprintf(":>");

         int n = 0;
         do {
               n = read(fdTelnet, TelnetRxBuffer, TELNET_RX_BUFSIZE);
               TelnetRxBuffer[n] = '\0';
               if(RepeatedCommand(TelnetRxBuffer))
               {
                   strcpy(TelnetRxBuffer,temp_string_copy);
                   strcpy(temp_string,temp_string_copy);
                   iprintf("%s",TelnetRxBuffer);
               }
               else
               {
                   ProcessTelnetCommand(TelnetRxBuffer,n);
               }
 
            } while (n > 0);
   // Don't foreget to close !
            close(fdTelnet);
         }
      }
   }
}
//end tj's
//////////////////////////////////////////////////////
// StartTelnet()  
// Starts Telnet task        
//////////////////////////////////////////////////////

void StartTelnet( void )  

{
   if(g_bTelnetEnable)
   {
      OSTaskCreate( TelnetTask,
                 0,
                 ( void * ) &TelnetTaskStk[USER_TASK_STK_SIZE],
                 ( void * ) TelnetTaskStk,
                 MAIN_PRIO - 1 );   //note higher priority at lower value.
   }

}


//////////////////////////////////////////////////////
// RepeatedCommand()  
// checks for repeated command uparrow key 1b,5b,41       
//////////////////////////////////////////////////////

BOOL RepeatedCommand( char* TelnetRxBuffer )  
{
    BOOL    bIsRepeated = FALSE;
    if((TelnetRxBuffer[0] == 0x1b) && (TelnetRxBuffer[1] == 0x5b) && (TelnetRxBuffer[2] == 0x41))
    {
        bIsRepeated = TRUE;
    }
    return(bIsRepeated);
}

