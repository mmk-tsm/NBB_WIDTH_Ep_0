//////////////////////////////////////////////////////
// PrntDiag     Print Diagnostics
//
// Prints the cycle diagnostics.
//
// Entry:   Void.
//
// Exit:    TRUE if able to print
//          FALSE if printer is busy.
//
// M.McKiernan                              15-12-2004
// First Pass - edited from PrntConf of CP2000.
//
// P.Smith                                8/11/05
// added PrintResetSource
// Print old style diagnostics to allow files to be analysed
//
// P.Smith                                5/5/06
// print set and actual grams per meter in diagnostics printout.
//
//
// P.Smith                                11/5/06
// added void PrintModbusMessage( void ) to print receive buffer.
//
// P.Smith                                11/5/06
// print more diagnostic data for cycle printing, g/m, cycle/load time, line speed etc
//
// P.Smith                      1/6/06
// first pass at netburner hardware conversion.
// BooL -> BOOL
// included  <basictypes.h>
////removed ServiceWatchdogTimer();     // service the watch dog timer
//
// P.Smith                      20/6/06
// make PrintResetSource ready for printing initial reset message
// implement modbus print diagnostic message
//
// P.Smith                      30/6/06
// PrintResetSource tidied up.
//
// P.Smith                      18/7/06
// print of cycle diagnostics is tidied up.
//
// P.Smith                      7/11/06
// print revision no of power up message.
//
// P.Smith                      5/2/07
// #include "Mtttymux.h"
// remove printfs
// tested printf & SetMitty
//
// P.Smith                      11/4/07
// after call to WriteDiagnosticDataToSDCard, ReInitialisePinsUsedBySDCard called
//
// P.Smith                      14/5/07
// Move WriteDiagnosticDataToSDCard inside diagnostics check.
//
// P.Smith                      28/5/07
// Only log to SD card if diagnostics is enabled and diagnostic password is entered.
// only print to mitty if shutdown time is set to 31
//
// P.Smith                      14/6/07
// allow a print of a certain no of components.
//
// P.Smith                      21/06/07
// PrintModbusMessage modified to print using a for loop
// g_CalibrationData.m_nCycleCounter now in non volatile memory storage.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          3/10/07
// print batch time and batch length on diagnostics
//
// P.Smith                          7/12/07
// if g_bShowInstantaneousWeight is false,copy inst component weight to buffer
// otherwise copy the instantaneous to buffer.
//
// P.Smith                          22/1/08
// print start up
// print normal or fill by weight for all components.
//
// P.Smith                          25/1/08
// print statistical data for components sample, sample counter, max, min, mean, standard deviation
// print all component weights in grams, required for graphing
//
// P.Smith                          19/2/08
// added check for g_CalibrationData.m_nLogToSDCard to indicate that printing should take place.
//
// P.Smith                          25/2/08
// remove csv version call for now by check if(0) before it
//
// P.Smith                          22/2/08
// print tacho inst ,average, per error to log file.
//
// P.Smith                          7/3/08
// added load cell tare counts and load cell counts to logged file
// when printing out the weight in the hopper use g_fOneSecondHopperWeight
// instead of the value calculated at the end of the cycle from the addition of
// the component weights.
//
// P.Smith                          20/3/08
// update CSVVersion to allow csv data to be sorted in excel.
//
// P.Smith                          27/3/08
// use g_ProcessData for cycle counter
//
// P.Smith                          23/4/08
// cycle counter renamed to g_nCycleCounter
// use g_fComponentActualPercentage if g_bShowInstantaneousWeight otherwise
// use g_fComponentActualPercentageRR
//
// P.Smith                          23/6/08
// check for Telnet option to allow diagnosics to be sent to Telnet
//
// P.Smith                          17/7/08
// call WriteDiagnosticDataToSDCard(); at end of reset message to show when a reset
// has occurred in the log file.
// finish off csv version of the log file, add g/m etc to csv log file
// if Telnet enabled, then use iprinf to print out diagnostic log info.
// add power on reset to event log
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms extern
//
// P.Smith                          23/7/08
// remove ReInitialisePinsUsedBySDCard
//
// P.Smith                          11/9/08
// print component retries in logged data
//
// P.Smith                          26/9/08
// temperature is added to log.
//
// P.Smith                          9/10/08
// print batch weight
//
// P.Smith                          13/10/08
// add liquid additive diagnostic print,calc set kg/hr, kg/hr calculation
// liquid additive d/a value
//
// P.Smith                          14/10/08
// too many variables in PrintControlDiagnostics sprintf
//
// P.Smith                          15/10/08
// copy write of hardware id to log, should be writing 8 chars not 7
//
// P.Smith                          13/11/08
// modify PrintModbusMessage to read a pointer to the rxbuff address and then
// print the data in a readable format.
// PrintModbusTCPMessage formed to print modbus tcp message to telnet if enabled
// make buffer size 500 inside function.
// in write of sd card data check if g_bSDCardAccessInProgress set,g_bWriteDiagnosticsToSDCard
// set to allow the write of the data when sd card write has finished.
// if g_bPrintCycleDataToTelnet set, write diagnostics data to telnet.
//
// P.Smith                          18/11/08
// check length of print buffer approx 3000
//
// P.Smith                          25/11/08
// add power on reset source to event log, allow for unknown source.
//
// P.Smith                          25/11/08
// dump out standard deviation % to log file.
// increase buffer size in PrintModbusMessage & PrintModbusTCPMessage
// improve printout
//
// P.Smith                          8/12/08
// a problem has been seen in PrintResetSource in that the inst
// g_cResetStatusRegister = sim.reset.rsr was setting g_sHourHistoryNetworkComms.m_lGoodTx
// to a strange value, when this was remove, the value was okay. nbb--todo-- needs to be
// resolved.
//
// P.Smith                          9/12/08
// make sure to read  reset status from cResetStatusRegister, not g_cResetStatusRegister
//
// P.Smith                          15/1/09
// added WriteLogDataToUSB to write log to usb
// name change g_bSDCardAccessInProgress to g_bTelnetSDCardAccessInProgress
//
// P.Smith                          16/1/09
// change extern  for g_cResetStatusRegister to char
//
// P.Smith                          10/2/09
// move usb logging outside of sd card log check.
//
// P.Smith                          26/3/09
// print panel software revision no and sbb hardware serial no
// added calculated max kg/hr possible using the cycle load time
//
//
// P.Smith                          1/4/09
// added csv version required for Taiwan customer
//
// P.Smith                          2/4/09
// correct double printout of actual weight
// make csv option the standard
//
// P.Smith                          23/4/09
// correct printout of modbus diagnostics, it was originally showing the
// wrong addresses for the data.
// check g_CalibrationData.m_bLogFormatIsCSV for csv version
// this will be disabled for now
//
// P.Smith                          13/5/09
// call ShowPrompt after print of log data
//
// P.Smith                          18/5/09
// store null to NBB_SERIAL_NUMBER_SIZE-1 not NBB_SERIAL_NUMBER_SIZE
// check for m_bLogToSDCard before printing to sd card log.
//
// P.Smith                          30/6/09
// a problem has been seen where the download of recipe being effected
// by Telnet.
// the telnet dump of the modbus message is now dumped out in the foreground
// instead of on the interrupt system
// as the panel version number is transmitted continuously a check is done for
// this and it is removed from the diagnostics

// P.Smith                          26/5/09
// add log of csv version and text file version.
// added pulses to log.
// if top up mode print out 8 components.
// print set weight to  csv log
//
// P.Smith                          26/5/09
// added blender name to batch log.
// correct print of set and actual weights in top up mode.
//
// P.Smith                          1/9/09
// added RecordChangedModbusData function to record a change to the modbus
// data. show that panel has been written to when recording the data
// check that string length for g_cModbusEventBuffer is less than the max
//
// P.Smith                          7/9/09
// added calculation of load time from pit based counter.
// add in latency plus one second and derate by 0.85
// add in latency to this time
// derate batch size by 0.85
//
// P.Smith                          10/9/09
// added line feed to RecordChangedModbusData
//
// P.Smith                          17/9/09
// check for LOG_BATCH_LOG_TO_SD_CARD in g_CalibrationData.m_nLogToSDCard
//
// P.Smith                          17/10/09
// removed set point data from RecordChangedModbusData, the log was getting too
// conjested, copy max possible kg/hr to g_fMaxKgPHrPossible
//
// P.Smith                          16/11/09
// Add settled a/d averaged reading to the log
// log a/d reading recorded during the stability test, including max / min values
//
// P.Smith                          7/12/09
// remove increase /decrease from modbus recorded change.
//
// P.Smith                          15/12/09
// correct compiler warning when printing longs for stability data
// remove strcat( g_cPrinterBuffer, ASCII_NULL );, causing a compiler warning.
//
// P.Smith                          6/1/10
// move write to usb outside check for log to sd card.
//
// P.Smith                          19/3/10
// correct printing of top up component weight, the weight that is printed
// is got from g_nTopUpComponent.
// when the top up component was any other component other than 1, it was showing
// 0 as the target for component 1
//
// P.Smith                          6/4/10
// print function running with power on reset message.
//
// P.Smith                          13/4/10
// added printing of license for blender
//
// P.Smith                          20/4/10
// correct print of diagnostics write from modbus.
// correct the checksum and the number of words written.
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <basictypes.h>
#include <Stdlib.h>
#include <Stdio.h>
#include <String.h>

