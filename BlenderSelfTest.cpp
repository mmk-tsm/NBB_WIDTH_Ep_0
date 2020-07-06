//////////////////////////////////////////////////////////
//
// P.Smith                      26/2/09
// blender self test
//
// P.Smith                      26/3/09
// only allow self test to happen if the blender is not cycling.
// a/d internal test added
// load cell tare and constant test check, the constant and tare value are stored
// when the diagnostics password is used.
// weigh hopper tare deviation checks for a large tare deviation change.
// component standard deviation checks for a particular std dev for particular values
// switched load cell voltage check also added.
// the resulting differrence is checked for and the load cell voltage is switched out.
// component tare deviation check checks for a difference in the component tare value.
// component retries are checked on various components.
// various events like tare, leak, dump alarms are checked for
// all results are copied to modbus
// eeprom self test modified to restore the eeprom value back again.
// a hash check is added for the factory settings to determine if the values are valid.
// if the hash is not valid, the factory settings are updated.
//
// P.Smith                      24/4/09
// correct load cell switched voltage text failed
//
// P.Smith                      13/5/09
/// show prompt after self test has been completed.
//
// P.Smith                      15/5/09
// do not run component deviation if top up mode
//
// P.Smith                      25/6/09
// move CopySelfResultsToMB down after all tests completed
// copy g_sSelfTest.m_nComponentStandardDeviationTest & g_sSelfTest.m_nSwitchedLoadCellVoltageTest
//
// P.Smith                      25/6/09
// set g_bSelfTestInProgress when self test is initiated and clear when finished.
//
// P.Smith                      16/11/09
// added delay to allow a/d check to work properly, it did not work due to the fact that
// the dac voltage did not have time to settle.
// print ratio to telnet anyway.
// correct print of factory settings
//
// P.Smith                      19/11/09
// pause blender when self test is started
// change bSelfTestPassed to a global g_bSelfTestPassed to allow a/d check results to
// be checked.
// check g_nTSMDebug.m_bAToDStabilityDebug
// call LoadCellAToDStabilityCheck at end of original self test.
// allow reference or load cell voltage to be switched in.
// pause blender when reference voltage is switched in.
// copy results of a/d self test to modbus table.
// added LoadCellAToDStabilityCheck & DecideOnLoadCellAToDStabilityTest
// added RefAToDStabilityCheck & DecideOnRefAToDStabilityTest
//
// P.Smith                      21/1/10
// when the self test is initiated, the load cell reading is disturbed
// when the a/d is checked.
// to counteract this the leak alarm check is disabled at the end of the self test.
//
// P.Smith                      9/3/10
// call Service_Watchdog during blender self test.
// in SwitchedLoadCellVoltageCheckOk reduce settling to 2 seconds.
// change time delay on SwitchedLoadCellVoltageCheckOk to 2 seconds
//////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>

#include "General.h"
#include "InitialiseHardware.h"

#include "BatVars.h"
#include "BatVars2.h"
#include "BatchCalibrationFunctions.h"
#include "init5270.h"
#include "EEPROMdriver.h"
#include "QSPIconstants.h"
#include "NBBGpio.h"
#include "BlenderSelfTest.h"
#include "constants.h"
#include "ucos.h"
#include "Sdcard.h"
#include "I2CFuncs.h"
#include "LTC2415.h"
#include "BatchCalibrationFunctions.h"
#include "Hash.h"
#include "Historylog.h"
#include "Telnetcommands.h"
#include "Pause.h"
#include "Conalg.h"
#include "Watchdog.h"


extern CalDataStruct    g_CalibrationData;
unsigned int    g_bSelfTestPassed = FALSE;


//////////////////////////////////////////////////////
// BlenderSelfTestDiagnostic( void )
// does blender self tests
//
//////////////////////////////////////////////////////

