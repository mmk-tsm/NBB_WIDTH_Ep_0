///////////////////////////////////////////////////////////////
// MBMastr.c
//

// M.McKiernan                         22-10-2003
//
// M.McKiernan                         17-01-2005
// First pass.
//
// P.Smith                              30-03-2005
// Call the SEI read 4 times a second
// And g_arrnWriteSEIMBTable[MB_SEI_COMMAND] with FF00 to ensure
// that the lsb of the message is reset to 00.
//
// P.Smith                              1/12/05
// opposite orientation for g_bSBBL3Fitted flag, remote display
// is considered the norm.
//
// P.Smith                              11/1/06
// Correct warnings
// nChecksum changed to unsigned int
// commented out int i=0 in ModbusMasterTest
//
// P.Smith                      23/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
// removed //nb #pragma TRAP_PROC SAVE_REGS
//
// P.Smith                      24/1/06
// added #include "NBBGpio.h"
// removed interrupt from void SerialHandler( void )
// make BOOL bTest = arrpDeviceList[i]->m_bPollingEnable volatile
// removed //nbb            int     nReplyCount = 0;
// added #include "16R40C.h"
// problem with arrpDeviceList[i]->arrnWriteData = g_arrTextBuffer; //
// added int		g_bCheckingComms = FALSE;
// added
// int		g_bCheckingComms = FALSE;
//BOOL	g_bCursorMode = FALSE;
//int		g_nCurrentRow = 0;
//int		g_nCurrentColumn = 0;
// g_bCheckingComms = FALSE; set to BOOL was originaly int
// added char	g_arrAttributeBuffer[ROWS * COLUMNS];
// added int		g_nKeyCode = KEY_INVALID_CODE;
//
//
// P.Smith                      7/2/06
// BOOL	g_bCheckingComms = FALSE;  changed to extern
//
// P.Smith                          15/2/06
// name change CharsAndInts -> CharsAndWords
//
// P.Smith                          15/2/06
// Remove commented out code, check for g_bNBBRevB1Hardware when switching on the transmitter
//
// P.Smith                  15/1/07
// Remove reference to Duart.
// Remove g_bNBBRevB1Hardware check.
//
// P.Smith                      2/2/07
// Removed unused functions
//
// P.Smith                      6/2/07
// remove EnableMasterTransmitter
//
// P.Smith                      7/2/07
// in TxRxOff switch off interrupts.
//
// P.Smith                      12/6/07
// added structControllerData  structOptimisationData
// check for optimisation enable in ConfigureModbusMaster
//
// P.Smith                      12/6/07
// correct the optimisation card comms, the check in DecodeReadResponse
//
// P.Smith                      19/6/07
// also the write to the optimisation card is also added.
//
// P.Smith                      24/10/07
// remove check of m_bSeiWriteCommandCompleted
//
// P.Smith                      7/12/07
// only run write to SEI if g_nWriteHoldOffCounter is zero, this allows the write to the sei
// to be held off initially.
// Remove original  AddToQueue( COMMS_WRITE_CALIBRATION,
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          11/2/08
// set g_bSEIReadDataReady to TRUE on finish of sei read comms
//
// P.Smith                          22/2/08
// added CheckForValidIncDecTachoSetpoint call, if okay then add to queue is called
// if not switch blender to manual and the write will be enabled to allow the normal
// write to the sei to happen.
//
// P.Smith                          11/3/08
// added case COMMS_INC_DEC_WRITE_COMMAND before generatewriterequest to allow the command to run
//
// P.Smith                          20/3/08
// set arrpDeviceList[i]->m_nIncDecSetpointWriteRetries to 0
// check for g_nSEIWriteRequiredCounter, if 0 use short command, if non zero
// use long command and decrement counter
// in GenerateWriteRequest, check for COMMS_INC_DEC_WRITE_COMMAND
// and set command to SEI_INCREASE_DECREASE_COMMANDS_START
// and SEI_INCREASE_DECREASE_COMMANDS_END
// set g_sModbusMaster.m_nStartIndex to SEI_INCREASE_DECREASE_COMMANDS_START
// in DecodeWriteResponse, set g_sModbusMaster.m_pController->m_nIncDecSetpointWriteRetries to 0
// if g_bSEIIncDecSetpointDownload is true  AddToQueue( COMMS_INC_DEC_WRITE_COMMAND) if valid set point
// if COMMS_INC_DEC_WRITE_COMMAND timed out add to queue again
// check for COMMS_INC_DEC_WRITE_COMMAND before GenerateWriteRequest
// added CheckForValidIncDecTachoSetpoint to check for acceptable tacho limit
//
// P.Smith                          20/3/08
// set nHiReg to SEI_INCREASE_DECREASE_COMMANDS_START
//
// M.McKiernan                      30/4/08
// Copy taken from batch blender v2.52.  Original MBMastr.cpp had not changed from time copy
// of blender software taken for LiW, i.e. 2.36.
// M.McKiernan                      06/5/08
// Defined read and write Modbus table arrays for LIW cards -  g_arrnReadLiWMBTable,  g_arrnWriteLiWMBTable
// In  ConfigureModbusMaster(), added intialisation for LiW's. -let LiW addresses start at 5
// In GenerateReadRequest( void ) added case for Liw (EQUIP_LIW).
// Added polling messages for LiW in PITMBMHandler.
// See all case EQUIP_LIW throughout file.
//
// M.McKiernan                      21/7/08
// Removed all polling for "old" display panel (UFP related).
// Experimented with polling LiW's 4 times/sec, see "moved to 4Hz see KeyPollTimer"
//
//
//
// M.McKiernan                      30/4/09
// moved LiW polling to 2Hz see DisplayPollTimer  ---- case EQUIP_LIW:
// g_bSecondRolloverForPolling resets DisplayPollTimer, i.e. synchronise to RTC
// UpdateLiWCardADCData called off DisplayPollTimer to synchronise.
//
//
// M.McKiernan                      01/5/09
// In DecodeReadResponse() diagnostics relating to LiW poll (A or B) to use. - EQUIP_LIW, see 05052009
// In PITMBMHandler() record poll time for LiW's: g_cLiWPollTime = 'A';    //A poll is first one
// Increment  g_nLiWPollDecisionCtr++; and every 20 seconds call DecideWhichLiWPollToUse();
// Function DecideWhichLiWPollToUse( ) added.
//
// P.Smith                          30/4/08
// set g_bSEIIncDecSetpointDownload to false after it is checked for true
// in AddToQueue( COMMS_INC_DEC_WRITE_COMMAND, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
// g_sModbusMaster.m_nMessageValue is set to 0.
//
// P.Smith                          19/6/09
// remove operator panel reference
//
// P.Smith                          19/6/09
// add to manual event to log file for increase/ mode
//
// P.Smith                          19/6/09
// remove g_arrnMBTable & comms array externs
//
// P.Smith                          19/6/09
// inc IncrementOngoingModulesHistoryChecksumErrorCtr,IncrementOngoingModulesHistoryGoodRxCtr
// IncrementOngoingModulesHistoryTimeoutCtr
//
// P.Smith                          2/7/09
// add back in lls add to queue, this was commented out for some reason.
//
// P.Smith                          17/9/09
// added nVAC8IOCards check for > 16 component and set g_nVAC8IOCards to the number
// of cards.
//
// P.Smith                          15/10/09
// added check for vac 8 expansion, add to comms if required.
// address of expansion card is FIRST_VAC8IO_EXPANSION_ADDRESS plus the vac 8 card
// that is being used.
// note that the vac 8 expansion card is the last in the list.
// set g_nVAC8ExpansionIndex to the index of the card.
//
// P.Smith                          7/12/09
// added  break in case EQUIP_OPTIMISATION:
//
// P.Smith                          8/6/10
// added summary and commands for ssif write short and long.
//
// P.Smith                          15/7/10
// define SSIF_ADDRESS as the ssif address, it may be necessary to change
// this as it is conflicting with the SEI at the moment.
////////////////////////////////////////////////////////////////
#include <Stdio.h>

//nbb #include "Hardware.h"
#include "General.h"
#include "SerialStuff.h"
#include "InitComms.h"

#include "TimeDate.h"
#include "ConfigurationFunctions.h"
#include "16R40C.h"
#include "BatVars.h"
#include "BatchCalibrationFunctions.h"
#include    "TSMPeripheralsMBIndices.h"
#include "MBMaster.h"
#include "CalculateModbusCRC.h"
#include "BatVars2.h"
#include <basictypes.h>
#include "NBBGpio.h"
#include <String.h>
#include "mbmasternbbhand.h"
#include "Conalg.h"
#include "Alarms.h"
#include "LiWVars.h"
//#include "ProcessLiWData.h"
#include "VacVars.h"
#include "HistoryLog.h"
#include "Eventlog.h"


extern  int g_nOperatorWaitTime;
extern  int g_nCurrentRow;
extern  int g_nCurrentColumn;
extern  BOOL    g_bCursorMode;
extern  WORD g_lRTCCount;


//extern    BOOL    g_bFrontRollReportStart;
//extern    BOOL    g_bBackRollReportStart;
//extern    BOOL    g_bOrderReportStart;


// Temp
extern  BOOL        g_bCheckingComms;

extern  int g_nPIT_Int_CounterU;

extern  char    g_arrTextBuffer[];

extern CalDataStruct    g_CalibrationData;
extern structLiWCardData g_sLiWCard[];

extern   structVacCalib   g_VacCalibrationData;
// modbus tables for TSM peripherals.


//
// The data for UFP1 is simply the VRAM in blender (640 bytes - 320 words)
// Also it is write only - not read back.
// #define g_arrnWriteUFP1MBTable[]     g_arrTextBuffer[]
//
int g_arrnWriteUFP2MBTable[UFP2_TABLE_WRITE_SIZE];
int g_arrnReadUFP2MBTable[UFP2_TABLE_READ_SIZE];

//LiW's
int g_arrnWriteLiWMBTable[MAX_LIW_CARDS][LIW_TABLE_WRITE_SIZE];
int g_arrnReadLiWMBTable[MAX_LIW_CARDS][LIW_TABLE_READ_SIZE];

//VAC8IO's - Can be more than 1.
int g_arrnWriteVAC8IOMBTable[MAX_VAC8IO_CARDS][VAC8IO_TABLE_WRITE_SIZE];
int g_arrnReadVAC8IOMBTable[MAX_VAC8IO_CARDS][VAC8IO_TABLE_READ_SIZE];


int g_arrnReadSSIFMBTable[SSIF_TABLE_READ_SIZE];
int g_arrnWriteSSIFMBTable[SSIF_TABLE_READ_SIZE];
BOOL	g_bSSIFWriteCalibrationRequired = FALSE;
BOOL	g_bSSIFWriteCommandRequired = FALSE;



// Structures for each TSM peripheral.

    structControllerData        structSSIFData;
    structControllerData        structUFP2Data;
    structControllerData        structSEIData;
    structControllerData        structLLSData;
    structControllerData        structOptimisationData;
    structControllerData        structLiWData[MAX_LIW_CARDS];
    structControllerData        structVAC8IOData[MAX_VAC8IO_CARDS];


    structControllerData    *arrpDeviceList[MAX_COMMS_UNITS];       // note this is an array of pointers to structures. nb removed far
    structCommsData         g_sModbusMaster;

    structAttributeData structUFPAttributeData[MAX_ATT_BLOCKS];

#define KEY_INVALID_CODE    -1
extern BOOL	g_bCheckingComms;   //
BOOL	g_bCursorMode = FALSE;
int		g_nCurrentRow = 0;
int		g_nCurrentColumn = 0;      //
char	g_arrAttributeBuffer[ROWS * COLUMNS];
int		g_nKeyCode = KEY_INVALID_CODE;


////////////////////////////////////////////////////////////////
// ConfigureModbusMaster()          // No direct equivalent in assembler
//
// How it works:
// Each device to be communicated with will maintain it's own data structure and
// message queue.    The timer handler will poll each device in
// turn, allowing it to process the top message in the queue.
// Provision is made for 4 controllers, Remote Operator Panel (which is treated as 2 MB slave devices UFP1 & UFP2)
// the SEI card and the LLS card.
// The addresses for the units are fixed, Panel (UFP1 and UFP2) are at address 1, SEI at 2, LLS at 3.
// The data structures are initialised for each device initially.  Then polling is enabled only for units
// that are present.  The presence of SEI and LLS are determined by calibration data (m_nPeripheralCardsPresent).
// Currently using SBB link L3 to determine if remote operator panel present.
//
// M.McKiernan                         17-01-2005
// First pass.
//
// P.Smith                              30-03-2005
// Change the SEI read to run 4 times per second,this higher polling is
// required to allow the SEI increase/ decrease to operate.
//
////////////////////////////////////////////////////////////////
void ConfigureModbusMaster( void )
{
  int i = 0;
  int j;
  char cBuffer[10];
  char cBuf[10];
  int nLiWCards = 0;
  int nVAC8IOCards = 0;

  if(g_CalibrationData.m_nComponents > 1)
   nLiWCards = 0;// nbb--todo--vac set to 0 for now g_CalibrationData.m_nComponents - 1;       // one less than no. of components.

   if(g_VacCalibrationData.m_nLoaders > 0 && g_VacCalibrationData.m_nLoaders <=8 )
      nVAC8IOCards = 1;
   else if(g_VacCalibrationData.m_nLoaders > 8 && g_VacCalibrationData.m_nLoaders <=16 )
      nVAC8IOCards = 2;
   else if(g_VacCalibrationData.m_nLoaders > 16 )
       nVAC8IOCards = 3;                           //NB limit to 16 loaders, 2 VAC8's for now.
   if(nVAC8IOCards > MAX_VAC8IO_CARDS)
      nVAC8IOCards = MAX_VAC8IO_CARDS;       // limit to max possible.
// store into global
   g_nVaccumLoadingVAC8IOCards = nVAC8IOCards;
   if(g_CalibrationData.m_bVac8Expansion) // expansion card required
   nVAC8IOCards++;
   g_nVAC8IOCards = nVAC8IOCards;

    // device specific stuff

// --REVIEW-- use L3 to indicated remote operator paneL fitted.
    i = 0;      // use i to count installed units.
//    if( !g_bSBBL3Fitted )
    if( 1 )
    {
        g_bRemoteOperatorPanelPresent = TRUE;
        // UFP1
        arrpDeviceList[i] = &structSSIFData;            // Struture always exists whether used or not.
            arrpDeviceList[i]->m_nSystemType = EQUIP_SSIF;  //
            arrpDeviceList[i]->m_nSystemAddress = SSIF_ADDRESS;
            sprintf( cBuffer, "UFP-1");
            strcpy( arrpDeviceList[i]->m_arrcDescriptor, cBuffer );
            arrpDeviceList[i]->arrnReadData = g_arrnReadSSIFMBTable;
            arrpDeviceList[i]->arrnWriteData = g_arrnWriteSSIFMBTable;
            arrpDeviceList[i]->m_bWriteCalibration = TRUE;          //

            // Write buffer for UFP1 is the display ram, i.e. VRAM
 //nbb--todolp-- causing compiler error  arrpDeviceList[i]->arrnWriteData = g_arrTextBuffer; //
            arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = 0;    // first register address
        arrpDeviceList[i]->m_bPollingEnable = TRUE;
        g_nDeviceListIndexUFP1 = i;                 // index in device list for UFP1

        i++;
        // UFP2
        arrpDeviceList[i] = &structUFP2Data;            //
            arrpDeviceList[i]->m_nSystemType = EQUIP_UFP2;  //
            arrpDeviceList[i]->m_nSystemAddress = 1;
            sprintf( cBuffer, "UFP-2");
            strcpy( arrpDeviceList[i]->m_arrcDescriptor, cBuffer );
            arrpDeviceList[i]->arrnReadData = g_arrnReadUFP2MBTable;
            arrpDeviceList[i]->arrnWriteData = g_arrnWriteUFP2MBTable;
            arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = UFP2_OFFSET;  // first register address in UFP2.
        arrpDeviceList[i]->m_bPollingEnable = TRUE;
        g_nDeviceListIndexUFP2 = i;                 // index in device list for UFP2

        i++;

    }
    else
    {
        g_bRemoteOperatorPanelPresent = FALSE;
    }
// Configure comms structure according to whether devices fitted or not.
// Calibration data - EIOMITTED - YES/NO (b0 = SEI)
    if( g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS )  // is SEI present??
    {
        g_bSEIPresent = TRUE;
        arrpDeviceList[i] = &structSEIData;         //
            arrpDeviceList[i]->m_nSystemType = EQUIP_SEI;   //
            arrpDeviceList[i]->m_nSystemAddress = 2;
            sprintf( cBuffer, "SEI");
            strcpy( arrpDeviceList[i]->m_arrcDescriptor, cBuffer );
            arrpDeviceList[i]->arrnReadData = g_arrnReadSEIMBTable;

            arrpDeviceList[i]->arrnWriteData = g_arrnWriteSEIMBTable;
            arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = 0;    // first register address
            arrpDeviceList[i]->m_bWriteCalibration = TRUE;          // trigger writing calibration data to SEI.
        arrpDeviceList[i]->m_bPollingEnable = TRUE;
        g_nDeviceListIndexSEI = i;                  // index in device list for SEI
        i++;    //
    }
    else
    {
        g_bSEIPresent = FALSE;
    }

//Calibration data, EIOMITTED - YES/NO (b1 = LLS,...)
// DEBUGGING  --REVIEW-- // test purposes make LLS present.
//  g_CalibrationData.m_nPeripheralCardsPresent |= 0x0002;      // test purposes make LLS present.

    if( g_CalibrationData.m_nPeripheralCardsPresent & LLSENBITPOS )  // is LLS present??  // EIOMITTED - YES/NO (b0 = SEI, b1 = LLS,...)
    {
        g_bLLSPresent = TRUE;
        arrpDeviceList[i] = &structLLSData;         //
            arrpDeviceList[i]->m_nSystemType = EQUIP_LLS;   //
            arrpDeviceList[i]->m_nSystemAddress = 3;
            sprintf( cBuffer, "LLS");
            strcpy( arrpDeviceList[i]->m_arrcDescriptor, cBuffer );
            arrpDeviceList[i]->arrnReadData = g_arrnReadLLSMBTable;
            arrpDeviceList[i]->arrnWriteData = g_arrnWriteLLSMBTable;
            arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = 0;    // first register address
        arrpDeviceList[i]->m_bPollingEnable = TRUE;
        g_nDeviceListIndexLLS = i;                  // index in device list for LLS
        i++;    //
    }
    else
    {
        g_bLLSPresent = FALSE;
    }


    if( g_CalibrationData.m_nPeripheralCardsPresent & OPTIMISATIONENABLEBITPOS )  // is LLS present??  // EIOMITTED - YES/NO (b0 = SEI, b1 = LLS,...)
    {
        g_bOptimisationPresent = TRUE;
        arrpDeviceList[i] = &structOptimisationData;         //
            arrpDeviceList[i]->m_nSystemType = EQUIP_OPTIMISATION;   //
            arrpDeviceList[i]->m_nSystemAddress = 4;
            sprintf( cBuffer, "Optimis");
            strcpy( arrpDeviceList[i]->m_arrcDescriptor, cBuffer );
            arrpDeviceList[i]->arrnReadData = g_arrnReadOptimisationMBTable;
            arrpDeviceList[i]->arrnWriteData = g_arrnWriteOptimisationMBTable;
            arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = 0;    // first register address
        arrpDeviceList[i]->m_bPollingEnable = TRUE;
        g_nDeviceListIndexOptimisation = i;                  // index in device list for LLS
        i++;    //
    }
    else
    {
        g_bOptimisationPresent = FALSE;
    }

// Liws
    if( nLiWCards > 0 )  // any LiW's present??  //
    {
      for( j=0; j<nLiWCards && j<MAX_LIW_CARDS; j++ )
      {
         arrpDeviceList[i] = &structLiWData[j];         //
            arrpDeviceList[i]->m_nSystemType = EQUIP_LIW;   //
            arrpDeviceList[i]->m_nSystemAddress = FIRST_LIW_ADDRESS+j;      //let LiW addresses start at 5
            sprintf( cBuffer, "LIW-");
            sprintf( cBuf, "%d", j);
            strcat( cBuffer, cBuf );          // card no.
            strcpy( arrpDeviceList[i]->m_arrcDescriptor, cBuffer );
            arrpDeviceList[i]->arrnReadData = g_arrnReadLiWMBTable[j];
            arrpDeviceList[i]->arrnWriteData = g_arrnWriteLiWMBTable[j];
            arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = 0;    // first register address
            arrpDeviceList[i]->m_bWriteCalibration = FALSE;          // trigger writing calibration data to SEI.
//ToDo
        arrpDeviceList[i]->m_bPollingEnable = TRUE;
        g_nDeviceListIndexLIW[j] = i;                  // index in device list for LiW(s).
        i++;    //
      }

    }
    else
    {
        ;      // nothing
    }

// end LiW's

// VAC8IO's
//    if( 0 )  // disable????  //
    if( nVAC8IOCards > 0 )  // any LiW's present??  //
    {
      for( j=0; j<nVAC8IOCards && j<MAX_VAC8IO_CARDS; j++ )
      {
            arrpDeviceList[i] = &structVAC8IOData[j];         //
            arrpDeviceList[i]->m_nSystemType = EQUIP_VAC8IO;   //
            if(g_CalibrationData.m_bVac8Expansion && (j == nVAC8IOCards-1))
            {
                arrpDeviceList[i]->m_nSystemAddress = FIRST_VAC8IO_EXPANSION_ADDRESS+j;      //let VAC8IO addresses start at
                g_nVAC8ExpansionIndex = j;
            }
            else
            {
                arrpDeviceList[i]->m_nSystemAddress = FIRST_VAC8IO_ADDRESS+j;      //let VAC8IO addresses start at
            }
            sprintf( cBuffer, "VAC8IO-");
            sprintf( cBuf, "%d", j);
            strcat( cBuffer, cBuf );          // card no.
            strcpy( arrpDeviceList[i]->m_arrcDescriptor, cBuffer );
            arrpDeviceList[i]->arrnReadData = g_arrnReadVAC8IOMBTable[j];
            arrpDeviceList[i]->arrnWriteData = g_arrnWriteVAC8IOMBTable[j];
            arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = 0;    // first register address
            arrpDeviceList[i]->m_bWriteCalibration = FALSE;          // trigger writing calibration data to VAC8IO
//ToDo
        arrpDeviceList[i]->m_bPollingEnable = TRUE;
        g_nDeviceListIndexVAC8IO[j] = i;                  // index in device list for VAC8IO(s).
        i++;    //
      }


    }

// end VAC8IO's

    g_sModbusMaster.m_nInstalledUnits = i;

    for( i = 0; i < g_sModbusMaster.m_nInstalledUnits; i++ )
    {
// load group no.
            arrpDeviceList[i]->m_cGroupNumber = 1;      // set all group no's to 1  --REVIEW--
//          arrpDeviceList[i]->m_nDeviceFirstRegisterOffset = 0;        // set all offsets to 0, will be configured seperately if non 0.

            arrpDeviceList[i]->m_nQueueHead = 0;
            arrpDeviceList[i]->m_nQueueTail = 0;
            arrpDeviceList[i]->m_bFull = FALSE;
            arrpDeviceList[i]->m_bEmpty = TRUE;
            arrpDeviceList[i]->m_bLock = FALSE;

            arrpDeviceList[i]->m_bPollCalibration = FALSE;
            arrpDeviceList[i]->m_bSetpointsValid = FALSE;
            arrpDeviceList[i]->m_bCalibrationValid = FALSE;
            arrpDeviceList[i]->m_bNewSummaryData = FALSE;
            arrpDeviceList[i]->m_bIDValid = FALSE;

            arrpDeviceList[i]->m_nSetpointRetries = 0;
            arrpDeviceList[i]->m_nCalibrationRetries = 0;
            arrpDeviceList[i]->m_nSingleWriteRetries = 0;
            arrpDeviceList[i]->m_nOptionWriteRetries = 0;
            arrpDeviceList[i]->m_nCommandWriteRetries = 0;
            arrpDeviceList[i]->m_nDisplayWriteRetries = 0;
            arrpDeviceList[i]->m_nIncDecSetpointWriteRetries = 0;

            arrpDeviceList[i]->m_nCommsTimeouts = 0;
            arrpDeviceList[i]->m_bActiveTimeoutAlarm = FALSE;

            arrpDeviceList[i]->m_lGoodTx = 0;
            arrpDeviceList[i]->m_lGoodRx = 0;
            arrpDeviceList[i]->m_lTxTimeoutCounter = 0;
            arrpDeviceList[i]->m_lRxTimeoutCounter = 0;
            arrpDeviceList[i]->m_lErrorResponseCounter = 0;
            arrpDeviceList[i]->m_lChecksumErrorCounter = 0;
            arrpDeviceList[i]->m_lInvalidInterruptCounter = 0;

    }


    // Enable the communications if some installed units.
    if( g_sModbusMaster.m_nInstalledUnits > 0 )
        g_sModbusMaster.m_bCommsEnable = TRUE;


}


