///////////////////////////////////////////////////////
// LoadConfiguration
//
// Loads the configuration from EEPROM and initialises
// the necessary data structures
//
// M.Parks                              24-03-2000
// Moved from main to seperate function.
// M.McKiernan                          25-06-2001
// turn off the polling disable, i.e. enable polling arrpDeviceList[i]->m_bPollingEnable = FALSE;
// M.McKiernan                          20-11-2003
// Load group number.
// SaveProcessData - saves process type, group descriptions, and group no.'s to eeprom.
// CheckSystemConfiguration
// Checks if no. of blenders, and blenders in each group, and no. of pumps and loaders.
// FindNthLayerInGroupY   Searches the configuration list and finds the specified layer in specified group.
// CheckLoaderConfiguration  Checks if no. pump no. is valid for each loader
// Write g_nRecipeEntryMode to eeprom in SaveOptions(), and read back in LoadConfiguration().
// initialise the fill no. to 0 on start-up //arrpDeviceList[i]->uniqueData.LoaderData.m_nLoaderFillNo = 0;     //

//******************* For Batch Blender ***************************************

// M.McKiernan                              24-05-2004
// Removed stuff at the //bb  - g_nTimer1Period,g_nTimer2Period
// Removed references to reports //bb g_FrontRollReport etc.
// Removed calls to AddToQueue, g_sModbusMaster
// M.McKiernan                              30-09-2004
// Set all component configurations, for TopUp mode
// M.McKiernan                              04-11-2004
// Added ForegroundSaveRecipe( ), SaveAllCalibrationDataToEEprom(), LoadAllCalibrationDataFromEEprom
// M.McKiernan                              14-12-2004
// Removed RecipeFormat.h, which was calling ModbusIndices.h
//
// M.McKiernan                              18-01-2005
// Removed  following // 18.01.2005 structControllerData    *arrpDeviceList[MAX_COMMS_UNITS];
//
// P.Smith                              1-04-2005
// Copy g_nHiLowOptionSwitchedOnOff to g_nHiLowOptionSwitchedOnOffHistory
//
// P.Smith                              6/9/05
// added SaveAllCalibrationDataToOnBoardEEprom
// correct read of onboard calibration data use ONBOARD_EEPROM_CALIBRATION_DATA_START
//
// P.Smith                              6/10/05
// generate check sum in SaveAllCalibrationDataToOnBoardEEprom
// ensure that the checksum value calculated does not include the checksum itself.
//
//
// P.Smith                              7/10/05
// reset CALINVALARM alarm when saveing calibration data to eeprom
//
// P.Smith                              11/1/06
// Correct warnings
// removed     int i;,char    *pPtr;char    cTemp;
// removed     nBytes = sizeof( g_TempRecipe );
//             nBytes2 = sizeof(g_LayerDataTestOnly);
//             nBytes3 = sizeof(g_CompDataTestOnly);
// removed     Bool    nResult;nBytes,nBytes2,nBytes3, in loadrecipe
// removed     int i in SaveCommsConfiguration
// removed      //bb    WriteRemovableEEPROMData( uData.cValue[0], EEPROM_TIMER_1_PERIOD );
//              bb    WriteRemovableEEPROMData( uData.cValue[1], EEPROM_TIMER_1_PERIOD + 1 );
//              uData.nValue = 12; //bb g_nTimer2Period;
//              bb    WriteRemovableEEPROMData( uData.cValue[0], EEPROM_TIMER_2_PERIOD );
//              bb    WriteRemovableEEPROMData( uData.cValue[1], EEPROM_TIMER_2_PERIOD + 1 );
//              // save timer periods.
//              uData.nValue = 12; //bb g_nTimer1Period;
//              union   CharsAndInt uData; in SaveProcessData
//              i removed in void ForegroundSaveRecipe( void )
//              unsigned char    g_arrcTemp[sizeof(g_CalibrationData)];
//
// comment out  g_nLanguage = g_CalibrationData.m_nLanguage;
// removed     unsigned char   cData from int LoadAllCalibrationDataFromEEprom( void )
// removed     unsigned char   cData from int LoadAllCalibrationDataFromOnBoardEEprom( void )
// comment out //    int i;in  FindNthLayerInGroupY( int nLayer, char cGroupY )
//              //    int     i in int FindNthLoader( int nLoader )
//              //    int     i;in int FindNthPump( int nPump )
                //    int nTempLayerCount = 0;
                //    int nTempPumpCount = 0;
                //    int nTempGroup0Count = 0;
                //    int nTempGroup1Count = 0;
                //    int nTempLoaderCount = 0;
                //    int i in void CheckSystemConfiguration( void )

                //    int nTempLoaderCount = 0;    in void CheckLoaderConfiguration( void )
                //    int nTempPumpCount = 0;
                //    int nPumpNo,nPumpCount;
                //    Bool bFound;
                //    Bool bErrorFound = FALSE;
                //    union   CharsAndInt uValue;