void BlenderSelfTestDiagnostic( void )
{
    ToManual();  // make sure that blender is in manual
    g_bNoLeakAlarmCheck = TRUE;
    g_nTSMDebug.m_bAToDStabilityDebug = TRUE;
    g_bSelfTestPassed = TRUE;
    ImmediatePauseOn();
    if(!g_bCycleIndicate)  //if not cycling
    {
        memset( &g_sSelfTest, 0, sizeof( structSelfTest ) ); // set results structure to 0
        CopySelfResultsToMB();// copy results of test to modbus

        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Hardware tests\n ");

        // Do hardware checks first
        if(EEPROMSelfTest())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n EEPROM test passed");
            g_sSelfTest.m_nEEPROMTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n EEPROM test failed");
            g_sSelfTest.m_nEEPROMTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }
        if(SdCardTest())
        {
            g_sSelfTest.m_nSDCardTest = PASSED;
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n SD Card test passed");
        }
        else
        {
            g_sSelfTest.m_nSDCardTest = FAILED;
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n SD Card test failed");
            g_bSelfTestPassed =FALSE;
        }

        Service_Watchdog();	// service watchdog timer in case it is running.
        if(AtoDInternalTest())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n A/d test passed");
            g_sSelfTest.m_nAtoDTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n A/d test failed");
            g_sSelfTest.m_nAtoDTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }
        // now load cell related stuff
    	Service_Watchdog();	// service watchdog timer in case it is running.

        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n\n Load cell calibration test");

        if(LoadCellCalibrationConstantCheck())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            printf("\n Calibration deviation test passed : Factory %5.0f Actual %5.0f", g_CalibrationData.m_fFactoryWeightConstant,g_CalibrationData.m_fWeightConstant);
            g_sSelfTest.m_nCalibrationConstantTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            printf("\n Calibration deviation test failed : Factory %5.0f Actual %5.0f", g_CalibrationData.m_fFactoryWeightConstant,g_CalibrationData.m_fWeightConstant);
            g_sSelfTest.m_nCalibrationConstantTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }
        if(LoadCellCalibrationTareCheck())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            printf("\n Tare test passed : Factory %ld Actual %ld", g_CalibrationData.m_lFactoryTareCounts,g_CalibrationData.m_lTareCounts);
            g_sSelfTest.m_nTareValueTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            printf("\n Tare test failed : Factory %ld Actual %ld", g_CalibrationData.m_lFactoryTareCounts,g_CalibrationData.m_lTareCounts);
            g_sSelfTest.m_nTareValueTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }

        if(HopperTareDeviationExceeded())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n\n Weigh Hopper Tare Deviation test passed\n");
             g_sSelfTest.m_nWeighHopperTareDeviationTest = PASSED;
       }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n\n Weigh Hopper Tare Deviation test failed\n");
            g_sSelfTest.m_nWeighHopperTareDeviationTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }
        if(!g_CalibrationData.m_bTopUpMode)
        {
            if(ComponentStandardDeviationOk())
            {
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                iprintf("\n Standard deviation test passed\n");
                g_sSelfTest.m_nComponentStandardDeviationTest = PASSED;
            }
            else
            {
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                iprintf("\n Standard deviation test failed\n");
                g_sSelfTest.m_nComponentStandardDeviationTest = FAILED;
                g_bSelfTestPassed =FALSE;
            }
        }
    	Service_Watchdog();	// service watchdog timer in case it is running.
        if(SwitchedLoadCellVoltageCheckOk())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n Load cell switched voltage test passed\n");
            g_sSelfTest.m_nSwitchedLoadCellVoltageTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n Load cell switched voltage test failed\n");
            g_sSelfTest.m_nSwitchedLoadCellVoltageTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }
    	Service_Watchdog();	// service watchdog timer in case it is running.
        if(ComponentHasRetried())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n Component Retry test passed\n");
            g_sSelfTest.m_nComponentRetryTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n\n Component Retry test failed\n");
            g_sSelfTest.m_nComponentRetryTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }

        if(!g_CalibrationData.m_bTopUpMode)
        {
            if(ComponentTareDeviationCheck())
            {
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                iprintf("\n Component Tare Deviation test passed\n");
                g_sSelfTest.m_nComponentTareTest = PASSED;
            }
            else
            {
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                iprintf("\n Component Tare Deviation test failed\n");
                g_sSelfTest.m_nComponentTareTest = FAILED;
                g_bSelfTestPassed =FALSE;
            }
        }



        // now check for individual events
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n\n Events Test");

        if(EventsCheck())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n Events check test passed");
            g_sSelfTest.m_nEventsTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n Events check test failed");
            g_sSelfTest.m_nEventsTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }

        if(CommunicationsTest())
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n Communication test passed");
            g_sSelfTest.m_nCommunicationsTest = PASSED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\nCommunication test failed");
            g_sSelfTest.m_nCommunicationsTest = FAILED;
            g_bSelfTestPassed =FALSE;
        }


        CopySelfResultsToMB();// copy results of test to modbus
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        {
        iprintf("\n\n");
        ShowPrompt();
        }

    }
    else
    {
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n blender cycling please wait\n");
    }
    LoadCellAToDStabilityCheck();
}

//////////////////////////////////////////////////////
// BlenderSelfTestDiagnostic( void )
// does blender self tests
//
//////////////////////////////////////////////////////
BOOL EEPROMSelfTest( void )
{
    BYTE cTxData,cTxData1,cOriginalData;
    BOOL bSuccess = FALSE;
    int  nChipSelectUsed;

    BYTE cRxData,cRxData1;
    WORD nAddress = 0;
    cTxData = 0xAA;
    g_bSPIBusBusy = TRUE;

    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    Remove_EEPROM_Write_Protect();
    UnProtectEEPROM( nChipSelectUsed );    //

    cOriginalData = EEPROMRead1Byte( nChipSelectUsed, nAddress );

    EEPROMWrite1Byte(nChipSelectUsed,  cTxData, nAddress);      //
    OSTimeDly(TICKS_PER_SECOND/40);      // wait 1/40 second.
    cRxData =  EEPROMRead1Byte( nChipSelectUsed, nAddress );
    cTxData1= 0x55;
    EEPROMWrite1Byte(nChipSelectUsed,  cTxData1, nAddress);      //
    OSTimeDly(TICKS_PER_SECOND/40);      // wait 1/40 second.
    cRxData1=  EEPROMRead1Byte( nChipSelectUsed, nAddress );
    g_bSPIBusBusy = FALSE;
    if((cTxData == cRxData)&&(cTxData1 == cRxData1))
    {
        bSuccess = TRUE;
    }
    EEPROMWrite1Byte(nChipSelectUsed,  cOriginalData, nAddress);      //

    return(bSuccess);
}