// ////////////////////////////////end of new section


////////////////////////////////////////////////////////////////
// InitialiseModbusMaster()         //
//
// Copied from CP2000
// M.McKiernan                         18-01-2005
// First pass.
////////////////////////////////////////////////////////////////
void InitialiseModbusMaster( void )
{
    g_sModbusMaster.m_bCommsEnable = FALSE;

    g_sModbusMaster.m_nCurrentUnit = 0;     //mmk new 24012005 - was MAX_COMMS_UNITS;
    g_sModbusMaster.m_nInstalledUnits = 0;
    g_sModbusMaster.m_pController = NULL;

    g_sModbusMaster.m_nInterface = INTERFACE_RS422;
    g_sModbusMaster.m_nStripLength = 0;

    g_sModbusMaster.m_nRetries = MODBUS_READ_RETRIES;
    g_sModbusMaster.m_nTimeoutCounter = MODBUS_WRITE_TIMEOUT;   //MODBUS_BLENDER_READ_TIMEOUT;
    g_sModbusMaster.m_bActive = FALSE;
    g_sModbusMaster.m_nMessageType = COMMS_READ_SUMMARY;
    g_sModbusMaster.m_nPollTimer = POLL_TIMER;
    g_sModbusMaster.m_nKeyPollTimer = POLL_TIMER;

    g_sModbusMaster.m_nMessageStartDelay = 0;
    g_sModbusMaster.m_bProcessingMessage = FALSE;
    g_sModbusMaster.m_bRunStartDelay = FALSE;

    g_sModbusMaster.m_bStartFound = FALSE;

    g_sModbusMaster.m_nTXIndex = 0;
    g_sModbusMaster.m_nTXEndIndex = 0;
    g_sModbusMaster.m_bTXEnable = FALSE;

    g_sModbusMaster.m_nRXIndex = 0;
    g_sModbusMaster.m_bRXEnable = FALSE;
    g_sModbusMaster.m_nRXLength = 0;        // Expected rx length


    g_sModbusMaster.m_lGoodTx = 0;
    g_sModbusMaster.m_lGoodRx = 0;
    g_sModbusMaster.m_lTxTimeoutCounter = 0;
    g_sModbusMaster.m_lRxTimeoutCounter = 0;
    g_sModbusMaster.m_lErrorResponseCounter = 0;
    g_sModbusMaster.m_lChecksumErrorCounter = 0;
    g_sModbusMaster.m_lInvalidInterruptCounter = 0;

    TxRxOff();
}


////////////////////////////////////////////////////////////////
// TxRxOff()                        //
//
// Copied from CP2000
// M.McKiernan                         18-01-2005
// First pass.
////////////////////////////////////////////////////////////////
void TxRxOff( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_MASTER_COMMS_PORT];
   ThisUart->uisr = NO_INTERRUPTS;

    g_sModbusMaster.m_bActive = FALSE;

    g_sModbusMaster.m_bTXEnable = FALSE;

	g_sModbusMaster.m_bRunStartDelay = FALSE;
}



////////////////////////////////////////////////////////////////
// GenerateReadRequest()                        //
//
// Takes the start and end registers for a Modbus read
// message and formats up the message for transmission.
//
// Entry:
//      Void.
// Exit:
//      Void.
//
// Copied from CP2000
// M.McKiernan                         18-01-2005
// First pass.
///////////////////////////////////////////////////////////
void GenerateReadRequest( void )
{
    int     nLength;
    unsigned int     nChecksum;
    int     nLoReg = -1;
    int     nHiReg = -1;
    int     i;

    // Clear the rx buffer before we transmit
    for( i = 0; i < COMMS_BUFFER_SIZE; i++ )
        g_sModbusMaster.m_cRxBuffer[i] = 0;

    // Using the structure pointed to and the message type
    // determine the start and end registers for this message
    switch( g_sModbusMaster.m_nMessageType )
    {
    case COMMS_READ_SUMMARY:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_SEI:
            nLoReg = SEI_SUMMARY_START;
            nHiReg = SEI_SUMMARY_END;

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_SUMMARY_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store
                g_sModbusMaster.m_nStartIndex = SEI_SUMMARY_START;
            break;

        case EQUIP_SSIF:        //
        	nLoReg = SSIF_SUMMARY_START;
            nHiReg = SSIF_SUMMARY_END;
            g_sModbusMaster.m_nStartIndex = SSIF_SUMMARY_START;
            break;
        case EQUIP_UFP2:        // ufp2 summary
            nLoReg = UFP2_SUMMARY_START;
            nHiReg = UFP2_SUMMARY_END;

                // If the last register is a long, we need to read in the second half by
                // Load the index to the data store
                g_sModbusMaster.m_nStartIndex = UFP2_SUMMARY_START;
            break;
        case EQUIP_LLS:
            nLoReg = LLS_SUMMARY_START;
            nHiReg = LLS_SUMMARY_END;

            // If the last register is a long, we need to read in the second half by
            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = LLS_SUMMARY_START;
            break;

        case EQUIP_OPTIMISATION:
            nLoReg = OPTIMISATION_SUMMARY_START;
            nHiReg = OPTIMISATION_SUMMARY_END;

            // If the last register is a long, we need to read in the second half by
            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = OPTIMISATION_SUMMARY_START;
            break;

        case EQUIP_LIW:
            nLoReg = LIW_SUMMARY_START;
            nHiReg = LIW_SUMMARY_END;

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_SUMMARY_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = LIW_SUMMARY_START;
            break;

        case EQUIP_VAC8IO:
            nLoReg = VAC8IO_SUMMARY_START;
            nHiReg = VAC8IO_SUMMARY_END;

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_SUMMARY_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = VAC8IO_SUMMARY_START;
            break;


        default:
            break;
        }
        break;
    case COMMS_READ_CALIBRATION:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_SEI:         // read all of the Read/Write area.
            nLoReg = SEI_COMMANDS_START;    //SEI_CALIBRATION_START;            // arrnBatchBlenderModbusTable[BATCH_CALIBRATION_START][TABLE_REGISTER_INDEX];
            nHiReg = SEI_CALIBRATION_END;               // arrnBatchBlenderModbusTable[BATCH_CALIBRATION_END][TABLE_REGISTER_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_CALIBRATION_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store

            g_sModbusMaster.m_nStartIndex = SEI_COMMANDS_START;     //SEI_CALIBRATION_START;
            break;
        case EQUIP_LLS:
            break;
        case EQUIP_SSIF:        // no read Calibration on UFP1 (just data for display)
         	break;
        case EQUIP_UFP2:        // no read Calibration on UFP2
            break;
        case EQUIP_VAC8IO:        // no read Calibration on VAC8IO
            break;
        case EQUIP_LIW:         // read all of the Read/Write area.
            nLoReg = LIW_COMMANDS_START;    //LIW_CALIBRATION_START;            // arrnBatchBlenderModbusTable[BATCH_CALIBRATION_START][TABLE_REGISTER_INDEX];
            nHiReg = LIW_CALIBRATION_END;               // arrnBatchBlenderModbusTable[BATCH_CALIBRATION_END][TABLE_REGISTER_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_CALIBRATION_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store

            g_sModbusMaster.m_nStartIndex = LIW_COMMANDS_START;     //LIW_CALIBRATION_START;
            break;

        default:
            break;
        }
        break;

    case COMMS_READ_ID:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_SEI:
            nLoReg = SEI_ID_START;          //
            nHiReg = SEI_ID_END;                //
            g_sModbusMaster.m_nStartIndex = SEI_ID_START - SEI_ID_OFFSET;   // the ID is offset in SEI (1000).
            break;

        case EQUIP_LLS:
        case EQUIP_SSIF:        // no ID read on other devices YET.
        case EQUIP_UFP2:        //
        case EQUIP_VAC8IO:       //
            break;
        case EQUIP_LIW:
            nLoReg = LIW_ID_START;          //
            nHiReg = LIW_ID_END;                //

            g_sModbusMaster.m_nStartIndex = LIW_ID_START;   // .
            break;

        default:
            break;
        }
        break;



    default:
        break;
    }

    if( nLoReg != -1 )
    {
        union   CharsAndWord     nDataValue;

        // Assemble the request
        g_sModbusMaster.m_cTxBuffer[0] = (char)g_sModbusMaster.m_pController->m_nSystemAddress; // Device address

        // Addresses starting with 40,000 use command 3.
        // Addresses starting with 30,000 use command 4.
        // All other addresses will use command 3.
        if( nLoReg >= 40000 )
        {
            g_sModbusMaster.m_cTxBuffer[1] = READ_N_WORDS_CMD_3;    // Function code

            // Strip out the high part of the address.
            nLoReg -= 40000;
            nHiReg -= 40000;
        }
        else if( nLoReg >= 30000 )
        {
            g_sModbusMaster.m_cTxBuffer[1] = READ_N_WORDS_CMD_4;    // Function code

            // Strip out the high part of the address.
            nLoReg -= 30000;
            nHiReg -= 30000;
        }
        else
            g_sModbusMaster.m_cTxBuffer[1] = READ_N_WORDS_CMD_3;    // Function code

        g_sModbusMaster.m_nTXIndex = 2;                                         // Next free buffer location

        // Store the address of the first int
        nDataValue.nValue = nLoReg;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
        g_sModbusMaster.m_nTXIndex++;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
        g_sModbusMaster.m_nTXIndex++;

        // If the number of registers exceeds the maximum
        // we will need to clip the length and run a second
        // message.
        nLength = (nHiReg - nLoReg + 1);
        if( nLength > MAX_NUMBER_REGISTERS )
        {
            // Limit the request to the maximum number of registers
            nLength = MAX_NUMBER_REGISTERS;

            // Look at the last register (nLoReg + nLength) in the table
            // and check if it's a long.  If it is we must back off by
            // one register to avoid reading a long in two seperate
            // messages and possibly displaying an incorrect value.
        }
        // Save the start register for this message
        g_sModbusMaster.m_nStartRegister = nLoReg;

        // Store the number of ints to read
        nDataValue.nValue = nLength;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
        g_sModbusMaster.m_nTXIndex++;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
        g_sModbusMaster.m_nTXIndex++;

        // calculate and store the checksum
        nChecksum = CalculateModbusCRC( g_sModbusMaster.m_cTxBuffer, g_sModbusMaster.m_nTXIndex );
        nDataValue.nValue = nChecksum;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
        g_sModbusMaster.m_nTXIndex++;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];

        // Point to the last byte in the buffer
        g_sModbusMaster.m_nTXEndIndex = g_sModbusMaster.m_nTXIndex;

        // Calculate the expected length of the response
        g_sModbusMaster.m_nRXLength = (nLength * MODBUS_REGISTER_LENGTH) + READ_RESPONSE_LENGTH;

        // If the protocol is configured for RS485 then the
        // transmit message will already be in the receive buffer.
        if( g_sModbusMaster.m_nInterface == INTERFACE_RS485 )
        {
            g_sModbusMaster.m_nRXLength += READ_REQUEST_LENGTH;

            // number of bytes to strip out.
            g_sModbusMaster.m_nStripLength = READ_REQUEST_LENGTH;
        }
        else
            g_sModbusMaster.m_nStripLength = 0;

        // Set the number of retries on this message
        g_sModbusMaster.m_nRetries = MODBUS_READ_RETRIES;

        // Set how long we'll wait for a response
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
         case EQUIP_SEI:
            g_sModbusMaster.m_nTimeoutPreset = MODBUS_SEI_READ_TIMEOUT;
            break;
       case EQUIP_UFP2:
            g_sModbusMaster.m_nTimeoutPreset = MODBUS_UFP2_READ_TIMEOUT;
            break;
        case EQUIP_LLS:
            g_sModbusMaster.m_nTimeoutPreset = MODBUS_LLS_READ_TIMEOUT;
            break;
        case EQUIP_OPTIMISATION:
            g_sModbusMaster.m_nTimeoutPreset = MODBUS_OPTIMISATION_READ_TIMEOUT;
            break;
        case EQUIP_LIW:
            g_sModbusMaster.m_nTimeoutPreset = MODBUS_LIW_READ_TIMEOUT;
            break;
        case EQUIP_VAC8IO:
            g_sModbusMaster.m_nTimeoutPreset = MODBUS_VAC8IO_READ_TIMEOUT;
            break;

        default:
            g_sModbusMaster.m_nTimeoutPreset = MODBUS_SEI_READ_TIMEOUT;
            break;
        }
    }
    else
    {
        // This is an unsupported message so it must be
        // removed from the queue
        RemoveFromQueue( g_sModbusMaster.m_pController );
    }

}