//
// P.Smith                      17/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
// added read of nbb eeprom.
//
// P.Smith                      20/2/06
//#include "EEPROMdriver.h"
//#include "QSPIconstants.h"
//#include "NBBGpio.h"
// implement LoadAllCalibrationDataFromEEprom
// implement CalculateCheckSum
//
// P.Smith                      2/3/06
// if(nResult & BAD_EEPROM_DATA_MASK) set, set "save calibration" flag
// g_bSaveAllCalibrationToEEprom = TRUE;  // save calibration data
//
// P.Smith                      31/5/06
// read data from eeprom before writing, if not equal then program
// else exit the function.
// PutAlarmTable( CALINVALARM,  0 ); added
// RemoveAlarmTable( CALINVALARM,  0 );      /* indicate alarm cleared, RMALTB */
// #include "Alarms.h"
// remove variables producing warnings.
//
// P.Smith                      13/6/06
// implement ForegroundSaveRecipe function
// set g_nSaveRecipeSequence to zero to stop continuous saving of the recipe.
// call  EEPROMWrite1Byte to store recipe data to eeprom
// uncomment RemoveAlarmTable( CALINVALARM,  0 );      /* indicate alarm cleared, RMALTB */
//
// P.Smith                      19/6/06
// remove commented out code from loadrecipe(1)
//
// P.Smith                      21/6/06
// removed all unused code
// added CheckForValidCalibrationData
//
// P.Smith                      5/7/06
// add more checks for valid calibration data.
//
// P.Smith                      14/7/06
// modify the write and read of calibration data to set and clear g_bSPIEEActive
// check g_bSPIAtDActive for active when writing to eeprom.
//
// P.Smith                      4/12/06
// put in tests to determine where software is getting stuck in eeprom write.
// removed for now
//
// P.Smith                      16/1/07
// call SPI_Select_EEPROM & SetPD11_Hi SetPD11_Lo (write protect)
// Improve the diagnostics in the write of fram data to catch the software bug.
//
// P.Smith                      5/2/07
// added timeout on g_bSPIAtDActive loop
// addedEEPROM_Write_Protect & Set_EEPROM_Write_Protect
//
// P.Smith                      5/2/07
// Remove check for g_bSPIAtDActive in SaveAllCalibrationDataToEEprom, this was causing the program to
// stay in this loop occasionallly stopping the SEI communications from operating.
// more validity checks on calibration data
// change to ForegroundSaveRecipe, brackets to include g_nSaveRecipeByteNo
//
// P.Smith                      1/3/07
// Ensure that the   nResult is set to BAD_EEPROM_DATA_MASK if the data is not within
// the correct boundaries. check validity of g_CalibrationData.m_nLineSpeedSignalType
//
// P.Smith                      3/5/07
// allow for reading chip select from ram, this is to cater for rev b1,b2 boards
//
// P.Smith                      22/6/07
// name change to proper blender names.
//
// P.Smith                      1/8/07
// added check for purge component
//
// P.Smith                      2/8/07
// added check for current extruder no & minimum latency
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      16/10/07
// implement storage of calibration data to the non volatile storage device.
// SaveAllCalibrationDataToOnBoardEEprom
// Implement LoadAllCalibrationDataFromOnBoardEEprom
//
// P.Smith                      31/10/07
// change nChipSelectUsed to a word.
//
// P.Smith                      6/11/07
// added check for last units mode on reset, cleaning was originally only a BOOL.
// and was checked for TRUE or FALSE
// check volumetric mode for valid BOOL
// Remove incorrect check on g_CalibrationData.m_nControlMode
//
// P.Smith                      12/11/07
// added check for default line speed average.
//
// P.Smith                      20/11/07
// add default checks for all BOOL data
// add maximum check for latency value
// batches in kg/hr average, screw speed signal type,remote modules,
// offline type, material transport delay,post material transport delay.
// fast control response,standard component config
//
//
// P.Smith                      12/5/07
// Correct check for cleaning, checks for last clean, this was
// originally checking for enabled / disabled. The cleaning is made up of
// FULLCLEAN , MIXERCLEAN
// check valid data for g_CalibrationData.m_nDeviationAlarmEnable and g_CalibrationData.m_nPauseOnErrorNo
//
// P.Smith                          20/3/08
// use DEFAULT_BATCH_SIZE when incorrect batch size
//
// P.Smith                          26/3/08
// correct check on manager password, remove ref to g_nManagerPassword &  g_nOperatorPassword
//
// P.Smith                          27/3/08
// SaveAllProcessDataToEEprom & LoadAllProcessDataFromEEprom
//
// P.Smith                          29/4/08
// name change g_CalibrationData.m_nCurrentExtruderNo to g_nCurrentExtruderNo
//
// P.Smith                          2/5/08
// added check for g_CalibrationData.m_stMultiblendExtruders[j].m_bEnabled validity.
// there is a problem with the panel in that if the option is not set to 0 or 1
// then the box is not displayed on the panel, there fore the parameters must be checked
// for valid data on power up.
//
// P.Smith                          7/5/08
// Remove check on current extruder no. Correct compiler warnings.
//
// P.Smith                          17/6/08
// Generate a checksum for process data. Correct checksum generation in the
// function LoadAllCalibrationDataFromEEprom. The size was calculated incorrectly.
// set g_bCalibrationDataInvalid flag is calibration is invalid
// add in additional checks in CheckForValidCalibrationData on other calibration
// data.
// add checksum check in LoadAllProcessDataFromEEprom
//
// P.Smith                          27/6/08
// Correct limit check on g_CalibrationData.m_nControlAvgFactor if 0 or greater than
// max (was 3 now 8) set to default value (3)
//
// P.Smith                          24/6/08
// remove g_arrnMBTable
//
// P.Smith                          3/10/08
// added validity check on m_bLiquidEnabled
// offline mixing time, max throughput, fill retries, clean bypass time limit.
//
// P.Smith                         13/10/08
// check g_CalibrationData.m_fLiquidAdditiveScalingFactor for value > 2
//
// P.Smith                         2/12/08
// check g_ProcessData.m_fRPMPerKilo for negative value or greater than 9999
//
// P.Smith                         11/12/08
// added validity check on usb / printer data.
//
// P.Smith                         29/1/09
// added check ong_CalibrationData.m_bNoParity
//
// P.Smith                         6/2/09
// remove set of g_nHiLowOptionSwitchedOnOffHistory from here.
// added m_nRecipeEntryType check for a valid character
//
// P.Smith                         6/3/09
// added LoadAllDescriptionDataFromEEprom function to read the nbb serial no
// that is position at the beginning of eeprom DESCRIPTION_DATA_EEPROM
//
// P.Smith                         18/5/09
// validity check done on line speed constant.
//
// P.Smith                         29/5/09
// added validity check on m_nCheckForCorrectComponentTargets
//
// P.Smith                         22/7/09
// added SaveAdditionalDataToEEPROM
//
// P.Smith                         24/8/09
// added check for valid batch size.
// generate a specific alarm if the batch size is not correct.
// set the batch weight for the machine that is currently being used in CheckForValidBatchSize
// check for max, mins for batch sizes and set accordingly.
//
// P.Smith                         3/9/09
// modify LoadAllCalibrationDataFromEEprom to return true if checksum is okay
// and false if not.
// check returned value from EEPROMWrite1Byte, if not okay generate an eeprom alarm.
//
// P.Smith                         7/9/09
// generate eeprom write alarm for calibration and process data
//
// P.Smith                         9/9/09
// do not check the batch size if the blender type has been updated.
// use history flag to check for a change.
//
// P.Smith                         10/9/09
// Bool added to CheckForValidCalibrationData to make sure that the batch
// check happens on reset.if no change in blender type then do not do check
//
// P.Smith                         17/9/09
// added m_nLogToSDCard check for > 3, this is now an integer
//
// P.Smith                         19/11/09
// correct compiler warnings.


// M.McKiernan                      1/12/09
// Fixed two occurrences of       int     i,nBytes,;
//
// P.Smith                         15/1/10
// add validity checks for min open time check, flow rate check, vac8 expansion.
//
// P.Smith                         5/3/10
// added checks for Optimix 50 and 250, not in model set yet.
//
// P.Smith                         8/3/10
// in CheckForValidBatchSize if LICENSE_DO_BATCH_WEIGHT_CHECK allow batch
// check
//
// P.Smith                         18/3/10
// correct return in batch weight check, the value should be returned
// outside of the check, also default does nothing.
//
//
// P.Smith                         27/4/10
// added check for valid baud rate.
//
// P.Smith                         30/4/10
// modify check for baud rate remove 0 check.
//
// P.Smith                         27/10/11
// check for valid m_bPCF8563RTCFitted
//
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include <basictypes.h>

#include "ConfigureSystem.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "Foreground.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"

#include "TimeDate.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "CsumCalc.h"
#include "String.h"
#include "BatchMBIndices.h"
#include "Alarms.h"
#include "EEPROMdriver.h"
#include "QSPIconstants.h"
#include "NBBGpio.h"
#include "Lspeed.h"
#include "MBProgs.h"
#include "License.h"
#include "InitNBBComms.h"

// Structure loaded from the configuration pointing to
// each of the configured units
// 18.01.2005 structControllerData  *arrpDeviceList[MAX_COMMS_UNITS];

// Global variable used to indicate the current language selection
int     g_nLanguage = LANGUAGE_ENGLISH;

// Layer count
int     g_nLayerCount = 0;
int     g_nBlendersInGroup[2] = {0,0};
char        g_cGroupSelected = 0;

// Pump count
int     g_nPumpCount = 0;
int     g_nLoaderCount= 0;

//char      g_cTestSEEArray[50];    //mmk used for test purposes only

// Settings for the measurement units for each variable
structMeasurementUnits  g_MeasurementUnits;

// Externally defined variables
// Data
//bb extern structCommsData g_sModbusMaster;
extern  int     g_nGeneralTickTimer;
//extern    structRecipeData    g_CurrentRecipe;

extern  CalDataStruct   g_CalibrationData;
extern  OnBoardCalDataStruct   g_OnBoardCalData;


extern  char    *strInvalidCalibrationData[];
extern  char    *strRecipeWriteFailure[];
extern  char    *strSaveRecipeText[];
extern  char    *strSaveConfigurationText[];
extern  char    g_RTCbuffer[];
extern  structSetpointDataAdditionalData  g_TempRecipeAdditionalData;


extern  int g_nRecipeEntryMode;
extern  int g_nProtocol;


int g_nCommsStructureSize = 0;
char    g_cGroup1Description[6];
char    g_cGroup2Description[6];
char g_cProcessType;
int  g_nFilterCleanFrequency;
//structComponentData   g_CompDataTestOnly;

structSetpointData  g_CurrentRecipe;
structSetpointData  g_TempRecipe;



///////////////////////////////////////////////////////////
// Load the recipe file from EEPROM
//
// Entry:   Void
//
// Exit:    FALSE if no errors.
//          TRUE if invalid values detected.
//          Recipe file loaded into g_TempRecipe structure.
///////////////////////////////////////////////////////////


BOOL LoadRecipe( int nRecipeNo )
{
    BOOL    bBadEEPROMData = FALSE;
    char    *pPtr;
    int     i,nBytes,nMaxFiles,nChipSelectUsed;
    WORD nAddress;

    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    pPtr = (char*)&g_TempRecipe;

    nBytes = sizeof( g_TempRecipe );
    nMaxFiles = (EEPROM_SIZE_BYTES - EEPROM_RECIPE_0_START) / nBytes;       // Maximum no. of files that can be stored.
    if(nRecipeNo >= nMaxFiles)
        nRecipeNo = 0;                          // dont read beyond eeprom range, default to file 0.


    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
//    SetSPIEX1_Lo();
//    SetSPIEX2_Lo();
    SPI_Select_EEPROM();  // nbb--todocheck-- check that this is working.

    UnProtectEEPROM( nChipSelectUsed );    // enable writes to eeprom (all).

    for( i = 0; i < nBytes; i++ )
    {
//       *(pPtr + i) = ReadRemovableEEPROMData( EEPROM_RECIPE_0_START + (nRecipeNo * nBytes) + i );
       nAddress = EEPROM_RECIPE_0_START + (nRecipeNo * nBytes) + i;    // specify address
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, nAddress );
    }
    // Do some sanity checks on the data
    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.

    return bBadEEPROMData;
}