//////////////////////////////////////////////////////
// SdCardTest( void )
// sd card test
//
//////////////////////////////////////////////////////
BOOL SdCardTest( void )
{
   F_FILE * fp;
  long lErrors=0;
  char cbuffer[50];
  char cread_buf[50];
  BOOL bSuccess = FALSE;
  if ( SDCardPresent() )
  {
  g_bTelnetSDCardAccessInProgress = TRUE;
  g_bSPIBusBusy = TRUE;
  InitialiseSDCard();

  fp=f_open("SelfTest.txt", "w");
  if(fp)
  {
      sprintf(cbuffer,"One day I'll fly away, need your love to stay.. ");
	  f_write(cbuffer,strlen(cbuffer),1,fp);
	  f_close(fp);
      fp=f_open("SelfTest.txt", "r");
      int n = f_read( cread_buf, 1, 49, fp );
      cread_buf[n] = '\0';

  }
  else
  {
       if(g_bTSMTelnetInSession && (fdTelnet > 0))
       iprintf("\n file not opened !!!!!!!!!!!!!!!!!%d",fp);
      lErrors++;
  }

  f_close(fp);
  f_delvolume(MMC_DRV_NUM);

  SPIEX1_Lo();   //  EX1 and EX2 must be both Hi for SD chip select.
  SPIEX2_Lo();   //  so this will de-activate SD chip selection.
  ReInitialisePinsUsedBySDCard();
  if(strcmp(cbuffer,cread_buf) == 0)
  {
      bSuccess = TRUE;
  }
  g_bSPIBusBusy = TRUE;
  g_bTelnetSDCardAccessInProgress = FALSE;

 }
    return(bSuccess);
}


//////////////////////////////////////////////////////
// AtoDInternalTest( void )
// a/d test
//
//////////////////////////////////////////////////////
BOOL AtoDInternalTest( void )
{
    BOOL bSuccess = FALSE;
    int nADIErrorCount,i;
    WORD nDACValue = 0;
    WORD chipSelectUsed;
    WORD RxBuffer[2];
    long lTemp,lTemp1024,lTemp512;
    long lRawADCountsLTC2415 = 0;  //
    float fRatio;
    WORD * ptrRxBuffer = RxBuffer;
    g_bSPIBusBusy = TRUE;
    lTemp1024 = 0;
    lTemp512 = 0;

    nADIErrorCount = 0;
    I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
    nDACValue = 0;
    AnalogMux_Select_DAV();    // select D/A input on mux
    WriteMAX5812DAC( nDACValue );
    OSTimeDly(TICKS_PER_SECOND/2);

    chipSelectUsed = LTC2415_SPI_CHIP_SELECT_REVB2;
    SPI_Select_AD();
    ReadLTC2415(chipSelectUsed, ptrRxBuffer);
    OSTimeDly( TICKS_PER_SECOND/2);
    for(i=0; i<4; i++)
    {
        ReadLTC2415(chipSelectUsed, ptrRxBuffer);
        lTemp = RxBuffer[1] | (RxBuffer[0] << 16);          //amalgamate words
        lTemp >>= 9;                     // shift 9 bits right, i.e. keep 20.
        lRawADCountsLTC2415 = lTemp & 0x000FFFFF;             // discard top 3 and lowest 9 bits.
        if( nDACValue == 1024)
        lTemp1024 = lRawADCountsLTC2415;
        else if( nDACValue == 512)
        lTemp512 = lRawADCountsLTC2415;
//               iprintf(" %ld %ld", lTemp1024, lTemp512 );

        fRatio = lTemp1024 / (float)lTemp512;

//              iprintf( "D/A Value = %d AD Cts = %ld", nDACValue, g_lRawADCountsLTC2415 );
        if(nDACValue == 1024)
        nDACValue = 512;
        else if(nDACValue == 512)
        nDACValue = 1024;
        else
        nDACValue = 512;
        WriteMAX5812DAC( nDACValue );

        OSTimeDly(TICKS_PER_SECOND/2);
//        printf("\nDAC Value = %d,  A/D = %ld, Ratio = %4.2f ",nDACValue, lRawADCountsLTC2415, fRatio );
        //OSTimeDly( TICKS_PER_SECOND*5 );
    }

    if(fdTelnet >0)
    printf("\n Ratio = %4.2f ", fRatio );

    if(fRatio > 1.94 && fRatio < 2.06 )
    {
      bSuccess = TRUE;
    }

    AnalogMux_Select_FANI();  // select a/d through multiplexer.
    g_bSPIBusBusy = FALSE;
    return(bSuccess);

}

//////////////////////////////////////////////////////
// LoadCellCalibrationConstantCheck( void )
// load cell constant
//
//////////////////////////////////////////////////////
BOOL LoadCellCalibrationConstantCheck( void )
{
    float fDiffPercentage;
    BOOL bSuccess = FALSE;
    if(g_CalibrationData.m_fFactoryWeightConstant > g_CalibrationData.m_fWeightConstant)
    {
        fDiffPercentage = 100 * (g_CalibrationData.m_fFactoryWeightConstant - g_CalibrationData.m_fWeightConstant)/g_CalibrationData.m_fFactoryWeightConstant;
    }
    else
    {
        fDiffPercentage = 100 * (g_CalibrationData.m_fWeightConstant - g_CalibrationData.m_fFactoryWeightConstant)/g_CalibrationData.m_fFactoryWeightConstant;
    }
    if(fDiffPercentage < FACTORY_WEIGHT_CONSTANT_PERCENTAGE_DIFFERENCE_ALLOWED)
    {
        bSuccess = TRUE;
    }
    return(bSuccess);
}