#include "SerialStuff.h"
#include "General.h"
#include "TimeDate.h"
#include "ftoa.h"
#include "16R40C.h"
#include    "ConversionFactors.h"
#include "BatVars.h"
#include "BatchCalibrationFunctions.h"
#include    "SetpointFormat.h"


#include "PrinterFunctions.h"
#include "PrntDiag.h"
#include    "BatchMBIndices.h"
#include "BatVars2.h"
#include <C:\Nburn\MOD5270\system\sim5270.h>

#include "gpio5270.h"
#include "NBBGpio.h"
#include <startnet.h>
#include "Debug.h"
#include "SDCard.h"
#include "Mtttymux.h"
#include <pins.h>
#include    "TSMPeripheralsMBIndices.h"
#include "Eventlog.h"
#include "VNC1L_g.h"
#include "CopyCalibrationDataToIniFile.h"
#include "Telnetcommands.h"
#include "Security.h"



// Externally defined global variables
// Data
extern  long        g_nHardwareSerialNumber;
extern  int     g_nLanguage;
//extern  char        g_CurrentTime[];
extern  char        g_cPrinterBuffer[];


// Text
extern  char    *strPrintConfigurationTitle[];
extern  char    *strNewLine;
extern  char    *RevisionNumber;
extern  char    *strHopperWeight[];
extern  char    *strWeightDisplayUnits[];
extern CalDataStruct    g_CalibrationData;
extern  OnBoardCalDataStruct   g_OnBoardCalData;
extern  char    *strAutomatic[];
extern  char    *strManual[];
extern  char    *strHardwareSerialNumber[];
extern  char    *strCycleDiagCompHeader1;
extern  char    *strCycleDiagCompHeader2;
extern  char    *strCycleDiagCompHeader3;
extern  char    *strWeightConstant[];
extern  char    *strRetry[];
extern  char    *strPowerOnReset;
extern  char    *strWatchDogTimerReset[];
extern  char    *strTargetWeightDiagnosticPrint[];
extern  char    *strActualWeightDiagnosticPrint[];
extern  char    *strTargetWeightRetryDiagnosticPrint[];
extern  char    *strActualWeightRetryDiagnosticPrint[];
extern  char    *strGramsPerMeterPrint[];
extern  char    *strCycleTime[];
extern  char    *strLoadTime[];
extern  char    *strLineSpeed[];
extern  char    *strExtruderSpeed[];
extern  char    *strKilosperhourPrint[];
extern  char    *strActualLoadCellCountsDiagnosticPrint[];
extern  char    *strTargetLoadCellCountsDiagnosticPrint[];
extern  char    *strTargetPercentageDiagnosticPrint[];
extern  char    *strActualPercentageDiagnosticPrint[];
extern  char    *strOpenTimeInPitsDiagnosticPrint[];
extern  char    *strOpenTimeInSecondsDiagnosticPrint[];
extern  char    *strCycleCounter[];
extern  char    *strFlowRateinGramsPerSecondDiagnosticPrint[];
extern  char    *strFlowRateinCountPerSecondDiagnosticPrint[];
extern  char    *strBatchLength[];
extern  char    *strStartUp[];
extern  char    *strFillByWeightHeading[];
extern  char    *strNoFillByWeightMessage[];
extern  char    *strFillByWeightMessage[];
extern  char    *strSampleCounter[];
extern  char    *strMaximumWeight[];
extern  char    *strMinimumWeight[];
extern  char    *strMeanWeight[];
extern  char    *strStandardDeviation[];
extern  char    *strStandardDeviationPercentage[];
extern  char    *strSamples[];
extern  char    *strLogBatch[];
extern  char    *strLogBatch[];
extern  char    *strLogBlender[];
extern  char    *strLogGeneral[];
extern  char    *strLogStatistics[];
extern  char    *strLogControl[];
extern  char    *strDToAPerKg[];
extern  char    *strRPMPerKg[];
extern  char    *strControlType[];
extern  char    *strControlModeDiag[];
extern  char    *strControlSetpoint[];
extern  char    *strExtruderRPM[];
extern  char    *strActualLoadCellRawCounts[];
extern  char    *strActualLoadCellTareCounts[];
extern  char    *strRetriesCounter[];
extern  char    *strTemperature[];
extern  char    *strLiquidAdditiveSetKgHr[];
extern  char    *strLiquidAdditiveKgHrCalibration[];
extern  char    *strLiquidAdditiveDToA[];

extern  char   *strLossOfLockReset;
extern  char   *strExternalDeviceReset;
extern  char   *strWatchdogReset;
extern  char   *strSoftwareReset;
extern  char   *strLossOfClockReset;
extern char    *strEvlEvent;
extern char    *strEvlOccurred;
extern  char   *strEvModbusWrite;
extern  char   *strEvModbusWriteToPanel;
extern  char   *strSettledLoadCellCounts[];
extern  char   *strAToDStability[];

extern  int         fdTelnet;                           // fd for telnet task - global.


extern int     g_nTickCounter;         //


extern  structSetpointData  g_CurrentRecipe;
extern  char     g_cResetStatusRegister;
extern  BOOL    g_bPrintAssembleInProgress;
extern  unsigned char    Rx1Buff[];
extern  BYTE DS2432Buffer[];

int g_nCSVHeaderCount = 0;
extern structUSB   g_sUSB;
BOOL  g_bWriteCycleDiagToUSBDisk = FALSE;
extern unsigned int g_nTopUpComponent;
extern WORD SRAMData[];
extern WORD g_wFunctionRunningAtPowerUp;
extern WORD g_wFunctionSubSectionRunningAtPowerUp;



//////////////////////////////////////////////////////////////////////
// PrintResetSource        ASM PRINTRESETSOURCE
//
// Prints reset message
//
// Entry:   none
//
// Exit:    void
//
//////////////////////////////////////////////////////
void PrintResetSource( void )
{
	char    cPrinterBuffer[MAX_PRINT_WIDTH+1];
	char    cBuffer[40];

    char    cResetStatusRegister;

    cPrinterBuffer[0] = '\0';
    cResetStatusRegister = sim.reset.rsr; //nbb--todo-- this is causing a problem.
//    g_cResetStatusRegister = sim.reset.rsr;
    //SetupMttty();
    //iprintf("\n g_cResetStatusRegister is %x",g_cResetStatusRegister);

    if(cResetStatusRegister & LOSS_OF_LOCK_RESET_STATUS_BIT)
    {
        strcat(cPrinterBuffer, strLossOfLockReset);
        strcat(cPrinterBuffer,RevisionNumber);
    }
    else if(cResetStatusRegister & LOSS_OF_CLOCK_RESET_STATUS_BIT)
    {
        strcat(cPrinterBuffer, strLossOfClockReset);
        strcat(cPrinterBuffer,RevisionNumber);
    }
    else if(cResetStatusRegister & EXTERNAL_DEVICE_RESET_STATUS_BIT)
    {
        strcat( cPrinterBuffer, strExternalDeviceReset);
        strcat(cPrinterBuffer,RevisionNumber);
    }
    else if(cResetStatusRegister & POWER_ON_RESET_STATUS_BIT)
    {
        strcat( cPrinterBuffer, strPowerOnReset);
        strcat(cPrinterBuffer,RevisionNumber);
    }
    else if(cResetStatusRegister & WATCHDOG_RESET_STATUS_BIT)
    {
        strcat(cPrinterBuffer, strWatchdogReset);
        strcat(cPrinterBuffer,RevisionNumber);
    }
    else if(cResetStatusRegister & SOFTWARE_RESET_STATUS_BIT)
    {
        strcat(cPrinterBuffer, strSoftwareReset);
        strcat(cPrinterBuffer,RevisionNumber);
    }
    else
    {
        strcat(cPrinterBuffer, "Reset source unknown");
        strcat(cPrinterBuffer,RevisionNumber);
    }


    sprintf( cBuffer,"fn %2d subfn %2d",g_wFunctionRunningAtPowerUp,g_wFunctionSubSectionRunningAtPowerUp);
    strcat(cPrinterBuffer,cBuffer);

    sprintf( cBuffer," License id %d %2x %2x %4x",g_OnBoardCalData.m_lSoftwareID,g_OnBoardCalData.m_cPermanentOptions,g_OnBoardCalData.m_cTemporaryOptions,g_OnBoardCalData.m_wBlenderOptions);
    strcat(cPrinterBuffer,cBuffer);




//    WriteDiagnosticDataToSDCard();   //nbb--todo--add this reset to log file.
    AddPowerOnResetToEventLog(cPrinterBuffer);
    SetupMttty();
    iprintf("%s",cPrinterBuffer);

}