////////////////////////////////////////////////////////////////
// DecodeReadResponse()                     //
//
// Takes the response to a Modbus read message and  verifies
// the checksum.  The data is transferred to the read array
// if the message is valid
//
//      Entry:
//              Void.
//      Exit:
//              Returns code indicating success/failure.
//
// Copied & adapted from CP2000
// M.McKiernan                         18-01-2005
// First pass.
///////////////////////////////////////////////////////////
int DecodeReadResponse( void )
{
    int     nResult;
    unsigned int     nChecksum;
    union   CharsAndWord     nDataValue;
      long lLiwADValue;
      union WordAndDWord DValue;
   int j;
   BYTE nTickTime;
   WORD nTempWord;
   BYTE nADIndex;

    // If we are in RS485 mode then we need to strip out
    // the transmit characters
    if( g_sModbusMaster.m_nInterface == INTERFACE_RS485 )
    {
        int     i;
        for( i = 0; i < g_sModbusMaster.m_nRXIndex - g_sModbusMaster.m_nStripLength; i++ )
            g_sModbusMaster.m_cRxBuffer[i] = g_sModbusMaster.m_cRxBuffer[i + g_sModbusMaster.m_nStripLength];
        g_sModbusMaster.m_nRXIndex -= g_sModbusMaster.m_nStripLength;
    }

    // calculate the checksum for the rx message
    nChecksum = CalculateModbusCRC( g_sModbusMaster.m_cRxBuffer, g_sModbusMaster.m_nRXIndex - 2 );

    // load the rx checksum
    nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[g_sModbusMaster.m_nRXIndex - 1];
    nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[g_sModbusMaster.m_nRXIndex - 2];

    // compare the rx and calculated checksums
    if( nChecksum != nDataValue.nValue )
    {
        nResult = MODBUS_CHECKSUM_ERROR;
        g_sModbusMaster.m_lChecksumErrorCounter++;
        g_sModbusMaster.m_pController->m_lChecksumErrorCounter++;
        IncrementOngoingModulesHistoryChecksumErrorCtr();
    }
    else
    {
        int     nDataIndex;
        int     nRegister;
        int     nByteCount;
        int     i;
//     float   fTest1,fTest2,fTest3;
//     long  lTest2;

        nResult = (int)MODBUS_GOOD_RECEIVE;


       g_sModbusMaster.m_lGoodRx++;
        g_sModbusMaster.m_pController->m_lGoodRx++;
        IncrementOngoingModulesHistoryGoodRxCtr();

        // Reset the timeout coounter that's used to indicate
        // a comms alarm.
        g_sModbusMaster.m_pController->m_nCommsTimeouts = 0;

        // Get the number of bytes returned in the message
        nByteCount = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_BYTE_COUNT];

        // Retrieve the start indices
        nDataIndex = g_sModbusMaster.m_nStartIndex;
        nRegister = g_sModbusMaster.m_nStartRegister;

        // Extract the data from the Rx buffer
        switch( g_sModbusMaster.m_pController->m_nSystemType )
            {
            case EQUIP_SEI:
            for( i = 0; i < nByteCount; i += MODBUS_REGISTER_LENGTH, nRegister++ )
            {
                nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i];
                nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i + 1];
               g_sModbusMaster.m_pController->arrnReadData[nDataIndex] = nDataValue.nValue;
               nDataIndex++;


// --review-- that this is only called at the end of the prod summary read message


               g_bSEIReadDataAvailable = TRUE;  // indicate that SEI read data is available
               g_bSEIReadDataReady = TRUE;
            }
                // If this is the summary message, process the alarms & flags
            switch( g_sModbusMaster.m_nMessageType )
            {
               case COMMS_READ_SUMMARY:
                // Flag that we have new summary data
                  g_sModbusMaster.m_pController->m_bNewSummaryData = TRUE;
                 break;
               case COMMS_READ_CALIBRATION:
                // Flag that we have valid calibration data
//                  g_sModbusMaster.m_pController->m_bCalibrationValid = TRUE;
                  g_sModbusMaster.m_pController->m_bPollCalibration = FALSE;
                  break;
               case COMMS_READ_ID:
                // Flag that we have valid ID data
                  g_sModbusMaster.m_pController->m_bIDValid = TRUE;
                  break;
               default:
                  break;
            } // end of switch on message type.
                break;  // end of EQUIP_SEI case.

            case EQUIP_LLS:     // Low Level sensor card.
            for( i = 0; i < nByteCount; i += MODBUS_REGISTER_LENGTH, nRegister++ )
            {
                nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i];
                nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i + 1];
               g_sModbusMaster.m_pController->arrnReadData[nDataIndex] = nDataValue.nValue;
               nDataIndex++;
            }


                // If this is the summary message, process the alarms & flags
            switch( g_sModbusMaster.m_nMessageType )
            {
               case COMMS_READ_SUMMARY:
                // Flag that we have new summary data
                  g_sModbusMaster.m_pController->m_bNewSummaryData = TRUE;
                 break;
                    default:
                        break;
            } // end of switch on message type.
                break;  // end of EQUIP_LLS case.


            case EQUIP_OPTIMISATION:     // Optimisation card.
            for( i = 0; i < nByteCount; i += MODBUS_REGISTER_LENGTH, nRegister++ )
            {
                nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i];
                nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i + 1];
               g_sModbusMaster.m_pController->arrnReadData[nDataIndex] = nDataValue.nValue;
               nDataIndex++;
            }

                // If this is the summary message, process the alarms & flags
            switch( g_sModbusMaster.m_nMessageType )
            {
               case COMMS_READ_SUMMARY:
                // Flag that we have new summary data
                  g_sModbusMaster.m_pController->m_bNewSummaryData = TRUE;
                 break;
                    default:
                        break;
            } // end of switch on message type.
                break;  // end of EQUIP_OPTIMISATION case.

            case EQUIP_SSIF:            // added read for ssif psmith 7/5/10
                //if(fdTelnet>0)
                //{
               // 	iprintf("\n transfer ssif data %d",nByteCount);
                //}
            	for( i = 0; i < nByteCount; i += MODBUS_REGISTER_LENGTH, nRegister++ )
                {
                   nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i];
                   nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i + 1];
                   g_sModbusMaster.m_pController->arrnReadData[nDataIndex] = nDataValue.nValue;
                   nDataIndex++;
                }

            	break;

            case EQUIP_UFP2:        // Remote operator panel - reading keys etc.
                nDataIndex -= UFP2_OFFSET;          // Note: the UFP2 data (keys etc) starts at 320.
                                                            // table in blender will start at 0, hence remove offset.
            for( i = 0; i < nByteCount; i += MODBUS_REGISTER_LENGTH, nRegister++ )
            {
                nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i];
                nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i + 1];
               g_sModbusMaster.m_pController->arrnReadData[nDataIndex] = nDataValue.nValue;
               nDataIndex++;
             }
                if( g_arrnReadUFP2MBTable[ MB_UFP2_KEY_REG - UFP2_OFFSET] & 0x00FF )
                    g_nKeyCode = g_arrnReadUFP2MBTable[ MB_UFP2_KEY_REG - UFP2_OFFSET] & 0x00FF;
//                  nTemp = g_arrnReadUFP2MBTable[MB_UFP2_KEY_REG - UFP2_OFFSET];   //DEBUGGING.
                // If this is the summary message, process the alarms & flags
            switch( g_sModbusMaster.m_nMessageType )
            {
               case COMMS_READ_SUMMARY:
                // Flag that we have new summary data
                  g_sModbusMaster.m_pController->m_bNewSummaryData = TRUE;
                 break;
                    default:
                        break;
            } // end of switch on message type.
                break;  // end of EQUIP_UFP2 case.
//...start LiW case
            case EQUIP_LIW:
//           iprintf( "\n8888888888888LIW decode read %d", nDataIndex );      //testonly
            j = g_sModbusMaster.m_pController->m_nSystemAddress - FIRST_LIW_ADDRESS;

            for( i = 0; i < nByteCount; i += MODBUS_REGISTER_LENGTH, nRegister++ )
            {

                nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i];
                nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i + 1];
               g_sModbusMaster.m_pController->arrnReadData[nDataIndex] = nDataValue.nValue;
               nDataIndex++;

// --review-- that this is only called at the end of the prod summary read message
               // not there can be more than one LIW card.
//               int j = g_sModbusMaster.m_pController->m_nSystemAddress - FIRST_LIW_ADDRESS;

               g_bLIWReadDataAvailable[j] = TRUE;  // indicate that LIW read data is available
               g_bLIWReadDataReady[j] = TRUE;
            }
                // If this is the summary message, process the alarms & flags
            switch( g_sModbusMaster.m_nMessageType )
            {
               case COMMS_READ_SUMMARY:
                // Flag that we have new summary data
                  g_sModbusMaster.m_pController->m_bNewSummaryData = TRUE;
                  g_nDPTAtResponse = g_sModbusMaster.m_nDisplayPollTimer;  // diagnostic save the poll timer on read.

// start 05052009
                   DValue.nValue[0] = g_arrnReadLiWMBTable[j][MB_LIW_AD];
                   DValue.nValue[1] = g_arrnReadLiWMBTable[j][MB_LIW_AD+1];
                   lLiwADValue = DValue.lValue;

                   nTempWord = g_arrnReadLiWMBTable[j][MB_LIW_INC_DEC_REG];   //A/D index & RA0Tick in MLIW04
                   nTickTime = (BYTE)(nTempWord >>8);    // in m.s. byte
                   nADIndex = (BYTE)(nTempWord & 0x00FF);         // in l.s. byte.

                   g_sLiWCard[j].m_nADReg = nTempWord;

                  if(g_cLiWPollTime == 'A')     // polling time or phase A.
                  {
                     if(lLiwADValue == g_sLiWCard[j].m_lADCValue_B)  // check vs previous B data
                     {
                        g_sLiWCard[j].m_nAEqualsPreviousBCtr++;
                        if( lLiwADValue == g_sLiWCard[j].m_lADCValue_A )  // check vs Prev. A data
                           g_sLiWCard[j].m_nAEqualsPreviousACtr++;

                     }
                     // store data in Liw card struct location A for use
                     g_sLiWCard[j].m_nADCValueMSW_A = DValue.nValue[0];    //msb for poll A
                     g_sLiWCard[j].m_nADCValueLSW_A = DValue.nValue[1];    //lsb for poll A
                     g_sLiWCard[j].m_lADCValue_A = lLiwADValue;       // amalgamated result for poll A.
                     g_sLiWCard[j].m_nTickA = nTickTime;
                     g_sLiWCard[j].m_nADIndexA = nADIndex;
                  }
                  else if(g_cLiWPollTime == 'B')   //polling time or phase B.
                  {
                     if( lLiwADValue == g_sLiWCard[j].m_lADCValue_A )  // check vs current A data
                     {
                        g_sLiWCard[j].m_nBEqualsCurrentACtr++;
                        if( lLiwADValue == g_sLiWCard[j].m_lADCValue_B )  // check vs Prev. B data
                           g_sLiWCard[j].m_nBEqualsPreviousBCtr++;
                     }


                     // store data in Liw card struct location B for use.
                     g_sLiWCard[j].m_nADCValueMSW_B = DValue.nValue[0];     //msb for poll B
                     g_sLiWCard[j].m_nADCValueLSW_B = DValue.nValue[1];     //lsb for poll B
                     g_sLiWCard[j].m_lADCValue_B = lLiwADValue;       // amalgamated result for poll B.
                     g_sLiWCard[j].m_nTickB = nTickTime;
                     g_sLiWCard[j].m_nADIndexB = nADIndex;

                  }

// end 05052009
//                  if(fdnet>0)
//                     printf("\n\nDPTR= %d", g_nDPTAtResponse);
//                  if(fdnet>0)
//                     printf("\n\n PiTsRead= %d DPT= %d", g_nPIT_Int_CounterU, g_sModbusMaster.m_nDisplayPollTimer);


                 break;
               case COMMS_READ_CALIBRATION:
                // Flag that we have valid calibration data
//                  g_sModbusMaster.m_pController->m_bCalibrationValid = TRUE;
                  g_sModbusMaster.m_pController->m_bPollCalibration = FALSE;
//                  iprintf( "\n999999LIWcal" );      //testonly

                  break;
               case COMMS_READ_ID:
                // Flag that we have valid ID data
                  g_sModbusMaster.m_pController->m_bIDValid = TRUE;
//                  iprintf( "\n9999999LIWID" );      //testonly

                  break;
               default:
                  break;
            } // end of switch on message type.
                break;

//....end Liw case
// start of VAC8io case
            case EQUIP_VAC8IO:
             j = g_sModbusMaster.m_pController->m_nSystemAddress - FIRST_VAC8IO_ADDRESS;

            for( i = 0; i < nByteCount; i += MODBUS_REGISTER_LENGTH, nRegister++ )
            {
                nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i];
                nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[MODBUS_READ_DATA_OFFSET + i + 1];
               g_sModbusMaster.m_pController->arrnReadData[nDataIndex] = nDataValue.nValue;
               nDataIndex++;

// --review-- that this is only called at the end of the prod summary read message
               g_bVAC8IOReadDataAvailable[j] = TRUE;  // indicate that VAC8IO read data is available
               g_bVAC8IOReadDataReady[j] = TRUE;

            }
                // If this is the summary message, process the alarms & flags
            switch( g_sModbusMaster.m_nMessageType )
            {
               case COMMS_READ_SUMMARY:
                // Flag that we have new summary data
                  g_sModbusMaster.m_pController->m_bNewSummaryData = TRUE;
                 break;
               case COMMS_READ_CALIBRATION:
                // Flag that we have valid calibration data
//                  g_sModbusMaster.m_pController->m_bCalibrationValid = TRUE;
                  g_sModbusMaster.m_pController->m_bPollCalibration = FALSE;
                  break;
               case COMMS_READ_ID:
                // Flag that we have valid ID data
                  g_sModbusMaster.m_pController->m_bIDValid = TRUE;
                  break;
               default:
                  break;
            } // end of switch on message type.
            break;  // end of EQUIP_VAC8io case.


// end of VAC8io case

            default:            // i.e. undefined equip type.
                break;
            }   // end of switch on Equip types.

    }   // end of else i.e. good message.

    return nResult;
}



////////////////////////////////////////////////////////////////
// GenerateStatusReadRequest()                      //
//
// Formats up a fast status read message.
//
//      Entry:
//              LoReg           Lowest register address in the request.
//              HiReg           Highest register address in the request.
//              UnitAddress     Address of device.
//      Exit:
//              Void.
//
// Copied  from CP2000 ( no changes made as no immediate plans for use)
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
void GenerateStatusReadRequest( void )
{
    union   CharsAndWord     nDataValue;
    unsigned int     nChecksum;
    int     i;

    // Clear the rx buffer before we transmit
    for( i = 0; i < COMMS_BUFFER_SIZE; i++ )
        g_sModbusMaster.m_cRxBuffer[i] = 0;

    // Assemble the request
    g_sModbusMaster.m_cTxBuffer[0] = (char)g_sModbusMaster.m_pController->m_nSystemAddress; // Device address

    g_sModbusMaster.m_cTxBuffer[1] = FAST_STATUS_READ;    // Function code
    g_sModbusMaster.m_nTXIndex = 2;

    // calculate and store the checksum
    nChecksum = CalculateModbusCRC( g_sModbusMaster.m_cTxBuffer, g_sModbusMaster.m_nTXIndex );
    nDataValue.nValue = nChecksum;
    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
    g_sModbusMaster.m_nTXIndex++;
    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];

    // Point to the last byte in the buffer
    g_sModbusMaster.m_nTXEndIndex = g_sModbusMaster.m_nTXIndex;

    // Calculate the expected length of the response
    g_sModbusMaster.m_nRXLength = FAST_STATUS_RESPONSE_LENGTH;

    // If the protocol is configured for RS485 then the
    // transmit message will already be in the receive buffer.
    if( g_sModbusMaster.m_nInterface == INTERFACE_RS485 )
    {
        g_sModbusMaster.m_nRXLength += FAST_STATUS_REQUEST_LENGTH;

        // number of bytes to strip out.
        g_sModbusMaster.m_nStripLength = FAST_STATUS_REQUEST_LENGTH;
    }
    else
        g_sModbusMaster.m_nStripLength = 0;

    // Set the number of retries on this message
    g_sModbusMaster.m_nRetries = MODBUS_STATUS_READ_RETRIES;

    // Set how long we'll wait for a response
    g_sModbusMaster.m_nTimeoutPreset = MODBUS_STATUS_READ_TIMEOUT;
}