//////////////////////////////////////////////////////
// LoadCellCalibrationTareCheck( void )
// load cell tare
//
//////////////////////////////////////////////////////
BOOL LoadCellCalibrationTareCheck( void )
{
    float fDiffPercentage;
    BOOL bSuccess = FALSE;
    if(g_CalibrationData.m_lFactoryTareCounts > g_CalibrationData.m_lTareCounts)
    {
        fDiffPercentage = 100 * (float)(g_CalibrationData.m_lFactoryTareCounts - g_CalibrationData.m_lTareCounts)/(float)g_CalibrationData.m_lFactoryTareCounts;
    }
    else
    {
        fDiffPercentage = 100 * (float)(g_CalibrationData.m_lTareCounts - g_CalibrationData.m_lFactoryTareCounts)/(float)g_CalibrationData.m_lFactoryTareCounts;
    }
    if(fDiffPercentage < FACTORY_TARE_PERCENTAGE_DIFFERENCE_ALLOWED)
    {
        bSuccess = TRUE;
    }
    return(bSuccess);

}

//////////////////////////////////////////////////////
// ComponentTareDeviationCheck( void )
//
//
//////////////////////////////////////////////////////
BOOL ComponentTareDeviationCheck( void )
{
    long lDiff;
    int i,nCurrentIndex,nPreviousIndex;
    nPreviousIndex = 0;
    nCurrentIndex = 0;
    BOOL bSuccess = FALSE;
    BOOL bDiffLimitExceeded = FALSE;
      i = 0;      // i in this case is index into sequence table, nCompIndex relates to component no.
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
    {
        if(i == 0)
        {
            nPreviousIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble, 0 based.
        }
        else
        {
            nCurrentIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;
        if(g_lCmpTareCounts[nCurrentIndex] > g_lCmpRawCounts[nPreviousIndex])
        {
            lDiff = g_lCmpTareCounts[nCurrentIndex] - g_lCmpRawCounts[nPreviousIndex];
        }
        else
        {
            lDiff = g_lCmpRawCounts[nPreviousIndex] - g_lCmpTareCounts[nCurrentIndex];
        }
        if(lDiff > COMPONENT_TARE_COUNTS_DIFFERENCE_ALLOWED)
        {
            bDiffLimitExceeded = TRUE;
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n current %d Previous %d Difference  for %d is %d",g_lCmpTareCounts[nCurrentIndex],g_lCmpTareCounts[nPreviousIndex],i+1,lDiff);
        }
        nPreviousIndex = nCurrentIndex;
        }
         i++;
        if(bDiffLimitExceeded)
        {
            bSuccess = FALSE;
        }
        else
        {
           bSuccess = TRUE;
         }
    }
    return(bSuccess);
}

//////////////////////////////////////////////////////
// EventsCheck( void )
// if events have occurred, then indicate that test has failed
//
//////////////////////////////////////////////////////
BOOL EventsCheck( void )
{
    BOOL bSuccess = TRUE;
    // tare alarms
    if((g_s24HrOngoingHistoryEventsLogData.m_nTareAlarms == 0) && (g_s24HrHistoryEventsLogData.m_nTareAlarms ==0))
    {
        g_sSelfTest.m_nTareAlarms = PASSED;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Tare alarm test passed");
    }
    else
    {
        bSuccess = FALSE;
        g_sSelfTest.m_nTareAlarms = FAILED;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Tare alarm test failed : ongoing %d 24hr history %d",g_s24HrOngoingHistoryEventsLogData.m_nTareAlarms,g_s24HrHistoryEventsLogData.m_nTareAlarms);
    }
    // leak alarms
    if((g_s24HrOngoingHistoryEventsLogData.m_nLeakAlarmsCtr == 0) && (g_s24HrHistoryEventsLogData.m_nLeakAlarmsCtr ==0))
    {
        g_sSelfTest.m_nLeakAlarms = PASSED;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Leak alarm test passed");
    }
    else
    {
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Leak alarms test failed : ongoing %d 24hr history %d",g_s24HrOngoingHistoryEventsLogData.m_nLeakAlarmsCtr,g_s24HrHistoryEventsLogData.m_nLeakAlarmsCtr);
        bSuccess = FALSE;
        g_sSelfTest.m_nLeakAlarms = FAILED;
    }

    // dump flap
    if((g_s24HrOngoingHistoryEventsLogData.m_nDumpFlapNotOpenAlarmsCtr == 0) && (g_s24HrHistoryEventsLogData.m_nDumpFlapNotOpenAlarmsCtr ==0))
    {
        g_sSelfTest.m_nDumpFlapAlarms = PASSED;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Dump flap test passed");
    }
    else
    {
        bSuccess = FALSE;
        g_sSelfTest.m_nDumpFlapAlarms = FAILED;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Dump flap test failed : ongoing %d 24 hr history %d",g_s24HrOngoingHistoryEventsLogData.m_nDumpFlapNotOpenAlarmsCtr,g_s24HrHistoryEventsLogData.m_nDumpFlapNotOpenAlarmsCtr);

    }
// motor trips

    if((g_ProcessData.m_n24HourOngoingMotorTripCounter == 0) && (g_s24HrHistoryEventsLogData.m_nMotorTrips == 0))
    {
        g_sSelfTest.m_nMotorTrip = PASSED;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Motor trip test passed");

    }
    else
    {
        bSuccess = FALSE;
        g_sSelfTest.m_nMotorTrip = FAILED;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Motor trip test failed : ongoing: %d 24hr history: %d",g_ProcessData.m_n24HourOngoingMotorTripCounter,g_s24HrHistoryEventsLogData.m_nMotorTrips);
    }

    if(g_bTSMTelnetInSession && (fdTelnet > 0))
    iprintf("\n Resets ongoing: %d  24hr history: %d",g_ProcessData.m_n24HourOngoingResetCounter,g_s24HrHistoryEventsLogData.m_nResetCtr);

    return(bSuccess);
}