////////////////////////////////////////////////////////
// PrintModbusMessage
//
// Prints modbus message to Telnet if enabled
//
//////////////////////////////////////////////////////

void PrintModbusMessage( unsigned char *pRxbuff )
{

        union   CharsAndWord     MBRegisterAddress;
        char    cBuffer[1000];
        char    cPrinterBuffer[1000];
        int i = 0;
        int nBytes,nLength;
        cBuffer[0] = '\0';
        cPrinterBuffer[0] = '\0';
        sprintf( cBuffer, "\n ");
       if(g_CalibrationData.m_bTelnet && (fdTelnet > 0)&& g_bShowNetworkWrites && g_bTSMTelnetInSession)
       {
        sprintf( cBuffer, "\n%02d-%02d-%04d  %02d:%02d:%02d ", g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_MONTH], g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
        strcat( cPrinterBuffer, cBuffer );

        sprintf( cBuffer,"\naddress  %d",pRxbuff[0]);
        strcat( cPrinterBuffer, cBuffer);
        sprintf(cBuffer,"\ncommand %d",pRxbuff[1]);
        strcat( cPrinterBuffer, cBuffer);
        if(pRxbuff[1] == 06)
        {
           sprintf(cBuffer,"  write 1 word");
           strcat( cPrinterBuffer, cBuffer);
           MBRegisterAddress.cValue[0] = pRxbuff[2];
           MBRegisterAddress.cValue[1] = pRxbuff[3];
           sprintf( cBuffer,"\naddress %d ",MBRegisterAddress.nValue);
           strcat( cPrinterBuffer, cBuffer);
           sprintf( cBuffer,"\ndata   ");
           strcat( cPrinterBuffer, cBuffer);
           nBytes = 2;
           for(i=0; i<nBytes; i++)
           {
               sprintf( cBuffer,"%d(%x),",pRxbuff[4+i],pRxbuff[4+i]);
               strcat( cPrinterBuffer, cBuffer);
           }
           sprintf( cBuffer,"\nchecksum  %x %x",pRxbuff[6],pRxbuff[7]);
           strcat( cPrinterBuffer, cBuffer);
        }
        else if(pRxbuff[1] == 16)
        {
           sprintf(cBuffer,"  write %d word(s)",(pRxbuff[6]/2));
           strcat( cPrinterBuffer, cBuffer);
           MBRegisterAddress.cValue[0] = pRxbuff[2];
           MBRegisterAddress.cValue[1] = pRxbuff[3];
           sprintf( cBuffer,"\naddress %d",MBRegisterAddress.nValue);
           strcat( cPrinterBuffer, cBuffer);
           sprintf( cBuffer,"\ndata   ");
           strcat( cPrinterBuffer, cBuffer);
           nBytes = pRxbuff[6];
            for(i=0; i<(nBytes/2); i++)
           {
               sprintf( cBuffer,"%d(ms):%d(%x),",(MBRegisterAddress.nValue)+i,pRxbuff[7+(i*2)],pRxbuff[7+(i*2)]);
               strcat( cPrinterBuffer, cBuffer);
               sprintf( cBuffer,"%d(ls):%d(%x),",(MBRegisterAddress.nValue)+i,pRxbuff[7+(i*2)+1],pRxbuff[7+(i*2)+1]);
               strcat( cPrinterBuffer, cBuffer);

           }
           sprintf( cBuffer,"\nchecksum  %x %x",pRxbuff[6+(i*2)+1],pRxbuff[7+(i*2)+1]);
           strcat( cPrinterBuffer, cBuffer);
        }
       	sprintf( cBuffer,"\n");
        strcat( cPrinterBuffer, cBuffer);
        }

// do not print the message inside the interrupt system
// this is having some strange effects with the rest of the
// system, the foreground stops running.
        if(MBRegisterAddress.nValue != BATCH_PANEL_VERSION_NUMBER)
        {
            nLength  = strlen(cPrinterBuffer);
            for(i=0; i<nLength; i++)
            {
                g_cModbusPrintBuffer[i] = cPrinterBuffer[i];
            }
            g_cModbusPrintBuffer[i] = '\0';
            g_bPrintModbusMessageToTelnet = TRUE;
        }
}

////////////////////////////////////////////////////////
// PrintModbusTCPMessage
//
// Prints modbus message to Telnet if enabled
//
//////////////////////////////////////////////////////
void PrintModbusTCPMessage( unsigned int start_addr, unsigned int length_in_bits )
{
   unsigned int i;
   char    cBuffer[1000];
   char    cPrinterBuffer[1000];
   unsigned int     nLength;
   if(g_CalibrationData.m_bTelnet && (fdTelnet > 0)&& g_bShowNetworkWrites && g_bTSMTelnetInSession)
   {
       cBuffer[0] = '\0';
       cPrinterBuffer[0] = '\0';
       sprintf(cBuffer,"\nModbus TCP" );
       strcat( cPrinterBuffer, cBuffer );
       sprintf( cBuffer, "\n%02d-%02d-%04d  %02d:%02d:%02d ", g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_MONTH], g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( cPrinterBuffer, cBuffer );
       sprintf(cBuffer,"\nStatr Addr = %d, Length = %d\n", start_addr-1, length_in_bits );
       strcat( cPrinterBuffer, cBuffer );

       for(i=0; i<length_in_bits; i++)
       {
           sprintf( cBuffer,"%d:%d(%x),", start_addr-1+i,g_arrnMBTable[i+start_addr-1],g_arrnMBTable[i+start_addr-1]);
           strcat( cPrinterBuffer, cBuffer);
       }

       	sprintf( cBuffer,"\n");
        strcat( cPrinterBuffer, cBuffer);

// do not print the message inside the interrupt system
// this is having some strange effects with the rest of the
// system, the foreground stops running.
        nLength  = strlen(cPrinterBuffer);
        for(i=0; i<nLength; i++)
        {
          g_cModbusPrintBuffer[i] = cPrinterBuffer[i];
        }
        g_cModbusPrintBuffer[i] = '\0';
        g_bPrintModbusMessageToTelnet = TRUE;
  }
}


////////////////////////////////////////////////////////
// RecordChangedModbusData
//
//
//////////////////////////////////////////////////////