///////////////////////////////////////////////////////////
// Saves the Temp recipe structure to EEPROM while in ForeGround routine
// It saves one byte on each call(pass) of the routine.
//
// Entry:   The file no. is in the recipe file - this determines address in EEPROM.
//              g_nSaveRecipeSequence contains program sequence:
//                          0 => Finished or not saving.
//                          SAVE_RECIPE_START   => first time into routine, must initialise pointers
//                          SAVE_RECIPE_ONGOING => in the middle of saving.
//              g_nRecipeByteNo contains the next byte to be saved, initialised to 0 on first pass.
//
// Exit:    Void.
///////////////////////////////////////////////////////////
void ForegroundSaveRecipe( void )
{
    unsigned int     nRecipeNo,nMaxFiles;
    unsigned int     nBytes,nChipSelectUsed;
    char    *pPtr;
    char    cData = 0x00;

    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    if(g_nSaveRecipeSequence == SAVE_RECIPE_START)
        g_nSaveRecipeByteNo = 0;        // initialise to 1st byte

    nBytes = sizeof( g_TempRecipe );
    nMaxFiles = (EEPROM_SIZE_BYTES - EEPROM_RECIPE_0_START) / nBytes;       // Maximum no. of files that can be stored.
    nRecipeNo = g_TempRecipe.m_nFileNumber;
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();

    Remove_EEPROM_Write_Protect();
    if(nRecipeNo < nMaxFiles  && nBytes > 0)                                // only write if within eeprom range.
    {
        pPtr = (char*)&g_TempRecipe;
        cData = *(pPtr + g_nSaveRecipeByteNo);  // load byte to save

        if(g_nSaveRecipeSequence == SAVE_RECIPE_START)
        {
            EEPROMWrite1Byte(nChipSelectUsed,  cData, EEPROM_RECIPE_0_START + (nRecipeNo * nBytes) + g_nSaveRecipeByteNo );      //
            g_nSaveRecipeByteNo++;
            if(g_nSaveRecipeByteNo < nBytes)
                g_nSaveRecipeSequence = SAVE_RECIPE_ONGOING;
            else
                g_nSaveRecipeByteNo = 0;        // signal finished.
        }
        else if(g_nSaveRecipeSequence == SAVE_RECIPE_ONGOING)
        {
            if(g_nSaveRecipeByteNo < nBytes)        // put in check in case byteno gets corrupted.
                 {
                 EEPROMWrite1Byte(nChipSelectUsed,  cData, EEPROM_RECIPE_0_START + (nRecipeNo * nBytes) + g_nSaveRecipeByteNo );      //
                 g_nSaveRecipeByteNo++;
                 }
            if(g_nSaveRecipeByteNo < nBytes)
                g_nSaveRecipeSequence = SAVE_RECIPE_ONGOING;
            else
                {
                    g_nSaveRecipeByteNo = 0;        // signal finished.
                    g_nSaveRecipeSequence = 0;  // signal end of save
                }
         }
        else
        {
            g_nSaveRecipeSequence = 0;  // signal end of save

        }

    }
    else // file error, quit.
    {
        g_nSaveRecipeSequence = 0;      // signal end of save
    }
    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.
    Set_EEPROM_Write_Protect();
}

///////////////////////////////////////////////////////////
// Saves the Calibration Data structure to EEPROM
//
// Entry:   none
//
// Exit:    Void.
///////////////////////////////////////////////////////////
void SaveAllCalibrationDataToEEprom( void )
{
    int     i,nBytes,nSize;
    WORD  nChipSelectUsed;
    char    *pPtr;
    BOOL    bOkay = FALSE;
    BOOL    bEEPROMCalibrationWriteError = FALSE;

    unsigned char    g_arrcTemp[sizeof(g_CalibrationData)];
    unsigned char   cData;

//    g_nSPIEEActiveTimeoutCounter = SPIEEACTIVETIMEOUT;
//    iprintf("\n No.1 g_bSPIAtDActive is  %d g_bSPIEEActive is %d pits are %d counter is %d",g_bSPIAtDActive,g_bSPIEEActive,g_nPIT_Int_CounterU,g_nSPIEEActiveTimeoutCounter); //nbb--testonly--
//    g_nloopctr = 0;
//    while(g_bSPIAtDActive && (g_nSPIEEActiveTimeoutCounter != 0) )
//    {
//        g_nloopctr++;
//    }
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
//    iprintf("\n No.4 g_bSPIAtDActive is  %d g_bSPIEEActive is %d pits are %d",g_bSPIAtDActive,g_bSPIEEActive,g_nPIT_Int_CounterU); //nbb--testonly--

    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CALINVALARMBIT) ) /* tare alarm bit set???  */
    {
        RemoveAlarmTable( CALINVALARM,  0 );      /* indicate alarm cleared, RMALTB */
    }

    pPtr = (char*)&g_CalibrationData;
    nSize = sizeof(g_CalibrationData)- sizeof(g_CalibrationData.m_nChecksum);
    // merge equivalent of CALCULATEEEPROMCHECKSUM inline
    memcpy(&g_arrcTemp, &g_CalibrationData,nSize );    // copy structure to array.

    g_CalibrationData.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);
    // Write out the data
    nBytes = sizeof( g_CalibrationData );

    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    Remove_EEPROM_Write_Protect();
    UnProtectEEPROM( nChipSelectUsed );    //
//    iprintf("\n No.5 g_bSPIAtDActive is  %d g_bSPIEEActive is %d pits are %d",g_bSPIAtDActive,g_bSPIEEActive,g_nPIT_Int_CounterU);  //nbb--testonly--

    for( i = 0; i < nBytes ; i++ )
      {
          cData = *(pPtr + i);
          bOkay = EEPROMWrite1Byte(nChipSelectUsed,  cData, EEPROM_CALIBRATION_DATA_START + i);      //
          if(!bOkay)
          {
              bEEPROMCalibrationWriteError = TRUE;
          }
      }
    g_bSPIEEActive = FALSE;
    if(bEEPROMCalibrationWriteError)
    {
        PutAlarmTable( CALIBRATIONWRITEALARM,  0 );
    }
    else
    {
       RemoveAlarmTable( CALIBRATIONWRITEALARM,  0 );
    }
     Set_EEPROM_Write_Protect();
}


///////////////////////////////////////////////////////////
// Saves the Calibration Data structure to on board EEPROM ASM = SAVECAL2DATA
//
// Entry:   none
//
// Exit:    Void.
///////////////////////////////////////////////////////////

void SaveAllCalibrationDataToOnBoardEEprom( void )
{
    int     i,nBytes,nChipSelectUsed;
    char    *pPtr;
    unsigned char   cData;

    g_bSPIEEActive = TRUE;                // indicate eeprom is active.

    pPtr = (char*)&g_OnBoardCalData;

    // Write out the data
    nBytes = sizeof( g_OnBoardCalData );
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    Remove_EEPROM_Write_Protect();
    UnProtectEEPROM( nChipSelectUsed );    //

    for( i = 0; i < nBytes ; i++ )
    {
        cData = *(pPtr + i);
        EEPROMWrite1Byte(nChipSelectUsed,  cData, ONBOARD_EEPROM_CALIBRATION_DATA_START + i);      //
    }
    g_bSPIEEActive = FALSE;
    Set_EEPROM_Write_Protect();
}




///////////////////////////////////////////////////////////
// SaveAdditionalDataToEEPROM
//
///////////////////////////////////////////////////////////