//////////////////////////////////////////////////////
// EventsCheck( void )
// if events have occurred, then indicate that test has failed
//
//////////////////////////////////////////////////////
BOOL CommunicationsTest( void )
{
    BOOL bSuccess = TRUE;
    // panel comms

    if(g_bTSMTelnetInSession && (fdTelnet > 0))
    iprintf("\n\n Communications Test");
    if(g_nBatchCommsStatusPanel == BATCH_COMMS_PROTOCOL_COMMS_OKAY)
    {
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Panel Communications test passed");
        g_sSelfTest.m_nPanelComms = PASSED;
    }
    else
    {
        bSuccess = FALSE;
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Panel Communications Failed");
        g_sSelfTest.m_nPanelComms = FAILED;
    }

    if(g_nBatchCommsStatusSEI == BATCH_COMMS_PROTOCOL_UNUSED)
    {
        g_sSelfTest.m_nSEIComms = 0;
    }
    else
    {
        if(g_nBatchCommsStatusSEI == BATCH_COMMS_PROTOCOL_COMMS_ERROR)
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n SEI Communications Test failed");
            g_sSelfTest.m_nSEIComms = FAILED;
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n SEI Communications Test passed");
            g_sSelfTest.m_nSEIComms = PASSED;
        }
    }
    if(g_nBatchCommsStatusLLS == BATCH_COMMS_PROTOCOL_UNUSED)
    {
        g_sSelfTest.m_nLLSComms = 0;
    }
    else
    {
        if(g_nBatchCommsStatusLLS == BATCH_COMMS_PROTOCOL_COMMS_ERROR)
        {
           bSuccess = FALSE;
           g_sSelfTest.m_nLLSComms = FAILED;
           if(g_bTSMTelnetInSession && (fdTelnet > 0))
           iprintf("\n LLS Communication test failed");
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n LLS Communication test passed");
            g_sSelfTest.m_nLLSComms = PASSED;
        }
    }
    if(g_nBatchCommsStatusNetwork == BATCH_COMMS_PROTOCOL_UNUSED)
    {
        g_sSelfTest.m_nNetworkComms = 0;
    }
    else
    {
        if(g_nBatchCommsStatusNetwork == BATCH_COMMS_PROTOCOL_COMMS_OKAY)
        {
            bSuccess = FALSE;
            g_sSelfTest.m_nNetworkComms = PASSED;
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n Network communications test passed");
        }
        else
        if(g_nBatchCommsStatusNetwork == BATCH_COMMS_PROTOCOL_UNUSED)
        {
            g_sSelfTest.m_nNetworkComms = 0;
        }
    }

    return(bSuccess);
}



//////////////////////////////////////////////////////
// ComponentHasRetried( void )
// checks ongoing and hourly components retries
// and indicates error if a certain no have occurred
//
//////////////////////////////////////////////////////
BOOL ComponentHasRetried( void )
{
    BOOL bSuccess = TRUE;
    unsigned int i,nTemp;
    if(g_bTSMTelnetInSession && (fdTelnet > 0))
    iprintf("\n\n Component Retry Test");
    for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        nTemp = g_sOngoingHistoryComponentLogData.m_nComponentRetryCtr[i] + g_sHourHistoryComponentLogData.m_nComponentRetryCtr[i];
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Retries ongoing : %d Previous hour: %d for component %d ",g_sOngoingHistoryComponentLogData.m_nComponentRetryCtr[i],g_sHourHistoryComponentLogData.m_nComponentRetryCtr[i],i+1);

        if(nTemp > MAX_RETRIES_ALLOWED_IN_SELF_TEST)
        {
            bSuccess = FALSE;
        }
    }

    return(bSuccess);
}


