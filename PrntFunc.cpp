///////////////////////////////////////////////////////////
// PrinterFunctions.c
//
// Displays the last Order report.
//
// Entry:   Void.
//
// Exit:    Void.
//
// M.Parks                                  12-05-2000
// First pass
// M.McKiernan                              15-12-2004
// Editing for Batch Blender application.  
// Printer buffer now a global character array.
// 
// P.Smith                                  11/1/06
// corrected warnings
// removed     int i in void StrobeData( void )
// removed unsigned int nData in    void SerialPrintHandler( void )
// nCol,i,removed from int CopyFloatDataJustified
//
// P.Smith                      26/1/06
// first pass at netburner hardware conversion.
// BooL -> BOOL
// included  <basictypes.h>
// included  "NBBGpio.h"
// remove #include "Hardware.h"
// far_strlen -> strlen
// included  "ConversionFactors.h"
// unsigned char *Buf to char *Buf
// comment out void SerialPrintHandler( void ) & serial based stuff
// add structMeasurementUnits  g_MeasurementUnits; temporarly
//
// P.Smith                      3/2/06
// structMeasurementUnits  g_MeasurementUnits; changed to extern structMeasurementUnits  g_MeasurementUnits  
// this variable was doubly defined.
//
// P.Smith                      16/1/07
// Removed refernces to Duart parallel port.
//
// P.Smith                      14/10/08
// IsPrinterBusy & PollPrinterAck not returning a value, return true for now.
///////////////////////////////////////////////////////////
#include <stdio.h>
#include <basictypes.h>

//nbb #include "Hardware.h"
#include "General.h"

#include "PrinterFunctions.h"
#include "ftoa.h"
#include "16R40C.h"
#include    "ConversionFactors.h"
#include "SCIComms.h"
#include "NBBGpio.h"
#include "InitComms.h"
#include <string.h>

// Externally defined global variables

// Locally declared global variables
//char  *g_pPrinterBuffer;
char    g_cPrinterBuffer[PRINT_BUFFER_SIZE];
int     g_nPrinterDataIndex = 0;
BOOL    g_bPrintInProgress = FALSE;
BOOL    g_bPrintAssembleInProgress = FALSE;
char    g_cPrinterCommand;
char g_cBlankLine[MAX_PRINT_WIDTH+1];


BOOL g_bSerialPrintInProgress = FALSE;
BOOL    g_bSerialPrintAssembleInProgress = FALSE;
int     g_nSerialPrinterDataIndex = 0;
extern  structMeasurementUnits  g_MeasurementUnits;
extern WORD g_lRTCCount;

//
// Parallel Port Initialisation.
//
void InitialiseParallelPrinter( void )
{
}        

//
// SEND CHARACTER TO PARALLEL PORT.    
//
void PrintSingleCharacter( char cValue )
{
}

//
// Checks for a print in progress or a print
// being formatted.
//
// Entry: Void
//
// Exit:    TRUE if print can proceed
//          FALSE if busy.
//
BOOL CanWePrint( void )
{
    if( g_bPrintInProgress || g_bPrintAssembleInProgress )
        return FALSE;
    else
    {
        g_bPrintAssembleInProgress = TRUE;
        return TRUE;
    }
}

//
// Enables the print
//
void EnableParallelPrint( void )
{
    g_bPrintInProgress = TRUE;
    g_nPrinterDataIndex = 0;
}

//
// The strobe line needs to be low for 0.5uS.
// Running at 4Mha with 4 cycles per instruction
// gives us an adequate delay in the code execution
// alone without the need for a delay routine.
//
void StrobeData( void )
{
}

//
// Checks printer busy status
//
// Entry.   Void
//
// Exit:    TRUE is busy
//          FALSE if ready
//
BOOL IsPrinterBusy( void )
{
  return(TRUE);
}
        
//
// Checks printer ack line.
//
// Entry:   Void
//
// Exit:    TRUE if ack is true
//          FALSE otherwise
//        
BOOL PollPrinterAck( void )
{
    return(TRUE);
}