void RecordChangedModbusData(WORD wModbusAddress,WORD wOld,WORD wNew,BOOL bIsPanel)
{
    char    cBuffer[500];
    BOOL    bIgnoreAddress = FALSE;
    // take increase /decrease command out
   if((wModbusAddress == BATCH_DTOA_INCREASE_COMMAND) || (wModbusAddress == BATCH_DTOA_DECREASE_COMMAND))
    bIgnoreAddress = TRUE;

    if((wModbusAddress >= BATCH_SETPOINT_PERCENT_1) && (wModbusAddress <= BATCH_SETPOINT_DOWNLOAD_MANAGER))
    bIgnoreAddress = TRUE;
    if((wModbusAddress >= BATCH_ICS_RECIPE_START) && (wModbusAddress <= BATCH_ICS_RECIPE_END))
    bIgnoreAddress = TRUE;
    if((wOld != wNew)&&!bIgnoreAddress)
    {
        if(strlen(g_cModbusEventBuffer) < MODBUS_EVENT_LOG_BUFFER_LENGTH)  // check for valid string length
        {
            if(bIsPanel)  //show that panel has been written to
            {
                sprintf( cBuffer, "\r\n%s\t%s\t%s",strEvlEvent,strEvlOccurred,strEvModbusWriteToPanel);
            }
            else
            {
                sprintf( cBuffer, "\n%s\t%s\t%s",strEvlEvent,strEvlOccurred,strEvModbusWrite);
            }
            strcat( g_cModbusEventBuffer,cBuffer );
            sprintf( cBuffer, "\tAddress %d old:%d(%x) new:%d(%x)\t", wModbusAddress, wOld, wOld,wNew, wNew);
            strcat( g_cModbusEventBuffer,cBuffer );
            sprintf( cBuffer, "%02d/%02d/%04d\t%02d:%02d:%02d\t",g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
            strcat( g_cModbusEventBuffer,cBuffer );
            sprintf( cBuffer, "%d\t", g_nCycleCounter  );    //
            strcat( g_cModbusEventBuffer,cBuffer );
            g_nHoldOffEventLogWriteCtr = 5; // 5 seconds
            g_bAddEventToEventLog = TRUE;
        }
        else
        {
 //           if(fdTelnet>0)
 //           iprintf("\n g_cModbusEventBuffer string length exceeded");
        }
    }
}

//////////////////////////////////////////////////////////////////////
// PrintCycleDiagnostics        ASM BBDIAPRN
//
// Prints batch cycle diagnostics
//
// Entry:   none
//
// Exit:    void
//
// M.McKiernan                              17-12-2003
// First pass.
//////////////////////////////////////////////////////
BOOL PrintCycleDiagnostics( void )
{

   float   fValue;
   unsigned int    i,j,nComponentsToPrint;
   BOOL    bResult = FALSE;
   char    cBuffer[MAX_PRINT_WIDTH];

   if(1)
   {
      g_bWriteCycleDiagToUSBDisk = FALSE;    // will be new cycle, so clear here.
        if(g_CalibrationData.m_bLogFormatIsCSV)
        {
            CSVVersion();
            WriteDiagnosticCSVDataToSDCard();
        }
        if(1)
        {
            strcpy( g_cPrinterBuffer, strNewLine );
            strcat( g_cPrinterBuffer, strNewLine );
        // Title
        strcat( g_cPrinterBuffer, RevisionNumber );
        sprintf( cBuffer, "Blender: %s", g_CalibrationData.m_cEmailBlenderName );     //
        strcat( g_cPrinterBuffer, cBuffer );


       sprintf( cBuffer, " Panel Software : %s",g_cPanelVersionNumber);
       strcat( g_cPrinterBuffer, cBuffer );

       sprintf( cBuffer, " Hardware Serial No:" );
       strcat( g_cPrinterBuffer, cBuffer );

       for(i=0; i < NBB_SERIAL_NUMBER_SIZE-1; i++ )
       {
           cBuffer[i] = g_sDescriptionData.m_cNBBSerialNo[i];        // M.s.Byte.
       }
       cBuffer[NBB_SERIAL_NUMBER_SIZE-1] = '\0';
       strcat( g_cPrinterBuffer, cBuffer );

        // time
        sprintf( cBuffer, " %02d-%02d-%04d  %02d:%02d:%02d ", g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_MONTH], g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
        strcat( g_cPrinterBuffer, cBuffer );
// Hopper wt.
        sprintf( cBuffer, strHopperWeight[0], strWeightDisplayUnits[0] );   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
        strcat( g_cPrinterBuffer, cBuffer );
//        fValue = g_fWeightInHopper;
        fValue = g_fOneSecondHopperWeight;

        sprintf( cBuffer, "    %2.3f ", fValue );   // g_fWeightInHopper );
        strcat( g_cPrinterBuffer, cBuffer );

        sprintf( cBuffer, strTemperature[0] );
        strcat( g_cPrinterBuffer, cBuffer );

        sprintf( cBuffer, "    %2d ",g_nTemperature);   //
        strcat( g_cPrinterBuffer, cBuffer );

        strcat( g_cPrinterBuffer, strNewLine );

//Next line
// Auto/manual
        if(g_bManualAutoFlag == MANUAL)
            sprintf( cBuffer, strManual[0] );
        else
            sprintf( cBuffer, strAutomatic[0] );

        strcat( g_cPrinterBuffer, cBuffer);

// Start up
        if(g_bExtruderControlStartUpFlag)
        {
            sprintf( cBuffer, strStartUp[0] );
            strcat( g_cPrinterBuffer, cBuffer);
        }


// Weight constant
        strcat( g_cPrinterBuffer, "     "); // spacer
        strcat( g_cPrinterBuffer, strWeightConstant[0]);
        sprintf( cBuffer, " %6.3f", g_CalibrationData.m_fWeightConstant );  // Weight constant
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );

// line speed
        strcat( g_cPrinterBuffer, strLineSpeed[0]);
        sprintf( cBuffer, " %3.2f", g_fLineSpeed5sec );  // line speed
        strcat( g_cPrinterBuffer, cBuffer);


// cycle time
        strcat( g_cPrinterBuffer, "     "); // spacer
        strcat( g_cPrinterBuffer, strCycleTime[0]);

        sprintf( cBuffer, "(%3.2f) ", g_fBatchTimeInSeconds );  // cycle time in seconds
        strcat( g_cPrinterBuffer, cBuffer);
// batch length
//
        strcat( g_cPrinterBuffer, strBatchLength[0]);
        sprintf( cBuffer, " %3.2f", g_fBatchLength );  // cycle time in seconds
        strcat( g_cPrinterBuffer, cBuffer);

// load time
        strcat( g_cPrinterBuffer, "     "); // spacer
        strcat( g_cPrinterBuffer, strLoadTime[0]);
        sprintf( cBuffer, " %3.1f ", g_fCycleLoadTimePrn );  // load time
        strcat( g_cPrinterBuffer, cBuffer);

// max kg/hr

      g_fMaxKgPHrPossible = 1.0 * g_CalibrationData.m_fBatchSize * 3600 /(g_fCycleLoadTimePrn);
      sprintf( cBuffer, "Max Kg/hr %4.2f", g_fMaxKgPHrPossible );  // cycle time in seconds
      strcat( g_cPrinterBuffer, cBuffer);


/* CALCMAXKGHR:
        JSR     CLRAC
        LDX     #$360
        STX     AREG+3          ; 360 TO A.
        LDX     DFTW1           ; TARGET WT TO C
        STX     MATHCREG+3      ;
        JSR     MUL
        LDX     #MATHCREG
        JSR     IFEREG          ; RESTORE VALUE.
        JSR     CLAREG
        LDD     CYCLELOADTIMEPRN
        STD     AREG+3          ;
        JSR     DIV             ; C/A
        RTS


*/

        strcat( g_cPrinterBuffer, strNewLine );


//Next line
// hardware ser. no.
        strcat( g_cPrinterBuffer, strHardwareSerialNumber[0] );

         for( i= 0; i < 8; i++ )
         {
         sprintf( cBuffer, "%x ",  DS2432Buffer[i] );    // hardware serial no.
        strcat( g_cPrinterBuffer, cBuffer );
         }
        strcat( g_cPrinterBuffer, strNewLine );
//cycle counter
        strcat( g_cPrinterBuffer, strCycleCounter[0] );
        sprintf( cBuffer, " %d ", g_nCycleCounter );    //
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );
        strcat( g_cPrinterBuffer, strNewLine );

// a/d stability check, show a/d values for the previous batch
        strcat( g_cPrinterBuffer, strAToDStability[g_nLanguage]);
        for( i=0; i<MAX_STABILITY_READINGS; i++)
        {
            sprintf( cBuffer, "%ld ", g_nAToDStabilityLog[i]  );
            strcat( g_cPrinterBuffer, cBuffer);
        }

        sprintf( cBuffer, "Max %ld Min %ld Diff %ld",g_nAToDStabilityMaxValue,g_nAToDStabilityMinValue,g_nAToDStabilityMaxValue-g_nAToDStabilityMinValue);
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );


 // g/m
        strcat( g_cPrinterBuffer, strGramsPerMeterPrint[0]);

        sprintf( cBuffer, " %4.3f ", g_CurrentRecipe.m_fTotalWeightLength );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, " %4.3f ", g_fWeightPerMeter );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, " %4.3f ", g_fWeightPerMeterRR );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, "%7.3f,",(1000.0 * g_fThroughputCalculationBatchWeight) );   // comp no.
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );


// kg/h
        strcat( g_cPrinterBuffer, strKilosperhourPrint[0]);

        sprintf( cBuffer, " %4.3f ", g_CurrentRecipe.m_fTotalThroughput );
        strcat( g_cPrinterBuffer, cBuffer);


        sprintf( cBuffer, " %4.3f ", g_fThroughputKgPerHour );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, " %4.3f ", g_fRRThroughputKgPerHour );
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );

// extruder speed in %

        strcat( g_cPrinterBuffer, strExtruderSpeed[0]);
        strcat( g_cPrinterBuffer, "     "); // spacer

        sprintf( cBuffer, " %3.2f  Control sp %d sei set %6d", g_fExtruderDAValuePercentage, g_nExtruderControlSetpointIncDec,g_arrnWriteSEIMBTable[MB_SEI_DA]);
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );

        sprintf( cBuffer, "tacho avg %6d inst %6d control %% error %3.3f ",g_nExtruderTachoReadingAverage,g_nExtruderTachoReading,g_fControlErrorPercentage);
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );


// d/a per kg
        strcat( g_cPrinterBuffer, strDToAPerKg[0]);
        strcat( g_cPrinterBuffer, "     "); // spacer
        sprintf( cBuffer, " %3.2f ", g_ProcessData.m_fDtoAPerKilo);
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, "     "); // spacer

// rpm per kg
        strcat( g_cPrinterBuffer, strRPMPerKg[0]);
        sprintf( cBuffer, " %3.2f ", g_ProcessData.m_fRPMPerKilo);
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, "     "); // spacer
        strcat( g_cPrinterBuffer, strNewLine );

// control type
        strcat( g_cPrinterBuffer, strControlType[0]);
        sprintf( cBuffer, " %d ", g_CalibrationData.m_nControlType);
        strcat( g_cPrinterBuffer, cBuffer);



// control mode
        strcat( g_cPrinterBuffer, strControlModeDiag[0]);
        sprintf( cBuffer, " %d ", g_CalibrationData.m_nControlMode);
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );

        if(g_CalibrationData.m_bLiquidEnabled)
        {
        // liquid additive kg/hr sp.

            strcat( g_cPrinterBuffer, strLiquidAdditiveSetKgHr[0]);
            sprintf( cBuffer, " %3.2f ", g_fKgperHour);
            strcat( g_cPrinterBuffer, cBuffer);
            strcat( g_cPrinterBuffer, strLiquidAdditiveKgHrCalibration[0]);
            sprintf( cBuffer, " %3.2f ", g_CalibrationData.m_fLiquidAdditiveKgPerHour);
            strcat( g_cPrinterBuffer, cBuffer);
            strcat( g_cPrinterBuffer, strLiquidAdditiveDToA[0]);
            sprintf( cBuffer, " %d ", g_nLiquidAdditiveOutputDAValue);
            strcat( g_cPrinterBuffer, cBuffer);
            strcat( g_cPrinterBuffer, strNewLine );

        }