//////////////////////////////////////////////////////
// HopperTareDeviationExceeded( void )
//
//////////////////////////////////////////////////////
BOOL HopperTareDeviationExceeded( void )
{
    BOOL bSuccess = TRUE;
    long lDiff;

    // ongoing tare value
    lDiff = g_sOngoingHistoryEventsLogData.m_lTareMaxValue - g_sOngoingHistoryEventsLogData.m_lTareMinValue;
    if(g_bTSMTelnetInSession && (fdTelnet > 0))
    {
        iprintf("\n Weigh hopper tare deviation test");
        iprintf("\n Tare max is %ld min is %ld",g_sOngoingHistoryEventsLogData.m_lTareMaxValue,g_sOngoingHistoryEventsLogData.m_lTareMinValue);
    }
    if(lDiff > BLENDER_SELF_TEST_MAX_TARE_DIFFERENCE_ALLOWED)
    {
        if(g_bTSMTelnetInSession && (fdTelnet > 0))
        iprintf("\n Tare Deviation exceeded %ld",lDiff);
        bSuccess = FALSE;
    }


    return(bSuccess);
}


//////////////////////////////////////////////////////
// TestFunction( void )
// test blender function
//
//////////////////////////////////////////////////////
void TestFunction( int nTest )
{
    if(nTest == BLENDER_SELF_TEST)
    {
         g_bDoSelfTestDiagnostics = TRUE;
         g_bSelfTestInProgress = TRUE;
         if(g_bCycleIndicate)
         {
             if(g_bTSMTelnetInSession && (fdTelnet > 0))
             iprintf("\n Please wait, cycle in progress");
         }
    }
    else
    if(nTest == SWITCH_IN_06_REFERENCE)
    {
        g_bRefAToDStabilityTestInProgress = TRUE;
        AnalogMux_Select_REF(); // select reference
        ImmediatePauseOn();

    }
    else
    if(nTest == SWITCH_IN_FANI_INPUT)
    {
        g_bRefAToDStabilityTestInProgress = FALSE;
        AnalogMux_Select_FANI();
    }
}


//////////////////////////////////////////////////////
// CopySelfResultsToMB( void )
// copy test results to mb
//
//////////////////////////////////////////////////////
void CopySelfResultsToMB( void )
{
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_EEPROM] =  g_sSelfTest.m_nEEPROMTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_SD_CARD] =  g_sSelfTest.m_nSDCardTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_A_TO_D] =  g_sSelfTest.m_nAtoDTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_CALIBRATION_CONSTANT_DEV] =  g_sSelfTest.m_nCalibrationConstantTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_TARE_VALUE_DEV] =  g_sSelfTest.m_nTareValueTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMP_TARE] =  g_sSelfTest.m_nComponentTareTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_EVENTS] =  g_sSelfTest.m_nEventsTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_TARE_ALARM] =  g_sSelfTest.m_nTareAlarms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_LEAK_ALARM] =  g_sSelfTest.m_nLeakAlarms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_DUMP_FLAP] =  g_sSelfTest.m_nDumpFlapAlarms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMMS_PANEL] =  g_sSelfTest.m_nPanelComms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMMS_SEI] =  g_sSelfTest.m_nSEIComms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMMS_LLS] =  g_sSelfTest.m_nLLSComms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMMS_OPT] =  g_sSelfTest.m_nOptimisationComms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMMS_NETWORK] =  g_sSelfTest.m_nNetworkComms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMMS_TCPIP] =    g_sSelfTest.m_nTCPIPComms;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMPONENT_RETRY] = g_sSelfTest.m_nComponentRetryTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_HOPPER_TARE_DEV] = g_sSelfTest.m_nWeighHopperTareDeviationTest;

    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_COMP_STDEV] = g_sSelfTest.m_nComponentStandardDeviationTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_LOAD_CELL_SWITCH] = g_sSelfTest.m_nSwitchedLoadCellVoltageTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_PASSED] = g_sSelfTest.m_nSelfTestResult;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_REF_STABILITY] = g_sSelfTest.m_nRefAToDStabilityTest;
    g_arrnMBTable[BATCH_BLENDER_SELF_TEST_LOAD_CELL_STABILITY] = g_sSelfTest.m_nLoadCellAToDStabilityTest;
}


//////////////////////////////////////////////////////
// CheckCalibrationHash( void )
//
//////////////////////////////////////////////////////
void CheckCalibrationHash( void )
{
    long lTemp;
    lTemp = GenerateFactoryCalibrationHash();
    if(g_bTSMTelnetInSession && (fdTelnet > 0))
    iprintf("\n hash %d wt constant %d",lTemp,g_CalibrationData.m_nFactoryWeightConstantHash);
    if(lTemp != g_CalibrationData.m_nFactoryWeightConstantHash)
    {
        UpDateFactorySettings();
    }

}


//////////////////////////////////////////////////////
// GenerateCalibrationHash( void )
//
//////////////////////////////////////////////////////
long GenerateFactoryCalibrationHash( void )
{
    WORD lTemp;
    int nSize;
    unsigned char cBuffer[20];
    nSize = sizeof(g_CalibrationData.m_fFactoryWeightConstant);
    memcpy(&cBuffer,&g_CalibrationData.m_fFactoryWeightConstant,nSize);    // copy structure to array.
    lTemp = TSMHash(cBuffer,nSize);
    return(lTemp);
}