////////////////////////////////////////////////////////////////
// DecodeStatusReadResponse()                       //
//
// Takes the response to a Modbus Fast Status Read message
// and  verifies the checksum.  The data is transferred to
// the read array if the message is valid
//
//      Entry:
//              Void.
//      Exit:
//              Returns code indicating success/failure.
//
// Copied  from CP2000
// Commented out some stuff, see " //mmk "  no immediate plans for use)
// Would have to provide a storage area for the fast status word, cannot go into MB table proper.
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
int DecodeStatusReadResponse( void )
{
    int     nResult;
    unsigned int     nChecksum;
    union   CharsAndWord     nDataValue;

    // If we are in RS485 mode then we need to strip out
    // the transmit characters
    if( g_sModbusMaster.m_nInterface == INTERFACE_RS485 )
    {
        int     i;
        for( i = 0; i < g_sModbusMaster.m_nRXIndex - g_sModbusMaster.m_nStripLength; i++ )
            g_sModbusMaster.m_cRxBuffer[i] = g_sModbusMaster.m_cRxBuffer[i + g_sModbusMaster.m_nStripLength];
        g_sModbusMaster.m_nRXIndex -= g_sModbusMaster.m_nStripLength;
    }

    // calculate the checksum for the rx message
    nChecksum = CalculateModbusCRC( g_sModbusMaster.m_cRxBuffer, g_sModbusMaster.m_nRXIndex - 2 );

    // load the rx checksum
    nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[g_sModbusMaster.m_nRXIndex - 1];
    nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[g_sModbusMaster.m_nRXIndex - 2];

    // compare the rx and calculated checksums
    if( nChecksum != nDataValue.nValue )
    {
        nResult = MODBUS_CHECKSUM_ERROR;
        g_sModbusMaster.m_lChecksumErrorCounter++;
        g_sModbusMaster.m_pController->m_lChecksumErrorCounter++;
    }
    else
    {
//mmk       int     nState;
        unsigned char cStatus;
//mmk       float   fValue;

        nResult = MODBUS_GOOD_RECEIVE;

        // Clear the comms alarm bit
//mmk       nState = (int)g_sModbusMaster.m_pController->arrnReadData[ALARM_STATUS];
//mmk       nState &= GENERIC_COMMS_ALARM_CLEAR;
//mmk       g_sModbusMaster.m_pController->arrnReadData[ALARM_STATUS] = (float)nState;

      g_sModbusMaster.m_lGoodRx++;
        g_sModbusMaster.m_pController->m_lGoodRx++;
        IncrementOngoingModulesHistoryGoodRxCtr();

        // Reset the timeout coounter that's used to indicate
        // a comms alarm.
        g_sModbusMaster.m_pController->m_nCommsTimeouts = 0;

        // Extract the data from the Rx buffer
//mmk        fValue = (float)g_sModbusMaster.m_cRxBuffer[MODBUS_STATUS_DATA_OFFSET];
      cStatus = g_sModbusMaster.m_cRxBuffer[MODBUS_STATUS_DATA_OFFSET];

        // Store the value in the data
//mmk       g_sModbusMaster.m_pController->arrnReadData[FAST_STATUS_WORD] = fValue;
        g_sModbusMaster.m_pController->m_cFastStatusByte = cStatus;
    }

   return nResult;
}


///////////////////////////////////////////////////////////
// GenerateWriteRequest()               //
//
// Takes the start and end registers for a Modbus write
// message and formats up the message for transmission.
//
//      Entry:
//              Void.
//      Exit:
//              Void.
//
// Copied & adapted from CP2000 MBMHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
void GenerateWriteRequest( void )
{
    unsigned int     nChecksum;
    int     nLoReg = -1;
    int     nHiReg = -1;
//nbb    int     nDecimals = 0;
    int     i;

    // Clear the rx buffer before we transmit
    for( i = 0; i < COMMS_BUFFER_SIZE; i++ )
        g_sModbusMaster.m_cRxBuffer[i] = 0;

    switch( g_sModbusMaster.m_nMessageType )
    {
    case COMMS_WRITE_COMMANDS:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_SEI:
            if(g_nSEIWriteRequiredCounter == 0)
            {
                nLoReg = SEI_COMMANDS_START;
                nHiReg = SEI_SHORTENED_COMMANDS_END;
            }
            else
            {
                g_nSEIWriteRequiredCounter--;
                nLoReg = SEI_COMMANDS_START;
                nHiReg = SEI_COMMANDS_END;
                //SetupMttty();
                //iprintf("\n adding long command");
            }

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_SETPOINT_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = SEI_COMMANDS_START;

            // Load the number of decimals in case this is a single
            // register write message
//          nDecimals = arrnBatchBlenderModbusTable[g_sModbusMaster.m_nStartIndex][TABLE_DECIMAL_INDEX];

            break;
        case EQUIP_LLS:
            nLoReg = LLS_COMMANDS_START;
            nHiReg = LLS_COMMANDS_END;
                g_sModbusMaster.m_nStartIndex = LLS_COMMANDS_START;
                break;
        case EQUIP_SSIF:
            nLoReg = SSIF_COMMANDS_START;
            nHiReg = SSIF_COMMANDS_END;
            g_sModbusMaster.m_nStartIndex = SSIF_COMMANDS_START;
          // Doesn't exist for the UFP1 - i.e. display VRAM
            break;
        case EQUIP_UFP2:
            nLoReg = UFP2_COMMANDS_START;
            nHiReg = UFP2_COMMANDS_END;
                g_sModbusMaster.m_nStartIndex = UFP2_COMMANDS_START - UFP2_OFFSET;
                break;
         case EQUIP_OPTIMISATION:
            nLoReg = OPTIMISATION_COMMANDS_START;
            nHiReg = OPTIMISATION_COMMANDS_END;
                g_sModbusMaster.m_nStartIndex = OPTIMISATION_COMMANDS_START;
                break;

        case EQUIP_LIW:
                nLoReg = LIW_COMMANDS_START;
                nHiReg = LIW_COMMANDS_END;


            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_SETPOINT_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = LIW_COMMANDS_START;


            // Load the number of decimals in case this is a single
            // register write message
//          nDecimals = arrnBatchBlenderModbusTable[g_sModbusMaster.m_nStartIndex][TABLE_DECIMAL_INDEX];

            break;

        case EQUIP_VAC8IO:
                nLoReg = VAC8IO_COMMANDS_START;
                nHiReg = VAC8IO_COMMANDS_END;


            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[BATCH_SETPOINT_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
//              nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = VAC8IO_COMMANDS_START;

            break;

        default:
            break;
        }
        break;


    case COMMS_INC_DEC_WRITE_COMMAND:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_SEI:
            nLoReg = SEI_INCREASE_DECREASE_COMMANDS_START;
            nHiReg = SEI_INCREASE_DECREASE_COMMANDS_START;
            g_sModbusMaster.m_nStartIndex = SEI_INCREASE_DECREASE_COMMANDS_START;
        // Doesn't exist for the other peripherals
        default:
            break;

        }
        break;



    case COMMS_WRITE_CALIBRATION:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_SEI:
            nLoReg = SEI_CALIBRATION_START;
            nHiReg = SEI_CALIBRATION_END;
                // Load the index to the data store
                g_sModbusMaster.m_nStartIndex = SEI_CALIBRATION_START;
            break;
        case EQUIP_LLS:
        case EQUIP_SSIF:
            nLoReg = SSIF_CALIBRATION_START;
            nHiReg = SSIF_CALIBRATION_END;
            g_sModbusMaster.m_nStartIndex = SSIF_CALIBRATION_START;
            break;

        case EQUIP_UFP2:
        case EQUIP_VAC8IO:
          // Doesn't exist for the other peripherals
                break;
        case EQUIP_LIW:
            nLoReg = LIW_CALIBRATION_START;
            nHiReg = LIW_CALIBRATION_END;
                // Load the index to the data store
                g_sModbusMaster.m_nStartIndex = LIW_CALIBRATION_START;
            break;

        default:
                break;
        }
        break;
    case COMMS_WRITE_DISPLAY:       // only used for writing to display, i.e. UFP1.
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_SSIF:
                if(g_nVramEndDisplayIndex >= MAX_DISPLAY_INDEX)
                {
                    g_nVramEndDisplayIndex = MAX_DISPLAY_INDEX - 1;
                }
            nLoReg = g_nVramStartDisplayIndex / 2;      // up to display update program to set up start and end indexes.
            nHiReg = g_nVramEndDisplayIndex / 2;

                if(nLoReg == nHiReg)    //always write more than 1 word.
                {
                    if( nLoReg )
                        nLoReg -= 1;
                    else
                        nHiReg += 1;
                 }

                g_sModbusMaster.m_nStartIndex = nLoReg;
            break;
        case EQUIP_SEI:
        case EQUIP_LLS:
        case EQUIP_UFP2:
          // Doesn't exist for the other peripherals
                break;
        default:
                break;
        }
        break;

/*
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {

        case EQUIP_BATCH_BLENDER:
            nLoReg = arrnBatchBlenderModbusTable[BATCH_CALIBRATION_START][TABLE_REGISTER_INDEX];
            nHiReg = arrnBatchBlenderModbusTable[BATCH_CALIBRATION_END][TABLE_REGISTER_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
            if( arrnBatchBlenderModbusTable[BATCH_CALIBRATION_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
                nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = BATCH_CALIBRATION_START;

            // Load the number of decimals in case this is a single
            // register write message
            nDecimals = arrnBatchBlenderModbusTable[g_sModbusMaster.m_nStartIndex][TABLE_DECIMAL_INDEX];
            break;
        case EQUIP_LOW_BLENDER:
            break;
        case EQUIP_WIDTH_CONTROL:
            break;
        case EQUIP_PROFILER:
            break;
        case EQUIP_LOADER:
        case EQUIP_PUMP:
            nLoReg = arrnLoaderModbusTable[LOADER_CALIBRATION_START][TABLE_REGISTER_INDEX];
            nHiReg = arrnLoaderModbusTable[LOADER_CALIBRATION_END][TABLE_REGISTER_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
            if( arrnLoaderModbusTable[LOADER_CALIBRATION_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
                nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex =LOADER_CALIBRATION_START;

            // Load the number of decimals in case this is a single
            // register write message
            nDecimals = arrnLoaderModbusTable[g_sModbusMaster.m_nStartIndex][TABLE_DECIMAL_INDEX];
            break;
        default:
            break;

        }
        break;
*/
/* - no set time message used.
    case COMMS_SET_TIME:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_BATCH_BLENDER:
            nLoReg = arrnBatchBlenderModbusTable[BATCH_TIME_START][TABLE_REGISTER_INDEX];
            nHiReg = arrnBatchBlenderModbusTable[BATCH_TIME_END][TABLE_REGISTER_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
            if( arrnBatchBlenderModbusTable[BATCH_TIME_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
                nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = BATCH_TIME_START;

            // Load the number of decimals in case this is a single
            // register write message
            nDecimals = arrnBatchBlenderModbusTable[g_sModbusMaster.m_nStartIndex][TABLE_DECIMAL_INDEX];
            break;
        case EQUIP_LOW_BLENDER:
            break;
        case EQUIP_WIDTH_CONTROL:
            break;
        case EQUIP_PROFILER:
            break;
        case EQUIP_LOADER:
        case EQUIP_PUMP:
            // Doesn't exist for the loader or pump.
            break;
        default:
            break;
        }
        break;
*/
/* no write comp. config used.
    case COMMS_WRITE_COMP_CONFIG:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_BATCH_BLENDER:
            nLoReg = arrnBatchBlenderModbusTable[BATCH_CALIBRATION_COMP1_CONFIG][TABLE_REGISTER_INDEX];
            nHiReg = arrnBatchBlenderModbusTable[BATCH_CALIBRATION_COMP8_CONFIG][TABLE_REGISTER_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
            if( arrnBatchBlenderModbusTable[BATCH_CALIBRATION_COMP8_CONFIG][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
                nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = BATCH_CALIBRATION_COMP1_CONFIG;

            // Load the number of decimals in case this is a single
            // register write message
            nDecimals = arrnBatchBlenderModbusTable[g_sModbusMaster.m_nStartIndex][TABLE_DECIMAL_INDEX];
            break;
        case EQUIP_LOW_BLENDER:
            break;
        case EQUIP_WIDTH_CONTROL:
            break;
        case EQUIP_PROFILER:
            break;
        case EQUIP_LOADER:
        case EQUIP_PUMP:
            break;
        default:
            break;
        }
        break;
*/

/* no write options message used.

    case COMMS_WRITE_OPTIONS:
        switch( g_sModbusMaster.m_pController->m_nSystemType )
        {
        case EQUIP_BATCH_BLENDER:
            nLoReg = arrnBatchBlenderModbusTable[BATCH_OPTIONS_START][TABLE_REGISTER_INDEX];
            nHiReg = arrnBatchBlenderModbusTable[BATCH_OPTIONS_END][TABLE_REGISTER_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
            if( arrnBatchBlenderModbusTable[BATCH_OPTIONS_END][TABLE_SIZE_INDEX] == TABLE_REGISTER_SIZE_LONG )
                nHiReg++;

            // Load the index to the data store
            g_sModbusMaster.m_nStartIndex = BATCH_OPTIONS_START;

            // Load the number of decimals in case this is a single
            // register write message
            nDecimals = arrnBatchBlenderModbusTable[g_sModbusMaster.m_nStartIndex][TABLE_DECIMAL_INDEX];

            break;
        case EQUIP_LOW_BLENDER:
            break;
        case EQUIP_WIDTH_CONTROL:
            break;
        case EQUIP_PROFILER:
            break;
        case EQUIP_LOADER:
        case EQUIP_PUMP:
            // Doesn't exist for the loader or pump.
            break;
        default:
            break;
        }
        break;
*/
    default:
        break;
    }

    if( nLoReg != -1 )
    {
        union   CharsAndWord     nDataValue;

        // Assemble the request
        g_sModbusMaster.m_cTxBuffer[0] = (char)g_sModbusMaster.m_pController->m_nSystemAddress; // Device address

        // Check if there is more than one register involved
        // and use the appropriate command
        if( nLoReg == nHiReg )  // one register, use the single word write command.
        {
//          float   fTemp;

            g_sModbusMaster.m_cTxBuffer[1] = WRITE_A_WORD;          // Function code
            g_sModbusMaster.m_nTXIndex = 2;                         // Next free buffer location

            // Store the register address
            nDataValue.nValue = nLoReg;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;

            // Retrieve the data value from the write array
            nDataValue.nValue = g_sModbusMaster.m_pController->arrnWriteData[g_sModbusMaster.m_nStartIndex];
/*
            fTemp = g_sModbusMaster.m_pController->arrnWriteData[g_sModbusMaster.m_nStartIndex];

            // Keep the required precision
            for( i = 0; i < nDecimals; i++ )
                fTemp *= 10;

            nDataValue.nValue = (int)fTemp;
*/

            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;
        }
        else    // > 1 word, use the multiple word write command.
        {
            int     nDataIndex;
            int     nRegister;
            int     nTemp;

            g_sModbusMaster.m_cTxBuffer[1] = WRITE_N_WORDS;         // Function code
            g_sModbusMaster.m_nTXIndex = 2;                         // Next free buffer location

            // Store the address of the first int
            nDataValue.nValue = nLoReg;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;

// *****
            // SHOULD ADD A CHECK HERE FOR THE MAXIMUM NUMBER OF REGISTERS
            // AND BREAK THE MESSAGE UP IF NECESSARY.

            // Store the number of registers to write
            nTemp = (nHiReg - nLoReg + 1);
            nDataValue.nValue = nTemp;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;

            // Store the number of bytes to write
            nTemp *= 2;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = (char)nTemp;
            g_sModbusMaster.m_nTXIndex++;

            // Transfer the data into the comms buffer
            // This is tricky because we must fill the unused registers
            // with zero as well as catering for longs and the number of
            // decimal places in each register value.

            // Load the start index for the data
            nDataIndex = g_sModbusMaster.m_nStartIndex;

            switch( g_sModbusMaster.m_pController->m_nSystemType )
            {
            case EQUIP_SEI:
            case EQUIP_LLS:
            case EQUIP_SSIF:
            case EQUIP_UFP2:
            case EQUIP_LIW:
            case EQUIP_VAC8IO:

            for( nRegister = nLoReg; nRegister <= nHiReg; nRegister++ )
            {
                  // Load the data value
                  nDataValue.nValue = g_sModbusMaster.m_pController->arrnWriteData[nDataIndex]; //[nRegister];

                  g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
                  g_sModbusMaster.m_nTXIndex++;
                  g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
                  g_sModbusMaster.m_nTXIndex++;
                        nDataIndex++;
            }
            break;
/*
            case EQUIP_BATCH_BLENDER:
                {
                    // move through the range of registers, checking each time
                    // that the register is in the controllers map and if so,
                    // whether it's a long and if there are decimal places
                    for( nRegister = nLoReg; nRegister <= nHiReg; nRegister++ )
                    {
                        // Is this a register in the modbus table ?
                        if( nRegister == arrnBatchBlenderModbusTable[nDataIndex][TABLE_REGISTER_INDEX] )
                        {
                            // Load the data value
                            float   fValue = g_sModbusMaster.m_pController->arrnWriteData[nDataIndex];

                            // Maintain the resolution by shifting over by the
                            // required number of decimal places
                            for( i = 0; i < arrnBatchBlenderModbusTable[nDataIndex][TABLE_DECIMAL_INDEX]; i++ )
                                fValue *= 10;

                            // Move an int or long as required
                            switch( arrnBatchBlenderModbusTable[nDataIndex][TABLE_SIZE_INDEX] )
                            {
                            case TABLE_REGISTER_SIZE_INT:
                                {
                                    nDataValue.nValue = (int)fValue;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
                                    g_sModbusMaster.m_nTXIndex++;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
                                    g_sModbusMaster.m_nTXIndex++;
                                }
                                break;
                            case TABLE_REGISTER_SIZE_LONG:
                                {
                                    union   IntsAndLong     lDataValue;

                                    lDataValue.lValue = (long)fValue;

                                    nDataValue.nValue = lDataValue.nValue[0];
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
                                    g_sModbusMaster.m_nTXIndex++;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
                                    g_sModbusMaster.m_nTXIndex++;

                                    nDataValue.nValue = lDataValue.nValue[1];
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
                                    g_sModbusMaster.m_nTXIndex++;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
                                    g_sModbusMaster.m_nTXIndex++;

                                    // Skip over the next register since we've
                                    // already dealt with it here.
                                    nRegister++;
                                }
                                break;
                            default:
                                break;
                            }

                            // Move on to the next entry in the data array
                            nDataIndex++;
                        }
                        else
                        {
                            // Unused registers are set to zero
                            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = 0x00;
                            g_sModbusMaster.m_nTXIndex++;
                            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = 0x00;
                            g_sModbusMaster.m_nTXIndex++;
                        }
                    }
                }
                break;
            case EQUIP_LOW_BLENDER:
                break;
            case EQUIP_WIDTH_CONTROL:
                break;
            case EQUIP_PROFILER:
                break;
            case EQUIP_LOADER:
            case EQUIP_PUMP:
                {
                    // move through the range of registers, checking each time
                    // that the register is in the controllers map and if so,
                    // whether it's a long and if there are decimal places
                    for( nRegister = nLoReg; nRegister <= nHiReg; nRegister++ )
                    {
                        // Is this a register in the modbus table ?
                        if( nRegister == arrnLoaderModbusTable[nDataIndex][TABLE_REGISTER_INDEX] )
                        {
                            // Load the data value
                            float   fValue = g_sModbusMaster.m_pController->arrnWriteData[nDataIndex];

                            // Maintain the resolution by shifting over by the
                            // required number of decimal places
                            for( i = 0; i < arrnLoaderModbusTable[nDataIndex][TABLE_DECIMAL_INDEX]; i++ )
                                fValue *= 10;

                            // Move an int or long as required
                            switch( arrnLoaderModbusTable[nDataIndex][TABLE_SIZE_INDEX] )
                            {
                            case TABLE_REGISTER_SIZE_INT:
                                {
                                    nDataValue.nValue = (int)fValue;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
                                    g_sModbusMaster.m_nTXIndex++;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
                                    g_sModbusMaster.m_nTXIndex++;
                                }
                                break;
                            case TABLE_REGISTER_SIZE_LONG:
                                {
                                    union   IntsAndLong     lDataValue;

                                    lDataValue.lValue = (long)fValue;

                                    nDataValue.nValue = lDataValue.nValue[0];
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
                                    g_sModbusMaster.m_nTXIndex++;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
                                    g_sModbusMaster.m_nTXIndex++;

                                    nDataValue.nValue = lDataValue.nValue[1];
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
                                    g_sModbusMaster.m_nTXIndex++;
                                    g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
                                    g_sModbusMaster.m_nTXIndex++;

                                    // Skip over the next register since we've
                                    // already dealt with it here.
                                    nRegister++;
                                }
                                break;
                            default:
                                break;
                            }

                            // Move on to the next entry in the data array
                            nDataIndex++;
                        }
                        else
                        {
                            // Unused registers are set to zero
                            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = 0x00;
                            g_sModbusMaster.m_nTXIndex++;
                            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = 0x00;
                            g_sModbusMaster.m_nTXIndex++;
                        }
                    }
                }
                break;
*/
            default:
                break;
            }
        }   // end of else for write N words

        // calculate and store the checksum
        nChecksum = CalculateModbusCRC( g_sModbusMaster.m_cTxBuffer, g_sModbusMaster.m_nTXIndex );
        nDataValue.nValue = nChecksum;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
        g_sModbusMaster.m_nTXIndex++;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];

        // Point to the last byte in the buffer
        g_sModbusMaster.m_nTXEndIndex = g_sModbusMaster.m_nTXIndex;

        // Calculate the expected length of the response
        g_sModbusMaster.m_nRXLength = WRITE_RESPONSE_LENGTH;

        // If the interface is RS485 then the tx message will come
        // straight back in in the receive buffer.   We need to
        // calculate the expected response length on this basis.
        if( g_sModbusMaster.m_nInterface == INTERFACE_RS485 )
        {
            if( nLoReg == nHiReg )
                g_sModbusMaster.m_nStripLength = WRITE_A_WORD_REQUEST_LENGTH;
            else
                g_sModbusMaster.m_nStripLength = (WRITE_N_WORDS_REQUEST_LENGTH + ((nHiReg - nLoReg + 1) * MODBUS_REGISTER_LENGTH));

            g_sModbusMaster.m_nRXLength += g_sModbusMaster.m_nStripLength;
        }
        else
            g_sModbusMaster.m_nStripLength = 0;

        // Set the number of retries on this message
        g_sModbusMaster.m_nRetries = MODBUS_WRITE_RETRIES;

        // Set how long we'll wait for a response
        g_sModbusMaster.m_nTimeoutPreset = MODBUS_WRITE_TIMEOUT;
    }
    else
    {
        // This is an unsupported message so it must be
        // removed from the queue
        RemoveFromQueue( g_sModbusMaster.m_pController );
    }
}