void SaveAdditionalDataToEEPROM( void )
{
    int     i,nBytes,nChipSelectUsed;
    char    *pPtr;
    unsigned char   cData;

    g_bSPIEEActive = TRUE;                // indicate eeprom is active.

    pPtr = (char*)&g_TempRecipeAdditionalData;

    // Write out the data
    nBytes = sizeof( g_TempRecipeAdditionalData );
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    Remove_EEPROM_Write_Protect();
    UnProtectEEPROM( nChipSelectUsed );    //

    for( i = 0; i < nBytes ; i++ )
    {
        cData = *(pPtr + i);
        EEPROMWrite1Byte(nChipSelectUsed,  cData, ONBOARD_EEPROM_ADDITIONAL_RECIPE_DATA_START + i);      //
    }
    g_bSPIEEActive = FALSE;
    Set_EEPROM_Write_Protect();
}


///////////////////////////////////////////////////////////
// LoadAdditionalDataToEEPROM
//
///////////////////////////////////////////////////////////
int LoadAdditionalDataFromEEPROM( void )
{
    int nChipSelectUsed;

    int     i,nBytes;
    char    *pPtr;
    pPtr = (char*)&g_TempRecipeAdditionalData;
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    UnProtectEEPROM( nChipSelectUsed );    // enable writes to eeprom (all).

    // Write out the data
    nBytes = sizeof( g_TempRecipeAdditionalData );

    for( i = 0; i < nBytes; i++ )
    {
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, ONBOARD_EEPROM_ADDITIONAL_RECIPE_DATA_START + i );
    }

    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.
    return( 0 );
}





///////////////////////////////////////////////////////////
// SaveAllProcessDataToEEprom
// save all process data to EEPROM
///////////////////////////////////////////////////////////

void SaveAllProcessDataToEEprom( void )
{
    int     i,nBytes,nChipSelectUsed,nSize;
    unsigned char    g_arrcTemp[sizeof(g_ProcessData)];
    char    *pPtr;
    unsigned char   cData;
    BOOL    bOkay = FALSE;
    BOOL    bEEPROMCalibrationWriteError = FALSE;

    g_bSPIEEActive = TRUE;                // indicate eeprom is active.

    pPtr = (char*)&g_ProcessData;

    // Write out the data
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    Remove_EEPROM_Write_Protect();
    UnProtectEEPROM( nChipSelectUsed );    //

    nSize = sizeof(g_ProcessData)- sizeof(g_ProcessData.m_nChecksum);
    // merge equivalent of CALCULATEEEPROMCHECKSUM inline
    memcpy(&g_arrcTemp, &g_ProcessData,nSize );    // copy structure to array.
    g_ProcessData.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);

    nBytes = sizeof( g_ProcessData );

    for( i = 0; i < nBytes ; i++ )
    {
        cData = *(pPtr + i);
        bOkay = EEPROMWrite1Byte(nChipSelectUsed,  cData, PROCESS_DATA_EEPROM_CALIBRATION_DATA + i);      //
        if(!bOkay)
        {
            bEEPROMCalibrationWriteError = TRUE;
        }

    }
    if(bEEPROMCalibrationWriteError)
    {
        PutAlarmTable( PROCESSDATAWRITEALARM,  0 );
    }
    else
    {
       RemoveAlarmTable( PROCESSDATAWRITEALARM,  0 );
    }


    g_bSPIEEActive = FALSE;
    Set_EEPROM_Write_Protect();
}



///////////////////////////////////////////////////////////
// Loads the Calibration Data structure from EEPROM   asm = LODCEE
//
// Entry:   none
//
// Exit:    Void.
///////////////////////////////////////////////////////////
BOOL LoadAllCalibrationDataFromEEprom( void )
{
    BOOL bOkay = FALSE;
    int nChipSelectUsed,nSize;
    unsigned char    g_arrcTemp[sizeof(g_CalibrationData)];

    int     i,nBytes;
    char    *pPtr;
    pPtr = (char*)&g_CalibrationData;
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
//    SetSPIEX1_Lo();
//    SetSPIEX2_Lo();
    SPI_Select_EEPROM();
    UnProtectEEPROM( nChipSelectUsed );    // enable writes to eeprom (all).

    // Write out the data
    nBytes = sizeof( g_CalibrationData );

    for( i = 0; i < nBytes; i++ )
    {
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, EEPROM_CALIBRATION_DATA_START + i );
    }

    memcpy(&g_arrcTemp, &g_CalibrationData, sizeof(g_CalibrationData));    // copy structure to array.
    nSize = sizeof(g_CalibrationData)- sizeof(g_CalibrationData.m_nChecksum);
    g_unCalibrationCheckSum = CalculateCheckSum(g_arrcTemp, nSize);

    if(g_unCalibrationCheckSum != g_CalibrationData.m_nChecksum)
    {
        bOkay = FALSE;
        g_bCalibrationDataInvalid = TRUE;
    }
    else
    {
        bOkay = TRUE;
        g_bCalibrationDataInvalid = FALSE;
    }

    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.
    return( bOkay );
}




///////////////////////////////////////////////////////////
// CheckForValidCalibrationData()     asm CHECKCD
// Loads the Calibration Data structure from OnBoard EEPROM
//
// Entry:   none
//
// Exit:    Void.
///////////////////////////////////////////////////////////