//////////////////////////////////////////////////////
// GenerateCalibrationHash( void )
//
//////////////////////////////////////////////////////
void UpDateFactorySettings( void )
{
    WORD lTemp;
    g_CalibrationData.m_fFactoryWeightConstant = g_CalibrationData.m_fWeightConstant;
    g_CalibrationData.m_lFactoryTareCounts = g_CalibrationData.m_lTareCounts;

    printf(" factory constant %f nom %f",g_CalibrationData.m_fFactoryWeightConstant,g_CalibrationData.m_fWeightConstant);
    printf(" factory tare %ld nom %ld",g_CalibrationData.m_lFactoryTareCounts,g_CalibrationData.m_lTareCounts);
    lTemp = GenerateFactoryCalibrationHash();
    g_ProcessData.m_n24HourOngoingResetCounter = 0;
    g_ProcessData.m_n24HourOngoingMotorTripCounter = 0;
    g_bSaveAllProcessDataToEEprom = TRUE;
    g_bAllowProcesDataSaveToEEPROM = TRUE;
    g_bSaveAllCalibrationToEEprom = TRUE;
    g_CalibrationData.m_nFactoryWeightConstantHash = lTemp;
}




//////////////////////////////////////////////////////
// ComponentStandardDeviationOk( void )
// checks if the component standard deviation is ok
//////////////////////////////////////////////////////
BOOL ComponentStandardDeviationOk( void )
{
    BOOL bSuccess = TRUE;
    int i,nComponentIndex;
    i = 0;      // i in this case is index into sequence table, nCompIndex relates to component no.
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
    {
        nComponentIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble, 0 based.
        if(g_fComponentTargetWeight[nComponentIndex] > 1.000f)  // > 1kg
        {
            if(g_fComponentWeightStdDevPercentage[nComponentIndex] > 3.0f)
            {
                bSuccess = FALSE;
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                printf("\n Component %d std deviation error target %2.3f std dev percentage = %2.2f",nComponentIndex+1,g_fComponentTargetWeight[nComponentIndex],g_fComponentWeightStdDevPercentage[nComponentIndex]);

            }
        }
        else
        if(g_fComponentTargetWeight[ nComponentIndex] > 0.100f)  // 100g - 1000g
        {
            if(g_fComponentWeightStdDevPercentage[nComponentIndex] > 7.0f)
            {
                bSuccess = FALSE;
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                printf("\n Component %d std deviation error target %2.3f std dev percentage = %2.2f",nComponentIndex+1,g_fComponentTargetWeight[nComponentIndex],g_fComponentWeightStdDevPercentage[nComponentIndex]);
            }
            else
            {
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                printf("\n Component %d std deviation target %2.3f std dev percentage = %2.2f",nComponentIndex+1,g_fComponentTargetWeight[nComponentIndex],g_fComponentWeightStdDevPercentage[nComponentIndex]);
            }
        }
        else
        if(g_fComponentTargetWeight[nComponentIndex] > 0.020f)  // 20g - 100g
        {
            if(g_fComponentWeightStdDevPercentage[nComponentIndex] > 10.0f)
            {
                bSuccess = FALSE;
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                printf("\n Component %d std deviation error target %2.3f std dev percentage = %2.2f",nComponentIndex+1,g_fComponentTargetWeight[nComponentIndex],g_fComponentWeightStdDevPercentage[nComponentIndex]);
            }
            else
            {
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                printf("\n Component %d std deviation target %2.3f std dev percentage = %2.2f",nComponentIndex+1,g_fComponentTargetWeight[nComponentIndex],g_fComponentWeightStdDevPercentage[nComponentIndex]);
            }

        }
        else
        if(g_fComponentWeightStdDevPercentage[nComponentIndex] > 15.0f)
        {
            bSuccess = FALSE;
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            printf("\n Component %d std deviation error target %2.3f std dev percentage = %2.2f",nComponentIndex+1,g_fComponentTargetWeight[nComponentIndex],g_fComponentWeightStdDevPercentage[nComponentIndex]);
        }
        else
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            printf("\n Component %d std deviation target %2.3f std dev percentage = %2.2f",nComponentIndex+1,g_fComponentTargetWeight[nComponentIndex],g_fComponentWeightStdDevPercentage[nComponentIndex]);
        }
        i++;
    }
    return(bSuccess);
}



//////////////////////////////////////////////////////
// SwitchedLoadCellVoltageCheckOk( void )
// checks if the switched load cell voltage check is ok
//////////////////////////////////////////////////////
BOOL SwitchedLoadCellVoltageCheckOk( void )
{
    long    lReadingBeforeSwitch,lReadingAfterSwitch,lDiff;
    BOOL bSuccess = TRUE;
    lReadingBeforeSwitch = g_lOneSecondWeightAverage;
    ANALOG_SWITCH_ON();   // switch in voltage
    OSTimeDly(2 * TICKS_PER_SECOND);
    lReadingAfterSwitch = g_lOneSecondWeightAverage;
    lDiff = lReadingAfterSwitch - lReadingBeforeSwitch;
    if(g_bTSMTelnetInSession && (fdTelnet > 0))
    iprintf("\n Before Switch %ld AfterSwitch %ld Difference %ld",lReadingBeforeSwitch,lReadingAfterSwitch,lDiff);
    if(g_CalibrationData.m_nBlenderType == TSM3000)
    {
        if((lDiff > TSM3000_LOW_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE)&&(lDiff < TSM3000_HIGH_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE))
        {
            bSuccess = TRUE;
        }
        else
        {
            bSuccess = FALSE;
        }
    }
    else  //not a TSM 3000
    {
        if((lDiff > LOW_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE)&&(lDiff < HIGH_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE))
        {
            bSuccess = TRUE;
        }
        else
        {
            bSuccess = FALSE;
        }
    }

    ANALOG_SWITCH_OFF();   // switch out voltage
    return(bSuccess);
}