//
// Parallel printer interrupt handler.
//
void ParallelPrintHandler( void )
{
     g_lRTCCount++;
    // Is printing required
    if( g_bPrintInProgress )
    {
       // Send a character if the printer isn't busy.
        if( !IsPrinterBusy() )
        {
            // Ensure that the buffer pointer is valid
//          if( g_pPrinterBuffer )
//          {
                // Print the next character
//              PrintSingleCharacter( *(g_pPrinterBuffer + g_nPrinterDataIndex) );
                PrintSingleCharacter( g_cPrinterBuffer[g_nPrinterDataIndex] );
                
                // Move on to the next character in the buffer
                g_nPrinterDataIndex++;

                // If this is the last character, free the memory allocated to the buffer
                // and turn off the printer interrupts.
//              if( *(g_pPrinterBuffer + g_nPrinterDataIndex) == ASCII_NULL )
                if( g_cPrinterBuffer[g_nPrinterDataIndex] == ASCII_NULL )
                {
//                  free( g_pPrinterBuffer );

                    g_bPrintInProgress = FALSE;

                    g_bPrintAssembleInProgress = FALSE;
                }
//          }
        }
    }
}

/*nbb

// Serial printer interrupt handler.
//
void SerialPrintHandler( void )
{

    // Is printing required
    if( g_bSerialPrintInProgress )
    {
        // Send a character if the printer isn't busy.
//      if( !IsPrinterBusy() )
        if( IsSCITxEmpty() )    //true => empty we can Tx a char.
        {
            // Ensure that the buffer pointer is valid
//          if( g_pPrinterBuffer )
//          {
                // Print the next character
//              PrintSingleCharacter( *(g_pPrinterBuffer + g_nPrinterDataIndex) );

//              SCITransmitSingleCharacter( g_cPrinterBuffer[g_nSerialPrinterDataIndex] );
//              nData = (int)g_cPrinterBuffer[g_nSerialPrinterDataIndex];       // note casting to int.
//              *(SERIAL_PORT_SCI + SCI_STATUS) = nData;
                TransmitCharSCI( g_cPrinterBuffer[g_nSerialPrinterDataIndex] );
                // Move on to the next character in the buffer
                g_nSerialPrinterDataIndex++;

                // If this is the last character, free the memory allocated to the buffer
                // and turn off the printer interrupts.
//              if( *(g_pPrinterBuffer + g_nPrinterDataIndex) == ASCII_NULL )
                if( g_cPrinterBuffer[g_nSerialPrinterDataIndex] == ASCII_NULL )
                {
//                  free( g_pPrinterBuffer );

                    g_bSerialPrintInProgress = FALSE;

                    g_bSerialPrintAssembleInProgress = FALSE;
                }
//          }
        }
    }
}

// Enables the print
//
void EnableSerialPrint( void )
{
    g_bSerialPrintInProgress = TRUE;
    g_nSerialPrinterDataIndex = 0;

    EnableTxSCI();
    EnableSCITxInterrupt();             // enable sci tx interrupt.

}

nbb*/
 
//--------------------------------------------------------------------------
// @MFUNC   CopyFloatDataJustified
//
// @PARM    pData | nRow | nColumn
//
// @RDESC   VOID
//--------------------------------------------------------------------------
int CopyFloatDataJustified( char *Buf, double fData, int nDigits, int nDecimals, int nColumn, int nJustification, int nUnits )
{
    char        cFormat[20];
    int     nBlanks;
    int     nOffset;
    int     nLength;        
    double  fConversion;

    fConversion =  MetricToImperial( fData, nUnits );

    // Convert to an ascii string
    ftoa( cFormat, fConversion, nDigits, nDecimals );

    nLength = strlen( cFormat );

    switch( nJustification )
    {
    case JUSTIFY_LEFT:
        // No need to do anything here.
        // text is displayed from the specified column on,
        nOffset = nColumn ;
        break;
    case JUSTIFY_RIGHT:
        {
            // Calculate the length of the string and
            // subtract it from the specified column
            // If it gives a negative column start at zero.

            int nCol = nColumn - nLength;
            if( nCol < 0 )
                nCol = 0;

            nOffset = nCol;
        }
        break;
    default:
        nOffset = nColumn ;         //let default = J.L.
        break;
    }

    // Copy the string into the display buffer
    // setting the changed flag for each byte
     nBlanks = nOffset - strlen(Buf);
    ClearLine( nBlanks );           // generate a blank string of length nBlanks

//  for( i = 0; i < nBlanks; i++ )
//  {
        strcat( Buf, g_cBlankLine);     
//  }

    strcat( Buf, cFormat);

    return nOffset;
}