///////////////////////////////////////////////////////////
// GenerateSingleWriteRequest()                 //
//
// Takes the start and end registers for a Modbus write
// message and formats up the message for transmission.
//
//      Entry:
//              Void.
//      Exit:
//              Void.
// Copied & adapted from CP2000 MBMHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
void GenerateSingleWriteRequest( int nIndex )
{
    unsigned int     nChecksum;
    int     nLoReg = -1;
    int     nHiReg = -1;
//  int     nDecimals;
    int     i;

    // Clear the rx buffer before we transmit
    for( i = 0; i < COMMS_BUFFER_SIZE; i++ )
        g_sModbusMaster.m_cRxBuffer[i] = 0;

    // Index to the data in the write array
    g_sModbusMaster.m_nStartIndex = nIndex;

    // Using the structure pointed to and the message type
    // determine the start and end registers for this message
    switch( g_sModbusMaster.m_pController->m_nSystemType )
    {
    case EQUIP_SEI:
        // Ensure that the index is in the write section of the
        // Modbus table.
        if( nIndex >= SEI_WRITE_START && nIndex <= SEI_WRITE_END )
        {
            nLoReg = nIndex;
            nHiReg = nIndex;

//          nDecimals = arrnBatchBlenderModbusTable[nIndex][TABLE_DECIMAL_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
//          if( arrnBatchBlenderModbusTable[nIndex][TABLE_SIZE_INDEX] == 1 )
//              nHiReg++;
        }
        break;
    case EQUIP_LLS:
        // Ensure that the index is in the write section of the
        // Modbus table.
        if( nIndex >= LLS_WRITE_START && nIndex <= LLS_WRITE_END )
        {
            nLoReg = nIndex;
            nHiReg = nIndex;
        }
        break;
    case EQUIP_SSIF:
        // Ensure that the index is in the write section of the
        // Modbus table.
        if( nIndex >= UFP1_WRITE_START && nIndex <= UFP1_WRITE_END )
        {
            nLoReg = nIndex;
            nHiReg = nIndex;
        }
        break;
    case EQUIP_UFP2:
        // Ensure that the index is in the write section of the
        // Modbus table.
        if( nIndex >= UFP2_WRITE_START && nIndex <= UFP2_WRITE_END )
        {
            nLoReg = nIndex;
            nHiReg = nIndex;

            nIndex -= UFP2_OFFSET;      // Data is offset in blender (UFP2 modbus values start at 320 in UFP)
        }
        break;
    case EQUIP_OPTIMISATION:
        // Ensure that the index is in the write section of the
        // Modbus table.
        if( nIndex >= OPTIMISATION_WRITE_START && nIndex <= OPTIMISATION_WRITE_END )
        {
            nLoReg = nIndex;
            nHiReg = nIndex;
        }
        break;

/*
    case EQUIP_LOADER:
    case EQUIP_PUMP:
        // Ensure that the index is in the write section of the
        // Modbus table.
        if( nIndex >= LOADER_CONTROL_OUTPUTS_ON && nIndex <= LOADER_CALIBRATION_END )
        {
            nLoReg = arrnLoaderModbusTable[nIndex][TABLE_REGISTER_INDEX];
            nHiReg = arrnLoaderModbusTable[nIndex][TABLE_REGISTER_INDEX];

            nDecimals = arrnLoaderModbusTable[nIndex][TABLE_DECIMAL_INDEX];

            // If the last register is a long, we need to read in the second half by
            // reading one more register more than specified.
            if( arrnLoaderModbusTable[nIndex][TABLE_SIZE_INDEX] == 1 )
                nHiReg++;
        }
        break;
*/
    default:
        break;
    }

    if( nLoReg != -1 )
    {
        union   CharsAndWord     nDataValue;
//      float   fValue;

        // Assemble the request

        // Device address
        g_sModbusMaster.m_cTxBuffer[0] = (char)g_sModbusMaster.m_pController->m_nSystemAddress;

        // Check if there is more than one register involved
        // and use the appropriate command
        if( nLoReg == nHiReg )
        {
            g_sModbusMaster.m_cTxBuffer[1] = WRITE_A_WORD;          // Function code
            g_sModbusMaster.m_nTXIndex = 2;                         // Next free buffer location

            // Store the address of the int
            nDataValue.nValue = nLoReg;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;

            // Load the value and retain the decimal places
            nDataValue.nValue = g_sModbusMaster.m_pController->arrnWriteData[nIndex];
/*
            fValue = g_sModbusMaster.m_pController->arrnWriteData[nIndex];
            for( i = 0; i < nDecimals; i++ )
                fValue *= 10;

            nDataValue.nValue = (int)fValue;
*/
            // transfer the data into the comms buffer
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;
        }
/*
        else        // MORE than one word to be written.  - not catered for
        {
            int     nTemp;
            union   IntsAndLong     lDataValue;

            g_sModbusMaster.m_cTxBuffer[1] = WRITE_N_WORDS;         // Function code
            g_sModbusMaster.m_nTXIndex = 2;                         // Next free buffer location

            // Store the address of the first int
            nDataValue.nValue = nLoReg;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;

            // Store the number of registers to write
            nTemp = (nHiReg - nLoReg + 1);
            nDataValue.nValue = nTemp;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;

            // Store the number of bytes to write
            nTemp *= 2;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = (char)nTemp;
            g_sModbusMaster.m_nTXIndex++;

            // Load the value and retain the decimal places
            fValue = g_sModbusMaster.m_pController->arrnWriteData[nIndex];
            for( i = 0; i < nDecimals; i++ )
                fValue *= 10;

            lDataValue.lValue = (long)fValue;

            // transfer the data into the comms buffer

            // Load up the ms reg
            nDataValue.nValue = lDataValue.nValue[0];

            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;

            // Load up the ls reg
            nDataValue.nValue = lDataValue.nValue[1];

            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];
            g_sModbusMaster.m_nTXIndex++;
            g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
            g_sModbusMaster.m_nTXIndex++;
        }
*/
        // calculate and store the checksum
        nChecksum = CalculateModbusCRC( g_sModbusMaster.m_cTxBuffer, g_sModbusMaster.m_nTXIndex );
        nDataValue.nValue = nChecksum;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[1];
        g_sModbusMaster.m_nTXIndex++;
        g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex] = nDataValue.cValue[0];

        // Point to the last byte in the buffer
        g_sModbusMaster.m_nTXEndIndex = g_sModbusMaster.m_nTXIndex;

        // Calculate the expected length of the response
        g_sModbusMaster.m_nRXLength = WRITE_RESPONSE_LENGTH;

        // If the interface is RS485 then the tx message will come
        // straight back in in the receive buffer.   We need to
        // calculate the expected response length on this basis.
        if( g_sModbusMaster.m_nInterface == INTERFACE_RS485 )
        {
            if( nLoReg == nHiReg )
                g_sModbusMaster.m_nStripLength = WRITE_A_WORD_REQUEST_LENGTH;
            else
                g_sModbusMaster.m_nStripLength = (WRITE_N_WORDS_REQUEST_LENGTH + ((nHiReg - nLoReg + 1) * MODBUS_REGISTER_LENGTH));

            g_sModbusMaster.m_nRXLength += g_sModbusMaster.m_nStripLength;
        }
        else
            g_sModbusMaster.m_nStripLength = 0;

        // Set the number of retries on this message
        g_sModbusMaster.m_nRetries = MODBUS_WRITE_RETRIES;

        // Set how long we'll wait for a response
        g_sModbusMaster.m_nTimeoutPreset = MODBUS_BYTE_WRITE_TIMEOUT;
    }
    else
    {
        // This is an unsupported message so it must be
        // removed from the queue
        RemoveFromQueue( g_sModbusMaster.m_pController );
    }
}




///////////////////////////////////////////////////////////
// DecodeWriteResponse()            //
//
// Takes the expected response length and parses the
// response, taking care of any error conditions that
// may occur.
//
//      Entry:
//              Void.
//      Exit:
//              Returns code indicating success/failure.
// Copied & adapted from CP2000 MBMHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
int DecodeWriteResponse( void )
{
    int     nResult;
    unsigned int     nChecksum;
    union   CharsAndWord     nDataValue;

    // If we are in RS485 mode then we need to strip out
    // the transmit characters
    if( g_sModbusMaster.m_nInterface == INTERFACE_RS485 )
    {
        int     i;
        for( i = 0; i < g_sModbusMaster.m_nRXIndex - g_sModbusMaster.m_nStripLength; i++ )
            g_sModbusMaster.m_cRxBuffer[i] = g_sModbusMaster.m_cRxBuffer[i + g_sModbusMaster.m_nStripLength];
        g_sModbusMaster.m_nRXIndex -= g_sModbusMaster.m_nStripLength;
    }

    // calculate the checksum for the rx message
    nChecksum = CalculateModbusCRC( g_sModbusMaster.m_cRxBuffer, g_sModbusMaster.m_nRXIndex - 2 );

    // load the rx checksum
    nDataValue.cValue[0] = g_sModbusMaster.m_cRxBuffer[g_sModbusMaster.m_nRXIndex - 1];
    nDataValue.cValue[1] = g_sModbusMaster.m_cRxBuffer[g_sModbusMaster.m_nRXIndex - 2];

    // compare the rx and calculated checksums

    if( nChecksum != nDataValue.nValue )
    {
        nResult = MODBUS_CHECKSUM_ERROR;

        g_sModbusMaster.m_lChecksumErrorCounter++;
        g_sModbusMaster.m_pController->m_lChecksumErrorCounter++;
    }
    else  // i.e. good checksum.
    {
//      int nState;

        // Clear the appropriate retry counters
        switch( g_sModbusMaster.m_nMessageType )
        {
//mmk       case COMMS_WRITE_SETPOINT:
//mmk           g_sModbusMaster.m_pController->m_nSetpointRetries = 0;
//mmk           break;
        case COMMS_WRITE_CALIBRATION:
            g_sModbusMaster.m_pController->m_nCalibrationRetries = 0;

            // Invalidate the calibration data held in the read array
//          g_sModbusMaster.m_pController->m_bCalibrationValid = FALSE;
            g_sModbusMaster.m_pController->m_bPollCalibration = TRUE;   // cause a read back of calibration data.
            // clear flag which is trigger write of calibration data to device.
            g_sModbusMaster.m_pController->m_bWriteCalibration = FALSE;

            break;
        case COMMS_WRITE_COMMANDS:
            g_sModbusMaster.m_pController->m_nCommandWriteRetries = 0;
            break;

        case COMMS_INC_DEC_WRITE_COMMAND:
            g_sModbusMaster.m_pController->m_nIncDecSetpointWriteRetries = 0;
            break;


        case COMMS_WRITE_DISPLAY:
            g_sModbusMaster.m_pController->m_nDisplayWriteRetries = 0;

            break;

//mmk       case COMMS_SET_TIME:
//mmk           break;
//mmk       case COMMS_WRITE_COMP_CONFIG:
//mmk           break;
        case COMMS_WRITE_SINGLE_WORD:
            g_sModbusMaster.m_pController->m_nSingleWriteRetries = 0;
            break;
//mmk       case COMMS_WRITE_OPTIONS:
//mmk           g_sModbusMaster.m_pController->m_nOptionWriteRetries = 0;

//mmk           // Invalidate the calibration data held in the read array
//mmk           g_sModbusMaster.m_pController->m_bCalibrationValid = FALSE;

//mmk           break;
        default:
            break;
        }

        nResult = MODBUS_GOOD_RECEIVE;
/*
        // Clear the comms alarm bit
        nState = (int)g_sModbusMaster.m_pController->arrnReadData[ALARM_STATUS];
        nState &= GENERIC_COMMS_ALARM_CLEAR;
        g_sModbusMaster.m_pController->arrnReadData[ALARM_STATUS] = (float)nState;
*/

        g_sModbusMaster.m_lGoodRx++;
            g_sModbusMaster.m_pController->m_lGoodRx++;
            IncrementOngoingModulesHistoryGoodRxCtr();

            // Reset the timeout coounter that's used to indicate
            // a comms alarm.
            g_sModbusMaster.m_pController->m_nCommsTimeouts = 0;
   }

    return nResult;
}




////////////////////////////////////////////////////////////
//  AddToQueue()                        //
//
// Adds the specified message to the message
// queue for the specified controller.
//
// Entry:   nMessage - message type
//          nValue - Reg index for 'write single
//          word' message.
//          pController - pointer to the controller
//          data structure.
//
// Exit:    TRUE if added successfully
//          FALSE if failed to add to queue
// New version - M. Parks 29-05-2001
// Copied directly from CP2000 MBMHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
#define MESSAGE_INSTANCE_LIMIT  2