//////////////////////////////////////////////////////
// LoadCellAToDStabilityCheck( void )
// starts a/d load cell stability test
//////////////////////////////////////////////////////
void LoadCellAToDStabilityCheck( void )
{
    if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
    iprintf("\n Load Cell Stability Test");
    g_sSelfTest.m_nLoadCellAToDStabilityTest = 0;
    g_bRefAToDStabilityTestInProgress = FALSE;
    AnalogMux_Select_FANI();
    g_nDelayBeforeLoadCellLogCtr = DELAY_BEFORE_STABILITY_DATA_READ;
}


//////////////////////////////////////////////////////
// DecideOnLoadCellAToDStabilityTest( void )
// decides if load cell test passed / failed
//////////////////////////////////////////////////////
void DecideOnLoadCellAToDStabilityTest( void )
{
    unsigned int i,nDiff;
    g_bRefAToDStabilityTestInProgress = FALSE;
    // a/d stability check, show a/d values for the previous batch
    if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
    iprintf("\n ");
    for( i=0; i<MAX_STABILITY_READINGS; i++)
    {
        if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
        iprintf("%d ", g_nAToDStabilityLog[i]);
    }
    nDiff = g_nAToDStabilityMaxValue-g_nAToDStabilityMinValue;
    if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
    iprintf("\n Max Ref %d Min Ref %d Diff %d", g_nAToDStabilityMaxValue,g_nAToDStabilityMinValue,nDiff);
    if(nDiff < ATOD_LOAD_CELL_MAXIMUM_DEVIATION_ALLOWED)
    {
        if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
        iprintf("\n Load Cell Stability Test passed");
        g_sSelfTest.m_nLoadCellAToDStabilityTest = PASSED;

    }
    else
    {
        if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
        iprintf("\n Load Cell Stability Test failed");
        g_sSelfTest.m_nLoadCellAToDStabilityTest = FAILED;
        g_bSelfTestPassed = FALSE;

    }
    if(g_bSelfTestInProgress)   // start the ref stability check if sself test running
    {
        RefAToDStabilityCheck();
    }

}



//////////////////////////////////////////////////////
// RefAToDStabilityCheck( void )
// starts a/d ref stability test
//////////////////////////////////////////////////////
void RefAToDStabilityCheck( void )
{
    if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
    iprintf("\n AToD Ref Stability Test");
    g_sSelfTest.m_nRefAToDStabilityTest = 0;
    g_bRefAToDStabilityTestInProgress = TRUE;
    AnalogMux_Select_REF(); // select reference
    g_nDelayBeforeRefLogCtr = DELAY_BEFORE_STABILITY_DATA_READ;
}

//////////////////////////////////////////////////////
// DecideOnRefAToDStabilityTest( void )
// decides if ref test passed / failed
//////////////////////////////////////////////////////
void DecideOnRefAToDStabilityTest( void )
{
    unsigned int i,nDiff;
    g_bRefAToDStabilityTestInProgress = FALSE;
    // a/d stability check, show a/d values for the previous batch
    if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
    iprintf("\n ");
    for( i=0; i<MAX_STABILITY_READINGS; i++)
    {
        if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
        iprintf("%d ", g_nAToDRefStabilityLog[i]);
    }
    nDiff = g_nAToDRefStabilityMaxValue-g_nAToDRefStabilityMinValue;
    if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
    iprintf("\n Max Ref %d Min Ref %d Diff %d", g_nAToDRefStabilityMaxValue,g_nAToDRefStabilityMinValue,nDiff);
    if(nDiff < ATOD_REF_MAXIMUM_DEVIATION_ALLOWED)
    {
        if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
        iprintf("\n AToD Reference Stability Test passed");
        g_sSelfTest.m_nRefAToDStabilityTest = PASSED;
    }
    else
    {
        if((fdTelnet >0) && g_nTSMDebug.m_bAToDStabilityDebug)
        iprintf("\n AToD Reference Stability Test failed");
        g_sSelfTest.m_nRefAToDStabilityTest = FAILED;
        g_bSelfTestPassed = FALSE;

    }
    if(g_bSelfTestInProgress)
    {
        g_bSelfTestInProgress = FALSE;
        if(g_bSelfTestPassed)
        {
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n SELF TEST passed >>>>>>>>>>");
            g_sSelfTest.m_nSelfTestResult = PASSED;
        }
        else
        {
            g_sSelfTest.m_nSelfTestResult = FAILED;
            if(g_bTSMTelnetInSession && (fdTelnet > 0))
            iprintf("\n SELF TEST failed !!!!!!!!!!!!");
        }

        g_nTSMDebug.m_bAToDStabilityDebug = FALSE;
        PauseOff();

    }
}