// extruder rpm
        strcat( g_cPrinterBuffer, strExtruderRPM[0]);
        sprintf( cBuffer, " %3.2f ", g_fExtruderRPM);
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );


        strcat( g_cPrinterBuffer, strNewLine );
        sprintf( cBuffer, "                ");   // comp no.
        strcat( g_cPrinterBuffer, cBuffer);
        if(g_bTopUpFlag)
        {
            if(g_CalibrationData.m_nComponents < 8)
            {
                nComponentsToPrint = 8;
            }
            else
            {
                nComponentsToPrint = g_CalibrationData.m_nComponents;
            }
        }
        else
        {
               nComponentsToPrint = g_CalibrationData.m_nComponents;
        }


        j=0;
        for( i=j; i<nComponentsToPrint; i++)
        {
            sprintf( cBuffer, "Comp%3d   ", i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);

        }

        strcat( g_cPrinterBuffer, strNewLine );





        /* target weight */
        if(g_nAnyRetry)
        strcat( g_cPrinterBuffer, strTargetWeightRetryDiagnosticPrint[g_nLanguage]);
        else
        strcat( g_cPrinterBuffer, strTargetWeightDiagnosticPrint[g_nLanguage]);


        for( i=j; i<nComponentsToPrint; i++)
        {
            if((g_bTopUpFlag)&& (i == g_nTopUpComponent-1))
            {
                sprintf( cBuffer, " %7.1f ", (1000.0 * g_fComponentTargetTopUpWeight[i]) );     // target wt.
            }
            else
            {
                sprintf( cBuffer, " %7.1f ", (1000.0 * g_fComponentTargetWeight[i]) );     // target wt.
            }

            strcat( g_cPrinterBuffer, cBuffer);
        }

        strcat( g_cPrinterBuffer, strNewLine );

        /* actual weight */
        if(g_nAnyRetry)
            strcat( g_cPrinterBuffer, strActualWeightRetryDiagnosticPrint[g_nLanguage]);
        else
            strcat( g_cPrinterBuffer, strActualWeightDiagnosticPrint[g_nLanguage]);


        for( i=j; i<nComponentsToPrint; i++)
        {
           if(g_bShowInstantaneousWeight)
           {
                sprintf( cBuffer, " %7.1f ", (1000.0 * g_fComponentActualWeight[i]) );     // actual wt.
           }
           else
           {
                sprintf( cBuffer, " %7.1f ", (1000.0 * g_fComponentActualWeightRR[i]) );     // actual wt.
           }
           strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// target a/d counts
        strcat( g_cPrinterBuffer, strTargetLoadCellCountsDiagnosticPrint[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            fValue = (float)g_lComponentTargetCounts[i];
            sprintf( cBuffer, " %6.0f  ", fValue  );     // TARGET CTS
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// actual a/d counts
        strcat( g_cPrinterBuffer, strActualLoadCellCountsDiagnosticPrint[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            fValue = (float)g_lComponentCountsActual[i];
            sprintf( cBuffer, " %6.0f  ", fValue  );         // Actual CTS
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// load cell counts
        strcat( g_cPrinterBuffer, strActualLoadCellRawCounts[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            fValue = (float)g_lCmpRawCounts[i];
            sprintf( cBuffer, " %6.0f  ", fValue  );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// settled load cell counts

        strcat( g_cPrinterBuffer, strSettledLoadCellCounts[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            fValue = (float)g_lSettledAverageCounts[i];
            sprintf( cBuffer, " %6.0f  ", fValue  );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


// load tare counts
        strcat( g_cPrinterBuffer, strActualLoadCellTareCounts[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            fValue = (float)g_lCmpTareCounts[i];
            sprintf( cBuffer, " %6.0f  ", fValue  );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


        // set %
        strcat( g_cPrinterBuffer, strTargetPercentageDiagnosticPrint[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            sprintf( cBuffer, "  %3.2f  ", g_CurrentRecipe.m_fPercentage[i] );    // set %
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


        // actual %
        strcat( g_cPrinterBuffer, strActualPercentageDiagnosticPrint[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            if(g_bShowInstantaneousWeight)
            {
                sprintf( cBuffer, "  %3.2f   ", g_fComponentActualPercentage[i] );     // Act %
            }
            else
            {
                sprintf( cBuffer, "  %3.2f   ", g_fComponentActualPercentageRR[i] );     // Act %
            }
            strcat( g_cPrinterBuffer, cBuffer);
       }
        strcat( g_cPrinterBuffer, strNewLine );

// pit open times

        strcat( g_cPrinterBuffer, strOpenTimeInPitsDiagnosticPrint[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
            fValue = (float)g_lCmpTime[i];
            sprintf( cBuffer, " %6.0f  ", fValue );  // Valve open time PIT's
            strcat( g_cPrinterBuffer, cBuffer);
       }
        strcat( g_cPrinterBuffer, strNewLine );


// open times in seconds

        strcat( g_cPrinterBuffer, strOpenTimeInSecondsDiagnosticPrint[g_nLanguage]);
        for( i=j; i<nComponentsToPrint; i++)
        {
           sprintf( cBuffer, " %3.4f   ", g_fComponentOpenSeconds[i] );  // Valve open time (s)
            strcat( g_cPrinterBuffer, cBuffer);
       }
        strcat( g_cPrinterBuffer, strNewLine );

// pits per second

		strcat( g_cPrinterBuffer, strFlowRateinCountPerSecondDiagnosticPrint[g_nLanguage]);
    for( i=j; i<nComponentsToPrint; i++)
    {
        sprintf( cBuffer, " %3.3f   ", g_ffComponentCPI[i] );     // Counts per int. stage 1.
        strcat( g_cPrinterBuffer, cBuffer);
    }

    // grams per second.

    strcat( g_cPrinterBuffer, strNewLine );
    strcat( g_cPrinterBuffer, strFlowRateinGramsPerSecondDiagnosticPrint[g_nLanguage]);
    for( i=j; i<nComponentsToPrint; i++)
    {
       sprintf( cBuffer, " %4.1f    ", (BLENDER_PIT_FREQUENCY * 1000.0 * g_ffComponentCPI[i])/g_CalibrationData.m_fWeightConstant );     // Counts per int. stage 1.
       strcat( g_cPrinterBuffer, cBuffer);
    }

        strcat( g_cPrinterBuffer, strNewLine );

// retries
            strcat( g_cPrinterBuffer, strRetriesCounter[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, "     %d  ", g_nRetryCounterStage1[i] );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );



    // filling  - fill by weight / normal

    strcat( g_cPrinterBuffer, strFillByWeightHeading[g_nLanguage]);  // heading
    for( i=j; i<nComponentsToPrint; i++)
    {
        if(g_bComponentHasFilledByWeight[i])
        {
            sprintf( cBuffer, " %s    ", strFillByWeightMessage[g_nLanguage]);     // Counts per int. stage 1.
            strcat( g_cPrinterBuffer, cBuffer);
        }
        else
        {
            sprintf( cBuffer, " %s    ", strNoFillByWeightMessage[g_nLanguage]);     // Counts per int. stage 1.
            strcat( g_cPrinterBuffer, cBuffer);
        }

    }
        strcat( g_cPrinterBuffer, strNewLine );



        if(1)
        {

// samples counter
            strcat( g_cPrinterBuffer, strSampleCounter[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, "     %d  ", g_nComponentWeightSampleCounter[i] );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );

// samples
            strcat( g_cPrinterBuffer, strSamples[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, " %7.1f ", (1000.0 * g_fComponentWeightSamples[i][g_nComponentWeightSampleCounter[i]-1]) );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );

// maximum
            strcat( g_cPrinterBuffer, strMaximumWeight[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, " %7.1f ", (1000.0 * g_fComponentWeightMaximum[i]) );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );

// minimum
            strcat( g_cPrinterBuffer, strMinimumWeight[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, " %7.1f ", (1000.0 * g_fComponentWeightMinimum[i]) );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );


// mean
            strcat( g_cPrinterBuffer, strMeanWeight[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, " %7.1f ", ( 1000.0 * g_fComponentWeightMean[i]) );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );

// standard deviation
            strcat( g_cPrinterBuffer, strStandardDeviation[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, " %7.3f ", (1000.0 * g_fComponentWeightStdDev[i]) );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );

// standard deviation %
            strcat( g_cPrinterBuffer, strStandardDeviationPercentage[g_nLanguage]);
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, " %7.3f ", (g_fComponentWeightStdDevPercentage[i]) );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );

// pulses
            strcat( g_cPrinterBuffer, "Pulses               ");
            for( i=j; i<nComponentsToPrint; i++)
            {
                sprintf( cBuffer, "%d        ", (g_nComponentPulseCounter[i]) );
                strcat( g_cPrinterBuffer, cBuffer);
            }
           strcat( g_cPrinterBuffer, strNewLine );

        }

        if(g_nAnyRetry != 0)
        {
            strcat( g_cPrinterBuffer, strRetry[0]);
            sprintf( cBuffer, " %d ", g_nAnyRetry );    //
            strcat( g_cPrinterBuffer, cBuffer);
            strcat( g_cPrinterBuffer, strNewLine );
           g_nAnyRetry = 0;
       }

       strcat( g_cPrinterBuffer, strNewLine );
       strcat( g_cPrinterBuffer, strNewLine );

      // if(fdTelnet > 0)
      // {
      //     iprintf("\n string length is %d",strlen( g_cPrinterBuffer ));
      // }
       if((g_bPrintCycleDataToTelnet)&& (fdTelnet > 0))
       {
           iprintf("%s",g_cPrinterBuffer); //nbb--testonly--
           ShowPrompt();
       }
     }

     if((g_CalibrationData.m_nLogToSDCard & LOG_BATCH_LOG_TO_SD_CARD) != 0)
     {
         if(!g_bTelnetSDCardAccessInProgress)  // only write to sd card if not already being accessed
         {
             WriteDiagnosticDataToSDCard();
         }
         else
         {
             g_bWriteDiagnosticsToSDCard = TRUE;
         }

     }
     WriteLogDataToUSB();
     }
    return bResult;
}

//////////////////////////////////////////////////////////////////////
// WriteLogDataToUSB
//
//////////////////////////////////////////////////////////////////////

void WriteLogDataToUSB( void )
{
      if((g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_USB) && g_CalibrationData.m_bBatchLogDumpFlag )  // use 1 for USB memory stick.      if( 1 )     // tthis works.
      {
      // try writing to flash memory stick.
          char    cName[20];
          sprintf( cName, "lg%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
          strcat( cName, ".txt");
          strncpy( g_sUSB.m_cDiagFileName, cName,USBFILENAMESTRINGLENGTH);     // save the filename for diagnostics
          g_sUSB.m_bStartDiagWrite = TRUE;
      }
      else if( g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_PRINTER && g_CalibrationData.m_bBatchLogDumpFlag )  // use 0 for UsB printer.
      {
         StartUSBPrint( g_cPrinterBuffer, FALSE );
      }
}


//////////////////////////////////////////////////////////////////////
// PrintSystemConfiguration     ASM
//
// Prints batch blender calibration data
// Dumps the calibration data section of the modbus table in format:
//
// Modbus Adress: Register data (decimal): Register data (hex).
//
//
// Entry:   none
//
// Exit:    void
//
// M.McKiernan                              21-12-2003
// First pass.
//////////////////////////////////////////////////////
BOOL PrintSystemConfiguration( void )
{
    BOOL    bResult = FALSE;
    int     i;
    int     nLoopCounter = 0;
    int nLen = 0;
    if( CanWePrint() )
    {
        char    cBuffer[MAX_PRINT_WIDTH+1];


        strcpy( g_cPrinterBuffer, strNewLine );

        // Title
        strcat( g_cPrinterBuffer, strPrintConfigurationTitle[0] );

        strcat( g_cPrinterBuffer, " - ");
        strcat( g_cPrinterBuffer, RevisionNumber );
        strcat( g_cPrinterBuffer, " - ");
        // time
        sprintf( cBuffer, " %02d-%02d-%04d  %02d:%02d:%02d ", g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_MONTH], g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
        strcat( g_cPrinterBuffer, cBuffer );
        strcat( g_cPrinterBuffer, strNewLine );

//Next line

//      for( i = BATCH_CALIBRATION_CONTROL_AVERAGE; i <= BATCH_CALIBRATION_CONTROL_AVERAGE+10; i++ ) // testonly - print 10 values.
        for( i = BATCH_CALIBRATION_CONTROL_AVERAGE; i <= BATCH_CALIBRATION_PASSWORD_SUPERVISOR; i++ )   //nbb--todolp-- extend this to the end
        {
            nLoopCounter++;
            if( nLoopCounter > 30)
            {
                nLoopCounter = 0;
//nbb                ServiceWatchdogTimer();     // service the watch dog timer
            }
            sprintf( cBuffer, "%5d ", i );
            strcat( g_cPrinterBuffer, cBuffer );

            sprintf( cBuffer, " %5d ", g_arrnMBTable[i] );      // print data in hex.
            strcat( g_cPrinterBuffer, cBuffer );
// data in hex:
            cBuffer[0] = '0';   //  as in 0x12f3
            cBuffer[1] = 'x';   //  as in 0x12f3

            cBuffer[2] = (g_arrnMBTable[i] & 0xF000) >>12;
            if( cBuffer[2] > 9 )
                cBuffer[2] += 0x37;     // cater for A-F
            else
                cBuffer[2] += 0x30;     // accomodate 0-9 values.

            cBuffer[3] = (g_arrnMBTable[i] & 0x0F00) >>8;
            if( cBuffer[3] > 9 )
                cBuffer[3] += 0x37;     // cater for A-F
            else
                cBuffer[3] += 0x30;     // accomodate 0-9 values.

            cBuffer[4] = (g_arrnMBTable[i] & 0x00F0) >>4;
            if( cBuffer[4] > 9 )
                cBuffer[4] += 0x37;     // cater for A-F
            else
                cBuffer[4] += 0x30;     // accomodate 0-9 values.

            cBuffer[5] = (g_arrnMBTable[i] & 0x000F);   // start with L.s. nibble.
            if( cBuffer[5] > 9 )
                cBuffer[5] += 0x37;     // cater for A-F
            else
                cBuffer[5] += 0x30;     // accomodate 0-9 values.

            cBuffer[6] = 0; // terminate string.

            strcat( g_cPrinterBuffer, cBuffer );
            strcat( g_cPrinterBuffer, strNewLine );

        }

        // Indicate the end of the print data
//nbb--todo--review        strcat( g_cPrinterBuffer, ASCII_NULL );

        nLen = strlen( g_cPrinterBuffer );

        // Indicate that we were able to print
        bResult = TRUE;

    }

    return bResult;

}


//////////////////////////////////////////////////////////////////////
// CSVVersion       ASM no equivalent
//
// Prints batch cycle diagnostics into printer buffer in CSV format.
//
// Entry:   none
//
// Exit:    void
//
// M.McKiernan                              21-12-2003
// First pass.
//////////////////////////////////////////////////////
#define CSV_HEADER_FREQUENCY        20          // Header for CSV is printed each nth batch.
void CSVVersion1( void )
{
    float fValue;
    unsigned int i;
    char    cBuffer[MAX_PRINT_WIDTH+1];

    g_cPrinterBuffer[0] = 0;

    g_nCSVHeaderCount++;


// Cycle information
//
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "Cycle," );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

// Revisiion
        sprintf( cBuffer, "%s,",RevisionNumber);
        strcat( g_cPrinterBuffer, cBuffer );


// hopper weight
        sprintf( cBuffer, strHopperWeight[0], strWeightDisplayUnits[0] );   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%2.3f,", g_fOneSecondHopperWeight );   // g_fWeightInHopper );
        strcat( g_cPrinterBuffer, cBuffer );

// Weight constant
        strcat( g_cPrinterBuffer, strWeightConstant[0]);
        sprintf( cBuffer, " %6.3f,", g_CalibrationData.m_fWeightConstant );  // Weight constant
        strcat( g_cPrinterBuffer, cBuffer);

// cycle time
        strcat( g_cPrinterBuffer, strCycleTime[0]);
        sprintf( cBuffer, ",%3d,", g_nCycleTimePrn );  // cycle time
        strcat( g_cPrinterBuffer, cBuffer);

// load time
        strcat( g_cPrinterBuffer, strLoadTime[0]);
        sprintf( cBuffer, " %3.1f", g_fCycleLoadTimePrn );  // load time
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );



        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );

// For each component:
       sprintf( cBuffer, "%s,", strTargetWeightDiagnosticPrint[g_nLanguage] );   //
       strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
        // comp no.
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
        // target wt.
            sprintf( cBuffer, "%7.3f,",(1000.0 * g_fComponentTargetWeight[i]) );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


// actual weight
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );

        sprintf( cBuffer, "%s,", strActualWeightDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
           // Actual wt.
            sprintf( cBuffer, "%7.3f,",(1000.0 * g_fComponentActualWeight[i]) );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


// target a/d counts
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strTargetLoadCellCountsDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            fValue = (float)g_lComponentTargetCounts[i];
            sprintf( cBuffer, "%6.0f,",fValue );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


// actual a/d counts
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strActualLoadCellCountsDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            fValue = (float)g_lComponentCountsActual[i];
            sprintf( cBuffer,"%6.0f,",fValue );   //
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

        // set %
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strTargetPercentageDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%3.2f,",g_CurrentRecipe.m_fPercentage[i] );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

        // Act %

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strActualPercentageDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            if(g_bShowInstantaneousWeight)
            {
                sprintf( cBuffer, "%3.2f,", g_fComponentActualPercentage[i] );
            }
            else
            {
                sprintf( cBuffer, "%3.2f,", g_fComponentActualPercentageRR[i] );
            }
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

        // Valve open time PIT's

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strOpenTimeInPitsDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            fValue = (float)g_lCmpTime[i];
            sprintf( cBuffer, "%6.0f,",fValue );
            strcat( g_cPrinterBuffer, cBuffer);
        }
            strcat( g_cPrinterBuffer, strNewLine );

        // Valve open time (s)
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strOpenTimeInSecondsDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%3.2f,",g_fComponentOpenSeconds[i] );
            strcat( g_cPrinterBuffer, cBuffer);
        }
            strcat( g_cPrinterBuffer, strNewLine );
// pits per second

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strFlowRateinCountPerSecondDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%3.3f,",g_ffComponentCPI[i] );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


    // grams per second.

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strFlowRateinGramsPerSecondDiagnosticPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer,"%4.1f,",(BLENDER_PIT_FREQUENCY * 1000.0 * g_ffComponentCPI[i])/g_CalibrationData.m_fWeightConstant );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// filling type

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogBlender[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,", strFillByWeightHeading[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            if(g_bComponentHasFilledByWeight[i])
            {
                sprintf( cBuffer, "%s,", strFillByWeightMessage[g_nLanguage]);     // Counts per int. stage 1.
                strcat( g_cPrinterBuffer, cBuffer);
            }
            else
            {
                sprintf( cBuffer, "%s,", strNoFillByWeightMessage[g_nLanguage]);     // Counts per int. stage 1.
                strcat( g_cPrinterBuffer, cBuffer);
            }
        }
        strcat( g_cPrinterBuffer, strNewLine );


 // g/m
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogControl[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strGramsPerMeterPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

        sprintf( cBuffer, "%4.3f,", g_CurrentRecipe.m_fTotalWeightLength );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, "%4.3f,", g_fWeightPerMeter );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, "%4.3f,", g_fWeightPerMeterRR );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, "%7.3f,",(1000.0 * g_fThroughputCalculationBatchWeight) );   // comp no.
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );


// kg/h
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogControl[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strKilosperhourPrint[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

       sprintf( cBuffer, "%4.3f,", g_CurrentRecipe.m_fTotalThroughput );
       strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, "%4.3f,", g_fThroughputKgPerHour );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, "%4.3f,", g_fRRThroughputKgPerHour );
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );




// control log
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogControl[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "Stats," );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

        if(g_bManualAutoFlag == MANUAL)
            sprintf( cBuffer, strManual[0] );
        else
            sprintf( cBuffer, strAutomatic[0] );

        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, ","); // spacer


//
// % error, control sp,rpm, tacho
       sprintf( cBuffer, "Percentage error, %3.3f,Control sp, %d, %s,%3.2f, tacho avg, %6d,", g_fControlErrorPercentage,g_arrnWriteSEIMBTable[MB_SEI_DA],strExtruderRPM[0],g_fExtruderRPM,g_nExtruderTachoReadingAverage);    //
       strcat( g_cPrinterBuffer, cBuffer );

// d/a per kg
        strcat( g_cPrinterBuffer, strDToAPerKg[0]);
        strcat( g_cPrinterBuffer, ","); // spacer
        sprintf( cBuffer, " %3.2f,", g_ProcessData.m_fDtoAPerKilo);
        strcat( g_cPrinterBuffer, cBuffer);

// rpm per kg
        strcat( g_cPrinterBuffer, strRPMPerKg[0]);
        sprintf( cBuffer, " %3.2f,", g_ProcessData.m_fRPMPerKilo);
        strcat( g_cPrinterBuffer, cBuffer);

// control type
        strcat( g_cPrinterBuffer, strControlType[0]);
        sprintf( cBuffer, ",%d,", g_CalibrationData.m_nControlType);
        strcat( g_cPrinterBuffer, cBuffer);

// control mode
        strcat( g_cPrinterBuffer, strControlModeDiag[0]);
        sprintf( cBuffer, ",%d,", g_CalibrationData.m_nControlMode);
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );

// line speed
// control log
        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogControl[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "Length," );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );

// line speed
        strcat( g_cPrinterBuffer, strLineSpeed[0]);
        sprintf( cBuffer, ",%3.2f,", g_fLineSpeed5sec );  // line speed
        strcat( g_cPrinterBuffer, cBuffer);

// batch length
//
        strcat( g_cPrinterBuffer, strBatchLength[0]);
        sprintf( cBuffer, " %3.2f,", g_fBatchLength );  // cycle time in seconds
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );



// statistical data

// samples counter

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogStatistics[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strSampleCounter[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%d,", g_nComponentWeightSampleCounter[i] );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );


// samples

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogStatistics[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strSamples[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%7.1f,", (1000.0 * g_fComponentWeightSamples[i][g_nComponentWeightSampleCounter[i]-1])  );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// maximum

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogStatistics[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strMaximumWeight[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%7.1f,", (1000.0 * g_fComponentWeightMaximum[i])  );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// minimum

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogStatistics[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strMinimumWeight[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%7.1f,", (1000.0 * g_fComponentWeightMinimum[i]) );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// mean

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogStatistics[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strMeanWeight[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%7.1f,", ( 1000.0 * g_fComponentWeightMean[i]) );
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );

// standard deviation

        sprintf( cBuffer, "%s,%s,",strLogBatch[g_nLanguage],strLogStatistics[g_nLanguage]);
        strcat( g_cPrinterBuffer, cBuffer );
        sprintf( cBuffer, "%s,",strStandardDeviation[g_nLanguage] );   //
        strcat( g_cPrinterBuffer, cBuffer);
// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            sprintf( cBuffer, "C%2d,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            sprintf( cBuffer, "%7.3f,", (1000.0 * g_fComponentWeightStdDev[i]));
            strcat( g_cPrinterBuffer, cBuffer);
        }
        strcat( g_cPrinterBuffer, strNewLine );
        strcat( g_cPrinterBuffer, strNewLine );


/*
            //fValue = (float)g_lCmpRawCounts[i];
            //sprintf( cBuffer, " %6.0f,", fValue  );     // Raw CTS
            //strcat( g_cPrinterBuffer, cBuffer);

            //fValue = (float)g_lHopperTareCounts;
            //sprintf( cBuffer, " %6.0f,", fValue  );     // Hopper tare CTS
            //strcat( g_cPrinterBuffer, cBuffer);

            //fValue = (float)g_lCmpTareCounts[i];
            //sprintf( cBuffer, " %6.0f,", fValue  );     // component tare CTS
            //strcat( g_cPrinterBuffer, cBuffer);

*/

}
void CSVVersion( void )
{
    unsigned int i;
    char    cBuffer[MAX_PRINT_WIDTH+1];

    g_cPrinterBuffer[0] = 0;

// date, time
       sprintf( cBuffer, "%04d/%02d/%02d,%02d:%02d:%02d,",g_CurrentTime[TIME_YEAR],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( g_cPrinterBuffer, cBuffer );
// batch no.
       sprintf( cBuffer, "%d,", g_nCycleCounter  );    //
       strcat( g_cPrinterBuffer, cBuffer );
// target %
        for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            if(g_CurrentRecipe.m_fPercentage[i] != 0.0f)
            {
                sprintf( cBuffer, "C%02d Set/Act %%=,  %3.02f  ,",i+1,g_CurrentRecipe.m_fPercentage[i] );    // set %
                strcat( g_cPrinterBuffer, cBuffer);
//                if(g_bShowInstantaneousWeight)
                if(1)
                {
                    sprintf( cBuffer, "%3.02f,", g_fComponentActualPercentage[i] );
                }
                else
                {
                    sprintf( cBuffer, "%3.02f,", g_fComponentActualPercentageRR[i] );
                }
                strcat( g_cPrinterBuffer, cBuffer);
            }
        }


       for( i=0; i<g_CalibrationData.m_nComponents; i++)
        {
            if(g_CurrentRecipe.m_fPercentage[i] != 0.0f)
            {
        // comp no.
            sprintf( cBuffer, "C%02d Tar&Act Wt=,",i+1 );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
        // target wt.
            if((g_bTopUpFlag)&& (i+1 == g_CalibrationData.m_nTopUpCompNoRef))
            {
                sprintf( cBuffer, "%7.1f, ", (1000.0 * g_fComponentTargetTopUpWeight[i]) );     // target wt.
            }
            else
            {
                sprintf( cBuffer, "%7.1f, ", (1000.0 * g_fComponentTargetWeight[i]) );     // target wt.
            }

            strcat( g_cPrinterBuffer, cBuffer);

        // Actual wt.
            sprintf( cBuffer, "%7.1f,",(1000.0 * g_fComponentActualWeight[i]) );   // comp no.
            strcat( g_cPrinterBuffer, cBuffer);
            }
        }
        sprintf( cBuffer, "Total Wt,%7.1f,",(1000.0 * g_fWeightInHopper) );   // comp no.
        strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, strNewLine );
}




//////////////////////////////////////////////////////
// function PrintControlDiagnostics
// prints a single line of diagnostics every second.
//
//
//////////////////////////////////////////////////////
void PrintControlDiagnostics( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];

    g_cPrinterBuffer[0] = 0;

 if(g_bManualAutoFlag == AUTO)
    {
     sprintf( cBuffer, "A" );
    }
    else
    {
     sprintf( cBuffer, "M" );
    }
    strcat( g_cPrinterBuffer, cBuffer );

 //   time
    sprintf( cBuffer, " %02d:%02d ", g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE]);
    strcat( g_cPrinterBuffer, cBuffer );

// g/m
//

        sprintf( cBuffer, " %3.1f ", g_CurrentRecipe.m_fTotalWeightLength );
        strcat( g_cPrinterBuffer, cBuffer);

//        sprintf( cBuffer, " %4.3f ", g_fWeightPerMeter );
//        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, " %3.1f", g_fWeightPerMeterRR );
        strcat( g_cPrinterBuffer, cBuffer);

        sprintf( cBuffer, " %2.2f", g_fLineSpeed5sec );  // line speed
        strcat( g_cPrinterBuffer, cBuffer);

        //d/a set point
        sprintf( cBuffer, " %4d", g_arrnWriteSEIMBTable[MB_SEI_DA] );  // d/a sp       strcat( g_cPrinterBuffer, cBuffer);
        strcat( g_cPrinterBuffer, cBuffer);


        sprintf( cBuffer, " %4d", g_nExtruderTachoReadingAverage );  // tacho
        strcat( g_cPrinterBuffer, cBuffer);

        // command

        sprintf( cBuffer, " %x", g_arrnWriteSEIMBTable[MB_SEI_COMMAND] );  //
        strcat( g_cPrinterBuffer, cBuffer);

        // alarm state

        sprintf( cBuffer, " %x", g_nSEIAlarmState );  // tacho
        strcat( g_cPrinterBuffer, cBuffer);

// control type
        sprintf( cBuffer, " %d", g_CalibrationData.m_nControlType);
        strcat( g_cPrinterBuffer, cBuffer);

// control mode
        sprintf( cBuffer, " %d", g_CalibrationData.m_nControlMode);
        strcat( g_cPrinterBuffer, cBuffer);

        strcat( g_cPrinterBuffer, strNewLine );

   if( g_CalibrationData.m_nMaterialTransportDelay == 59 )
   {
       WriteDiagnosticDataToSDCard();
       ReInitialisePinsUsedBySDCard();
   }
//   else
//   if( g_CalibrationData.m_nMaterialTransportDelay == 60 )
//   {
//       SetupMttty();
//       iprintf("%s",g_cPrinterBuffer);
//   }

}


// print time, g/m sp, act, % error,line speed, tachosp,tachoact,command, sei inputs



/*
BATCH BLENDER DIAGNOSTIC MCE09-152 02-11-04, 17:49:03       WEIGHT(kg) 1797.1
 AUTO     MAX KG/H    00
KG/H $5      220.00  219.95  219.95 HAUL-OFF(m/min)         CONTROL      %    .00
* WT/M  $6 *    .00     .00     .00  CYCLE TIME &   49.87 LOAD TIME   00 DRIVE $4   41.3
Hardware serial No :      13         00
           COMP 1   COMP 2   COMP 3   COMP 4
TAR $1    1419.9      0.0      0.0      0.0
ACT $213   1797.1      0.0      0.0      0.0
TARGET    36711       00       00       00
ACTUAL    46461       00       00       00
RAW CT   129405       00       00       00
TARE CT   82916       00       00       00
TARE CT   82971       00       00       00
WT CST     3868       00       00       00
SET %     71.00    28.00     1.00      .00
ACT %    100.00      .00      .00      .00
TIME       1737       00       00       00

TIME  $3  1.5273     .0000    .0000    .0000
CNT/INT  26.7478    .0000    .0000    .0000


*/
/*

PRINTRESETSOURCE:
        LDAB    #$01
        STAB    PIP_FLAG
        LDAB    #RAMBANK
        TBZK
        LDZ     #PRNT_BUF
PSOURCE JSR     CRLF
        LDAA    RESETSOURCE
        ANDA    #%01000000      ; POWER ON RESET
        BEQ     NOTPOR  ; NO

        LDY     #POWERONRM    ;
        JSR     YCSPBUF
NOTPOR  LDAA    RESETSOURCE
        ANDA    #%00100000      ; SOFTWARE WATCHDOG
        BEQ     NOTSWW

        LDY     #SOFTWATDM
        JSR     YCSPBUF

NOTSWW: LDAA    RESETSOURCE
        ANDA    #%10000000
        BEQ     NOTEXT
        LDY     #EXTERNRM
        JSR     YCSPBUF
NOTEXT:
        JSR     PRNTD
        JSR     CRLF
        LDY      #SERIALNOMS      ; HARDWARE SERIAL NO.
        JSR      YCSPBUF         ; DISPLAY
        LDX      #SBBSERNO
        JSR     CCPBUF
        JSR     CRLF             ; FEED.
        LDY      #SWSERIALNOMS      ; HARDWARE SERIAL NO.
        JSR      YCSPBUF         ; DISPLAY
        LDX      #CALMODSERNO
        JSR     CCPBUF
        JSR     CRLF
        JSR     TERMPBUF
        LDX     #PRNT_BUF
        JSR     ST_PRINT
        RTS


*/