BOOL AddToQueue( int nMessage, int nValue, structControllerData *pController )
{
    BOOL    bResult = FALSE;

    // Try to lock the queue
    if( !pController->m_bLock )
    {
        pController->m_bLock = TRUE;

        // Is there space in the queue ?
        if( !pController->m_bFull )
        {
            BOOL bFound = FALSE;

            // If the queue isn't empty check if this
            // message is already in the queue.
            if( !pController->m_bEmpty )
            {
                int     i;
                int     nInstance = 0;

                if( pController->m_nQueueTail > pController->m_nQueueHead )
                {
                    for( i = pController->m_nQueueHead; i < pController->m_nQueueTail && i < MAX_QUEUE_SIZE && !bFound; i++ )
                    {
                        if( pController->m_arrMessageQueue[i].m_nMessageType == nMessage )
                        {
                            // Increment the count of messages found
                            nInstance++;

                            // Check the limit for this message type
                            switch( nMessage )
                            {
                            case COMMS_WRITE_SINGLE_WORD:
                                if( nInstance >= MESSAGE_INSTANCE_LIMIT )
                                {
                                    // If there already enough messages in the queue
                                    // then we block any further additions.
                                    bFound = TRUE;
                                }
                                break;
                            default:
                                // The default is one instance only.
                                bFound = TRUE;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    for( i = pController->m_nQueueHead; i < MAX_QUEUE_SIZE && !bFound; i++ )
                    {
                        if( pController->m_arrMessageQueue[i].m_nMessageType == nMessage )
                        {
                            // Increment the count of messages found
                            nInstance++;

                            // Check the limit for this message type
                            switch( nMessage )
                            {
                            case COMMS_WRITE_SINGLE_WORD:
                                if( nInstance >= MESSAGE_INSTANCE_LIMIT )
                                {
                                    // If there already enough messages in the queue
                                    // then we block any further additions.
                                    bFound = TRUE;
                                }
                                break;
                            default:
                                // The default is one instance only.
                                bFound = TRUE;
                                break;
                            }
                        }
                    }

                    // If the tail is before the head we need to check from the start down
                    if( pController->m_nQueueTail < pController->m_nQueueHead )
                    {
                        for( i = 0; i < pController->m_nQueueTail && i < MAX_QUEUE_SIZE && !bFound; i++ )
                        {
                            if( pController->m_arrMessageQueue[i].m_nMessageType == nMessage )
                            {
                                // Increment the count of messages found
                                nInstance++;

                                // Check the limit for this message type
                                switch( nMessage )
                                {
                                case COMMS_WRITE_SINGLE_WORD:
                                    if( nInstance >= MESSAGE_INSTANCE_LIMIT )
                                    {
                                        // If there already enough messages in the queue
                                        // then we block any further additions.
                                        bFound = TRUE;
                                    }
                                    break;
                                default:
                                    // The default is one instance only.
                                    bFound = TRUE;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // If the message isn't already in the queue, we can add it.
            if( !bFound )
            {
                // Add the message
                pController->m_arrMessageQueue[pController->m_nQueueTail].m_nMessageType = nMessage;

                // Add the value
                pController->m_arrMessageQueue[pController->m_nQueueTail].m_nValue = nValue;

                // Point to the next free space
                pController->m_nQueueTail++;

                // If we've reached the end of the queue, wrap around
                // to the start again.
                if( pController->m_nQueueTail >= MAX_QUEUE_SIZE )
                    pController->m_nQueueTail = 0;

                // Indicate that the queue is no longer empty
                pController->m_bEmpty = FALSE;

                // If the tail has wrapped around to the head, then we are full
                if( pController->m_nQueueHead == pController->m_nQueueTail )
                    pController->m_bFull = TRUE;

                // Indicate that we were successful
                bResult =TRUE;
            }
        }

        // Unlock the queue
        pController->m_bLock = FALSE;
    }
    return bResult;
}

////////////////////////////////////////////////////////////
// GetTopOfQueue()              //
//
// Gets the top message from the message
// queue for the specified controller.
//
// Entry:   pController points to the controller
//          data structure.
//
// Exit:    index into queue if message available
//          INVALID_MESSAGE (-1) if queue empty.
//
// Copied directly from CP2000 MBMHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
int GetTopOfQueue( structControllerData *pController )
{
    int     nMessage = COMMS_INVALID_MESSAGE;

    // Try to lock the queue
    if( !pController->m_bLock )
    {
        pController->m_bLock = TRUE;

        // Is there a message in the queue ?
        if( !pController->m_bEmpty )
            nMessage = pController->m_nQueueHead;

        // Unlock the queue
        pController->m_bLock = FALSE;
    }
    return nMessage;
}


////////////////////////////////////////////////////////////
// RemoveFromQueue()                    //
//
// Removes the top message from the message
// queue for the specified controller.
//
// Entry:   pController - pointer to the controller data
//          structure.
//
// Exit:    Void
//
//
// Copied directly from CP2000 MBMHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
void RemoveFromQueue( structControllerData *pController )
{
    // Try to lock the queue
    if( !pController->m_bLock )
    {
        pController->m_bLock = TRUE;

        if( !pController->m_bEmpty )
        {
            // Move the head forward
            pController->m_nQueueHead++;
            if( pController->m_nQueueHead >= MAX_QUEUE_SIZE )
                pController->m_nQueueHead = 0;

            // If we've deleted something we can't be full
            pController->m_bFull = FALSE;

            // If the head has reached the tail then we
            // must be empty.
            if( pController->m_nQueueHead == pController->m_nQueueTail )
                pController->m_bEmpty = TRUE;
        }
        // Unlock the queue
        pController->m_bLock = FALSE;
    }
}

////////////////////////////////////////////////////////////
// EmptyQueue()                         //
//
// Resets all of the queue flags to empty it.
//
// Entry:   pController - pointer to the controller data
//          structure.
//
// Exit:    Void
//
//
// Copied directly from CP2000 MBMHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
void EmptyQueue( structControllerData *pController )
{
    // Try to lock the queue
    if( !pController->m_bLock )
    {
        pController->m_bLock = TRUE;

        if( !pController->m_bEmpty )
        {
            pController->m_nQueueHead = 0;

            pController->m_bFull = FALSE;

            pController->m_nQueueTail = 0;

            pController->m_bEmpty = TRUE;
        }
        // Unlock the queue
        pController->m_bLock = FALSE;
    }
}


///////////////////////////////////////////////////////////
// PITMBMHandler()          //
//
// Runs on PIT at full speed. (1170Hz)  -- INTERRUPT PROGRAM ---
// Schedules Modbus master serial comms stuff
//
//      Entry:
//              Void.
//      Exit:
//              Returns code indicating success/failure.
// Copied & adapted from CP2000 PITHand.c
//
// M.McKiernan                         19-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
void PITMBMHandler( void )
{
int j1;
int nPollTimeA;
int nPollTimeB;

    if( g_sModbusMaster.m_bCommsEnable && !g_bCheckingComms )
    {
        int i, ni;
         int nIndex;

        // Indicate that we are checking the comms in case the
        // PIT interrupt comes back in on top of us.
        g_bCheckingComms = TRUE;

        // Run a timer loop to enable us to add to the queues
        // once a second
        if( g_sModbusMaster.m_nPollTimer > 0 )
            g_sModbusMaster.m_nPollTimer--;
        else // i.e. polltimer = 0, 1 second expired.
        {

         if( 0 )
         {
//            g_sModbusMaster.m_nPollTimer = POLL_TIMER;
            iprintf("\nPIT_MB-DAC, %d %d %d", g_arrnWriteLiWMBTable[0][MB_LIW_DA], g_arrnWriteLiWMBTable[1][MB_LIW_DA], g_arrnWriteLiWMBTable[2][MB_LIW_DA]);
            iprintf("\n");

            for(j1=0; j1 < LIW_TABLE_WRITE_SIZE ; j1++)
            {
               iprintf("%x. ", g_arrnWriteLiWMBTable[0][j1] );
            }
            iprintf("\n");
            for(j1=0; j1 < LIW_TABLE_WRITE_SIZE ; j1++)
            {
               iprintf("%x, ", g_arrnWriteLiWMBTable[1][j1] );
            }
            iprintf("\n");
            for(j1=0; j1 < (LIW_TABLE_WRITE_SIZE+10) ; j1++)
            {
               iprintf("%x: ", g_arrnWriteLiWMBTable[2][j1] );
            }

         }


            for( i = 0; i < g_sModbusMaster.m_nInstalledUnits; i++ )
            {

              if(arrpDeviceList[i]->m_bPollingEnable )
              {
//              AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
                switch( arrpDeviceList[i]->m_nSystemType )
                {
                case EQUIP_SEI:
                     AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
                     if(g_nWriteHoldOffCounter == 0)
                     {

                        if(g_bSEIIncDecSetpointDownload)
                         {
                           g_bSEIIncDecSetpointDownload = FALSE;

                           if(CheckForValidIncDecTachoSetpoint())
                             {

                                 AddToQueue( COMMS_INC_DEC_WRITE_COMMAND, 0, arrpDeviceList[i] );
                             }
                             else
                             {
                                 ToManual();
                                 AddBlenderToManualIncDecSetpointErrorEventToLog();
                              }

                         }

                         AddToQueue( COMMS_WRITE_COMMANDS, 0, arrpDeviceList[i] );

                     }
                    if( !arrpDeviceList[i]->m_bIDValid )    // if we dont have valid ID info.
                    {
                        AddToQueue( COMMS_READ_ID, 0, arrpDeviceList[i] ); // Read the SEI ID information.
                    }
                    break;
                case EQUIP_LLS:

                    AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
                    AddToQueue( COMMS_WRITE_COMMANDS, 0, arrpDeviceList[i] );

                    break;
                case EQUIP_SSIF:
                	if(g_bSSIFWriteCalibrationRequired)
                	{
                		g_bSSIFWriteCalibrationRequired = FALSE;
                        AddToQueue( COMMS_WRITE_CALIBRATION, 0, arrpDeviceList[i] );
                	}
                	else
                	if(g_bSSIFWriteCommandRequired)
                	{
                		g_bSSIFWriteCommandRequired = FALSE;
                        AddToQueue( COMMS_WRITE_COMMANDS, 0, arrpDeviceList[i] );
                	}
                	else
                	{
                		AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
                	}
                    break;
                case EQUIP_UFP2:
/*
                    if( !g_bCommsAttributeUpdateActive )
                        GetNextAttributeIndices();          // --REVIEW-- may want to do in foreground.
                    if( g_bCursorMode )
                    {
                        nCursorIndex = (g_nCurrentRow * COLUMNS) + g_nCurrentColumn;
                        g_arrnWriteUFP2MBTable[ MB_UFP2_CURSOR - UFP2_OFFSET] = nCursorIndex;
                        g_arrnWriteUFP2MBTable[ MB_UFP2_COMMAND1 - UFP2_OFFSET] |= UFP2_CMD_CURSOR_ON_BIT;
                    }
                    else
                        g_arrnWriteUFP2MBTable[ MB_UFP2_COMMAND1 - UFP2_OFFSET] &= UFP2_CMD_CURSOR_OFF_BIT;
*/
//                    AddToQueue( COMMS_WRITE_COMMANDS, 0, arrpDeviceList[i] );   //

                    break;
                case EQUIP_OPTIMISATION:
                    AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
                    break;

                case EQUIP_LIW:
// moved to 2Hz see DisplayPollTimer
//                     AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
//                     g_nLiWAtQCtr++;
//TestOnly
          /*          g_arrnWriteLiWMBTable[0][MB_LIW_DA] = 0x04FF;
                    g_arrnWriteLiWMBTable[1][MB_LIW_DA] = 0x05FF;
                    g_arrnWriteLiWMBTable[2][MB_LIW_DA] = 0x0800;
           */
                     nIndex = arrpDeviceList[i]->m_nSystemAddress - FIRST_LIW_ADDRESS;
//                     iprintf("\nPIT_MB---DAC,%d = %d %d", nIndex, g_arrnWriteLiWMBTable[1][MB_LIW_DA], g_arrnWriteLiWMBTable[2][MB_LIW_DA]);
                   AddToQueue( COMMS_WRITE_COMMANDS, 0, arrpDeviceList[i] );

                    if( !arrpDeviceList[i]->m_bIDValid )    // if we dont have valid ID info.
                    {
                        AddToQueue( COMMS_READ_ID, 0, arrpDeviceList[i] ); // Read the LIW ID information.
//                        iprintf("\nW R.Id");
                     }
/*
                     if( !arrpDeviceList[i]->m_bIDValid )
                    {
                        AddToQueue( COMMS_READ_ID, 0, arrpDeviceList[i] );
                        iprintf("\nWriting Read Id command to %d %d", i, g_sModbusMaster.m_nInstalledUnits);
                     }
*/
                    break;


/*
                case EQUIP_LOADER:
                case EQUIP_PUMP:
                    // No setpoint messages in loaders or pumps
// MP 07-01-2001 Temp test
//AddToQueue( COMMS_FAST_STATUS_READ, 0, arrpDeviceList[i] );
                    break;
                case EQUIP_BATCH_BLENDER:
                    // If there are 5 or more components, read the
                    // second half of the summary message
                    if( GetComponentCount( i ) > 4 )
                        AddToQueue( COMMS_READ_SUMMARY_PART2, 0, arrpDeviceList[i] );
*/
                default:
//                  AddToQueue( COMMS_READ_SETPOINT, 0, arrpDeviceList[i] );
                    break;
                }  // end of differing system types iprintf("\nPoll Calib. added to Q");


                // Check for the need to write the calibration data
                ni = i;
                if( arrpDeviceList[i]->m_bWriteCalibration )
 //                   AddToQueue( COMMS_WRITE_CALIBRATION, 0, arrpDeviceList[i] );


                // Check for the need to read the calibration data
                ni = i;
                if( arrpDeviceList[i]->m_bPollCalibration )
                    AddToQueue( COMMS_READ_CALIBRATION, 0, arrpDeviceList[i] );

              } //end of if for polling enabled

            } // end of for loop

//          if( !g_bCommsAttributeUpdateActive )
//              GetNextAttributeIndices();          // --DEBUGGING--

            g_sModbusMaster.m_nPollTimer = POLL_TIMER;
        }  // end of else

// Set up to poll remote operator panel, key registers repeatedly (4 times per second)
        if( g_sModbusMaster.m_nKeyPollTimer > 0 )
            g_sModbusMaster.m_nKeyPollTimer--;
        else
        {
            // For the loaders and pumps we want to do the status read continuously
            for( i = 0; i < g_sModbusMaster.m_nInstalledUnits; i++ )
            {
              if(arrpDeviceList[i]->m_bPollingEnable  && arrpDeviceList[i]->m_nSystemType == EQUIP_UFP2)
              {
//                    AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
              }

              if(arrpDeviceList[i]->m_bPollingEnable  && arrpDeviceList[i]->m_nSystemType == EQUIP_SEI)
              {
                    AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
              }

              if(arrpDeviceList[i]->m_bPollingEnable  && arrpDeviceList[i]->m_nSystemType == EQUIP_LIW)
              {
//                    AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
              }
// POLL THE VAC8io'S 4 times/second.
              if(arrpDeviceList[i]->m_bPollingEnable  && arrpDeviceList[i]->m_nSystemType == EQUIP_VAC8IO)
              {
                    AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
                    AddToQueue( COMMS_WRITE_COMMANDS, 0, arrpDeviceList[i] );
              }


            }

            g_sModbusMaster.m_nKeyPollTimer = POLL_TIMER / 4;       // Set to poll keys 4 times per second.
        }

// Set up to poll remote operator panel, display part - 10 times per second, only triggers comms
      //Display poll timer icrements eaach blender PIT, cleared by RTC to get accurate 1 second
      // The POLL_TIMER+5 check is only in case rTC not running.
         if( g_sModbusMaster.m_nDisplayPollTimer > POLL_TIMER+5  || g_bSecondRolloverForPolling )
         {
            g_sModbusMaster.m_nDisplayPollTimer = 0;
            g_bSecondRolloverForPolling = FALSE;
         }
         else
            g_sModbusMaster.m_nDisplayPollTimer++;

            //note this is after decrement.
         nPollTimeA = POLL_TIMER/4;
         nPollTimeB = (POLL_TIMER/2 + POLL_TIMER/4);

        if(  g_sModbusMaster.m_nDisplayPollTimer == nPollTimeA || g_sModbusMaster.m_nDisplayPollTimer == nPollTimeB )
//        else  //
        {
            // For the loaders and pumps we want to do the status read continuously
            for( i = 0; i < g_sModbusMaster.m_nInstalledUnits; i++ )
            {
              if(arrpDeviceList[i]->m_bPollingEnable  && arrpDeviceList[i]->m_nSystemType == EQUIP_LIW )
              {
                    AddToQueue( COMMS_READ_SUMMARY, 0, arrpDeviceList[i] );
                    g_nLiWAtQCtr++;    // diagnostic only.
//                    if(fdnet>0)
//                     printf("\n\n PiTsAtQ= %d DPT= %d", g_nPIT_Int_CounterU, g_sModbusMaster.m_nDisplayPollTimer);

              }

              if( g_sModbusMaster.m_nDisplayPollTimer == nPollTimeA )
               g_cLiWPollTime = 'A';    //A poll is first one
              else
               g_cLiWPollTime = 'B';

            }

//            g_sModbusMaster.m_nDisplayPollTimer = POLL_TIMER / 10;      // Set to poll display 10 times per second.
////            g_sModbusMaster.m_nDisplayPollTimer = POLL_TIMER / 2;      // Set to poll LiW 2 times per second.

        }

//        if(  g_sModbusMaster.m_nDisplayPollTimer == 5 )     // i.e. just after polling LiW's on 0.
        if(  g_sModbusMaster.m_nDisplayPollTimer == (nPollTimeA-2) )     // i.e. just after polling LiW's on A.
        {
           if(g_CalibrationData.m_nComponents > 1)
           {
               g_nLiWPollDecisionCtr++;
               if( g_nLiWPollDecisionCtr > 20 )    // i.e. every 20 seconds
               {
                  g_nLiWPollDecisionCtr = 0;
                  DecideWhichLiWPollToUse();
               }

 //nbb--todo--vacloader              UpdateLiWCardADCData();                   //update (read) the A/D data from LiW cards.
            }

        }

//        if(g_sModbusMaster.m_nDisplayPollTimer == 0)
//         {
//            g_sModbusMaster.m_nDisplayPollTimer = POLL_TIMER;      // to keep timing right, need to add 1.
//         }


/*  --REVIEW--  catering for timeouts, may be a mechanism to detect if device is fitted.
        // For the loaders and pumps we want to do the status read continuously
        for( i = 0; i < g_sModbusMaster.m_nInstalledUnits; i++ )
        {

            // Check for comms timeout alarms
            if( arrpDeviceList[i]->m_nCommsTimeouts >= COMMS_TIMEOUT_LIMIT )
            {
                if( !arrpDeviceList[i]->m_bActiveTimeoutAlarm )
                {
                    structAlarmMessage  alarmMessage;

                    // Get the current time
                    GetTime();
                    memcpy( alarmMessage.m_cTime, g_CurrentTime, TIME_ARRAY_SIZE );

                    alarmMessage.nIndex = i;

                    alarmMessage.m_nAlarmType = ALARM_COMMS_TIMEOUT;

                    alarmMessage.m_bState = ALARM_ACTIVE;

                    // Add the event to the list
                    AddToAlarmQueue( &alarmMessage );

                    // Indicate the alarm state
                    arrpDeviceList[i]->m_bActiveTimeoutAlarm = TRUE;
                }
            }
            else
            {
                // We're not at the timeout limit, check if this is because
                // we've just recieved a good message after being in alarm
                if( arrpDeviceList[i]->m_bActiveTimeoutAlarm )
                {
                    structAlarmMessage  alarmMessage;

                    // Get the current time
                    GetTime();
                    memcpy( alarmMessage.m_cTime, g_CurrentTime, TIME_ARRAY_SIZE );

                    alarmMessage.nIndex = i;

                    alarmMessage.m_nAlarmType = ALARM_COMMS_TIMEOUT;

                    alarmMessage.m_bState = ALARM_INACTIVE;

                    // Add the event to the list
                    AddToAlarmQueue( &alarmMessage );

                    arrpDeviceList[i]->m_bActiveTimeoutAlarm = FALSE;
                }
            }  // end of else
        }
*/
        ///////////////////////////////
        // Deal with the communications
        ///////////////////////////////

        // If the comms is active, check the timeout counter
        if( g_sModbusMaster.m_bActive )
        {
//nbb            int     nReplyCount = 0;
/*
            // Are we monitoring front report timeouts ?
            if( g_bFrontRollReportStart )
            {
                // Flag all layers as failing
                for( i = 0; i < MAX_LAYERS; i++ )
                    g_bFrontReportStatus[i] = FALSE;

                // Check for replies
                for( i = 0; i < g_nLayerCount && i < MAX_LAYERS; i++ )
                {
                    int nIndex = FindNthBlender( i + 1 );

                    if( arrpDeviceList[nIndex]->uniqueData.BlenderData.m_bFrontRollAvailable )
                    {
                        // Flag a successful report on this layer
                        g_bFrontReportStatus[i] = TRUE;
                        nReplyCount++;
                    }
                }

                // Have we waited long enough ?
                g_nFrontRollReportTimer++;
                if( g_nFrontRollReportTimer > REPORT_TIMEOUT )
                {
                    // Signal an alarm
                    structAlarmMessage  alarmMessage;

                    // Get the current time
                    GetTime();
                    memcpy( alarmMessage.m_cTime, g_CurrentTime, TIME_ARRAY_SIZE );

                    alarmMessage.nComponent = -1;

                    alarmMessage.m_nAlarmType = ALARM_FRONT_TIMEOUT;

                    alarmMessage.m_bState = ALARM_ACTIVE;

                    // Add the event to the list
                    AddToAlarmQueue( &alarmMessage );

                    // Move the report on
                    nReplyCount = g_nLayerCount;
                }

                // Have all of the layers responded (or we've timed out)
                if( nReplyCount == g_nLayerCount )
                {
                    // Indicate to the foreground to generate a front roll report
                    g_bGenerateFrontRollReport = TRUE;

                    // Flag that we are done
                    for( i = 0; i < g_nLayerCount && i < MAX_LAYERS; i++ )
                    {
                        int nIndex = FindNthBlender( i + 1 );

                        arrpDeviceList[nIndex]->uniqueData.BlenderData.m_bFrontRollAvailable = FALSE;
                    }

                    // Reset the global variables
                    g_nFrontRollReportTimer = 0;

                    g_bFrontRollReportStart = FALSE;
                }
            }
*/
/*
            // Are we monitoring Back report timeouts ?
            if( g_bBackRollReportStart )
            {
                // Flag all layers as failing
                for( i = 0; i < MAX_LAYERS; i++ )
                    g_bBackReportStatus[i] = FALSE;

                // Check for replies
                for( i = 0; i < g_nLayerCount && i < MAX_LAYERS; i++ )
                {
                    int nIndex = FindNthBlender( i + 1 );

                    if( arrpDeviceList[nIndex]->uniqueData.BlenderData.m_bBackRollAvailable )
                    {
                        // Flag a successful report on this layer
                        g_bBackReportStatus[i] = TRUE;
                        nReplyCount++;
                    }
                }

                // Have we waited long enough ?
                g_nBackRollReportTimer++;
                if( g_nBackRollReportTimer > REPORT_TIMEOUT )
                {
                    // Signal an alarm
                    structAlarmMessage  alarmMessage;

                    // Get the current time
                    GetTime();
                    memcpy( alarmMessage.m_cTime, g_CurrentTime, TIME_ARRAY_SIZE );

                    alarmMessage.nComponent = -1;

                    alarmMessage.m_nAlarmType = ALARM_BACK_TIMEOUT;

                    alarmMessage.m_bState = ALARM_ACTIVE;

                    // Add the event to the list
                    AddToAlarmQueue( &alarmMessage );


                    // Move the report on
                    nReplyCount = g_nLayerCount;
                }

                // Have all of the layers responded (or we've timed out)
                if( nReplyCount == g_nLayerCount )
                {
                    // Indicate to the foreground to generate a back roll report
                    g_bGenerateBackRollReport = TRUE;

                    // Flag that we are done
                    for( i = 0; i < g_nLayerCount && i < MAX_LAYERS; i++ )
                    {
                        int nIndex = FindNthBlender( i + 1 );

                        arrpDeviceList[nIndex]->uniqueData.BlenderData.m_bBackRollAvailable = FALSE;
                    }

                    // Reset the global variables
                    g_nBackRollReportTimer = 0;

                    g_bBackRollReportStart = FALSE;
                }
            }
*/

/*
            // Are we monitoring Order report timeouts ?
            if( g_bOrderReportStart )
            {
                // Flag all layers as failing
                for( i = 0; i < MAX_LAYERS; i++ )
                    g_bOrderReportStatus[i] = FALSE;

                // Check for replies
                for( i = 0; i < g_nLayerCount && i < MAX_LAYERS; i++ )
                {
                    int nIndex = FindNthBlender( i + 1 );

                    if( arrpDeviceList[nIndex]->uniqueData.BlenderData.m_bOrderAvailable )
                    {
                        // Flag a successful report on this layer
                        g_bOrderReportStatus[i] = TRUE;
                        nReplyCount++;
                    }
                }

                // Have we waited long enough ?
                g_nOrderReportTimer++;
                if( g_nOrderReportTimer > REPORT_TIMEOUT )
                {
                    // Signal an alarm
                    structAlarmMessage  alarmMessage;

                    // Get the current time
                    GetTime();
                    memcpy( alarmMessage.m_cTime, g_CurrentTime, TIME_ARRAY_SIZE );

                    alarmMessage.nComponent = -1;

                    alarmMessage.m_nAlarmType = ALARM_ORDER_TIMEOUT;

                    alarmMessage.m_bState = ALARM_ACTIVE;

                    // Add the event to the list
                    AddToAlarmQueue( &alarmMessage );


                    // Move the report on
                    nReplyCount = g_nLayerCount;
                }

                // Have all of the layers responded (or we've timed out)
                if( nReplyCount == g_nLayerCount )
                {
                    // Indicate to the foreground to generate a back roll report
                    g_bGenerateOrderReport = TRUE;

                    // Flag that we are done
                    for( i = 0; i < g_nLayerCount && i < MAX_LAYERS; i++ )
                    {
                        int nIndex = FindNthBlender( i + 1 );

                        arrpDeviceList[nIndex]->uniqueData.BlenderData.m_bOrderAvailable = FALSE;
                    }

                    // Reset the global variables
                    g_nOrderReportTimer = 0;

                    g_bOrderReportStart = FALSE;
                }
            }
*/

            // Decrement the timeout counter
            if( g_sModbusMaster.m_nTimeoutCounter > 0 )
                {
                g_sModbusMaster.m_nTimeoutCounter--;
                if(g_sModbusMaster.m_nTimeoutCounter == 2)
                    g_sModbusMaster.m_nTimeoutCounter = 2;
                }
            else    // timeout counter = 0
            {
                // Increment the error counter
                g_sModbusMaster.m_lRxTimeoutCounter++;
                g_sModbusMaster.m_pController->m_lRxTimeoutCounter++;
                IncrementOngoingModulesHistoryTimeoutCtr();

                // Shut the comms down
                TxRxOff();

                // Increment the comms alarm timeout counter
                if( g_sModbusMaster.m_pController->m_nCommsTimeouts < COMMS_TIMEOUT_LIMIT )
                    g_sModbusMaster.m_pController->m_nCommsTimeouts++;

                // Check the number of retries and if
                // necessary, start the tx again.
                if( g_sModbusMaster.m_nRetries > 0 )
                {
                    g_sModbusMaster.m_nRetries--;

                    // Initialise the pre transmission delay
                    g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                    g_sModbusMaster.m_bRunStartDelay = TRUE;
                }
                else
                {
                    // Otherwise remove the message from the top of the queue
                    RemoveFromQueue( g_sModbusMaster.m_pController );

                    // If it's a write message check if it should be queued again.
                    // If it's a fast status read, zero it so that the sequencer
                    // doesn't make incorrect descisions.
                    switch( g_sModbusMaster.m_nMessageType )
                    {
/*
                    case COMMS_FAST_STATUS_READ:
                        g_sModbusMaster.m_pController->arrnReadData[FAST_STATUS_WORD] = 0;
                        break;
*/

/*
                    case COMMS_WRITE_SETPOINT:
                        // If a write message fails we need to queue
                        // it again for the required number of retries
                        g_sModbusMaster.m_pController->m_nSetpointRetries++;
                        if( g_sModbusMaster.m_pController->m_nSetpointRetries < MODBUS_WRITE_QUEUE_RETRIES )
                            AddToQueue( COMMS_WRITE_SETPOINT, 0, g_sModbusMaster.m_pController );
                        else
                        {
                            g_sModbusMaster.m_pController->m_nSetpointRetries = 0;

                            // generate an alarm
                        }
                        break;
*/
                    case COMMS_WRITE_CALIBRATION:
                        // If a write message fails we need to queue
                        // it again for the required number of retries
                        g_sModbusMaster.m_pController->m_nCalibrationRetries++;
                        if( g_sModbusMaster.m_pController->m_nCalibrationRetries < MODBUS_WRITE_QUEUE_RETRIES )
                            AddToQueue( COMMS_WRITE_CALIBRATION, 0, g_sModbusMaster.m_pController );
                        else
                        {
                            g_sModbusMaster.m_pController->m_nCalibrationRetries = 0;

                            // generate an alarm
                        }
                        break;
                    case COMMS_WRITE_COMMANDS:
                        // If a write message fails we need to queue
                        // it again for the required number of retries
                        g_sModbusMaster.m_pController->m_nCommandWriteRetries++;
                        if( g_sModbusMaster.m_pController->m_nCommandWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                            AddToQueue( COMMS_WRITE_COMMANDS, 0, g_sModbusMaster.m_pController );
                        else
                        {
                            g_sModbusMaster.m_pController->m_nCommandWriteRetries = 0;

                            // generate an alarm
                        }
                        break;
                    case COMMS_WRITE_DISPLAY:       // --REVIEW-- perhaps shouldn't be any retries on display writes.
                        // If a write message fails we need to queue
                        // it again for the required number of retries
                        g_sModbusMaster.m_pController->m_nDisplayWriteRetries++;
                        if( g_sModbusMaster.m_pController->m_nDisplayWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                            AddToQueue( COMMS_WRITE_DISPLAY, 0, g_sModbusMaster.m_pController );
                        else
                        {
                            g_sModbusMaster.m_pController->m_nDisplayWriteRetries = 0;

                            // generate an alarm
                        }
                        break;

/*
                    case COMMS_WRITE_OPTIONS:
                        // If a write message fails we need to queue
                        // it again for the required number of retries
                        g_sModbusMaster.m_pController->m_nOptionWriteRetries++;
                        if( g_sModbusMaster.m_pController->m_nOptionWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                            AddToQueue( COMMS_WRITE_OPTIONS, 0, g_sModbusMaster.m_pController );
                        else
                        {
                            g_sModbusMaster.m_pController->m_nOptionWriteRetries = 0;

                            // generate an alarm
                        }
                        break;
*/
                    case COMMS_WRITE_SINGLE_WORD:
                        // If a write message fails we need to queue
                        // it again for the required number of retries
                        g_sModbusMaster.m_pController->m_nSingleWriteRetries++;
                        if( g_sModbusMaster.m_pController->m_nSingleWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                            AddToQueue( COMMS_WRITE_SINGLE_WORD, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                        else
                        {
                            g_sModbusMaster.m_pController->m_nSingleWriteRetries = 0;

                            // generate an alarm
                        }
                        break;

                        case COMMS_INC_DEC_WRITE_COMMAND:
                        // If a write message fails we need to queue
                        // it again for the required number of retries
                        g_sModbusMaster.m_pController->m_nIncDecSetpointWriteRetries++;
                        if( g_sModbusMaster.m_pController->m_nIncDecSetpointWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                            AddToQueue( COMMS_INC_DEC_WRITE_COMMAND, 0, g_sModbusMaster.m_pController );
                        else
                        {
                            g_sModbusMaster.m_pController->m_nIncDecSetpointWriteRetries = 0;

                            // generate an alarm
                        }
                        break;


/*
                    case COMMS_READ_FRONT_REPORT:
                        // Never give up
                        AddToQueue( COMMS_READ_FRONT_REPORT, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                        break;
                    case COMMS_READ_BACK_REPORT:
                        // Never give up
                        AddToQueue( COMMS_READ_BACK_REPORT, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                        break;
                    case COMMS_READ_ORDER_REPORT:
                        // Never give up
                        AddToQueue( COMMS_READ_ORDER_REPORT, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                        break;
                    case COMMS_CLEAR_FRONT_FLAG:
                        // Never give up
                        AddToQueue( COMMS_CLEAR_FRONT_FLAG, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                        break;
                    case COMMS_CLEAR_BACK_FLAG:
                        // Never give up
                        AddToQueue( COMMS_CLEAR_BACK_FLAG, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                        break;
                    case COMMS_CLEAR_ORDER_FLAG:
                        // Never give up
                        AddToQueue( COMMS_CLEAR_ORDER_FLAG, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                        break;
                    case COMMS_WRITE_COMP_CONFIG:
//  *****
// Do something here
                        break;
*/
                    default:
                        // Unsupported message type - flag an error somewhere.
                        break;
                    }
                }
            }
        }
        else        // i.e. comms not active.
        {
            if( !g_sModbusMaster.m_bRunStartDelay && !g_sModbusMaster.m_bProcessingMessage )
            {
                int nQueueIndex;
                int nUnit;
//              int nTemp;

                // Move on to the next unit with polling enabled.
                if( g_sModbusMaster.m_nInstalledUnits > 1 )
                {
                  nUnit = g_sModbusMaster.m_nCurrentUnit;
                  do
                  {
                        g_sModbusMaster.m_nCurrentUnit++;
                        if( g_sModbusMaster.m_nCurrentUnit >= g_sModbusMaster.m_nInstalledUnits ) //  MAX_COMMS_UNITS )
                            g_sModbusMaster.m_nCurrentUnit = 0;
                  }
                  while( !arrpDeviceList[g_sModbusMaster.m_nCurrentUnit]->m_bPollingEnable && g_sModbusMaster.m_nCurrentUnit != nUnit );
                }
                else // only 1 installed unit, current unit must always be 0.
                    g_sModbusMaster.m_nCurrentUnit = 0;

/*
                // Verify that the current unit is installed
                if( g_sModbusMaster.m_nCurrentUnit >= g_sModbusMaster.m_nInstalledUnits || g_sModbusMaster.m_nCurrentUnit >= MAX_COMMS_UNITS )
                    g_sModbusMaster.m_nCurrentUnit = 0;
*/

                // Load the pointer to the controllers data
                g_sModbusMaster.m_pController = arrpDeviceList[g_sModbusMaster.m_nCurrentUnit];

                // Get the next message in the queue
                nQueueIndex = GetTopOfQueue( g_sModbusMaster.m_pController );
                if( nQueueIndex != COMMS_INVALID_MESSAGE )
                {
                    // Show that we're assembling a message
                    g_sModbusMaster.m_bProcessingMessage = TRUE;

                    g_sModbusMaster.m_nMessageType = g_sModbusMaster.m_pController->m_arrMessageQueue[nQueueIndex].m_nMessageType;
                    g_sModbusMaster.m_nMessageValue = g_sModbusMaster.m_pController->m_arrMessageQueue[nQueueIndex].m_nValue;

                    // Switch on the message type to decide if it's a read
                    // or a write.
                    switch( g_sModbusMaster.m_nMessageType )
                    {
/*
                    case COMMS_FAST_STATUS_READ:
                        // Format up a request
                        GenerateStatusReadRequest();

                        // Start the ball rolling
                        g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                        g_sModbusMaster.m_bRunStartDelay = TRUE;
                        break;
*/
                    case COMMS_READ_SUMMARY:
//                  case COMMS_READ_SUMMARY_PART2:
//                  case COMMS_READ_SETPOINT:
                    case COMMS_READ_CALIBRATION:
                    case COMMS_READ_ID:
//                  case COMMS_READ_FRONT_REPORT:
//                  case COMMS_READ_BACK_REPORT:
//                  case COMMS_READ_ORDER_REPORT:
//                  case COMMS_READ_DIAGNOSTICS:
                        // Format up a request
                        GenerateReadRequest();

                        // Start the ball rolling
                        g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                        g_sModbusMaster.m_bRunStartDelay = TRUE;
                        break;
//                  case COMMS_WRITE_SETPOINT:
                    case COMMS_WRITE_COMMANDS:
                    case COMMS_WRITE_CALIBRATION:
//                  case COMMS_SET_TIME:
//                  case COMMS_WRITE_COMP_CONFIG:
//                  case COMMS_WRITE_OPTIONS:
                    case COMMS_WRITE_DISPLAY:
                    case COMMS_INC_DEC_WRITE_COMMAND:

                        // Format up a request
                        GenerateWriteRequest();

                        // Start the ball rolling
                        g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                        g_sModbusMaster.m_bRunStartDelay = TRUE;
                        break;
                    case COMMS_WRITE_SINGLE_WORD:
                        // Format up a request
                        GenerateSingleWriteRequest( g_sModbusMaster.m_nMessageValue );

                        // Start the ball rolling
                        g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                        g_sModbusMaster.m_bRunStartDelay = TRUE;
                        break;
/*
                    case COMMS_CLEAR_FRONT_FLAG:
                        nTemp = (int)g_sModbusMaster.m_pController->arrnWriteData[BATCH_CONTROL_CLEAR_COMMAND];
                        nTemp |= BATCH_CLEAR_FRONT_FLAG;
                        g_sModbusMaster.m_pController->arrnWriteData[BATCH_CONTROL_CLEAR_COMMAND] = (float)nTemp;

                        // Format up a request
                        GenerateSingleWriteRequest( BATCH_CONTROL_CLEAR_COMMAND );

                        // Start the ball rolling
                        g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                        g_sModbusMaster.m_bRunStartDelay = TRUE;
                        break;
                    case COMMS_CLEAR_BACK_FLAG:
                        nTemp = (int)g_sModbusMaster.m_pController->arrnWriteData[BATCH_CONTROL_CLEAR_COMMAND];
                        nTemp |= BATCH_CLEAR_BACK_FLAG;
                        g_sModbusMaster.m_pController->arrnWriteData[BATCH_CONTROL_CLEAR_COMMAND] = (float)nTemp;

                        // Format up a request
                        GenerateSingleWriteRequest( BATCH_CONTROL_CLEAR_COMMAND );

                        // Start the ball rolling
                        g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                        g_sModbusMaster.m_bRunStartDelay = TRUE;
                        break;
                    case COMMS_CLEAR_ORDER_FLAG:
                        nTemp = (int)g_sModbusMaster.m_pController->arrnWriteData[BATCH_CONTROL_CLEAR_COMMAND];
                        nTemp |= BATCH_CLEAR_ORDER_FLAG;
                        g_sModbusMaster.m_pController->arrnWriteData[BATCH_CONTROL_CLEAR_COMMAND] = (float)nTemp;

                        // Format up a request
                        GenerateSingleWriteRequest( BATCH_CONTROL_CLEAR_COMMAND );

                        // Start the ball rolling
                        g_sModbusMaster.m_nMessageStartDelay = INTER_MESSAGE_DELAY;
                        g_sModbusMaster.m_bRunStartDelay = TRUE;
                        break;
*/
                    case COMMS_INVALID_MESSAGE:
                        // This just shows that the queue is empty.
                        // no need to do anything
                        break;
                    default:
                        // Unsupported message type - flag an error somewhere.

                        // remove the unsupported message from the queue
                        RemoveFromQueue( g_sModbusMaster.m_pController );
                        break;
                    }

                    // Show that we're not assembling a message
                    g_sModbusMaster.m_bProcessingMessage = FALSE;
                }
            }
        }
        // If we are delaying at the start of transmission,
        // decrement the counter
        if( g_sModbusMaster.m_bRunStartDelay )
        {
            if( g_sModbusMaster.m_nMessageStartDelay > 0 )
                g_sModbusMaster.m_nMessageStartDelay--;
            else
            {
                // If the timer has elapsed and the comms isn't started,
                // then start the transmitter.
                if( !g_sModbusMaster.m_bActive )
                   {
                            MasterTxOn();
                   }
            }
        }

        // Indicate that we're finished checking the comms.
        g_bCheckingComms = FALSE;
    }
}


///////////////////////////////////////////////////////////
// GetNextDisplayWriteIndices()         //
//
//
// Gets the indices for writing to display via comms.
// Searches through a maximum of 160 characters to find first one requiring update.
// Then generates a start and end indices from that first character.  End index is max of first one plus
// 80 (max 80 characters can be written at a time - PIC limitation).
//
//
//      Entry:  g_nCommsUpdateStartIndex is the lowest display address, address from which starts
//                      looking for character requiring update.
//
//      Exit:
//              g_nVramStartDisplayIndex    is the index to the first char requiring update.
//                   g_nVramEndDisplayIndex is index of last char to be written in this message.
//               g_bCommsUpdateActive will be set if valid indices have been found, i.e. if a message
//                   is required.
//
// M.McKiernan                         26-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
void GetNextDisplayWriteIndices( void )
{
    BOOL bFound = FALSE;
    int i;
    int nLo,nHi,nEnd;

// set limits for search.
    nLo = g_nCommsUpdateStartIndex;
    nHi = nLo + 160;                    // going to check 160 chars maximum.
    if(nHi > MAX_DISPLAY_INDEX)
        nHi = MAX_DISPLAY_INDEX;            // dont go beyond the end of the display.

// search for first char to be updated, i.e. with its Rem change bit set.
    i = nLo;
    while ( !bFound && i < nHi )
    {
        if( g_arrAttributeBuffer[i] & REM_TEXT_CHANGED_MASK )
            bFound = TRUE;
        else
            i++;
    }

// has there been a character found?
    if( bFound )
    {
        nLo = i;
        nEnd = i+80;            // can only write 80 chars max.
        if( nEnd > MAX_DISPLAY_INDEX )
            nEnd = MAX_DISPLAY_INDEX;
        for(i = nLo; i < nEnd; i++)
        {
            if( g_arrAttributeBuffer[i] & REM_TEXT_CHANGED_MASK )
            {
                nHi = i;        // will locate char requiring update.
                g_arrAttributeBuffer[i] &= REM_CLEAR_TEXT_CHANGED_MASK;
            }

        }
        g_bCommsUpdateActive = TRUE;        // signal that there is A comms display update happening.
        g_nVramStartDisplayIndex = nLo;
        g_nVramEndDisplayIndex = nHi;
    }
    else    // no char found in this section needed updating (writing to remote panel)
    {
        g_bCommsUpdateActive = FALSE;       // signal that there is no comms display update happening.

        // set the update index for next pass of function.
        g_nCommsUpdateStartIndex = nHi;
        if( g_nCommsUpdateStartIndex >= MAX_DISPLAY_INDEX )
            g_nCommsUpdateStartIndex = 0;
    }
}



///////////////////////////////////////////////////////////
// GetNextAttributeIndices()            //
//
//
// Gets the indices for writing attribute data to display via comms.
// Searches from start index (g_nStartAttributeUpdateIndex ) to find first one requiring update.
// Then counts characters with attrib. change set.
// MAX_ATT_BLOCKS (8) (blocks of data maximum).
//
//
//      Entry:  g_nStartAttributeUpdateIndex is the lowest display address, address from which starts
//                      looking for attribute requiring update.
//
//      Exit:
//              structUFPAttributeData[] contains the attribute data [attribute, size of block, address or index]
//
// M.McKiernan                         26-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
/*
void GetNextAttributeIndices( void )
{
    int i;
    int nBlock = 0;
    int nCount = 0;
    unsigned char cType = 0xFF;
    BOOL    bFound = FALSE;

    BOOL bUpdateSet = FALSE;        // remote update bit set.
//  nLo = g_nStartAttributeUpdateIndex;     // place from which to start.
  if( !g_bCommsAttributeUpdateActive )
  {
    cType = g_arrAttributeBuffer[g_nStartAttributeUpdateIndex] & 0x0F;  // load first attribute type.

    for( i = g_nStartAttributeUpdateIndex; i<MAX_DISPLAY_INDEX && nBlock < MAX_ATT_BLOCKS; i++)
    {
            if( g_arrAttributeBuffer[i] & REM_ATTRIBUTE_CHANGED_MASK )
            {
                bFound = TRUE;
                bUpdateSet = TRUE;          // indicates char with attr. change set..
                if( (g_arrAttributeBuffer[i] & 0x0F) == cType ) // same as previous or first pass.
                {
                    if( nCount == 0)
                    {
                        structUFPAttributeData[nBlock].nAddress = i;        // start address for block.
                    }
                    nCount++;
                    structUFPAttributeData[nBlock].cType = g_arrAttributeBuffer[i] & 0x0F;      // attribute type.
                    structUFPAttributeData[nBlock].cSize = nCount;      //
                    // have we reached the maximum no. of attrib.'s in block.
                    if( nCount > MAX_ATTRIB_BLOCK_SIZE )
                    {
                        nBlock++;           // advance to next attribute block.
                        nCount = 0;         // reset the counter.
                    }
                }
                else    // different type of attribute
                {
                    cType = g_arrAttributeBuffer[i] & 0x0F;     // store type for next pass.
                    nBlock++;           // advance to next attribute block.
                    nCount = 1;
                    structUFPAttributeData[nBlock].nAddress = i;        // start address for block.
                    structUFPAttributeData[nBlock].cType = cType;       // attribute type.
                    structUFPAttributeData[nBlock].cSize = nCount;      //
                }

                g_arrAttributeBuffer[i] &= REM_CLEAR_ATTRIBUTE_CHANGED_MASK;    // clear change bit.
            }
            else    // attribute change bit is clear,
            {
//              structUFPAttributeData[nBlock].cSize = nCount;      //
                if(bUpdateSet)
                    nBlock++;           // advance to next attribute block, if last char had its update set.
                cType = g_arrAttributeBuffer[i] & 0x0F;     // store type for next pass.
                nCount = 0;
                bUpdateSet = FALSE;
            }

    }

    if(bFound)      // Has at least one block of attributes been found.
    {
        g_bCommsAttributeUpdateActive = TRUE;   // flag will be cleared when update has been completed on Comms.
        memcpy( &g_arrnWriteUFP2MBTable[MB_UFP2_ATTRIBUTES_1 - UFP2_OFFSET], &structUFPAttributeData[0], ( sizeof( structAttributeData ) * MAX_ATT_BLOCKS ) );
    }
    else
    {
        memset( &g_arrnWriteUFP2MBTable[MB_UFP2_ATTRIBUTES_1 - UFP2_OFFSET], 0, ( sizeof( structAttributeData ) * MAX_ATT_BLOCKS ) );
    }
    // store start index for next pass.
    if( i >= MAX_DISPLAY_INDEX)
        g_nStartAttributeUpdateIndex = 0;       //
    else
        g_nStartAttributeUpdateIndex = i;

  }
}
*/
///////////////////////////////////////////////////////////
// GetNextAttributeIndices()            //
//
//
// Gets the indices for writing attribute data to display via comms.
// Searches from start index (g_nStartAttributeUpdateIndex ) to find first one requiring update.
// Then counts characters with attrib. change set.
// MAX_ATT_BLOCKS (8) (blocks of data maximum).
//
//
//      Entry:  g_nStartAttributeUpdateIndex is the lowest display address, address from which starts
//                      looking for attribute requiring update.
//
//      Exit:
//              structUFPAttributeData[] contains the attribute data [attribute, size of block, address or index]
//
// M.McKiernan                         26-01-2005
// First pass.
///////////////////////////////////////////////////////////
//
void GetNextAttributeIndices( void )
{
    BOOL bSet = FALSE;
    int i,n;
    int j = 0;
    unsigned char cPrev,cCurrent;
 cPrev = 0;
 if( !g_bCommsAttributeUpdateActive )
 {
        g_bCommsAttributeUpdateActive = TRUE;   // flag will be cleared if no update needed or when completed on Comms.

// find first char with update set.
    for(i=g_nStartAttributeUpdateIndex; i < MAX_DISPLAY_INDEX && !bSet; i++)
    {
        if( g_arrAttributeBuffer[i] & REM_ATTRIBUTE_CHANGED_MASK )
        {
            // clear attribute structure
            for(n=0; n<MAX_ATT_BLOCKS; n++)
            {
                structUFPAttributeData[n].cSize = 0;
                structUFPAttributeData[n].cType = 0;
                structUFPAttributeData[n].nAddress = 0;
            }

            bSet = TRUE;

            cPrev = g_arrAttributeBuffer[i] & 0x0F;
            structUFPAttributeData[j].nAddress = i;
            structUFPAttributeData[j].cType = cPrev;
        }
    }

// is i pointing to  char with update, or to next one??????????????????
    i = i-1;    // go back to char just found.
    if( bSet )
    {
//      for(i=i; i < MAX_DISPLAY_INDEX && j<MAX_ATT_BLOCKS; i++)
        for( ; i < MAX_DISPLAY_INDEX && j<MAX_ATT_BLOCKS; i++)
        {
            // is update bit set for this attribute??
            if( g_arrAttributeBuffer[i] & REM_ATTRIBUTE_CHANGED_MASK )
            {
                cCurrent =  g_arrAttributeBuffer[i] & 0x0F;

                if( bSet && (cCurrent == cPrev) && ( structUFPAttributeData[j].cSize < MAX_ATTRIB_BLOCK_SIZE ) )    // previous Attrib had update bit set, and was same type.
                {
                    structUFPAttributeData[j].cSize++;  // increment counter
                }
                else
                {
                    j++;    // advance to the next block.
                    if( j < MAX_ATT_BLOCKS )
                    {
                        structUFPAttributeData[j].nAddress = i;
                        structUFPAttributeData[j].cType = cCurrent;
                        structUFPAttributeData[j].cSize = 1;    // first update.
                    }
                    else
                        i -=1;      // we have exceeded blocks, go back one for next pass.
                }

                bSet = TRUE;
                cPrev = cCurrent;
                g_arrAttributeBuffer[i] &= REM_CLEAR_ATTRIBUTE_CHANGED_MASK;
            }
            else  // update bit clear
            {
                bSet = FALSE;   //
            }

        }

        // copy structure into MB table for update.
//      g_bCommsAttributeUpdateActive = TRUE;   // flag will be cleared when update has been completed on Comms.
        memcpy( &g_arrnWriteUFP2MBTable[MB_UFP2_ATTRIBUTES_1 - UFP2_OFFSET], &structUFPAttributeData[0], ( sizeof( structAttributeData ) * MAX_ATT_BLOCKS ) );


    }
    else
    {   // no update required. - CLEAR MB table which will be written to UFP.
        memset( &g_arrnWriteUFP2MBTable[MB_UFP2_ATTRIBUTES_1 - UFP2_OFFSET], 0, ( sizeof( structAttributeData ) * MAX_ATT_BLOCKS ) );
        g_bCommsAttributeUpdateActive = FALSE;
    }

    // store start index for next pass.
    if( i >= MAX_DISPLAY_INDEX)
        g_nStartAttributeUpdateIndex = 0;       //
    else
        g_nStartAttributeUpdateIndex = i;


  }
}


////////////////////////////////test stuff///////////////////////
// for test purposes only.  DEBUGGING purposes.  Called from InitBlnd.c on startup.
void ModbusMasterTest( void )
{
//    int i=0;
//  int nIndex;
// put some data in SEI MB write area.
/*
        for( i=0; i<SEI_TABLE_WRITE_SIZE; i++)
         g_arrnWriteSEIMBTable[i] = 0x24+i; // 0x11c0 + i;

        g_arrnWriteSEIMBTable[0] = 0x0e84;  //
        g_arrnWriteSEIMBTable[1] = 0x0003;  //

                for( i=0; i<LLS_TABLE_WRITE_SIZE; i++)
         g_arrnWriteLLSMBTable[i] = 0x74+i; // 0x11c0 + i;

        g_arrnWriteLLSMBTable[MB_LLS_COMMAND] = 0xa5a5;     //
*/

//      g_nStartAttributeUpdateIndex = 15*COLUMNS;

/*
        g_arrnWriteUFP2MBTable[MB_UFP2_CURSOR - UFP2_OFFSET] = ROWS*3 + 20;     // CURSOR AT ROW 3 COL. 20
        g_arrnWriteUFP2MBTable[MB_UFP2_COMMAND1 - UFP2_OFFSET] = UFP2_CMD_CURSOR_ON_BIT;    // cursor on.
        g_arrnWriteUFP2MBTable[MB_UFP2_ATTRIBUTES_1 - UFP2_OFFSET] = 0x0807 ;   // 7 Rev Blinking attributes at x
        g_arrnWriteUFP2MBTable[MB_UFP2_ATTRIBUTES_1 - UFP2_OFFSET + 1] = COLUMNS*7 + 10;    // Row 7, col. 10
*/
//      g_nVramStartDisplayIndex = 0;
//      g_nVramEndDisplayIndex = g_nVramStartDisplayIndex + 79;         // 2 lines (80 chars) of text..

/*
    int nValue[7];
    int nAddr, nType;

    ConfigureModbusMaster();
    InitialiseModbusMaster();

        for( nIndex = 0; nIndex < 20; nIndex++ )
        {
            // Set the display character to a blank
            g_arrTextBuffer[nIndex] = 'A' + nIndex; //
//          g_arrAttributeBuffer[nIndex] |= TEXT_CHANGED_MASK;
        }

// testing access of data
        for( i = 0; i < 5; i++ )
        {
            nValue[i] = arrpDeviceList[0]->arrnWriteData[i];
        }

        nAddr = structSEIData.m_nSystemAddress;
        nType = structSEIData.m_nSystemType;


        nAddr = arrpDeviceList[0]->m_nSystemAddress;
        nType = arrpDeviceList[1]->m_nSystemType;

        g_sModbusMaster.m_pController = arrpDeviceList[3];
        nAddr = g_sModbusMaster.m_pController->m_nSystemAddress;
        nType = g_sModbusMaster.m_pController->m_nSystemType;


        g_sModbusMaster.m_pController = arrpDeviceList[0];

        nValue[0] = g_sModbusMaster.m_pController->m_bPollingEnable;
        g_sModbusMaster.m_pController = arrpDeviceList[1];
        nValue[1] = g_sModbusMaster.m_pController->m_bPollingEnable;
        g_sModbusMaster.m_pController = arrpDeviceList[2];
        nValue[2] = g_sModbusMaster.m_pController->m_bPollingEnable;
        g_sModbusMaster.m_pController = arrpDeviceList[3];
        nValue[3] = g_sModbusMaster.m_pController->m_bPollingEnable;

*/
}

BOOL CheckForValidIncDecTachoSetpoint( void )
{
    BOOL bIsGoodSet = TRUE;
    int nTachoHigh,nTachoLow;

    if((g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) && (g_bManualAutoFlag == AUTO))
    {



        if(g_arrnWriteSEIMBTable[MB_TACHO_SETPOINT] != g_arrnReadSEIMBTable[MB_TACHO_SETPOINT])
        {

            nTachoHigh = g_nExtruderTachoReadingAverage + (int)(MAXEXTRUDERTACHO * 0.20f);
            nTachoLow = g_nExtruderTachoReadingAverage - (int)(MAXEXTRUDERTACHO * 0.20f);
            if(nTachoLow < 100)
            {
                nTachoLow = 100;
            }

          if((g_arrnWriteSEIMBTable[MB_TACHO_SETPOINT] > nTachoHigh) || (g_arrnWriteSEIMBTable[MB_TACHO_SETPOINT] < nTachoLow))
          {
              bIsGoodSet = FALSE;
              PutAlarmTable( LSMONITORALARM,  0 );     /* indicate alarm, Component no. is zero. PTALTB */
          }
          else
          {
//          SetupMttty();
//          iprintf("\n sp is okay");
          }
       }
    }
    return (bIsGoodSet);
}
//////////////////////////////////////////////////////////////////
// DecideWhichLiWPollToUse( )
// Note Liw's polled twice per second.
// Treat first poll result as A, second as B.
// Data at LiW updated once per second.  Decide which poll result to use
//    A0    B0    A1    B1    A2    B2
// ----------------------------------------------------------
//
//////////////////////////////////////////////////////////////////
void DecideWhichLiWPollToUse( void )
{
unsigned int j;
   for(j=0; j<g_CalibrationData.m_nComponents-1 && j<MAX_LIW_CARDS; j++ )
   {
      // if A = Prev A, then use B data.
      if(g_sLiWCard[j].m_nAEqualsPreviousACtr > 0)
         g_sLiWCard[j].m_bUsePollB = TRUE;   // use B
      // if B = Prev B, then use A value.
      else if(g_sLiWCard[j].m_nBEqualsPreviousBCtr > 0)
         g_sLiWCard[j].m_bUsePollB = FALSE;               // i.e. use A poll data
      // if Bx = Ax, then  LiW data changes at Bx-1 to Ax, use Ax, the earliest one.
      else if( g_sLiWCard[j].m_nBEqualsCurrentACtr > g_sLiWCard[j].m_nAEqualsPreviousBCtr )
         g_sLiWCard[j].m_bUsePollB = FALSE;              // i.e. use A poll data
      // if Ax = Prev Bx, then LiW changes between A - B, use B.
      else if(g_sLiWCard[j].m_nAEqualsPreviousBCtr > g_sLiWCard[j].m_nBEqualsCurrentACtr)
         g_sLiWCard[j].m_bUsePollB = TRUE;              // i.e. use B poll data
      else  //default
         g_sLiWCard[j].m_bUsePollB = TRUE;              // i.e. use B poll data


// reset the counters for next pass.
      g_sLiWCard[j].m_nBEqualsCurrentACtr = 0;  //counter for data from polls A and B being equal
      g_sLiWCard[j].m_nAEqualsPreviousBCtr = 0; // counter for data from poll A equalling previous B poll.
      g_sLiWCard[j].m_nBEqualsPreviousBCtr = 0; //
      g_sLiWCard[j].m_nAEqualsPreviousACtr = 0; //

   }



}

