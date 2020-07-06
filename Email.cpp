//////////////////////////////////////////////////////
// Filename = Email.cpp
//
// Functions for sending email.
// 
// 
// M.McKiernan                          12-06-2006
// First Pass
//
// P.Smith                              19/06/06
// Added subject parameter to testemail
// pass email address and subject into the testemail function.
//
// P.Smith                              6/2/07
// Remove printfs
//        
// P.Smith                          30/1/08
// correct compiler warnings
//

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <mailto.h>
#include <taskmon.h>
#include "string.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars2.h"



// To test email.
char var_user[80];
char var_svrip[80];
char var_from[80];
char var_to[80];
char var_sub[80];
char var_body[80];
int result;
extern CalDataStruct    g_CalibrationData;

void LastResult( int sock, PCSTR url )
{
   writestring( sock, "<TABLE BORDER=\"1\"><TR><TD>" );
   if ( result )
   {
      writestring( sock, "Mail sent to</TD><TD>\r\n" );
   }
   else
   {
      writestring( sock, "Failed to send mail</TD><TD>\r\n" );
   }

   writestring( sock, var_to );
   writestring( sock, "</TR><TR><TD>from:</TD><TD>\r\n" );
   writestring( sock, var_from );
   writestring( sock, "</TR><TR><TD>Subject:</TD><TD>\r\n" );
   writestring( sock, var_sub );
   writestring( sock, "</TR><TR><TD>Body </TD><TD>\r\n" );
   writestring( sock, var_body );
   writestring( sock, "</TD></TR></TABLE>\r\n" );
}




int MyDoPost( int sock, char *url, char *pData, char *rxBuffer )
{
   var_user[0] = 0; 
   var_svrip[0] = 0;
   var_from[0] = 0; 
   var_to[0] = 0;   
   var_sub[0] = 0;  
   var_body[0] = 0; 

//   printf( "Initiating extraction of post data\r\n" ); //nbb--testonly--

   if ( ExtractPostData( "user", pData, var_user, 80 ) <= 0 )
   {
//      printf( "Error in ExtractPostData() for \"user\"\r\n" ); //nbb--testonly--
   }
   
   if ( ExtractPostData( "svrip", pData, var_svrip, 80 ) <= 0 )
   {
//      printf( "Error in ExtractPostData() for \"svrip\"\r\n" ); //nbb--testonly--
   }

   if ( ExtractPostData( "from", pData, var_from, 80 ) <= 0 )
   {
//      printf( "Error in ExtractPostData() for \"from\"\r\n" ); //nbb--testonly--
   }

   if ( ExtractPostData( "to", pData, var_to, 80 ) <= 0 )
   {
//      printf( "Error in ExtractPostData() for \"to\"\r\n" ); //nbb--testonly--
   }

   if ( ExtractPostData( "sub", pData, var_sub, 80 ) <= 0 )
   {
//      printf( "Error in ExtractPostData() for \"sub\"\r\n" ); //nbb--testonly--
   }

   if ( ExtractPostData( "body", pData, var_body, 80 ) <= 0 )
   {
//      printf( "Error in ExtractPostData() for \"body\"\r\n" ); //nbb--testonly--
   }
   
//   printf( "End of post data extraction\r\n" ); //nbb--testonly--

   result = SendMail( AsciiToIp( var_svrip ), /*IP address of the POP server */
      var_user, var_from, var_to, var_sub, var_body );

   RedirectResponse( sock, "SENT.HTM" );
   return 0;
}

void RegisterPost()
{
   SetNewPostHandler( MyDoPost );
}


   
void TestEmail(char cEmailAddress[EMAIL_STRING_SIZE],char cSubject[EMAIL_STRING_SIZE])
{
int result;
//   iprintf("\n %s print buffer",g_arrcEmailBuffer); //nbb--testonly--
   char *ptrEmailBuffer = g_arrcEmailBuffer; 
   
//   printf( "Email Application started\n" ); //nbb--testonly--
      
      result = SendMail( AsciiToIp( g_CalibrationData.m_cEmailMailServer ), /*IP address of the POP server */
     g_CalibrationData.m_cEmailUserName  /*var_user*/, g_CalibrationData.m_cEmailFromEmailAddress /*var_from*/, cEmailAddress /*var_to*/, cSubject /*var_sub*/, ptrEmailBuffer /*var_body*/ );
//     iprintf("\n result is %d",result); //nbb--testonly--
   
   RegisterPost();

}