//////////////////////////////////////////////////////////////////////
// ClearLine
//
// Clears line of length nLength, inserts 0 at end.  (for sprintf)
//
// Entry:   nLength
//
// Exit:    void
//
// M.McKiernan                              09-12-2003
// First pass.
//////////////////////////////////////////////////////
void ClearLine( int nLength )
{
    int i;
    if (nLength > MAX_PRINT_WIDTH)
        nLength = MAX_PRINT_WIDTH;              // max length of line is MAX_PRINT_WIDTH, i.e. width of paper.

    for( i = 0; i < nLength; i++ )
    {
        g_cBlankLine[i] = ASCII_BLANK;

    }
        g_cBlankLine[nLength] = 0;
}



/*
PR_CHAR:                  ;subroutine to send out one byte(CHAR in A) to SCI
        PSHA     
CHKSTS2 LDAB   #HARDWAREBANK    ; HARDWARE BANK
        TBZK                    ; STORE BANK NO          
        LDZ     #SCSR           ;read SCI status reg to check/clear TDRE bit
        LDAA    0,Z
;        LDAA    SCSR           ;read SCI status reg to check/clear TDRE bit
        ANDA    #$01           ;check only the TDRE flag bit
        BEQ     CHKSTS2        ;if TDR is not empty, go back to check it again
        PULB
        CLRA
        LDZ     #SCDR
        STD     0,Z           ;
;        STD    SCDR           ;transmit one ASCII character to the screen
        RTS                    ;~;finish sending out byte
#define  SCI_SCCR0 (*((int *)0xFFC08))      // PS
#define  SCI_SCCR1 (*((int *)0xFFC0A))      // PS
#define  SCI_SCSR  (*((int *)0xFFC0C))      // PS
#define  SCI_SCDR  (*((int *)0xFFC0E))      // PS
;*****************************************************************************;
; FUNCTION NAME : SCISERIALINTERRUPT        From SCIMON.asm                                  ;  
; FUNCTION      : HANDLER SCI SERIAL INTERRUPT.                               ;
;               :                                                             ;
;*****************************************************************************;


SCISERIALINTERRUPT:
        PSHM    D,E,X,Y,Z,K
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO          
;        LDD    SCCR1
        LDZ     #SCCR1
        LDD     0,Z
        ANDD   #SCIRECEIVEINTERRUPTENABLED
        BEQ     NORECEIVE                
;        LDD     SCSR           ;read SCI status reg to check/clear TDRE bit
        LDZ     #SCSR           ;read SCI status reg to check/clear TDRE bit
        LDD     0,Z
        ANDD    #SCIRECEIVERREADYBIT  ;
        BEQ     RECEIVEERROR          ; NO RECEIVE ERROR.
        JSR     MBRXBHNDSCI           ; MODBUS RECEIVE HANDLER.
NORECEIVE:
;        LDD    SCCR1
        LDZ    #SCCR1
        LDD     0,Z
        ANDD     #SCITRANSMITINTERRUPTENABLED
        BEQ     NOTRANSMIT                     ; NO TRANSMIT.
;        LDD     SCSR           ;read SCI status reg to check/clear TDRE bit
        LDZ     #SCSR           ;read SCI status reg to check/clear TDRE bit
        LDD     0,Z
        ANDD    #SCITRANSMITREADYBIT           ;check only the TDRE flag bit
        BEQ     TRANSMITERROR              ; NO TRANSMISSION.       
        JSR     SCINETWORKTX
        BRA     XITSCI                ; EXIT SCI INTERRUPT HANDLER.
NOTRANSMIT:
XITSCI  PULM    D,E,X,Y,Z,K
        RTS


RECEIVEERROR:
        NOP
        BRA     XITSCI                  ; EXIT INTERRUPT.
                

TRANSMITERROR:
        NOP
        BRA     XITSCI                  ; EXIT INTERRUPT.
                




ENPR_IRQ:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO          
        LDZ     #SCCR1
        LDD     0,Z
;        LDD    SCCR1
        ORD     #$0080          ;SCI TXMIT INT. ENABLED
        STD     0,Z           ;
;        STD     SCCR1           ;
        RTS

DSPR_IRQ:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO          
        LDZ     #SCCR1
        LDD     0,Z
;        LDD    SCCR1
        ANDD    #$FF7F          ;SCI TXMIT INT. DISABLED
        STD     0,Z           ;
;        STD     SCCR1           ;
        RTS
*/        