int    CheckForValidCalibrationData(BOOL bCheckBatchSize)
{
    int nResult = 0;
    int    i;
    unsigned int nLength;
    g_nProtocol = PROTOCOL_MODBUS;  // Hardcode for modbus protocol.
    // Do some sanity checks on the data
    if((g_CalibrationData.m_nComponents > MAX_COMPONENTS) || (g_CalibrationData.m_nComponents == 0x0))
    {
       g_CalibrationData.m_nComponents = 1;
       nResult |= BAD_EEPROM_DATA_MASK;
    }

     nLength = strlen( g_CalibrationData.m_cEmailMailServer );
    if(nLength > EMAIL_STRING_SIZE)
    {
        g_CalibrationData.m_cEmailMailServer[0] = ' ';
        g_CalibrationData.m_cEmailMailServer[1] = ASCII_NULL;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    nLength = strlen( g_CalibrationData.m_cEmailUserName );
    if(nLength > EMAIL_STRING_SIZE)
    {
        g_CalibrationData.m_cEmailUserName[0] = ' ';
        g_CalibrationData.m_cEmailUserName[1] = ASCII_NULL;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    nLength = strlen( g_CalibrationData.m_cEmailPassword );
    if(nLength > EMAIL_STRING_SIZE)
    {
        g_CalibrationData.m_cEmailPassword[0] = ' ';
        g_CalibrationData.m_cEmailPassword[1] = ASCII_NULL;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    nLength = strlen( g_CalibrationData.m_cEmailSupervisorEmailAddress );
    if(nLength > EMAIL_STRING_SIZE)
    {
        g_CalibrationData.m_cEmailSupervisorEmailAddress[0] = ' ';
        g_CalibrationData.m_cEmailSupervisorEmailAddress[1] = ASCII_NULL;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    nLength = strlen( g_CalibrationData.m_cEmailSupportEmailAddress );
    if(nLength > EMAIL_STRING_SIZE)
    {
        g_CalibrationData.m_cEmailSupportEmailAddress[0] = ' ';
        g_CalibrationData.m_cEmailSupportEmailAddress[1] = ASCII_NULL;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    nLength = strlen( g_CalibrationData.m_cEmailFromEmailAddress );
    if(nLength > EMAIL_STRING_SIZE)
    {
        g_CalibrationData.m_cEmailFromEmailAddress[0] = ' ';
        g_CalibrationData.m_cEmailFromEmailAddress[1] = ASCII_NULL;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    nLength = strlen( g_CalibrationData.m_cEmailBlenderName );
    if(nLength > EMAIL_STRING_SIZE)
    {
        g_CalibrationData.m_cEmailBlenderName[0] = ' ';
        g_CalibrationData.m_cEmailBlenderName[1] = ASCII_NULL;
        nResult |= BAD_EEPROM_DATA_MASK;
    }


    if( g_CalibrationData.m_lManagerPassWord < 0 || g_CalibrationData.m_lManagerPassWord > 9999 )
    {
        g_CalibrationData.m_lManagerPassWord = 1;
        nResult |= BAD_EEPROM_DATA_MASK;

    }
    if( g_CalibrationData.m_lOperatorPassWord < 0 || g_CalibrationData.m_lOperatorPassWord > 9999 )
    {
        g_CalibrationData.m_lOperatorPassWord = 1;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    if((g_CalibrationData.m_nBlenderType == g_nBlenderTypeHistory)|| bCheckBatchSize)
    {
        BOOL bOkay = CheckForValidBatchSize();
        if(!bOkay)
        {
            nResult |= BAD_EEPROM_DATA_MASK;
        }
    }
    g_nBlenderTypeHistory = g_CalibrationData.m_nBlenderType;
    if((g_CalibrationData.m_nControlAvgFactor == 0) || (g_CalibrationData.m_nControlAvgFactor > MAX_LIMIT_CONTROL_AVERAGE_FACTOR))
    {
        g_CalibrationData.m_nControlAvgFactor = DEFAULT_LIMIT_CONTROL_AVERAGE_FACTOR;
        nResult |= BAD_EEPROM_DATA_MASK;
     }
    if(g_CalibrationData.m_nDiagnosticsMode > LAST_DIAGNOSTICS_MODE)
    {
        g_CalibrationData.m_nDiagnosticsMode = 0;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if(g_CalibrationData.m_nBlenderMode > LASTBLENDERMODE)
    {
        g_CalibrationData.m_nBlenderMode = MODE_BLENDINGONLY;
         nResult |= BAD_EEPROM_DATA_MASK;

    }
    if((g_CalibrationData.m_fControlGain < MIN_LIMIT_CONTROL_GAIN) ||(g_CalibrationData.m_fControlGain < MAX_LIMIT_CONTROL_GAIN))
    {
    }
    if(g_CalibrationData.m_nBlenderType > LASTBLENDERTYPE)
    {
        g_CalibrationData.m_nBlenderType = TSM1000;
        nResult |= BAD_EEPROM_DATA_MASK;

    }
    if(g_CalibrationData.m_nControlType > LASTCONTROLTYPE)
    {
         g_CalibrationData.m_nControlType = CONTROL_TYPE_0_10;
         nResult |= BAD_EEPROM_DATA_MASK;

    }
   if(g_CalibrationData.m_nControlMode > LASTCONTROLMODE)
   {
         g_CalibrationData.m_nControlMode = CONTROL_MODE_EXTRUSION;
          nResult |= BAD_EEPROM_DATA_MASK;
 }
   if(g_CalibrationData.m_nStageFillEn > LASTSTAGEFILL)
   {
      g_CalibrationData.m_nStageFillEn = 0;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

   if(g_CalibrationData.m_nUnitType > LAST_UNITS_MODE )
   {
      g_CalibrationData.m_nUnitType = UNIT_METRIC;
      nResult |= BAD_EEPROM_DATA_MASK;
    }
   if((g_CalibrationData.m_nPhase1OnTime <  MIN_LIMIT_PHASE1_CLEAN_ON_TIME) ||(g_CalibrationData.m_nPhase1OffTime >  MAX_LIMIT_PHASE1_CLEAN_ON_TIME))
   {
      g_CalibrationData.m_nPhase1OnTime = NOMINAL_PHASE1_CLEAN_ON_TIME;
      nResult |= BAD_EEPROM_DATA_MASK;
    }
   if((g_CalibrationData.m_nPhase1OffTime <  MIN_LIMIT_PHASE1_CLEAN_OFF_TIME) || (g_CalibrationData.m_nPhase1OffTime >  MAX_LIMIT_PHASE1_CLEAN_OFF_TIME))
   {
      g_CalibrationData.m_nPhase1OffTime = NOMINAL_PHASE1_CLEAN_OFF_TIME;
      nResult |= BAD_EEPROM_DATA_MASK;
    }
   if((g_CalibrationData.m_nPhase2OnTime <  MIN_LIMIT_PHASE2_CLEAN_ON_TIME) || (g_CalibrationData.m_nPhase2OffTime >  MAX_LIMIT_PHASE2_CLEAN_ON_TIME))
   {
      g_CalibrationData.m_nPhase2OnTime = NOMINAL_PHASE2_CLEAN_ON_TIME;
      nResult |= BAD_EEPROM_DATA_MASK;
   }
   if((g_CalibrationData.m_nPhase2OffTime <  MIN_LIMIT_PHASE2_CLEAN_OFF_TIME) || (g_CalibrationData.m_nPhase2OffTime >  MAX_LIMIT_PHASE2_CLEAN_OFF_TIME))
   {
      g_CalibrationData.m_nPhase2OffTime = NOMINAL_PHASE2_CLEAN_OFF_TIME;
      nResult |= BAD_EEPROM_DATA_MASK;
   }
// blender clean repetitions
   if((g_CalibrationData.m_nCleanRepetitions <  MIN_LIMIT_CLEAN_REPETITIONS) || (g_CalibrationData.m_nCleanRepetitions >  MAX_LIMIT_CLEAN_REPETITIONS))
   {
      g_CalibrationData.m_nCleanRepetitions = NOMINAL_CLEAN_REPETITIONS;
      nResult |= BAD_EEPROM_DATA_MASK;
   }

// mixing clean repetitions
   if((g_CalibrationData.m_nMixerCleanRepetitions <  MIN_LIMIT_MIXER_CLEAN_REPETITIONS) || (g_CalibrationData.m_nMixerCleanRepetitions >  MAX_LIMIT_MIXER_CLEAN_REPETITIONS))
   {
      g_CalibrationData.m_nMixerCleanRepetitions = NOMINAL_MIXER_CLEAN_REPETITIONS;
      nResult |= BAD_EEPROM_DATA_MASK;
   }

// mixing motor shut off time
   if((g_CalibrationData.m_nMixShutoffTime < MIN_LIMIT_SHUTOFFTIME) || (g_CalibrationData.m_nMixShutoffTime > MAX_LIMIT_SHUTOFFTIME))
   {
      g_CalibrationData.m_nMixShutoffTime = NOMINAL_SHUTOFFTIME;
      nResult |= BAD_EEPROM_DATA_MASK;
   }

// intermittent mixing time
   if((g_CalibrationData.m_nIntermittentMixTime < MIN_LIMIT_INTERMITTENT_MIX_TIME) || (g_CalibrationData.m_nMixShutoffTime > MAX_LIMIT_INTERMITTENT_MIX_TIME))
   {
      g_CalibrationData.m_nIntermittentMixTime = INTERMITTENT_MIX_TIME_MIX_ALWAYS;
      nResult |= BAD_EEPROM_DATA_MASK;
   }

 //control average
    if((g_CalibrationData.m_nControlAvgFactor == 0) || (g_CalibrationData.m_nControlAvgFactor > MAX_LIMIT_CONTROL_AVERAGE_FACTOR))
    {
      g_CalibrationData.m_nControlAvgFactor = MAX_LIMIT_CONTROL_AVERAGE_FACTOR;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// weight averaging factor
    if((g_CalibrationData.m_nWeightAvgFactor == 0) || (g_CalibrationData.m_nWeightAvgFactor > MAX_LIMIT_WEIGHT_AVERAGE_FACTOR))
    {
      g_CalibrationData.m_nWeightAvgFactor = MAX_LIMIT_WEIGHT_AVERAGE_FACTOR;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// line speed signal type
    if(g_CalibrationData.m_nLineSpeedSignalType > LASTLINESPEEDSIGNALTYPE)
    {
      g_CalibrationData.m_nLineSpeedSignalType = PULSES_LINE_SPEED;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// purge enable
    if(g_CalibrationData.m_nPurgeEnable > LASTPURGEMODE)
    {
        g_CalibrationData.m_nPurgeEnable = NO_PURGE;
         nResult |= BAD_EEPROM_DATA_MASK;

    }

// cleaning
    if(g_CalibrationData.m_nCleaning > LASTCLEAN)
    {
      g_CalibrationData.m_nCleaning = 0;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

    if(g_CalibrationData.m_nDeviationAlarmEnable > LASTCOMPONENTPERRNABLE)
    {
      g_CalibrationData.m_nDeviationAlarmEnable = 0;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

    if((g_CalibrationData.m_nRecipeEntryType > 3) || (g_CalibrationData.m_nRecipeEntryType < 0))
    {
      g_CalibrationData.m_nRecipeEntryType = 0;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

    if((g_CalibrationData.m_nPauseOnErrorNo == 0) || (g_CalibrationData.m_nPauseOnErrorNo > MAX_PAUSE_ON_ERROR_NO ))
    {
//     SetupMttty();
//     iprintf("\m er no is %d",g_CalibrationData.m_nPauseOnErrorNo);
      g_CalibrationData.m_nPauseOnErrorNo = DEFAULT_PAUSE_ON_ERROR_NO;
      nResult |= BAD_EEPROM_DATA_MASK;
    }





// latency
    if((g_CalibrationData.m_fLatency < MINIMUMLATENCY) || (g_CalibrationData.m_fLatency > MAXIMUMLATENCY) )
    {
        g_CalibrationData.m_fLatency = NOMINAL_LATENCY;
         nResult |= BAD_EEPROM_DATA_MASK;
    }

// line speed constant
    if((g_CalibrationData.m_fLineSpeedConstant < MINIMUM_LINESPEED_CONSTANT)||(g_CalibrationData.m_fLineSpeedConstant > MAXIMUM_LINESPEED_CONSTANT))
    {
        g_CalibrationData.m_fLineSpeedConstant = NOMINAL_LINESPEED_CONSTANT;
         nResult |= BAD_EEPROM_DATA_MASK;
    }




// volumetric mode
    if((g_CalibrationData.m_bVolumetricModeFlag != TRUE) && (g_CalibrationData.m_bVolumetricModeFlag != FALSE))
    {
        g_CalibrationData.m_bVolumetricModeFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// line speed average
    if((g_CalibrationData.m_nLineSpeedAverage == 0) || (g_CalibrationData.m_nLineSpeedAverage > MAX_LINE_SPEED_AVERAGE))
    {
      g_CalibrationData.m_nLineSpeedAverage = DEFAULT_LINE_SPEED_AVERAGE;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// batches in kg/hr average
    if((g_CalibrationData.m_nBatchesInKghAvg == 0) || (g_CalibrationData.m_nBatchesInKghAvg > MAX_BATCHES_IN_KGHR_AVERAGE))
    {
      g_CalibrationData.m_nBatchesInKghAvg = DEFAULT_BATCHES_IN_KGHR_AVERAGE;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// screw speed signal type
    if(g_CalibrationData.m_nScewSpeedSignalType > LAST_SCREW_SPEED_SIGNAL_TYPE)
    {
      g_CalibrationData.m_nScewSpeedSignalType = SCEW_SPEED_SIGNAL_TACHO;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// remote modules
    if(g_CalibrationData.m_nPeripheralCardsPresent > MAX_PERIPHERAL_CARD_SETTING)
    {
      g_CalibrationData.m_nPeripheralCardsPresent = 0;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// offline type
    if(g_CalibrationData.m_nOfflineType > LAST_OFFLINE_TYPE)
    {
      g_CalibrationData.m_nOfflineType = OFFLINETYPE_HILO;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// offline mixing time
    if((g_CalibrationData.m_nMixingTime == 0) || (g_CalibrationData.m_nMixingTime > MAX_OFFLINE_MIXING_TIME))
    {
      g_CalibrationData.m_nMixingTime = DEFAULT_OFFLINE_MIXING_TIME;
      nResult |= BAD_EEPROM_DATA_MASK;
    }


// max throughput
    if( (g_CalibrationData.m_nMaxThroughput == 0) || (g_CalibrationData.m_nMaxThroughput > MAX_MAX_THROUGHPUT))
    {
      g_CalibrationData.m_nMaxThroughput = DEFAULT_MAX_THROUGHPUT;
      nResult |= BAD_EEPROM_DATA_MASK;
    }
// fill retry
    if(g_CalibrationData.m_nFillRetries > MAX_FILL_RETRIES)
    {
      g_CalibrationData.m_nFillRetries = DEFAULT_FILL_RETRIES;
      nResult |= BAD_EEPROM_DATA_MASK;
    }


// material transport delay.
    if( g_CalibrationData.m_nMaterialTransportDelay == 0 )
    {
      g_CalibrationData.m_nMaterialTransportDelay = DEFAULT_MATERIAL_TRANSPORT_DELAY;
      nResult |= BAD_EEPROM_DATA_MASK;
    }
// post material transport delay.
    if( g_CalibrationData.m_nPostMaterialTransportDelay == 0 )
    {
      g_CalibrationData.m_nPostMaterialTransportDelay = DEFAULT_POST_MATERIAL_TRANSPORT_DELAY;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// Bools
// fast control response
    if((g_CalibrationData.m_bFastControlResponse != TRUE) && (g_CalibrationData.m_bFastControlResponse != FALSE))
    {
        g_CalibrationData.m_bFastControlResponse = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// standard component config
    if((g_CalibrationData.m_bStandardCompConfig != TRUE) && (g_CalibrationData.m_bStandardCompConfig != FALSE))
    {
        g_CalibrationData.m_bStandardCompConfig = TRUE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
// high low regrind.
    if((g_CalibrationData.m_bHighLoRegrindOption != TRUE) && (g_CalibrationData.m_bHighLoRegrindOption != FALSE))
    {
        g_CalibrationData.m_bHighLoRegrindOption = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// first component compensation
    if((g_CalibrationData.m_bFirstComponentCompensation != TRUE) && (g_CalibrationData.m_bFirstComponentCompensation != FALSE))
    {
        g_CalibrationData.m_bFirstComponentCompensation = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
// correct every cycle.
    if((g_CalibrationData.m_bCorrectEveryNCycles != TRUE) && (g_CalibrationData.m_bCorrectEveryNCycles != FALSE))
    {
        g_CalibrationData.m_bCorrectEveryNCycles = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// layering
    if((g_CalibrationData.m_bLayering != TRUE) && (g_CalibrationData.m_bLayering != FALSE))
    {
        g_CalibrationData.m_bLayering = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// mixing screw detection.
    if((g_CalibrationData.m_bMixerScrewDetect != TRUE) && (g_CalibrationData.m_bMixerScrewDetect != FALSE))
    {
        g_CalibrationData.m_bMixerScrewDetect = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// double dump
   if((g_CalibrationData.m_bDoubleDump != TRUE) && (g_CalibrationData.m_bDoubleDump != FALSE))
    {
        g_CalibrationData.m_bDoubleDump = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// moulding
   if((g_CalibrationData.m_bMouldingOption != TRUE) && (g_CalibrationData.m_bMouldingOption != FALSE))
    {
        g_CalibrationData.m_bMouldingOption = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
// regrind retry
   if((g_CalibrationData.m_bRegrindRetry != TRUE) && (g_CalibrationData.m_bRegrindRetry != FALSE))
    {
        g_CalibrationData.m_bRegrindRetry = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// bypass
   if((g_CalibrationData.m_bBypassMode != TRUE) && (g_CalibrationData.m_bBypassMode != FALSE))
    {
        g_CalibrationData.m_bBypassMode = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// manual fill sort (sensitivity)
   if((g_CalibrationData.m_bFillOrderManualSelect != TRUE) && (g_CalibrationData.m_bFillOrderManualSelect != FALSE))
    {
        g_CalibrationData.m_bFillOrderManualSelect = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// top up
   if((g_CalibrationData.m_bTopUpMode != TRUE) && (g_CalibrationData.m_bTopUpMode != FALSE))
    {
        g_CalibrationData.m_bTopUpMode = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// blow every cycle.
   if((g_CalibrationData.m_bBlowEveryCycle != TRUE) && (g_CalibrationData.m_bBlowEveryCycle != FALSE))
    {
        g_CalibrationData.m_bBlowEveryCycle = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// single word write
   if((g_CalibrationData.m_bSingleWordWrite != TRUE) && (g_CalibrationData.m_bSingleWordWrite != FALSE))
    {
        g_CalibrationData.m_bSingleWordWrite = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
// email on order change
    if((g_CalibrationData.m_bSendEmailOnOrderChange != TRUE) && (g_CalibrationData.m_bSendEmailOnOrderChange != FALSE))
    {
        g_CalibrationData.m_bSendEmailOnOrderChange = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// optimisation
    if((g_CalibrationData.m_bOptimisation != TRUE) && (g_CalibrationData.m_bOptimisation != FALSE))
    {
        g_CalibrationData.m_bOptimisation = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// component naming
    if((g_CalibrationData.m_bComponentNamingAlphanumeric != TRUE) && (g_CalibrationData.m_bComponentNamingAlphanumeric != FALSE))
    {
        g_CalibrationData.m_bComponentNamingAlphanumeric = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// pause on power up

    if((g_CalibrationData.m_bPauseBlenderOnPowerup != TRUE) && (g_CalibrationData.m_bPauseBlenderOnPowerup != FALSE))
    {
        g_CalibrationData.m_bPauseBlenderOnPowerup = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// log to sd card

    if(g_CalibrationData.m_nLogToSDCard > 3)
    {
        g_CalibrationData.m_nLogToSDCard = 0;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// log format
    if((g_CalibrationData.m_bLogFormatIsCSV != TRUE) && (g_CalibrationData.m_bLogFormatIsCSV != FALSE))
    {
        g_CalibrationData.m_bLogFormatIsCSV = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
// allow continuous download
    if((g_CalibrationData.m_bAllowForContinuousDownload != TRUE) && (g_CalibrationData.m_bAllowForContinuousDownload != FALSE))
    {
        g_CalibrationData.m_bAllowForContinuousDownload = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// Telnet
    if((g_CalibrationData.m_bTelnet != TRUE) && (g_CalibrationData.m_bTelnet != FALSE))
    {
        g_CalibrationData.m_bTelnet = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
// liquid additive
    if((g_CalibrationData.m_bLiquidEnabled != TRUE) && (g_CalibrationData.m_bLiquidEnabled != FALSE))
    {
        g_CalibrationData.m_bLiquidEnabled = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
//
    if((g_CalibrationData.m_bAlarmDumpFlag != TRUE) && (g_CalibrationData.m_bAlarmDumpFlag != FALSE))
    {
        g_CalibrationData.m_bAlarmDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if((g_CalibrationData.m_bOrderReportDumpFlag != TRUE) && (g_CalibrationData.m_bOrderReportDumpFlag != FALSE))
    {
        g_CalibrationData.m_bOrderReportDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if((g_CalibrationData.m_bRollReportDumpFlag != TRUE) && (g_CalibrationData.m_bRollReportDumpFlag != FALSE))
    {
        g_CalibrationData.m_bRollReportDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if((g_CalibrationData.m_bHourlyDumpFlag != TRUE) && (g_CalibrationData.m_bHourlyDumpFlag != FALSE))
    {
        g_CalibrationData.m_bHourlyDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if((g_CalibrationData.m_bBatchLogDumpFlag != TRUE) && (g_CalibrationData.m_bBatchLogDumpFlag != FALSE))
    {
        g_CalibrationData.m_bBatchLogDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if((g_CalibrationData.m_bEventLogDumpFlag != TRUE) && (g_CalibrationData.m_bEventLogDumpFlag != FALSE))
    {
        g_CalibrationData.m_bEventLogDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if((g_CalibrationData.m_bHistoryLogDumpFlag != TRUE) && (g_CalibrationData.m_bHistoryLogDumpFlag != FALSE))
    {
        g_CalibrationData.m_bHistoryLogDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
    if((g_CalibrationData.m_bMonthlyDumpFlag != TRUE) && (g_CalibrationData.m_bMonthlyDumpFlag != FALSE))
    {
        g_CalibrationData.m_bMonthlyDumpFlag = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

// network parity
    if((g_CalibrationData.m_bNoParity != TRUE) && (g_CalibrationData.m_bNoParity != FALSE))
    {
        g_CalibrationData.m_bNoParity = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }
//

    if((g_CalibrationData.m_bCheckForValidComponentTargetPercentage != TRUE) && (g_CalibrationData.m_bCheckForValidComponentTargetPercentage != FALSE))
    {
        g_CalibrationData.m_bCheckForValidComponentTargetPercentage = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    if((g_CalibrationData.m_bMinimumOpeningTimeCheck != TRUE) && (g_CalibrationData.m_bMinimumOpeningTimeCheck != FALSE))
    {
        g_CalibrationData.m_bMinimumOpeningTimeCheck = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

    if((g_CalibrationData.m_bFlowRateCheck != TRUE) && (g_CalibrationData.m_bFlowRateCheck != FALSE))
    {
        g_CalibrationData.m_bFlowRateCheck = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

   if((g_CalibrationData.m_bVac8Expansion != TRUE) && (g_CalibrationData.m_bVac8Expansion != FALSE))
    {
        g_CalibrationData.m_bVac8Expansion = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

   // rtc fitted
   if((g_CalibrationData.m_bPCF8563RTCFitted != TRUE) && (g_CalibrationData.m_bPCF8563RTCFitted != FALSE))
    {
	   g_CalibrationData.m_bPCF8563RTCFitted = FALSE;
        nResult |= BAD_EEPROM_DATA_MASK;
    }



    if((g_CalibrationData.m_fLiquidAdditiveScalingFactor < 0.001)||(g_CalibrationData.m_fLiquidAdditiveScalingFactor > 1.5f))
    {
        g_CalibrationData.m_fLiquidAdditiveScalingFactor = 1.000f;
        nResult |= BAD_EEPROM_DATA_MASK;
    }

//  bypass time limit
    if( (g_CalibrationData.m_nBypassTimeLimit == 0) || (g_CalibrationData.m_nBypassTimeLimit > MAX_BYPASS_TIME_LIMIT))
    {
      g_CalibrationData.m_nBypassTimeLimit = DEFAULT_BYPASS_TIME_LIMIT;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

    if((g_ProcessData.m_fRPMPerKilo < 0.0)||(g_ProcessData.m_fRPMPerKilo > 9999))
    {
        g_ProcessData.m_fRPMPerKilo = 0.0f;
    }

// clean bypass time limit
    if( (g_CalibrationData.m_nBypassTimeLimitClean == 0) || (g_CalibrationData.m_nBypassTimeLimitClean > MAX_CLEAN_BYPASS_TIME_LIMIT))
    {
      g_CalibrationData.m_nBypassTimeLimitClean = DEFAULT_CLEAN_BYPASS_TIME_LIMIT;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

// dump device type
    if(g_CalibrationData.m_nDumpDeviceType > LAST_DUMP_DEVICE_TYPE)
    {
      g_CalibrationData.m_nDumpDeviceType = 0;
      nResult |= BAD_EEPROM_DATA_MASK;
    }

       for(i=0; i<MULTIBLEND_EXTRUDER_NO; i++)
       {
           if((g_CalibrationData.m_stMultiblendExtruders[i].m_bEnabled != TRUE) && (g_CalibrationData.m_stMultiblendExtruders[i].m_bEnabled != FALSE))
           {
               g_CalibrationData.m_stMultiblendExtruders[i].m_bEnabled = FALSE;
           }
       }

       if(g_CalibrationData.m_nBaudRate > BAUD_RATE_LAST_ID)
       {
    	   g_CalibrationData.m_nBaudRate = BAUD_RATE_9600_ID;
    	     nResult |= BAD_EEPROM_DATA_MASK;
       }

       //nbb--todolp-- more checks for valid calibration data required here

    if(nResult & BAD_EEPROM_DATA_MASK)
    {
        g_bSaveAllCalibrationToEEprom = TRUE;  // save calibration data
        CopyCalibrationDataToMB();                // copy data back to mb
        PutAlarmTable( INVALIDCONFIGDATA,  0 );     /* indicate alarm, PTALTB   */
    }
    else
    {
        RemoveAlarmTable( INVALIDCONFIGDATA,  0 );
    }

    return( nResult );
}



///////////////////////////////////////////////////////////
// Loads the Calibration Data structure from OnBoard EEPROM
//
// Entry:   none
//
// Exit:    Void.
///////////////////////////////////////////////////////////
int LoadAllCalibrationDataFromOnBoardEEprom( void )
{
    int nResult = 0;
    int     i,nBytes,nChipSelectUsed;
    char    *pPtr;
    pPtr = (char*)&g_OnBoardCalData;

    // Write out the data
    nBytes = sizeof( g_OnBoardCalData );
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();

    for( i = 0; i < nBytes; i++ )
    {
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, ONBOARD_EEPROM_CALIBRATION_DATA_START + i );
    }
    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.
    return  nResult;
}

///////////////////////////////////////////////////////////
// Loads the Process Data from EEPROM
// LoadAllProcessDataFromEEprom
///////////////////////////////////////////////////////////
int LoadAllProcessDataFromEEprom( void )
{
    int nResult = 0;
    int nChipSelectUsed,nSize;
    unsigned char    g_arrcTemp[sizeof(g_ProcessData)];
    int     i,nBytes;
    char    *pPtr;
    pPtr = (char*)&g_ProcessData;

    // Write out the data
    nBytes = sizeof( g_ProcessData );
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();

    for( i = 0; i < nBytes; i++ )
    {
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, PROCESS_DATA_EEPROM_CALIBRATION_DATA + i );
    }
    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.

    memcpy(&g_arrcTemp, &g_ProcessData, sizeof(g_ProcessData));    // copy structure to array.
    nSize = sizeof(g_ProcessData)- sizeof(g_ProcessData.m_nChecksum);
    g_unCalibrationCheckSum = CalculateCheckSum(g_arrcTemp, nSize);

    if(g_unCalibrationCheckSum != g_ProcessData.m_nChecksum)
    {
        g_bProcessDataInvalid = TRUE;
    }
    else
    {
        g_bProcessDataInvalid = FALSE;
    }


    return  nResult;
}

///////////////////////////////////////////////////////////
//
// LoadAllDescriptionDataFromEEprom
// reads description data from eeprom
///////////////////////////////////////////////////////////
int LoadAllDescriptionDataFromEEprom( void )
{
    int nResult = 0;
    int nChipSelectUsed;
    unsigned char    g_arrcTemp[sizeof(g_sDescriptionData)];
    int     i,nBytes;
    char    *pPtr;
    pPtr = (char*)&g_sDescriptionData;

    // Write out the data
    nBytes = sizeof( g_sDescriptionData );
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();

    for( i = 0; i < nBytes; i++ )
    {
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, DESCRIPTION_DATA_EEPROM + i );
    }
    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.

    memcpy(&g_arrcTemp, &g_sDescriptionData, sizeof(g_sDescriptionData));    // copy structure to array.

    return  nResult;
}




///////////////////////////////////////////////////////////
//
// CheckForValidBatchSize
///////////////////////////////////////////////////////////
BOOL CheckForValidBatchSize( void )
{
        BOOL    bOkay = TRUE;
        if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_DO_BATCH_WEIGHT_CHECK) != 0)
        {
        	switch( g_CalibrationData.m_nBlenderType )
        	{
        	case TSM1000:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_TSM1000)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_TSM1000))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_TSM1000;
        			bOkay = FALSE;
        		}
        		break;
        	case TSM1500:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_TSM1500)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_TSM1500))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_TSM1500;
        			bOkay = FALSE;
        		}
        		break;
        	case TSM650:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_TSM650)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_TSM650))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_TSM650;
        			bOkay = FALSE;
        		}
        		break;
        	case TSM3000:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_TSM3000)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_TSM3000))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_TSM3000;
        			bOkay = FALSE;
        		}
        		break;
        	case OPTIMIX50:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_OPTIMIX50)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_OPTIMIX50))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_OPTIMIX50;
        			bOkay = FALSE;
        		}
        		break;
        	case OPTIMIX150:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_OPTIMIX150)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_OPTIMIX150))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_OPTIMIX150;
        			bOkay = FALSE;
        		}
        		break;
        	case TSM150:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_TSM150)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_TSM150))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_TSM150;
        			bOkay = FALSE;
        		}
        		break;
        	case OPTIMIX250:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_OPTIMIX250)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_OPTIMIX250))
                {
                    g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_OPTIMIX250;
                    bOkay = FALSE;
                }
               break;
        	case TSM350:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_TSM350)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_TSM350))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_TSM350;
        			bOkay = FALSE;
        		}
        		break;
        	case TSM50:
        		if((g_CalibrationData.m_fBatchSize > MAX_BATCH_SIZE_TSM50)||(g_CalibrationData.m_fBatchSize < MIN_BATCH_SIZE_TSM50))
        		{
        			g_CalibrationData.m_fBatchSize = DEFAULT_BATCH_SIZE_TSM50;
        			bOkay = FALSE;
        		}
        		break;
				default:
        		break;

        	}  // end of switch statement.
        }
   return(bOkay);
}


/*

;
;
;* SAVCEE  IS A SUBR. TO SAVE CALIBRATION DATA IN EEPROM
;          A CHECKSUM IS GENERATED ON RAM CALIBRATION DATA AND
;          DATA AND CHECKSUM ARE WRITTEN TO EEPROM
;
;
BANK1PROGS:        SECTION

SAVECAL2DATA:
        CLR    EEPFAU                   ;CLEAR EEPROM FAULT FLAG AT START
;        LDAA    MBPROALRM
;        ANDA    #CALINVALARMBIT         ; TRANSITION.
;        BEQ     SACEE2                  ; NO TRANSITION.
;        LDAA    #$50                    ; CAL DATA INVALID ALARM.
;        JSR     RMALTB                  ; REMOVE ALARM

SACEE2 LDX    #CAL2BLOCKEND             ;~END OF CALIBRATION DATA IN RAM
       STX    ENDADD
       LDX    #CAL2BLOCKSTART           ;~START OF CALIB. DATA IN RAM
       JSR    CSUM16                    ;~CALCULATE 2 BYTE C'SUM
       STAA   CAL2CKSUM                 ;~STORE CHECKSUM
       STAB   CAL2CKSUM+1               ;~*
       LDX    #CAL2BLOCKSTART           ;~START ADDR. OF DATA FOR BLOCK WRITE
       STX    MEMPTR
       LDD    #CAL2BLOCKEND
       SUBD   #CAL2BLOCKSTART
       STD    EEBYTS

       LDY    #EEC2DST                  ;~START OF CAL. DATA IN EEPROM
       JSR    WBLKEE                    ;~WRITE BLOCK TO EEPROM
       LDX    #EEC2DST
       LDD    #CAL2CKSUM
       SUBD   #CAL2BLOCKSTART           ;GET LENGTH OF CALIB BLOCK IN D
       ADX                              ;ADD D TO X, NOW X POINTS TO EECDSM
       LDAA   CAL2CKSUM                 ;~WRITE CHECKSUM TO EEPROM
       JSR    WRITAEE2
       AIX   #1
       LDAA   CAL2CKSUM+1               ;~2ND BYTE OF CHECKSUM
       JSR    WRITAEE2

       TST    EEPFAU
       LBNE    SVCEER                   ;ERROR IF NON ZERO
       BRA    SVCEEX                    ;~OK; EXIT

SVCEER LDAA   #$60                      ;~"EEPROM WRITE PROBLEM" ALARM CODE
;       JSR    PTALTB                    ;~PUT IN ALARM TABLE
XITEEP RTS

SVCEEX LDAA     MBPROALRM               ; CHECK EXISTING ALARM.
;       ANDA     #EEWRITEALARM           ;
;       BEQ      XITEEP                  ; NOT A TRANSITION.
;       LDAA     #$60                    ; EEPROM WRITE ALARM.
;       JSR      RMALTB                  ; REMOVE ALARM.
       BRA      XITEEP
;
;*  LODCEE  IS A SUBR. TO LOAD CAL. DATA FROM EEPROM
LODCAL2 LDAB   #RAMBANK                  ;EEPROM BANK ADDRESS
       TBYK                             ;SET YK TO POINT TO EEPROM
       LDY    #CAL2BLOCKSTART           ;~START ADDR. OF CAL. DATA IN RAM
       LDD    #CAL2BLOCKEND
       SUBD   #CAL2BLOCKSTART           ;FIND LENGTH OF CALIB. DATA BUFFER
       STD    YXBKSIZ
       LDX    #EEC2DST                  ;~START ADDR. OF CAL. DATA IN EEPROM
       JSR    EEPMOVBKXY

       LDX    #EEC2DST                  ;~CONFIRM DATA WRITTEN CORRECTLY BY
       LDD    #CAL2BLOCKEND
       SUBD   #CAL2BLOCKSTART           ;GET LENGTH OF CALIB BLOCK IN D
       ADX                              ;ADD D TO X, NOW X POINTS TO EECDSM
       AIX      #1                      ; POINT TO CHECKSUM.
       JSR      BYTE_READ2
       PSHA
       AIX      #1
       JSR      BYTE_READ2
       TAB
       PULA
       STD    CAL2CKSUM                 ;~LOAD & STORE IN RAM C'SUM OF CAL DATA
       LDX    #CAL2BLOCKEND             ;~CHECK THAT CHECKSUM OK
       STX    ENDADD
       LDX    #CAL2BLOCKSTART
       JSR    CSUM16                    ;~CALC. 2 BYTE C'SUM
       CPD    CAL2CKSUM
       BEQ    LDCEEX                    ;~OK; JUMP
;       LDAA   #$50                      ;~CALIBRATION DATA ALARM
;       JSR    PTALTB                    ;~STORE IN ALARM TABLE
LDCEEX RTS


*/
