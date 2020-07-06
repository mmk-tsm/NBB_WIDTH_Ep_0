//////////////////////////////////////////////////////
// ConAlg.
//
//
// P.Smith                              01-05-2005
// First Pass at control algorithm
// Added ToAuto / ToManual and Auto / Manual toggle.
//
// P.Smith                              8-02-2005
// Added LimitCorrection(),
//
// P.Smith                              9-02-2005
// Test and verification of control algorithm.
// SetVolumetricExtruderSpeed(), EstimateKgPerHour() per hour
// Control algorithm basics verified, control action calculation
// checking control bands. coming out of start up mode.
//
// P.Smith                                                  10-02-2005
// Copy d/a to modbus table on restart g_arrnWriteSEIMBTable[MB_SEI_DA]
//
// P.Smith                                                  14-02-2005
// added CheckForMaxExtruderPercentage()
// added CheckAlarmLimit, SetPointChange, CheckForMaxExtruderPercentage
//
// P.Smith                                                  31-03-2005
// uncomment SetVolumetricLineSpeed & CopyGPMToMB
// Correct control alarm remove check
// uncomment CheckForMaxExtruderPercentage
//
// P.Smith                                                  30-08-2005
// Correct the ToAuto function, checking mode incorrectly
//
//
// P.Smith                                                  28-09-2005
// only run control algorithm if licensing enabled.
//
// P.Smith                                                  11/10/05
// store absolute value to g_fPositiveControlErrorPercentage
// percentage is calculated as % not as a decimal
//
// P.Smith                                                  11/10/05
// Reset the round robin when blender is in Manual or start up.
//
// P.Smith                                                  17/10/05
// added check for g_CalibrationData.m_nMaxThroughput being non zero before call
// CMR calculation
// Calculate positive error to allow alarm bands to be checked.
// change m_nCorrectEveryNCycles to Bool.
// Ensure that if haul off that sign of error is inverted
// Ensure that bDoCorrection is set to FALSE;
// Ensure that LimitCorrection() does not updated g_nExtruderDAValue
// transfer is done after the call to this function
// g_nExtruderDAValue <= g_nTempExtruderDAValue;

//
// P.Smith                                                  18/10/05
// Correct check for correct every n cycles, only correct every n cycles if g_CalibrationData.m_bCorrectEveryNCycles
// is false.
//
// P.Smith                                                  18/11/05
// Correct g/m check on entering control algorithm, ensure that g/m mode is checked for
// in SetExtruderControlStartup only set start up if in auto.
//
// P.Smith                                                  25/11/05
// Correct d/a per kg and d/a per meter per minute calculation in CalculateDAPerKg
// make nMaxExtruder unsigned integer.
// only call CheckForMaxExtruderPercentage if control mode is 0 - 10 volts.
//
// P.Smith                                                  30/11/05
// Correct SetVolumetricLineSpeed, ensure that no of estimation is set to allow
// the estimated g/m to be calculated for a no of seconds after the estimation
// has been carried out.
//
// P.Smith                                                  1/12/05
// correct check for licensing options, in limit correction ControlAllowed flag
// is checked and non of the d/a calculation is performed.
//
//
// P.Smith                                                  10/1/06
// correct g_nTempExtruderDAValue conversion use (unsigned int)
// g_nTempExtruderDAValue = g_nExtruderDAValue + (unsigned int)(nMaxExtruder * fControlLimit);
// copy g_fThroughputKgPerHour to g_fRRThroughputKgPerHour, was a dead assignment
// g_nTempExtruderDAValue cast from float to int corrected.
//
//
// P.Smith                      16/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
// #include "ExStall.h"
//
// P.Smith                      28/2/06
// ResetKgHRoundRobin();               // RESRROB - Reset kg/h round robin.
// ResetGPMRoundRobin();               //ASM = RESRROB = Reset gpm round robin
// #include "KghCalc.h"
// #include "GPMCalc.h"
// CopyKghToMB();//                      ASM =   CPYKGHMB
// CopyGPMToModbus();                    //  ASM = CPYGPMMB
// CalculateGramsPerSquareMeter();       //ASM = GPMTOGRAMPERSM
//
// P.Smith                                                  12/5/06
// in void EstimateKgPerHour( void ) check if the d/a per kg is greater than 1.0, if greater
// then allow calculation to proceed, other wise do not allow to proceed.
//
// P.Smith                                                  28/6/06
// MINIMUM_DTOA_PER_KIL0 used when checking d/a per kg min
//
// P.Smith                                                  14/7/06
// if a level sensor alarm has occurred, reset the average and exit
// with no control, set the estimation counter to ensure that the
// next few kg/h calculations are done from the estimated value
// if the level sensor alarm persists, then after a no of
// consecutive alarms then switch the blender to manual.
//
// check for auto before checking for level sensor alarm.
//
// P.Smith                                               4/12/06
// Store d/a per kg d/a per m/min to calibration data eeprom so that it can be saved every cycle.
// and reloaded on reset.
//
// P.Smith                                               5/2/07
/// Remove unused iprintf
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          13/11/07
// Modified control mode format implemented
// allow for new bit set up for control  mode.
//
// P.Smith                          15/11/07
// use g_CalibrationData.m_fDtoAPerKilo in the calculation of the d/a per kg
// it will be rpm / kg in effect
// implement this in CalculateDAPerKg & EstimateKgPerHour, allow for possible divide by zero.
// put in min check to avoid divide by zero.
//
// P.Smith                          28/11/07
// in calculation of cmr, set g_bCCVsFlag to ensure that voltage is output to the sei
// this generates the control voltage calculation and transfers the output to the
// d/a on the sei.
//
// P.Smith                          5/12/07
// Calcualte rpm per kilo, the original d/a per kg is also calculated as before
// Do not run the control algorithm unless g_nSuspendControlCounter is 0.
//
// P.Smith                          9/1/08
// remove printf
//
// P.Smith                          11/2/08
// name change CheckDToALimit to CheckDToALimit6
// in volumetric set store new d/a set point in g_nExtruderVolumetricDAValue
// this is then checked every second to determine if it has been downloaded.
//
// P.Smith                          25/2/08
// In InititateCorrection, set g_nIncDecExtruderSetpoint to g_nTempExtruderDAValue
// set g_arrnWriteSEIMBTable[MB_SEI_DA] to g_nIncDecExtruderSetpoint to transfer the
// set point to the sei.
//
//
// P.Smith                          6/3/08
// g_bCCVsFlag not being set to true at the end of the limit correction function.
//
// P.Smith                          6/3/08
// call CheckCalibrationDataChecksum incase of calibration data getting corrupted.
// seterate out 0 - 10 v control
//
// P.Smith                          7/3/08
// in new function, ensure that g_bDisableControlAction is set to stop control action on
// the next cycle following the volumetric corection.
// in inc/dec volumetric set, ensure that g_nEstimationContinueCounter is set to NUMBEROFESTIMATIONS;
//
// P.Smith                          11/3/08
// correct CalculateDAPerKg, check for line speed is in the wrong place, it needs to be
// before the d/a per mpm check
//
// P.Smith                          18/3/08
// in increase /decrease mode when doing limit check, only check against the current
// tacho reading and the new set point, not the old set point
// in increase / decrease limit check, remove check for initial limit of 10% of 32768
// remove g_nExtruderDAValue = g_nTempExtruderDAValue;
// remove g_nExtruderDAValue = nExtruderControlSetpointIncDec;
//
// P.Smith                          26/3/08
// m_fDtoAPerKilo,m_fRPMPerKilo,m_fDtoAPerMeterPerMinute,m_fDtoAPerKiloCMR
// set g_bSaveAllProcessDataToEEprom to save process data to eeprom.
//
// P.Smith                          26/3/08
// initial investigation of g/12sq feet control, the volumetric correction does not work
// as the g/12sq feet is not checked for in the blender mode, modify mode check to include
// MODE_GP12SQFEET
// remove LimitCorrection1
// remove  else in SetVolumetricLineSpeed, allow g_nTempExtruderDAValue to be calculated all
// the time
//
// P.Smith                          14/5/08
// in EstimateKgPerHour, check the rpm per kilo figure not the d/a per kg before estimating the
// kg/hr.
// in kwh, the adreading is not available, therefore the d/a per kg will be zero.
// therefore it is necessary to check the rpm per kg before estimating the kg/hr from the
// rpm.
// the kg/hr is estimated from the rpm and not from the d/a per kg anymore in inc /dec mode
// of operation.
//
// P.Smith                          12/6/08
// In CalculateDAPerKg check for g_nDisableDTAPerKgCtr and g_nCleanCycleSeq equal to zero
// No d/a per kg calc if g_nDisableDTAPerKgCtr is non zero, also decrement counter is non
// zero.
//
// P.Smith                          1/7/08
// if the error drifts outside the control band, then the blender tries to correct the error
// if the error persists for 3 batches then it drops to manual and gives a control alarm.
// before this, if something changed on the line and the error drifted outside the 10%,
// it dropped to manual after 3 consecutive % deviation outside the alarm band
// reset the round robin when the error has drifted outside the control band.
// set bDoCorrection to true if g_nControlErrorCounter is non zero, this allow a correction to
// be made.
//
// P.Smith                          7/7/08
// log to auto and manual transition and log to manual extruder stalled.
//
// P.Smith                          21/7/08
// remove LimitCorrection1,remove printfs, remove unused else.
//
// P.Smith                          24/6/08
// remove g_arrnMBTable & g_arrnWriteSEIMBTable
//
// P.Smith                          14/10/08
// set nExtruderSet & nFinalExtruderSet to 0 to remove compiler warning.
//
// P.Smith                          29/10/08
// remove --todo--
//
// P.Smith                          4/12/08
// increment control alarm history counter.
//
// P.Smith                          8/6/09
// Correct the max check on the extruder d/a value, this was being checked in a function
// CheckDToALimit4, but was checking the wrong variable, switch to manual and generate a
// control alarm
//
// P.Smith                          8/6/09
// use round robin value for 0 - 10 volt cmr mode, this was originally using the
// non averaged value.
//
// P.Smith                          5/3/09
// do not allow the blender to be switched to auto if licensing is not enabled.
// checks are done on kg/hr and grams per meter
////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "KghCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "Conalg.h"
#include "Alarms.h"
#include "TSMPeripheralsMBIndices.h"
#include "GPMCalc.h"
#include "Rwsei.h"
#include "Progloop.h"
#include "ExStall.h"
#include <basictypes.h>
#include <stdio.h>
#include "Eventlog.h"
#include "HistoryLog.h"
#include "License.h"


extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  OnBoardCalDataStruct   g_OnBoardCalData;



//////////////////////////////////////////////////////
// ExtruderControlAlgotithm( void )         from ASM = ALGOR1
// Routine calculates error and initiates control action.
//
//
//////////////////////////////////////////////////////

void ExtruderControlAlgotithm( void )
{
   BOOL    bControlAllowed = TRUE;
    if(g_nSuspendControlCounter != 0)
    {
        g_nSuspendControlCounter--;
        ResetKgHRoundRobin();               // RESRROB - Reset kg/h round robin.
        ResetGPMRoundRobin();               //ASM = RESRROB = Reset gpm round robin
    }
    else
    {
        if(g_bLevelSensorAlarmOccurred & (g_bManualAutoFlag == AUTO))  //sbb--todo--
        {
            g_nLevelSensorAlarmCounter++;
            if(g_nLevelSensorAlarmCounter > MAXIMUM_CONTROL_LEVEL_SENSOR_ALARMS)
            {
                g_nLevelSensorAlarmCounter = 0;
                bControlAllowed = FALSE;
                ToManual();                        // switch to manual mode.
                AddBlenderToManualStarvationEventToLog();
             }
            else
            {
                bControlAllowed = FALSE;
                g_nEstCtrUse = NO_OF_ESTIMATED_CYCLES;
                ResetKgHRoundRobin();               // RESRROB - Reset kg/h round robin.
                ResetGPMRoundRobin();               //ASM = RESRROB = Reset gpm round robin
            }
        }
        else
        {
            g_nLevelSensorAlarmCounter = 0;
        }
        if(((g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY) && (g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET)) || (g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE) && bControlAllowed) // blender in grams per meter mode ?
        {
            if((g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY) && (g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET))
            {
                g_fControlErrorPercentage = 100 * (g_CurrentRecipe.m_fTotalWeightLength - g_fWeightPerMeterRR)/g_fWeightPerMeterRR;
                if((g_CalibrationData.m_nControlMode & CONTROL_MODE_HAULOFF) != 0)
                {
                    g_fControlErrorPercentage *= -1;        // haul off control invert sign of error
                }

            }
            else
            if(g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)
            {
                g_fControlErrorPercentage = 100 * (g_CurrentRecipe.m_fTotalThroughput - g_fRRThroughputKgPerHour)/g_fRRThroughputKgPerHour;

            }
            if(g_fControlErrorPercentage < 0.0f)
            {
                g_fPositiveControlErrorPercentage = g_fControlErrorPercentage * -1;
            }
            else
            {
                g_fPositiveControlErrorPercentage = g_fControlErrorPercentage;
            }

            CheckAlarmLimit();                       // check the current alarm limit  ASM = CHECKALARMLIMIT
            if(g_bManualAutoFlag)

            {
                if(g_bExtruderControlStartUpFlag)       // if start up check if inside the alarm band.
                { /* STNWER */
                    g_nControlErrorCounter = 0;

                    if(g_fPositiveControlErrorPercentage < STARTUPCONTROLBANDPERCENTAGE)
                    {
                        g_bExtruderControlStartUpFlag = FALSE;               // reset start up flag
                        g_bExtruderControlStartUpTransition = FALSE;        // reset start up transition flag
                        g_nAfterStartUpCtr = NOOFCORRECTIONSAFTERSTARTUP; // no of corrections after start up.
                        if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CONTROLALARMBIT ) // --review-- may not be necessary
                        RemoveAlarmTable(CONTROLALARM,0 );       // indicate Control alarm cleared.
                    }
                }
                else // out of start up
                {
                    if(g_fPositiveControlErrorPercentage > STARTUPCONTROLBANDPERCENTAGE)
                    {
                        g_nControlErrorCounter++;
                        if(g_nControlErrorCounter > MAXCONTROLERRORPASSES)
                        {
                            g_nControlErrorCounter = 0;
                            // asm TERCT2
                            g_bExtruderControlStartUpFlag = FALSE;               // reset start up flag
;                           g_bExtruderControlStartUpTransition = FALSE;        // reset start up transition flag
                                                                                /* No need to check alarm limit here done already */
                            ToManual();                                         // switch blender to manual
                            AddBlenderToManualControlAlarmEventToLog();

                        }
                        else
                        {
                            InitiateCorrection();   // initiate correction.
                            ResetGPMRoundRobin();
                            ResetKgHRoundRobin();
                        }
                    }

                }

                InitiateCorrection();   // initiate correction.
            }
            else
            {
                g_nControlErrorCounter = 0;
            }



        }

        else
        if((g_CalibrationData.m_nBlenderMode == MODE_MAXTHROUGHPUTZT) && (g_CalibrationData.m_nMaxThroughput != 0))  // blender max throughput 0 -10 volts ?
        {
            CalculateCMRVoltage();           // calculate the CMR 0-10 volts based on kg/h // ASM = CALCCMR
        }
    }
}


//////////////////////////////////////////////////////
// InitiateCorrection( void )           ASM = MXERCK
// Calculate the correction required and send it down
// to the SEI module
//
//
//////////////////////////////////////////////////////

void InitiateCorrection( void )

{
   BOOL bDoCorrection;  // indicates that correction should take place
    unsigned int nTachoSetpoint,nTempExtruderZeroToTenDASetPointValue;
    CheckCalibrationDataChecksum();  // nbb--todolp-- check if this is okay
    bDoCorrection = TRUE;
    if(g_bManualAutoFlag)
    {
        if(g_fPositiveControlErrorPercentage > CONTROLDEADBANDPERCENTAGE)  // only correct if > deadband
        {
            if(g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10) // 0 -10 v
            {
                nTempExtruderZeroToTenDASetPointValue =(int) ((g_nExtruderDAValue * (g_fControlErrorPercentage / 100) * g_CalibrationData.m_fControlGain) + g_nExtruderDAValue);

               if(nTempExtruderZeroToTenDASetPointValue > MAXEXTRUDERDTOA) // limit correction to 4095  ASM = DACLIM   --review-- is a min value required ?
               {
                   nTempExtruderZeroToTenDASetPointValue =  MAXEXTRUDERDTOA;
                   ToManual();  //
                   if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CONTROLALARMBIT) == 0 ) /* bit not set???  */
                   {
                        g_sOngoingHistoryEventsLogData.m_nControlAlarmsCtr++;
                        PutAlarmTable( CONTROLALARM,  0 );     /* indicate alarm, Component no. is zero. PTALTB */
                   }
               }


                if(g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10) // 0 -10 v
                {
                    g_nTempExtruderDAValue = LimitCorrectionForZeroToTen(nTempExtruderZeroToTenDASetPointValue,g_nExtruderDAValue );                        // limit inc /dec correction.
                }
            }

            else
            if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
            {
                nTachoSetpoint = (int)((g_nExtruderTachoReadingAverage * (g_fControlErrorPercentage / 100) * g_CalibrationData.m_fControlGain ) + g_nExtruderTachoReadingAverage);
                g_nTempExtruderDAValue = LimitCorrectionForIncDec(nTachoSetpoint,g_nExtruderTachoReadingAverage );                        // limit inc /dec correction.
            }

            if(!g_bExtruderControlStartUpFlag)                 // out of start up ?
            {
                if(g_nAfterStartUpCtr ==0)  // yes
                {
                    if(!g_CalibrationData.m_bCorrectEveryNCycles)
                    {
                        g_nCorrectEveryNCycleCtr++; // increment correction counter
                        if(g_nCorrectEveryNCycleCtr > g_CalibrationData.m_nControlAvgFactor)
                        {
                            g_nCorrectEveryNCycleCtr = 0;           // reset counter
                            bDoCorrection = TRUE;

                        }
                        else
                        {
                            bDoCorrection = FALSE;
                        }

                    }
                }
                else
                {
                    g_nAfterStartUpCtr--;   // decrement after start up counter
                }

            }
            if(g_nControlErrorCounter != 0)
            {
                bDoCorrection = TRUE;
            }
            if(bDoCorrection)  //DOCORR
            {
                if(g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10) // asm = INCREASEDECREASE
                {
                    g_nExtruderDAValue = g_nTempExtruderDAValue;
                    g_nExtruderControlZeroToTenSetpoint = g_nTempExtruderDAValue;  // update extruder value
                    g_arrnWriteSEIMBTable[MB_SEI_DA] = g_nExtruderControlZeroToTenSetpoint;
                }
                else
                if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
                {
                    g_nExtruderControlSetpointIncDec = g_nTempExtruderDAValue;  // update extruder value
                    g_arrnWriteSEIMBTable[MB_TACHO_SETPOINT] = g_nExtruderControlSetpointIncDec; //
                }
            }
            else
            {
            }

        }

    }
}


//////////////////////////////////////////////////////
// AutoManualToggle( void )           ASM = AMKEY
// Toggle Auto / Manual
//
//
// P.Smith                             3-2-2005
// First pass
//////////////////////////////////////////////////////


void AutoManualToggle( void )

{
    if(g_CalibrationData.m_nBlenderMode != MODE_OFFLINEMODE)
    {
        if(g_nPauseFlag)
        {
            ToManual();  // --review--  should the blender be switched to manual if already paused ?
            g_bToManualEvent = TRUE;
            g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;

        }
        else
        if(g_bManualAutoFlag == MANUAL)
        {
            ToAuto();       // switch to auto.
            g_bToAutoEvent = TRUE;
            g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;

        }
        else
        {
            ToManual();     // switch to manual
            g_bToManualEvent = TRUE;
            g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;

        }
    }
}


//////////////////////////////////////////////////////
// CheckForMaxExtruderPercentage( void )           ASM = CMXDA1
// Toggle Auto / Manual
//
//
// P.Smith                             14-02-2005
// First pass
//////////////////////////////////////////////////////


void CheckForMaxExtruderPercentage()
{
float   ftemp,ftemp1;
    ftemp = (float) g_nTempExtruderDAValue;     // convert to float
    ftemp1 = (100 * ftemp / MAXEXTRUDERDTOA);   // calculate max % allowed
    if(ftemp1 >= g_CalibrationData.m_fMaxExtruderSpeedPercentage)
    {
        g_nTempExtruderDAValue = (int)(MAXEXTRUDERDTOA * g_CalibrationData.m_fMaxExtruderSpeedPercentage/100);   // set to max d/a allowed.
    }
}




//////////////////////////////////////////////////////
// ToAuto( void )                         ASM = TOAUTO
// Switch blender to Auto
//
//
// P.Smith                              4-2-2005
// First pass
//////////////////////////////////////////////////////


void ToAuto( void )
{
	BOOL bAutoAllowed = FALSE;
    if((g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY) && (g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET))
    {
    	if(g_OnBoardCalData.m_cPermanentOptions & LICENSE_GPMOPTIONBIT)//gpm mode
    	{
    		bAutoAllowed = TRUE;
    	}
    }
    else
    if(g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)
    {
       	if(g_OnBoardCalData.m_cPermanentOptions & LICENSE_KGHROPTIONBIT)//gpm mode
        {
            bAutoAllowed = TRUE;
        }
    }
    if(g_CalibrationData.m_nBlenderMode < MODE_GPMGPMENTRY)   // not g/m mode   --review--
    {
    	if(bAutoAllowed)
    	{
    		GoToAuto();
    	}
    }
    else
    {
    	if(g_fLineSpeed5sec > MINIMUMLINESPEED)
    	{
    		if(bAutoAllowed)
    		{
    			GoToAuto();
    		}
    	}
    }
}







//////////////////////////////////////////////////////
// GoToAuto( void )                         ASM = GOTOAUTO
// Switch blender to Auto and initiate start up
//
//
// P.Smith                              3-3-2005
//////////////////////////////////////////////////////


void GoToAuto( void )
{
    if(!g_bStopped)
    {
        g_bManualAutoFlag = AUTO;
        g_bAutoTransition = TRUE;
        SetVolumetricExtruderSpeed();  // set extruder speed volumetrically  ASM=    NEWVOLSET
        SEIAutoLampOn();  // set SEI Auto Manual lamp on.     ASM =   SEIAMLAMPON  ; A/M LAMP ON FOR SEI
        SetExtruderControlStartup();                     //         ASM =   SETSTART
        g_bToAutoEvent = TRUE;
        g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;

    }
}



//////////////////////////////////////////////////////
// GoToAuto( void )                         ASM = GOTOAUTO
// Switch blender to Auto and initiate start up
//
//
// P.Smith                              3-3-2005
//////////////////////////////////////////////////////


void SetVolumetricExtruderSpeed( void )
{

    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        SetVolumetricExtruderSpeedForIncreaseDecrease();
    }
    else
    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10) // asm = INCREASEDECREASE
    {
        SetVolumetricExtruderSpeedZeroToTen();  // set extruder speed volumetrically  ASM=    NEWVOLSET
    }

}

/*
TOAUTO:
       LDAA    BLENDERMODE                        ; CHECK OPERATING MODE.
       CMPA    #GPMGPMENTRY                    ; G/M MODE.
       BLO     GOTOATO                         ; NO
       LDD     LSPD5S+2                         ;
       BNE     GOTOATO                          ; EXIT.
       LDAA     LSPD5S+4                        ; READ DECIMAL PART
       CMPA     #$25
       BLO      AMKYXX                          ; EXIT.
GOTOATO:
       TST    STOPPED
       BNE    AMKYXX  ; NO AUTO IF STOPPED
       LDAB   #$AA
       STAB   MANATO   ;~SET MAN/AUTO FLAG TO AUTO
       STAB   EATOFG   ;~SET FLAG FOR ENTERING AUTO
       STAB   AMANTR   ;SET TRANSITION FLAG
       JSR    NEWVOLSET
       JSR      SEIAMLAMPON  ; A/M LAMP ON FOR SEI.
SETSTART:
       LDAB   #$AA
       STAB   STRUPF   ;SET START-UP MODE
       STAB   STUPTR   ;SET TRANSITION FLAG
       CLR    CORRECTEVERYNCYCLECTR
AMKYXX RTS

*/




//////////////////////////////////////////////////////
// void SetExtruderControlStartup        ASM = SETSTART
// Set start up mode
//
//
// P.Smith                              4-2-2005
//////////////////////////////////////////////////////

void SetExtruderControlStartup()
{
    if(g_bManualAutoFlag)
    {
        g_bExtruderControlStartUpFlag = TRUE;
        g_bExtruderControlStartUpTransition = TRUE;
        g_nCorrectEveryNCycleCtr = 0;                   // reset correction counter if start up mode
    }
}



/*
TOAUTO:
       LDAA    BLENDERMODE                        ; CHECK OPERATING MODE.
       CMPA    #GPMGPMENTRY                    ; G/M MODE.
       BLO     GOTOATO                         ; NO
       LDD     LSPD5S+2                         ;
       BNE     GOTOATO                          ; EXIT.
       LDAA     LSPD5S+4                        ; READ DECIMAL PART
       CMPA     #$25
       BLO      AMKYXX                          ; EXIT.
GOTOATO:
       TST    STOPPED
       BNE    AMKYXX  ; NO AUTO IF STOPPED
       LDAB   #$AA
       STAB   MANATO   ;~SET MAN/AUTO FLAG TO AUTO
       STAB   EATOFG   ;~SET FLAG FOR ENTERING AUTO
       STAB   AMANTR   ;SET TRANSITION FLAG
       JSR    NEWVOLSET
       JSR      SEIAMLAMPON  ; A/M LAMP ON FOR SEI.
SETSTART:
       LDAB   #$AA
       STAB   STRUPF   ;SET START-UP MODE
       STAB   STUPTR   ;SET TRANSITION FLAG
       CLR    CORRECTEVERYNCYCLECTR
AMKYXX RTS

*/

//////////////////////////////////////////////////////
// ToManual( void )                         ASM = TOAUTO
// Switch blender to manual
//
//
// P.Smith                              4-2-2005
// First pass
//////////////////////////////////////////////////////


void ToManual( void )
{
    ResetKgHRoundRobin();               // RESRROB - Reset kg/h round robin.
    ResetGPMRoundRobin();               //ASM = RESRROB = Reset gpm round robin
    g_bManualAutoFlag = MANUAL;
    g_bAutoTransition = FALSE;
    g_bExtruderControlStartUpFlag = FALSE; // reset start up flag
    g_bExtruderControlStartUpTransition = FALSE;
    SEIAutoLampOff();
}


/*
TOMANUAL:
       JSR    RESRROB ; RESET ROUND ROBIN VALUE.
       JSR    EXCTRL
       CLR    AMANTR   ;CLEAR TRANSITION FLAG
       CLR    STRUPF   ;CLEAR START-UP FLAGS
       CLR    STUPTR
       JSR      SEIAMLAMPOFF   ; A/M LAMP OFF FOR SEI.
       BRA    AMKYXX   ;~QUIT

EXCTRL JSR    AMLMPOF                   ;~TURN AUTO/MANUAL LAMP OF
       CLR    MANATO            ;~SET MANUAL/AUTO FLAG TO MANUAL.
       RTS

*/

/*


;  AUTO/MANUAL KEY, IN AUTO GO TO MANUAL AND VICE VERSA
;
AMKEY  LDAA   BLENDERMODE
       CMPA   #OFFLINEMODE
       BEQ    AMKYXX
       LDAB   #$AA
       CMPB   PAUSFLG  ;~PAUSED MODE??
       BNE    AMKY02
       BRA    TOMANUAL
AMKY02 CMPB   MANATO   ;~ARE WE IN AUTO OR MANUAL
       BNE    TOAUTO   ;~NO, THEN ENTER AUTO
TOMANUAL:
       JSR    RESRROB ; RESET ROUND ROBIN VALUE.
       JSR    EXCTRL
       CLR    AMANTR   ;CLEAR TRANSITION FLAG
       CLR    STRUPF   ;CLEAR START-UP FLAGS
       CLR    STUPTR
       JSR      SEIAMLAMPOFF   ; A/M LAMP OFF FOR SEI.
       BRA    AMKYXX   ;~QUIT
TOAUTO:
       LDAA    BLENDERMODE                        ; CHECK OPERATING MODE.
       CMPA    #GPMGPMENTRY                    ; G/M MODE.
       BLO     GOTOATO                         ; NO
       LDD     LSPD5S+2                         ;
       BNE     GOTOATO                          ; EXIT.
       LDAA     LSPD5S+4                        ; READ DECIMAL PART
       CMPA     #$25
       BLO      AMKYXX                          ; EXIT.
GOTOATO:
       TST    STOPPED
       BNE    AMKYXX  ; NO AUTO IF STOPPED
       LDAB   #$AA
       STAB   MANATO   ;~SET MAN/AUTO FLAG TO AUTO
       STAB   EATOFG   ;~SET FLAG FOR ENTERING AUTO
       STAB   AMANTR   ;SET TRANSITION FLAG
       JSR    NEWVOLSET
       JSR      SEIAMLAMPON  ; A/M LAMP ON FOR SEI.
SETSTART:
       LDAB   #$AA
       STAB   STRUPF   ;SET START-UP MODE
       STAB   STUPTR   ;SET TRANSITION FLAG
       CLR    CORRECTEVERYNCYCLECTR
AMKYXX RTS

*/


//////////////////////////////////////////////////////
// void CalculateDAPerKg()              ASM = CALCDACPKG
// Calculates d/a per kg  and d/a per meter per minute
//
//
// P.Smith                              4-2-2005
//////////////////////////////////////////////////////

void CalculateDAPerKg()  //CALCDACPKG
{
    if(g_nDisableDTAPerKgCtr == 0)
    {
        if(!g_bDisableControlAction && !g_bScrewIncreaseDeceasePressed && (g_nCleanCycleSeq == 0) )  // no calculation if disabled this time
        {
            {
                if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC)
                {
                    g_ProcessData.m_fDtoAPerKilo = g_nExtruderTachoReading/g_fThroughputKgPerHour; // d/a / kg/h throughput
                    g_ProcessData.m_fRPMPerKilo = g_fExtruderRPM/g_fThroughputKgPerHour; // rpm per kg/hr
                    g_bSaveAllProcessDataToEEprom = TRUE;

                 if((g_fThroughputKgPerHour > 0.05) && (g_fLineSpeed5sec > 0.05))
                {
                    g_ProcessData.m_fDtoAPerMeterPerMinute = g_nExtruderTachoReading/g_fLineSpeed5sec; // d/a / meter / min
                    g_bSaveAllCalibrationToEEprom = TRUE;
                }
                g_nDACPerKgDoneFlag = TRUE;  // --review this flag may not be necessary ?
            }
            else
            if(g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10) // 0 -10 v

            {
                if(g_nExtruderDAValue > MINIMUMDAVALUEFORDA)
                {
                    if(g_fLineSpeed5sec > 0.05) // check for divide by zero.
                    {
                        g_ProcessData.m_fDtoAPerMeterPerMinute = g_nExtruderDAValue/g_fLineSpeed5sec; // d/a / meter /min
                    }
                    g_nDACPerKgDoneFlag = TRUE;  // --review this flag may not be necessary ?
                }
                if(g_fThroughputKgPerHour > 0.05)
                {
                    g_ProcessData.m_fDtoAPerKilo = g_nExtruderDAValue/g_fThroughputKgPerHour; // d/a / kg/h throughput
                    g_bSaveAllProcessDataToEEprom = TRUE;
                }
                g_nDACPerKgDoneFlag = TRUE;  // --review this flag may not be necessary ?
            }
        }
    }
    g_bScrewIncreaseDeceasePressed = FALSE;
    }
    else
    {
        g_nDisableDTAPerKgCtr--;
    }
}



/*

; CALCULATION OF D/A COUNTS PER KG/HR VALUE AND CALCULATION OF NEW D/A TARGET
;
;
CALCDACPKG:
        TST     DISCONACTION    ; DISABLE CONTROL ACTION ON THE NEXT CYCLE.
        LBNE     NOESTIM
        TST     SCREWINCDECPR   ; HAS SOMEBODY PRESSED "INC/DEC" KEYS
        LBNE     YESSOMEONEHAS
        LDAA    CONTROLTYPE     ; CHECK MODE
        CMPA    #INCREASEDECREASE
        BNE     NORDA           ; NORMAL D/A
        LDD     SEIADREADING
        BRA     CHKIDDA
NORDA   LDD     DA1TMP          ;
CHKMNDA CPD     #MINDAPKGCALC   ; MINIMUM D/A PER KG CALCULATION.
        LBLO     NOESTIM         ; NO ESTIMATE
CHKIDDA PSHM    D               ; SAVE D.

; CALCULATE D/A PER METER PER MINUTE.
;
; LINE SPEED STORED AS XX XX .YY IN LSPD2S + 2.


        JSR     CLCREG          ; C REGISTER CLEARED.
        PULM    D
        PSHM    D
        JSR     HEXBCD2         ; CONVERT TO BCD
        STE     MATHCREG
        JSR     CLAREG          ;
        LDX     LSPD5S+2        ; METERS / MINUTES INTEGER.
        STX     AREG+2
        LDAA    LSPD5S+4
        STAA    AREG+4          ; DAC VALUE / KG/HR
        JSR     DIV             ; XX XX .00 00 00 / 00 00 XX XX YY
        LDX     #DACPMPM        ; D/A PER METER PER MINUTE
        JSR     IFEREG
        JSR     CLCREG7
        JSR     CLAREG7

        PULM    D
        JSR     HEXBCD2         ; CONVERT TO BCD
        STAB    MATHCREG+2
        STE     MATHCREG+3
        LDX     #$0006
        STX     MATHCREG
        LDX     #$0004
        STX     AREG
        LDX     TPTKGH          ; KG/HR READING
        STX     AREG+2
        LDAA    TPTKGH+2
        STAA    AREG+4          ; DAC VALUE / KG/HR
        JSR     FDIV             ; XX XX .00 00 00 / 00 00 XX XX YY
        JSR     FPTINC          ; TO INTEGER.
        LDX     MATHDREG+2
        STX     DACPKGHR
        LDX     MATHDREG+4
        STX     DACPKGHR+2
        LDAA    MATHDREG+6
        STAA    DACPKGHR+4
        LDD     #DACPKGDONEID   ; D/A PER KG CALCULATED.
        STD     DACPKGDONEFLAG  ;
NOESTIM RTS

YESSOMEONEHAS:
        CLR     SCREWINCDECPR   ; RESET FOR THE NEXT CYCLE.
        RTS

*/

//////////////////////////////////////////////////////
// void CalculateDAPerKgCMR()          ASM = CLDACPKGCMR
// Calculates d/a per kg  for CMR
//
//
//
// P.Smith                              7-2-2005
//////////////////////////////////////////////////////



void CalculateDAPerKgCMR( void )
{
    if(g_CalibrationData.m_nBlenderMode == MODE_MAXTHROUGHPUTZT)
    {

        g_ProcessData.m_fDtoAPerKiloCMR = MAXEXTRUDERDTOA / g_CalibrationData.m_nMaxThroughput;
        g_bSaveAllProcessDataToEEprom = TRUE;
    }

}


/*

CLDACPKGCMR:
;        LDAA     BLENDERMODE     ; READ BLENDER MODE.
;        CMPA     #MAXTHROUGHPUTZT ; 0 -10VOLTS OPTION.
;        BNE     XITDAK
        JSR     CLCREG          ; C REGISTER CLEARED.
        LDE     #$4095
        STE     MATHCREG
        JSR     CLAREG          ;
        LDX     MXTPT1          ; KG/HR READING
        STX     AREG+2
        LDAA    MXTPT1+2
        STAA    AREG+4          ; DAC VALUE / KG/HR
        JSR     DIV             ; XX XX .00 00 00 / 00 00 XX XX YY
        LDX     #DACPKGCMR      ; D/A PER KG/HR
        JSR     EMOVE
XITDAK  RTS


*/

//////////////////////////////////////////////////////
// void EstimateKgPerHour()          ASM = ESTKGHR
// Estimate kg/hr from the extruder d/a value
// D/a per kg * d/a value -> estimated kg/hr
//
//
//
// P.Smith                              7-2-2005
//////////////////////////////////////////////////////

void EstimateKgPerHour( void )
{
    if((g_CalibrationData.m_nControlMode & CONTROL_MODE_HAULOFF) != 0)
    {
        if(g_fLineSpeed5sec > 0.05)
        {
        g_fWeightPerMeter = 1000 * g_fThroughputKgPerHour / (g_fLineSpeed5sec * 60);       // divide kg in 1 hour by the meters in one hour
        g_fWeightPerMeterRR = g_fWeightPerMeter;                                  // transfer result to round robin value
        }
    }
    else

    {
        if((g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC)&& (g_ProcessData.m_fRPMPerKilo > 0.001f) )    // kwh
        {
            g_fThroughputKgPerHour = g_fExtruderRPM / g_ProcessData.m_fRPMPerKilo;
            g_fRRThroughputKgPerHour = g_fThroughputKgPerHour;
        }
        else
        if((g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10) && (g_ProcessData.m_fDtoAPerKilo > MINIMUM_DTOA_PER_KIL0))       //
        {
            g_fThroughputKgPerHour = g_nExtruderDAValue / g_ProcessData.m_fDtoAPerKilo;
            g_fRRThroughputKgPerHour = g_fThroughputKgPerHour;
        }

    }



        if(g_fLineSpeed5sec > 0.05)
        {
        g_fWeightPerMeter = 1000 * g_fThroughputKgPerHour / (g_fLineSpeed5sec * 60);     // divide kg in 1 hour by the meters in one hour
        g_fWeightPerMeterRR = g_fWeightPerMeter;                                  // transfer result to round robin value
        }

//--todolp--        CalculateAcutalMircons();             ASM =   CALC_ACTMIC     ; RELATE THIS BACK TO MICRONS.

// insert CONVKGHRI inline
        g_fThroughputLbPerHour = g_fBatchKgPerHour * THROUGHPUT_CONVERSION_FACTOR;  // ASM = CONVKGHRI
// insert CONVKGHRRRI inline
        g_fRRThroughputLbPerHour = g_fRRThroughputKgPerHour * THROUGHPUT_CONVERSION_FACTOR; // ASM = CONVKGHRRRI
// insert CONVGPM in line
        // convert to English units.
        g_fLbsPer1000Feet = g_fWeightPerMeterRR * WEIGHT_PER_LENGTH_CONVERSION_FACTOR; // ASM = CONVGPM
        CopyKghToMB();//                      ASM =   CPYKGHMB
        CopyGPMToModbus();                    //  ASM = CPYGPMMB
         CalculateGramsPerSquareMeter();       //ASM = GPMTOGRAMPERSM
}



/*
;;

ESTKGHR LDAA    CRLMODE         ; CHECK MODE OF OPERATION.
        CMPA    #HAULOFF        ; HAUL OFF CONTROL.
        BEQ     NOKGHRE         ; SKIP KG/H ESTIMATION.
        JSR     CLCREG7:
        JSR     CLAREG7:

        LDX     #$0006          ;
        STX     AREG
        LDX     DACPKGHR        ; D/A PER KG
        STX     AREG+2
        LDX     DACPKGHR+2      ;
        STX     AREG+4
        LDAA    DACPKGHR+4      ;
        STAA    AREG+6

        LDD     #$0006          ; SEI A/D READING
        STD     MATHCREG        ; TO C REGISTER.

        LDAA    CONTROLTYPE     ; CHECK MODE
        CMPA    #INCREASEDECREASE
        BNE     NORDA1          ; NORMAL D/A
        LDD     SEIADREADING
        BRA     CHKEST          ; CALCULATE ESTIMATE

NORDA1  LDD     DA1TMP          ; READ D/A VALUE
CHKEST  JSR     HEXBCD2         ; CONVERT TO BCD
        STAB    MATHCREG+2
        STE     MATHCREG+3
        JSR     FDIV             ; C/A
        JSR     FPTINC
        LDD     MATHDREG+3
        STD     TPTKGH
        STD     RRKGPH
        LDAA    EREG
        STAA    TPTKGH+2
        STAA    RRKGPH+2
NOKGHRE:
        JSR     CLAREG
        LDX     #LSPD5S         ; ESTIMATE G/M
        JSR     ITCREG
        LDAA    #6
        STAA    AREG+4
        JSR     MUL             ; LSPEED * 6

        JSR     CLCREG
        LDX     TPTKGH
        STX     MATHCREG
        LDAA    TPTKGH+2
        STAA    MATHCREG+2
        LDX     #EREG
        JSR     ITAREG          ; L SPEED XX.YY METERS / HOUR
        JSR     DIV             ; C/A KG.HR XXXX.YY0000 / XX.YYM *6
        LDD     EREG+2
        STD     WTPMVL
        STD     RRGMPM
        LDAA    EREG+4
        STAA    WTPMVL+2
        STAA    RRGMPM+2
        LDX     #WTPMVL5
        JSR     IFEREG
        LDX     #RRGMPM5
        JSR     IFEREG

        JSR     CALC_ACTMIC     ; RELATE THIS BACK TO MICRONS.
        JSR     CONVGPMRRI      ; G/M IN IMPERIAL
        JSR     CONVKGHRRRI     ; KG/HR IN IMPERIAL
        JSR     CONVKGHRI       ; AS ABOVE
        JSR     CONVGPM
        JSR     CPYKGHMB
        JSR     CPYGPMMB
        JSR     GPMTOGRAMPERSM
        RTS



*/

//////////////////////////////////////////////////////
// void SetVolumetricExtruderSpeed()          ASM = NEWVOLSET
// Set volumetric extruder speed from the grams per meter set point
//  DToA value = Set Weight Per Length * LineSpeed * 60 * d/a per Kilo
//
//
// P.Smith                              7-2-2005
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// void SetVolumetricExtruderSpeed()          ASM = NEWVOLSET
// Set volumetric extruder speed from the grams per meter set point
//  DToA value = Set Weight Per Length * LineSpeed * 60 * d/a per Kilo
//
//
// P.Smith                              7-2-2005
//////////////////////////////////////////////////////

void SetVolumetricExtruderSpeedZeroToTen( void )
{
   unsigned int nExtruderSet,nFinalExtruderSet;
   nExtruderSet = 0;
   nFinalExtruderSet = 0;
   g_nIgnoreLSTime = 2;            // ignore level sensor time for the next two batches
   CheckCalibrationDataChecksum();  // nbb--todolp-- check if this is okay

    if((g_CalibrationData.m_nControlMode & CONTROL_MODE_HAULOFF) != 0)
   {
       SetVolumetricLineSpeed();    // ASM = NEWLSVOLSET
   }
   else
    {
        if(g_nDACPerKgDoneFlag)
        {
            if(g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)
            {
                nExtruderSet = (int) (g_CurrentRecipe.m_fTotalThroughput * g_ProcessData.m_fDtoAPerKilo);
            }
            else
            if(((g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY) && (g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET)) || (g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)) // blender in grams per meter mode ?
            {
                nExtruderSet = (int) (g_ProcessData.m_fDtoAPerKilo * ((g_CurrentRecipe.m_fTotalWeightLength * g_fLineSpeed5sec) * 60/1000));
            }

// merge this inline           CheckDToALimit5();                   //   ASM = DACLIM

            if(nExtruderSet > MAXEXTRUDERDTOA) // limit correction to 4095  ASM = DACLIM   --review-- is a min value required ?
            {
                nExtruderSet =  MAXEXTRUDERDTOA-5;
            }

            nFinalExtruderSet = LimitCorrectionForZeroToTen(nExtruderSet,g_nExtruderDAValue );                        // limit inc /dec correction.
            g_bRunEst = TRUE;
            g_bDisableControlAction = TRUE; // no control action on the next cycle.

        }
        g_nExtruderDAValue = nFinalExtruderSet;
    }
}

//////////////////////////////////////////////////////
// void SetVolumetricExtruderSpeedForIncreaseDecrease()          ASM = NEWVOLSET
// Set volumetric extruder speed from the grams per meter set point
//  DToA value = Set Weight Per Length * LineSpeed * 60 * d/a per Kilo
//
//////////////////////////////////////////////////////

void SetVolumetricExtruderSpeedForIncreaseDecrease( void )
{
    unsigned int nCalculatedTarget,nExtruderVolumetricTachoSetpoint;
    CheckCalibrationDataChecksum();  // nbb--todolp-- check if this is okay
    g_nIgnoreLSTime = 2;            // ignore level sensor time for the next two batches
    nCalculatedTarget = g_nExtruderTachoReadingAverage;  /* asm SEIADREADINGAVG  transfer tacho to d/a */
    if(g_nDACPerKgDoneFlag)
    {
        if(g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)
        {
            nCalculatedTarget = (int) (g_CurrentRecipe.m_fTotalThroughput * g_ProcessData.m_fDtoAPerKilo);
        }
        else
        if(((g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY) && (g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET)) || (g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)) // blender in grams per meter mode ?
        {
            nCalculatedTarget = (int) (g_ProcessData.m_fDtoAPerKilo * ((g_CurrentRecipe.m_fTotalWeightLength * g_fLineSpeed5sec) * 60/1000));
        }

        nExtruderVolumetricTachoSetpoint = LimitCorrectionForIncDec(nCalculatedTarget,g_nExtruderTachoReadingAverage );                        // limit inc /dec correction.
        g_nExtruderControlSetpointIncDec = nExtruderVolumetricTachoSetpoint;
        g_arrnWriteSEIMBTable[MB_TACHO_SETPOINT] = g_nExtruderControlSetpointIncDec;
        g_nEstimationContinueCounter = NUMBEROFESTIMATIONS; // for increase decrease mode / need to continue estimations for a few seconds.
        g_bDisableControlAction = TRUE; // no control action on the next cycle.

    }
}

//////////////////////////////////////////////////////
// void LimitCorrectionForIncDec()          ASM = NEWVOLSET loosely
// limit correction for inc dec.
//
//////////////////////////////////////////////////////

unsigned int LimitCorrectionForIncDec( unsigned int nNewTarget, unsigned int nOldTarget )
{
    float fPercentageChange;
    unsigned int nReturnedTarget;    // returned target
        if(nNewTarget > nOldTarget)  // check if new target > than old
        {
            fPercentageChange = (float)(nNewTarget - nOldTarget) / (float)nOldTarget;

            if(fPercentageChange >  PERCENTAGECHANGELIMITFORINCDEC) // check for % change in range
            {
                nReturnedTarget = nOldTarget + (int)(nOldTarget * PERCENTAGECHANGELIMITFORINCDEC);
            }
            else
            {
                nReturnedTarget = nNewTarget;
            }
        }
        else  // new less than old
        {
            fPercentageChange = float(nOldTarget - nNewTarget) / (float)nOldTarget;

            if(fPercentageChange >  PERCENTAGECHANGELIMITFORINCDEC)   // if > 10%, set to 10%
            {
                nReturnedTarget = nOldTarget - (int)(nOldTarget * PERCENTAGECHANGELIMITFORINCDEC);
            }
            else
            {
                nReturnedTarget = nNewTarget;
            }
        }
        return (nReturnedTarget);
}


//////////////////////////////////////////////////////
// void LimitCorrectionForZeroToTen()
//
//////////////////////////////////////////////////////

unsigned int LimitCorrectionForZeroToTen( unsigned int nNewTarget, unsigned int nOldTarget )
{

    unsigned int nReturnedTarget;    // returned target
        if(nNewTarget > nOldTarget)  // check if new target > than old
        {
            if((nNewTarget - nOldTarget) > (MAXEXTRUDERDTOA * MAXEXTRUDERDTOAPER))
            {
                nReturnedTarget = nOldTarget + (int)(MAXEXTRUDERDTOA * MAXEXTRUDERDTOAPER);
            }
            else
            {
                nReturnedTarget = nNewTarget;
            }
        }
        else
        {
            if((nOldTarget - nNewTarget) > (MAXEXTRUDERDTOA * MAXEXTRUDERDTOAPER))
            {
                nReturnedTarget = nOldTarget - (int)(MAXEXTRUDERDTOA * MAXEXTRUDERDTOAPER);
            }
            else
            {
                nReturnedTarget = nNewTarget;
            }
        }

        g_bCCVsFlag = TRUE; // ensure that calculation is done
        return (nReturnedTarget);
}




/*

 ;
;       THIS LIMITS CORRECTION TO 'PERLIM' EG THE LIMIT IS SET AT 819, IE 20%
;       IF INCREASE /DECREASE USE PERLIMIDC % LIMIT FOR INCREASE / DECREASE CONTROL.
;       ELSE PERLIMNC FOR NORMAL CONTROL.


LIMITCORR:
        PSHM    D               ; SAVE CORRECTION.
        LDAA    CONTROLTYPE     ; CHECK CONTROL TYPE.
        CMPA    #INCREASEDECREASE ;
        BNE     NORMLC          ; NORMAL CONTROL.
        LDD     #PERLIMIDC      ; PER LMIIT FOR INCREASE DECREASE CONTROL.
        BRA     PROCLC          ; PROCEED WITH LIMIT CHECK.
NORMLC  LDD     #PERLIMNC       ; % LIMIT FOR NORMAL CONTROL.
PROCLC  STD     PERLIMSTO       ; % LIMIT STORAGE
        PULM    D
        PSHM    D               ; SAVE NEW D/A SETTING
        LDAB    OPTIONCODE
        ORAB    OPTIONCODE+1    ; CHECK OPTION
        LDAA    BLENDERMODE     ; CHECK MODE OF OPERATION.
        CMPA    #KGHCONTROLMODE ; KG/HR MODE ?
        BNE     NOTKGM          ; NO.
        TBA                     ; CHECK BIT SETTING
        ANDA    #KGHROPTIONBIT  ; KG/H OPTION ENABLED ?
        BNE     DOCORR          ; FUNCTION ENABLED - DO CORRECTION.
        PULM    D
        BRA     ERROREXIT       ; EXIT.

NOTKGM  CMPA    #GPMGPMENTRY    ; G/M
        BLO     DOCORR          ; THIS SITUATION SHOULD NOT ARISE
        TBA                     ; CHECK BIT SETTING
        ANDA    #GPMOPTIONBIT   ; KG/H OPTION ENABLED ?
        BNE     DOCORR          ; FUNCTION ENABLED - DO CORRECTION.
        PULM    D               ; RECOVER D REGISTER
        BRA     ERROREXIT       ; EXIT ON ERROR.

DOCORR  PULM    D               ; RECOVER D/A SETPOINT.
        PSHM    D               ; SAVE AGAIN.
        CPD     DA1TMP          ;COMPARE NEW WITH OLD
        BHI     POSCHANGE       ; CHANGE IS POSITIVE
        LDE     DA1TMP          ; READ PREVIOUS SETTING
        SDE                     ; CHECK DIFFERENCE.
        CPE     PERLIMSTO         ;
        BHI     OVERLIMITN      ; OVER LIMIT
        PULM    D
        BRA     STOREMSET
OVERLIMITN:
        PULM    D
        LDD     DA1TMP
        SUBD    PERLIMSTO         ;
        BRA     STOREMSET

POSCHANGE:
        LDE     DA1TMP
        XGDE                    ; CHANGE TO GET ERROR DIRECTION CORRECT.
        SDE
        CPE     PERLIMSTO
        BHI     OVERLIMITP      ; OVER LIMIT
        PULM    D
        BRA     STOREMSET
OVERLIMITP:
        PULM    D

        LDD     DA1TMP
        ADDD    PERLIMSTO

STOREMSET:
        TDE                     ; SAVE RESULT.
        TST    CONTROLTYPE
        BNE     UPDATE          ; NO MAX CHECK..
        CPD    MAXDA1
        BLS    UPDATE
        LDE    MAXDA1
UPDATE  STE    DA1TMP  ; NEW CONTROL SETTING.
        JSR     STARTSEIDAWR
        LDAA    #$0F
        STAA    CCVSFLG            ;~FLAG TO CALL CALCVS
ERROREXIT:
        RTS










;
;       THIS PROGRAM CALCULATES THE NEW TARGET FOR THE SYSTEM USING THE
;       D/A PER KG VALUE, IF IN G/M MODE THE SYSTEM WILL CALCULATE THE
;       KG/HR TARGET FROM THE G/M SET POINT AND USE THIS TO CALCULATE THE
;       NEW D/A TARGET.



NEWVOLSET:
        LDAA    #TWO
        STAA    IGNORELSTIME
        LDAA    CONTROLTYPE
        CMPA    #INCREASEDECREASE
        BNE     NOINDE           ; NOT INCREASE /DECREASE
        LDD     SEIADREADINGAVG
        STD     DA1TMP          ; STORE INITIAL READING
NOINDE  LDAA     CRLMODE         ; CHECK CONTROL MODE.
        CMPA     #HAULOFF        ; HAUL OFF CONTROL ?
        LBEQ      NEWLSVOLSET     ; VOLUMETRIC SET VIA LINE SPEED.
        LDD     DACPKGDONEFLAG
        CPD     #DACPKGDONEID
        LBNE     XITVLS          ; EXIT VOLUMETRIC SET.
        LDAA    BLENDERMODE     ; CHECK THE SET POINT MODE IN OPERATION
        CMPA    #KGHCONTROLMODE       ; KG/HR MODE
        BEQ     ISKGPHRMODE     ; SOME OTHER ENTRY
        CMPA    #GPMGPMENTRY    ; IS THIS GRAM PER METER MODE ?
        LBLO     ERROREXIT

        JSR     CLRAC           ;
        LDX     #LSPD5S
        JSR     ITAREG
        LDX     WTPMSP
        STX     MATHCREG+1
        LDX     WTPMSP+2
        STX     MATHCREG+3
        CLR     MATHCREG

;        LDX     #WTPMSP         ;
;        JSR     CMOVE
        JSR     MUL             ; LINE SPEED ABCD.EF * G/M XX XX XX XX .YY
        LDX     #AREG
        JSR     IFEREG
        JSR     CLCREG
        LDAA    #6              ; ABOVE * 6 -> KG/HR XX XX .YY
        STAA    MATHCREG+4
        JSR     MUL


USETHISKG:
        LDX     EREG
        STX     MATHCREG+2
        LDAA    EREG+2
        STAA    MATHCREG+4
        CLRW    MATHCREG
        BRA     CALCNEW

ISKGPHRMODE:
        LDX     #TKGHSP
        JSR     CMOVE

CALCNEW LDX     #DACPKGHR
        JSR     ITAREG

CONTODA JSR     MUL             ; KG/HR SETPOINT * D/A PER KG/HR
        LDD     EREG
        LDX     #MATHDREG+4
        JSR     BCDHEX3X         ; CONVERSION
        TED
        TST    CONTROLTYPE
        BNE    NOMXID          ; NO MAX CHECK IN INCREASE DECREASE.
        JSR     DACLIM          ; CHECK DAC LIMIT
NOMXID  JSR     LIMITCORR       ; LIMIT CORRECTION TO XX %
        LDAA    #$0F
        STAA    DISCONACTION    ; DISABLE CONTROL ACTION ON THE NEXT CYCLE.

        LDAA    CONTROLTYPE
        CMPA    #INCREASEDECREASE
        BNE     NOTID
        LDAA    #NOOFESTIMATIONS        ;
        STAA    CONESTIMATION           ; NO OF TIMES TO CONTINUE ESTIMATION CALCULATION.
NOTID   LDAA    #1
        STAA    RUNEST
XITVLS  RTS

*/

//////////////////////////////////////////////////////
// SetVolumetricLineSpeed()                 ASM = NEWLSVOLSET
// Set volumetric extruder speed from the grams per meter set point
//  DToA value = Set Weight Per Length * LineSpeed * 60 * d/a per Kilo
//                       (
//
//
// P.Smith                              7-2-2005
//////////////////////////////////////////////////////

void SetVolumetricLineSpeed( void )
{
    unsigned int nLinespeedsp;
    nLinespeedsp =(int)((g_ProcessData.m_fDtoAPerMeterPerMinute * 1000 * g_fThroughputKgPerHour)/(g_CurrentRecipe.m_fTotalWeightLength * 60));
    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10) // 0 -10 v
    {
        if(nLinespeedsp > MAXEXTRUDERDTOA) // limit correction to 4095  ASM = DACLIM   --review-- is a min value required ?
        {
            nLinespeedsp =  MAXEXTRUDERDTOA;
        }
        g_nTempExtruderDAValue = LimitCorrectionForZeroToTen(nLinespeedsp,g_nExtruderDAValue );                        // limit inc /dec correction.
    }
    else
    {
             g_nTempExtruderDAValue = LimitCorrectionForIncDec(nLinespeedsp,g_nExtruderControlSetpointIncDec );                        // limit inc /dec correction.
    }
    g_nExtruderDAValue = g_nTempExtruderDAValue;  // update extruder value
    g_bDisableControlAction = TRUE; // no control action on the next cycle.
    g_nEstimationContinueCounter = NUMBEROFESTIMATIONS; // for increase decrease mode / need to continue estimations for a few seconds.
    g_bRunEst = TRUE;
}

/*

; CALCULATION OF LINE SPEED FROM KG/H AND GRAMS PER METER.
;
NEWLSVOLSET:
        JSR     CLRAC                   ; RESET REGISTERS.
        LDAA    #6
        STAA    AREG+4
;        LDD     WTPMSP                  ; GRAM PER METER SET POINT.
;        STD     MATHCREG+2
;        LDAA    WTPMSP+2
;        STAA    MATHCREG+4              ; STORE.

        LDD     WTPMSP                  ; GRAM PER METER SET POINT.
        STD     MATHCREG+1
        LDD     WTPMSP+2                ; .
        STD     MATHCREG+3              ; THIS WAS INCORRECT. (+2)


        JSR     MUL                     ; 6 X G/M SET POINT.
        LDX     #AREG                   ;
        JSR     IFEREG                  ; RESULT TO A.
        LDX     TPTKGH                  ; ACTUAL KG/H.
        STX     MATHCREG
        LDAA    TPTKGH+2
        STAA    MATHCREG+2
        JSR     DIV                     ; (KG /HR) / (6 X GRAMS PER METER.)

        JSR     CLRAC                   ; RESET.
        LDX     #AREG                   ; READ LINE SPEED. XX.YY METERS PER MINUTE.
        JSR     IFEREG                  ; LINE SPEED RESULT.
        LDX     #DACPMPM                ; D/A PER METER PER MINUTE.
        JSR     ITCREG                  ; TRANSFER.
        JSR     CONTODA                 ; CONVERT TO D/A
        LDAA    #NOOFESTIMATIONS        ;
        STAA    CONESTIMATION           ; NO OF TIMES TO CONTINUE ESTIMATION CALCULATION.
        RTS




*/


//////////////////////////////////////////////////////
// CheckForRoundRobinReset()                 ASM = RESRROB
// Checks if round robin should be reset.
// Note that the round robin is reset if in start up and if the
// calibration averaging factor has been changed
//
// P.Smith                              10-2-2005
//////////////////////////////////////////////////////


void CheckForRoundRobinReset()   // Check for round robin reset.
{
    if(g_bExtruderControlStartUpFlag)
    {
        ResetKgHRoundRobin();
    }
}


//////////////////////////////////////////////////////
// CheckForRoundRobinReset()                 ASM = RESRROB
// Checks if round robin should be reset.
// Note that the round robin is reset if in start up and if the
// calibration averaging factor has been changed
//
// P.Smith                              10-2-2005
//////////////////////////////////////////////////////


void CheckDToALimit1( void )           // check d/a limit for max
{
   if(g_nTempExtruderDAValue > MAXEXTRUDERDTOA) // limit correction to 4095  ASM = DACLIM   --review-- is a min value required ?
    {
    g_nTempExtruderDAValue =  MAXEXTRUDERDTOA-1;
    }
}

void CheckDToALimit2( void )           // check d/a limit for max
{
   if(g_nTempExtruderDAValue > MAXEXTRUDERDTOA) // limit correction to 4095  ASM = DACLIM   --review-- is a min value required ?
    {
    g_nTempExtruderDAValue =  MAXEXTRUDERDTOA-2;
    }
}


void CheckDToALimit3( void )           // check d/a limit for max
{
   if(g_nTempExtruderDAValue > MAXEXTRUDERDTOA) // limit correction to 4095  ASM = DACLIM   --review-- is a min value required ?
    {
    g_nTempExtruderDAValue =  MAXEXTRUDERDTOA-3;
    }
}


void CheckDToALimit6( void )           // check d/a limit for max
{
   if(g_nTempExtruderDAValue > MAXEXTRUDERDTOA) // limit correction to 4095  ASM = DACLIM   --review-- is a min value required ?
    {
    g_nTempExtruderDAValue =  MAXEXTRUDERDTOA-6;
    }
}

/*

SPCHANG TST     MANATO          ; AUTO
        BEQ     XITNCH          ; MANUAL EXIT.
        LDAA    #1
        STAA    ESTCTRUSE       ; USE ESTIMATED VALUE FOR THE NEXT CORRECTION.
        JSR     NEWVOLSET
XITNCH  RTS

*/


//////////////////////////////////////////////////////
// CheckAlarmLimit()               ASM = CHECKALARMLIMIT
// checks if control error is outside alarm band
//
// P.Smith                              14-2-2005
//////////////////////////////////////////////////////


void CheckAlarmLimit( void )   // Check for round robin reset.
{
    if(g_bManualAutoFlag)
    {
        if(!g_bExtruderControlStartUpFlag)                 // only run calculation if out of startup ?
       {
            if(g_fPositiveControlErrorPercentage >= (float)g_CalibrationData.m_nControlPercentageLimit)
            {
                if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CONTROLALARMBIT) == 0 ) /* bit not set???  */
                {
                    g_sOngoingHistoryEventsLogData.m_nControlAlarmsCtr++;
                    PutAlarmTable( CONTROLALARM,  0 );     /* indicate alarm, Component no. is zero. PTALTB */
                }
            }
            else
            {
                if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CONTROLALARMBIT ) /* set?  */
                {
                    RemoveAlarmTable( CONTROLALARM,  0 );     /* Remove alarm */
                }
            }
       }
    }
}



//////////////////////////////////////////////////////
// CalcCMRVoltage                      ASM = CALCCMR
// Calculates CMR voltage
// d/a =
//
//
// P.Smith                              10-2-2005
//////////////////////////////////////////////////////


void CalculateCMRVoltage( void )
{
    g_nCMRValidTimer = TIMEFORCMRVALID; // time for which cmr o/p is held on.
    g_nTempExtruderDAValue = (int) (g_ProcessData.m_fDtoAPerKiloCMR * g_fRRThroughputKgPerHour);
    CheckDToALimit6();
    g_nExtruderDAValue = g_nTempExtruderDAValue;  // update extruder value
    g_bCCVsFlag = TRUE;                       // CCVSFLG note that this allows the voltage to be sent to the sei
}


//////////////////////////////////////////////////////
// ExtruderStallCheck                      ASM = EXTSCHK
// Checks for extruder stall
// d/a =
//
//
// P.Smith                              16-3-2005
//////////////////////////////////////////////////////


void ExtruderStallCheck( void )
{
    if(g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS)   // sei enabled
    {
         if(!g_nToStallTime )      // appropriate to do stall check.         // TOSTALTIM
        {
              if(g_nSEIInputState & SEI_INPUT_EXTRUDER_STALL_BIT)     // stall input active ?
            {
//                iprintf("\n extruder stall bit detected");  //nbb--testonly--
                g_nExtruderStalledCounter++;        //increment stalled counter
                if(g_nExtruderStalledCounter >= SECONDSTOACCEPTSTALL) // stall for n seconds ?
                {
                    g_nExtruderStalledCounter = 0; // set extruder stalled counter to 0
                    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & SHUTDOWNALARMBIT) == 0 ) // shut down alarm bit not set???
                    {
                        if(g_nExtruderDAValue > MINDAFOREXTRUDERSTALL)  // only alarm if > d/a min
                        {
                            PutAlarmTable( SHUTDOWNALARM,  0 );     // indicate alarm, Component no. is zero.
                            ClearThroughputTotals();                // reset totals
                        }
                        g_nExtruderDAValue = 0; // DA1TMP.  - Shut down the extruder, setpoint voltage -> 0.
                        CalculateDToAPercentage();          // d/a % calculation
                        ToManual();             // switch blender to manual
                        AddBlenderToManualExtruderstalledEventToLog();
                        g_bManualChangeFlag = TRUE; // indicate transition to manual
                        g_nToStallTime = EXTRUDERSTALLINHIBITTIME;        // TOSTALTIM

                    }
                }
                else // no extruder stall
                {
                }
            }
            else // input not active
            {
                if(g_nExtruderDAValue > MINDAFOREXTRUDERSTALL)
                {
                    if(g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & SHUTDOWNALARMBIT) // shut down alarm bit set???
                    {
                        RemoveAlarmTable( SHUTDOWNALARM,  0 );     // clear alarm,
                    }
                }
                else
                {
                    g_nExtruderStalledCounter = 0; // set extruder stalled counter to 0
                }

            }

        }
        else
        {
            g_nExtruderStalledCounter = 0; // set extruder stalled counter to 0
        }
    }
}




/*
ALGOR1 NOP
        TST     LEVELSENALOC    ; INDICATE ALARM OCCURRED.
        BEQ     NOLSAL          ; NO LEVEL SENSOR ALARM
        INC     LEVELSENSORALARMCTR     ; RESET LEVEL SENSOR COUNTER
        LDAA    LEVELSENSORALARMCTR     ; RESET LEVEL SENSOR COUNTER
        CMPA    #LEVELSENSORALARMSACCEPTED
        BLO     ACCEPTFN                ; ACCEPT FOR NOW
        JSR     TOMANUAL
        BRA     EXIT04

ACCEPTFN:
        LDAA    #NOESTIMATEDCYCLES
        STAA    ESTCTRUSE       ; USE ESTIMATION FOR A FEW CYCLES.
        JSR     RESETROUNDROBIN
        BRA     EXIT04


NOLSAL  CLR     LEVELSENSORALARMCTR     ; RESET LEVEL SENSOR COUNTER
        LDAA    BLENDERMODE        ;
        CMPA    #GPMGPMENTRY-1    ; G/M
        BHI     YESGPMMODE      ;
        CMPA    #KGHCONTROLMODE ; KG/HR MODE
        BEQ     YESKGHMODE      ; KG/H MODE
        CMPA    #MAXTHROUGHPUTZT
        BNE     EXIT04          ; CALCULATE CORRECTION         ;
        TSTW    MXTPT1          ; KG/HR READING
        BEQ     YESKGHMODE
        JSR     CALCCMR  ; UPDATE D/A DEPENDING ON ACTUAL KG/HR
        BRA     EXIT04          ; EXIT

YESKGHMODE:
        LDX     #TKGHSP         ;
        STX     SETGPMORKGPHPTR ; SET POINTER
;        LDD     #$0100
;        STD     BATCHKGPHR+2
;        LDAA    #0
;        STAA    BATCHKGPHR+4
;        LDD     #0
;        STD     BATCHKGPHR

        LDX     #BATCHKGPHR     ;
          STX     ACTGPMORKGPHPTR ; ACTUAL VALUE POINTER
        BRA     CONALG1         ; CONTINUE
;
;       GRAM PER METER MODE
;

YESGPMMODE:
        LDX     #WTPMSP
        STX     SETGPMORKGPHPTR ; SET POINTER
        LDX     #BATCHGPERMETER;
        STX     ACTGPMORKGPHPTR ; ACTUAL VALUE POINTER

CONALG1:
;
;       VARIABLE IS XX XX XX XX .YY
;
        LDX     ACTGPMORKGPHPTR  ;
        LDD     #0008
        STD     AREG            ; SET EXPONENT
        LDD     0,X
;;P        LDD     #0
        STD     AREG+2          ; STORE DATA
        LDD     2,X
;;P        LDD     #$0060
        STD     AREG+4
        LDAA    4,X
;;P        LDAA    #$01
        STAA    AREG+6
        LDX     #AREG
        JSR     PAK             ; PACK RESULT


       LDAA   AREG
       ANDA   #$BF     ;~CHECK RESULT WITHIN LIMITS
       LBNE    EXIT04   ;~OUTSIDE LIMITS; JUMP
       LDAA   AREG+1   ;~CHECK EXP. MAGNITUDE
       CMPA   #$09     ;~EXP. > 9 ?
       LBHI    EXIT04   ;~YES; JUMP

; THIS SECTION CALCULATES A CORRECTION FACTOR TO BRING
; THE MEASURED VALUE TO THE SET POINT.


       LDAA     AREG
       ANDA   #$20     ;~TEST FOR NON-ZERO WT/PM VALUE
       BEQ    CALCOR   ;~NON-ZERO WT/MT VALUE
       JMP    EXTCTL   ;~NO FURTHER CONTROL ACTION

CALCOR  LDX     SETGPMORKGPHPTR ; SETPOINT
        LDAA    BLENDERMODE
        CMPA    #KGHCONTROLMODE
        BNE     GPMCON          ; GRAMS PER METER CONTROL.
        LDD     #$0004          ;
        STD     MATHCREG        ; TO  C REGISTER
        LDD     0,X             ;
        STD     MATHCREG+2      ;  SET POINT IS XX XX .YY
        LDAA    2,X             ;
        STAA    MATHCREG+4
        CLRW    MATHCREG+5      ; CLEAR
        BRA     KGHCON

GPMCON  LDD     #$0006          ;
        STD     MATHCREG        ; TO  C REGISTER
        LDD     0,X             ;
        STD     MATHCREG+2      ;  SET POINT IS XX XX .YY
        LDD     2,X
        STD     MATHCREG+4

KGHCON  JSR    FSUB     ;~CALCULATE ERROR SIZE
        LDX    #MATHCREG    ;~LOAD S.A. OF C-REG
        JSR    FFDREG   ;~COPY RESULT BACK TO C-REG
        JSR    FDIV     ;~CALCULATE % ERROR
NORCOR:


;CALCOR  LDX     SETGPMORKGPHPTR ; SETPOINT
;        LDD     #$0004          ;
;        STD     MATHCREG        ; TO  C REGISTER
;        LDD     0,X             ;
;        STD     MATHCREG+2      ;  SET POINT IS XX XX .YY
;        LDAA    2,X             ;
;        STAA    MATHCREG+4
;        CLRW    MATHCREG+5      ; CLEAR
;        JSR    FSUB     ;~CALCULATE ERROR SIZE
;        LDX    #MATHCREG    ;~LOAD S.A. OF C-REG
;        JSR    FFDREG   ;~COPY RESULT BACK TO C-REG
;        JSR    FDIV     ;~CALCULATE % ERROR
;NORCOR:

;      % ERROR STORED AS EG 80 00 16 66 66 66 66
;   THIS WOULD REPRESENT 16 %


       LDAA   MATHDREG     ;~LOAD SIGN OF NUMBER
;;P       ANDA   #$F0     ;~ENSURE EXP. MSD = 0
       LDAB   CRLMODE
       CMPB     #HAULOFF
       BEQ      INSIGN  ; YES HAUL OFF CONTROL.
;;P       BNE    INSIGN   ;~YES; JUMP
       EORA   #$80     ;~INVERT SIGN
INSIGN STAA   MATHDREG     ;~STORE NEW SIGN BYTE
;
; THIS SECTION CHECKS MAGNITUDE OF ERROR. IF IN AUTO
; BUT NOT IN START-UP, IT IGNORES ERRORS OVER 5% UNLESS
; IT IS THE THIRD CONSECUTIVE LARGE ERROR. IN MANUAL
; OR START-UP THE ROUTINE IS BY-PASSED.
;
       LDAA   MANATO
       BNE    ISAUTO   ;~STORE IF IN MANUAL
       JSR    RESETROUNDROBIN
       LBRA   STNWER   ;~STORE IF IN MANUAL

ISAUTO LDAA   STRUPF
       BNE    STNWER   ;~STORE IF IN START-UP

       LDX     #TEMPDSTO
       JSR     FFDREG
       JSR     FPTINC   ; CONVERT TO AN INTEGER.
       LDD     MATHDREG+3   ; CHECK %
       BNE     ERTOLG   ;~EXP.=0; ERROR TOO LARGE
       LDD     EREG     ; CHECK DECIMAL POINT
       PSHM    D        ;
       LDX     #TEMPDSTO
       JSR     FTDREG   ; RESTORE VALUE.
       PULM    D
       CPD     #MAXCONTROLERROR   ; COMPARE TO MAX CONTROL ERROR
       BLS     STNWER   ; ERROR GREATER THAN LIMIT.

;       LDAB   MATHDREG
;       ANDB   #$40     ;~TEST EXP. SIGN
;       BEQ    ERTOLG   ;~EXP. +VE; ERROR TOO LARGE
;       LDAA   MATHDREG+1


;  PAGE 005  S5KALG24.SA:1

;       BEQ    ERTOLG   ;~EXP.=0; ERROR TOO LARGE
;       CMPA   #$01
;       BNE    STNWER   ;~EXP.>1; ERROR TOO SMALL
;       LDAA   #$90     ;~SET ERROR LIMIT AT 5.0%
;       CMPA   MATHDREG+2   ;~COMPARE ERROR SIZE
;       BHI    STNWER   ;~UNDER LIMIT; JUMP
ERTOLG INC    CERCT1   ;~INCREMENT ERROR COUNT


        LDX     #TEMPDSTO
        JSR     FFDREG
        JSR     FPTINC   ; CONVERT TO AN INTEGER.

        LDX     #PERERROR ; PERCENTAGE ERROR.
        LDAA    #$0F
        STAA    PERERRORUPD     ; SET UPDATE FLAG.
        LDAA    MATHDREG+4   ;
        STAA    0,X
        LDD     EREG
        STD     1,X

        LDX     #TEMPDSTO
        JSR     FTDREG          ; RESTORE VALUE.

       LDAA   CERCT1
       CMPA   #$02     ;~COUNT LIMIT EXCEEDED ?
       BLS    EXITA1   ;~NO
       CLR    CERCT1   ;~RESET ERROR COUNT
       CLR    AVRGK2   ;~RESET AVERAGING ALGORITHIM
       LDX    #AWTMER  ;~COPY ERROR TO RESULT STORE
       JSR    FFDREG
       LDX    #WTPMER  ;~COPY FOR NEXT LIMIT CHECK
       JSR    FFDREG
       LDX    #AWTMERAC  ;~LOAD S.A. OF AVERAGED RESULT
       JSR    FFDREG   ;~COPY RESULT
       JMP    MXERCK   ;~*** GOTO CORRECTION
STNWER CLR    CERCT1   ;~RESET ERROR COUNT
       LDX    #WTPMER  ;~LOAD UP FOR AVERAGING
       JSR    FFDREG
       LDX    #AWTMERAC  ;~LOAD S.A. OF AVERAGED RESULT
       JSR    FFDREG   ;~COPY RESULT
EXITA1 NOP
;
;


; THIS SECTION PERFORMS AVERAGING OF THE CALCULATED
; CORRECTION FACTOR ( ERROR ). THE ALGORITHIM HAS
; ESSENTIALLY THE SAME STRUCTURE AS THE LOAD CELL
; AVERAGING S/R.
;
; THIS PORTION MANAGES THE OPERATION OF THE ALGORITHIM
; AND CALL'S THE OTHER S/R'S.
;
AVERG2 NOP
       TST    STRUPF   ;~IS SYSTEM IN START UP
       BEQ    SYSINS   ;~BRANCH IF NOT
       LDAA   #01
       STAA   AVRGK2   ;~SET AVERAGING TO 1
       CLR    STUPTR   ;~CLEAR START UP TRANSITION
       BRA    CONRSA   ;~CONTINUE AND RESET AVERAGE
SYSINS
       LDAA   CONTAVGFACTOR   ;~LOAD AVERAGING FACTOR
       ANDA   #$0F     ;~MASK OUT MSD
       BEQ    NAVRG2   ;~NO AVERAGING:SUBSTITUTE INST. ERROR
       LDAB   STUPTR   ;~TEST START-UP TRANSITION FLAG
       BNE    RSAVG2   ;~RESET AVERAGING IF FLAG SET
       CMPA   AVRGK2   ;~CHECK FOR NEW AVERAGING FACTOR
       BEQ    STAVG2   ;~UN-CHANGED:BEGIN AVERAGING
RSAVG2 CLR    STUPTR   ;~CLEAR START-UP TRANSITION FLAG
       STAA   AVRGK2   ;~STORE NEW AVERAGING FACTOR
CONRSA JSR    RESETROUNDROBIN

;       CLR    SUMCT2   ;~RESET QUE #2 SAMPLE COUNTER
;       CLR    CDENP2   ;~RESET QUE #2 DATA POINTER
;       LDX    #DATST2  ;~LOAD QUE #2 DATA S.A.
;       STX    QCADR2   ;~STORE IN QUE #2 ADDRESS POINTER
;       LDX    #SUMTT2  ;~LOAD S.A. OF SUMMATION TOTAL
;       JSR    CLRNM2   ;~RESET SUMMATION TOTAL
STAVG2 JSR    Q2DAEN   ;~CALL INSERTION S/R #2
       JSR    SUMDT2   ;~CALL SUMMATION S/R #2
       JSR    DIVDT2   ;~CALL DIVISION S/R #2
       JSR     CALCRRAVERAGE   ; CALCULATE ROUND ROBIN AVERAGE.
       LDAA   AVRGK2
       CMPA   SUMCT2   ;~AVERAGE FULL ?
       BEQ    TAKACT   ;~YES; ACTIVATE CORRECTION

;  PAGE 006  S5KALG24.SA:1

        JMP    EXTCTL     ;~EXIT ALGORITHIM
TAKACT  LDX     #AWTMER  ; READ STARTUP RELATED READING.
        JSR     FTDREG   ;
        JSR     FPTINC   ; CONVERT TO AN INTEGER.

        LDX     #PERERROR ; PERCENTAGE ERROR.
        LDAA    #$0F
        STAA    PERERRORUPD     ; SET UPDATE FLAG.
        LDAA    MATHDREG+4   ;
        STAA    0,X
        LDD     EREG
        STD     1,X
;       CLR    AVRGK2   ;~RESET AVERAGING FACTOR
;       CLR    SUMCT2   ;~RESET SUMMATION COUNT
       JMP    MXERCK   ;~PROCEED TO LIMIT CHECK
NAVRG2 LDX    #AWTMER  ;~NO AVERAGING:COPY INST. ERROR
       JSR    DAMOV2   ;~COPY INST. ERROR
       BRA    TAKACT

;
MXERCK TST    MANATO   ;~SYSTEM IN AUTO ?
       BEQ    CTLGA2   ;~NO; BYPASS CHECKS
       TST    STRUPF   ;~SYSTEM IN START-UP
       BEQ    MXERC3   ;~NO; JUMP
       LDAB   AWTMER
       ANDB   #$40     ;~CHECK IF EXP. +VE
       BEQ    CTLGA2   ;~EXP +VE; JUMP
       LDAA   AWTMER+1  ;~EXP = 0 ?
       BEQ    CTLGA2   ;~YES; JUMP
       CMPA   #$01     ;~EXP >1 ?
       BEQ    MXERC2   ;~NO; JUMP
MXERC1 LDAA     #NOOFCORRAFTERSTARTUP
       STAA     AFTERSTARTUPCTR       ; NO OF CORRECTIONS AFTER STARTUP
       CLR    STRUPF   ;~RESET START-UP FLAGS
       CLR    STUPTR
       LDAA     MBPROALRM       ;
       ANDA     #CONTROLALARMBIT ; WAS THE ALARM THERE THE LAST TIME.
;       LBEQ      NOCONAL
       LBEQ      CTLGA2
;       LDAA   #CONTROLALARM     ;$40              ;~M.S. NIBBLE = 4 FOR CONTROL ALARM
;       JSR    RMALTB            ;~PUT ALARM INTO ALARM TABLE, WITH TIME
       BRA    CTLGA2
MXERC2 LDAA   AWTMER+2  ;~READ ERROR VALUE
       CMPA   #$95     ;~COMPARE WITH 9.5% LIMIT
       BHI    CTLGA2   ;~OVER LIMIT; JUMP
       BRA    MXERC1   ;~RESET START-UP
MXERC3 LDAB   AWTMER
       ANDB   #$40     ;~EXP TOO LARGE IF +VE
       BEQ    MXERC4   ;~+VE; JUMP + ABORT
       LDAA   AWTMER+1
       BEQ    MXERC4   ;~EXP TOO LARGE; JUMP
       CMPA   #$01     ;~EXP RANGE 1-9% ?
       BNE    CTLGA2   ;~EXP. TOO SMALL; JUMP
       LDAA   AWTMER+2  ;~READ ERROR VALUE
;;P       CMPA   ALMLMT   ;~COMPARE WITH PRESET LIMIT
        CMPA    #$80    ; LEAVE % LIMIT AT 8 %

       BLS    CTLGA2   ;~UNDER LIMIT; JUMP
MXERC4 INC    CTLEF1   ;~SET ERROR FLAG
       JMP    TERCT2   ;~RESET AUTO MODE
;
; THIS ROUTINE MULTIPLYS THE AVERAGED ERROR BY
; A CONTROL GAIN FACTOR. THE RANGE OF THIS
; FACTOR IS 0.00 TO 0.99.
;
CTLGA2 LDX    #AWTMER  ;~LOAD S.A. OF AVERAGED ERROR
       JSR    FTAREG   ;~COPY TO A-REG
       LDX    #MATHCREG    ;~LOAD START ADDRESS OF C-REG
       JSR    CLRNM2   ;~CLEAR C-REG
       LDAA   CLGAIN   ;~LOAD GAIN FACTOR
;;P        LDAA    #$80
       STAA   MATHCREG+2   ;~STORE IN MSB OF MAGNITUDE
       JSR    FMUL     ;~MULTIPLY ERROR BY GAIN FACTOR
       LDX    #AWTMER  ;~LOAD S.A. OF AVERAGED ERROR STORE
       JSR    FFDREG   ;~COPY RESULT TO STORE
;
; THIS ROUTINE EXAMINES THE SIZE OF THE AVERAGED
; ERROR TO SEE IF IT IS GREATER THAN A PRESET
; LIMIT IN THE RANGE 0.0% TO 0.9%. IF IT IS UNDER
; THIS LIMIT NO CONTROL ACTION WILL BE TAKEN.
;
       LDAA   AWTMER   ;~LOAD EXP. SIGN
       ANDA   #$40     ;~TEST IF EXP. +VE
       BEQ    TCTLEN   ;~CONTROL ENABLED IF +VE
       LDAA   AWTMER+1  ;~LOAD EXP. MAGNITUDE
       CMPA   #$01     ;~TEST FOR 1% ERROR
       BLS    TCTLEN   ;~JUMP IF EQUAL OR GREATER
       CMPA   #$02     ;~TEST FOR LESS THAN 1% ERROR
       BHI    ABCTL3   ;~ABORT IF LESS THAN 0.0%
;;P       LDAA   DDBAND   ;~READ DEAD BAND
        LDAA    #$30
       CMPA   AWTMER+2  ;~COMPARE WITH ERROR MAGNITUDE
       BLS    TCTLEN   ;~JUMP IF ERROR GREATER
ABCTL3 JMP    EXTCTL   ;~ABORT FURTHER CONTROL ACTION
;






;  PAGE 009  S5KALG24.SA:1

; THIS SECTION CHECK'S THE AUTO/MAN FLAG AND
; TRANSITION FLAG. IF IN AUTO CONTROL ACTION IS
; TAKEN. IF A TRANSITION HAS OCCURED THEN THE
; CURRENT MOTOR POSITION IS STORED.
;
TCTLEN LDAA   MANATO   ;~LOAD AUTO/MAN FLAG
       BEQ    ABCTL3   ;~ABORT IF IN AUTO
       LDAA   AMANTR   ;~LOAD AUTO/MAN TRANSITION FLAG
       BEQ    FMNWSP   ;~FORM NEW S.P. IF CLEAR
       CLR    AMANTR   ;~RESET TRANSITION FLAG
;
; THIS SECTION USES THE CALCULATED ERROR TO PRODUCE A
; NEW CONTROL SET POINT, BASED UPON THE LAST CONTROL
; SETTING.

FMNWSP:

        LDX    #AWTMER  ;~LOAD S.A. OF AVERAGED ERROR
        JSR    FTCREG   ;~COPY DATA TO C-REG
        LDX    #AREG    ;~LOAD S.A. OF A-REG
        JSR    CLRNM2   ;~CLEAR A-REG

NOTO    LDAA    CONTROLTYPE    ; CHECK CONTROL TYPE
        CMPA    #INCREASEDECREASE
        BNE     NOTIDM         ; NOT VF MODE
        LDD     SEIADREADINGAVG
        BRA     CALTAR  ; CALCULATE TARGET.
NOTIDM  LDD     DA1TMP  ; CURRENT D/A VALUE
CALTAR  JSR     HEXBCD2 ; TO BCD RESULT IN B:E
        LDAA   #$06     ;~LOAD EXPONENT VALUE
        STAA   AREG+1   ;~STORE EXPONENT
        STAB    AREG+2
        STE     AREG+3  ; MAX VALUE OF 4095
        JSR    FMUL     ;~GET READING ERROR
        LDX    #MATHCREG    ;~LOAD S.A. OF C-REG
        JSR    FFDREG   ;~COPY RESULT BACK TO C-REG
        JSR    FADD     ;~CALCULATE NEW CONTROL SET POINT
        JSR     FPTINC  ; CONVERT TO INTEGER
        LDX     #MATHDREG+2 ; RESULT DECIMAL PART
        JSR     BCDHEX3X ; BACK TO HEX
        XGDE            ; TO D
        STD     DA1TMP


;        LDX    #AWTMER  ;~LOAD S.A. OF AVERAGED ERROR
;        JSR    FTCREG   ;~COPY DATA TO C-REG
;        LDX    #AREG    ;~LOAD S.A. OF A-REG
;        JSR    CLRNM2   ;~CLEAR A-REG
;        LDD     DA1TMP  ; CURRENT D/A VALUE
;        JSR     HEXBCD2 ; TO BCD RESULT IN B:E
;        LDAA   #$04     ;~LOAD EXPONENT VALUE
;        STAA   AREG+1   ;~STORE EXPONENT
;        STE     AREG+2  ; MAX VALUE OF 4095
;        JSR    FMUL     ;~GET READING ERROR
;        LDX    #MATHCREG    ;~LOAD S.A. OF C-REG
;        JSR    FFDREG   ;~COPY RESULT BACK TO C-REG
;        JSR    FADD     ;~CALCULATE NEW CONTROL SET POINT
;        JSR     FPTINC  ; CONVERT TO INTEGER
;        LDX     #MATHDREG+2 ; RESULT DECIMAL PART
;        JSR     BCDHEX3X ; BACK TO HEX
        TST     STRUPF   ; IF START UP CORRECT EVERY TIME
        BNE     DOCORR   ; CORRECT


        TST     AFTERSTARTUPCTR                         ; CHECK AFTER START UP COUNTER.
        BEQ     AFTSTUP                                 ; SKIP CONTROL ACTION.
        DEC     AFTERSTARTUPCTR                         ; DECREMENT
        BRA     DOCORR                                  ; INITIATE CORRECTION.


AFTSTUP TST     CORRECTEVERYNCYCLES
        BEQ     DOCORR  ; NO CORRECTION
        INC     CORRECTEVERYNCYCLECTR
        LDAA    CORRECTEVERYNCYCLECTR
        CMPA    CONTAVGFACTOR
        BLO     NOCONAL         ; NO CORRECTION.
;        JSR     RESETROUNDROBIN ;
        CLR     CORRECTEVERYNCYCLECTR
DOCORR  LDD     DA1TMP                          ; READ BEFORE CHECKING LIMIT.
        TST     CONTROLTYPE
        BNE     NOLIMCH ; NO LIMIT CHECK.
        JSR     DACLIM  ; CHECK DAC LIMIT
NOLIMCH JSR     LIMITCORR ; LIMIT CORRECTION TO XX %
NOCONAL:
EXTCTL  JSR     CHECKALARMLIMIT ; RUN CHECK ON THE % ERROR.
EXTCT   JSR     CALCRRAVERAGE   ; CALCULATE ROUND ROBIN AVERAGE.
        RTS

CKSPLM  RTS
;EXTCTL  RTS
TERCT2  CLR    AMANTR
        CLR    STRUPF   ;~RESET START-UP FLAGS
        CLR    STUPTR
        JSR     CHECKALARMLIMIT ; RUN CHECK ON THE % ERROR.
        JSR    TOMANUAL
        RTS

;
; THIS S/R INSERTS A 7 BYTE DATA BLOCK INTO QUE #2
;
Q2DAEN LDAA   CDENP2   ;~LOAD QUE #2 DATA POINTER
       CMPA   AVRGK2   ;~COMPARE WITH AVERAGING FACTOR
       BNE    INCQU2   ;~JUMP IF NOT AT TOP OF QUE #2
       CLRA            ;~AT TOP OF QUE:RESET POINTER
       LDX    #DATST2  ;~RESET DATA ADDRESS POINTER
       BRA    QUEDA2   ;~INSERT DATA INTO QUE #2
INCQU2 LDX    QCADR2   ;~LOAD DATA ADDRESS POINTER #2
       INCA            ;~INCREMENT DATA POINTER #2
        AIX     #7
;       INX             ;~INC. ADDR. TO NEXT ENTRY
;       INX
;       INX
;       INX
;       INX
;       INX
;       INX
QUEDA2 STAA   CDENP2   ;~STORE NEW DATA POINTER
       STX    QCADR2   ;~STORE NEW ADDRESS POINTER
       JSR    DAMOV2   ;~COPY DATA INTO QUE
       RTS             ;~END OF INSERTION S/R #2
;
; THIS S/R PERFORMS THE SUMMATION OF ALL READINGS
; IN QUE #2. IT OPERATES IN A 'ROUND ROBIN' FASHION
; AS DESCRIBED PREVIOUSLY.
;
SUMDT2 LDAA   SUMCT2   ;~LOAD SUMMATION #2 DATA COUNTER
       CMPA   AVRGK2   ;~COMPARE WITH AVERAGING CONSTANT
       BEQ    SUMFL2   ;~SUMMATION FULL IF EQUAL
       BLS    SMSTF2   ;~SUMMATION STILL FILLING IF LESS
       LDX    #SUMTT2  ;~SUMMATION TOO FULL :RESET
       JSR    CLRNM2   ;~CLEAR SUMMATION TOTAL
       CLRA            ;~RESET SUMMATION COUNT
SMSTF2 INCA            ;~INCREMENT SUMMATION DATA COUNT
       STAA   SUMCT2   ;~STORE NEW COUNT
       LDX    QCADR2   ;~LOAD CURRENT DATA ADDRESS
       JSR    FTCREG   ;~COPY TO C-REG
       LDX    #SUMTT2  ;~LOAD SUMMATION TOTAL S.A.
       JSR    FTAREG   ;~COPY TO A-REG
       JSR    FADD     ;~ADD NEW READING TO SUMMATION
       LDX    #SUMTT2  ;~LOAD S.A. OF SUMMATION TOTAL
       JSR    FFDREG   ;~COPY NEW SUMMATION TOTAL
       RTS             ;~END OF SUMMMATION S/R
SUMFL2 LDAA   CDENP2   ;~LOAD CURRENT DATA POINTER
       CMPA   AVRGK2   ;~CHECK FOR TOP OF QUE #2
       BNE    SMADR2   ;~NOT AT TOP: FORM DATA ADDRESS
       LDX    #DATST2  ;~RESET DATA ADDRESS TO BOTTOM
       BRA    SMNUM2   ;~PERFORM F.I.F.O. SUMMATION
SMADR2 LDX    QCADR2   ;~LOAD CURRENT DATA ADDRESS
        AIX     #7
SMNUM2 JSR    FTCREG   ;~COPY DATA TO C-REG
       LDX    #SUMTT2  ;~LOAD SUMMATION #2 S.A.
       JSR    FTAREG   ;~COPY TO A-REG
       JSR    FSUB     ;~SUBTRACT OLDEST ENTRY
       LDX    #AREG    ;~LOAD S.A. OF A-REG
       JSR    FFDREG   ;~COPY RESULT BACK TO A-REG
       LDX    QCADR2   ;~LOAD CURRENT ENTRY ADDRESS
       JSR    FTCREG   ;~COPY DATA TO C-REG
       JSR    FADD     ;~ADD NEW ENTRY TO SUMMATION
       LDX    #SUMTT2  ;~LOAD S.A. OF SUMMATION #2 STORE
       JSR    FFDREG   ;~STORE NEW SUMMATION TOTAL
       RTS             ;~EXIT S/R
;
; THIS S/R DIVIDES SUMMATION TOTAL #2 BY ITS SAMPLE
; SIZE TO PRODUCE THE AVERAGED ERROR. THE RESULT IS THEN
; STORED.
;
DIVDT2 LDX    #SUMTT2  ;~LOAD S.A. OF SUMMATION #2
       JSR    FTCREG   ;~COPY TO C-REG
       LDX    #AREG    ;~LOAD S.A. OF A-REG
       JSR    CLRNM2   ;~CLEAR A-REG
       LDAA   #$02     ;~LOAD EXP. SIZE
       STAA   AREG+1   ;~STORE EXP.
       LDAA   SUMCT2   ;~LOAD SUMMATION DATA SIZE COUNTER
       STAA   AREG+2   ;~STORE MAGNITUDE TERM
       JSR    FDIV     ;~DIVIDE SUMMATION BY SAMPLE SIZE
       LDX    #AWTMER  ;~LOAD S.A. OF AVERAGED RESULT
       JSR    FFDREG   ;~COPY RESULT
       LDX    #AWTMERAC  ;~LOAD S.A. OF AVERAGED RESULT
       JSR    FFDREG   ;~COPY RESULT



       RTS             ;~END OF DIVISION
;
; THIS ROUTINE COPIES THE INSTANTENOUS CONTROL ERROR
; FROM ITS STORE TO THE ADDRESS POINTED TO BY THE X-REG.
;
DAMOV2 STX    ADRTO    ;~STORE DESTINATION ADDRESS
       LDX    #WTPMER  ;~LOAD SOURCE ADDRESS
       STX    ADRFM    ;~STORE SOURCE ADDRESS
       LDAB   #$07     ;~NUMBER OF BYTES TO MOVE
       JSR    MOVER    ;~COPY DATA
       RTS
;
; THIS ROUTINE RESETS THE START-UP FLAGS IF THE AVERAGED
; ERROR IS WITHIN 5.0%. IT THEN CHECKS THE MAGNITUDE OF
; THE AVERAGE ERROR AND COMPARES IT WITH THE PRESET ERROR
; LIMIT. IF THE LIMIT IS EXCEEDED THEN THE SYSTEM ENTERS
; THE MANUAL MODE.

RESETROUNDROBIN:
       CLR    SUMCT2   ;~RESET QUE #2 SAMPLE COUNTER
       CLR    CDENP2   ;~RESET QUE #2 DATA POINTER
       LDX    #DATST2  ;~LOAD QUE #2 DATA S.A.
       STX    QCADR2   ;~STORE IN QUE #2 ADDRESS POINTER
       LDX    #SUMTT2  ;~LOAD S.A. OF SUMMATION TOTAL
       JSR    CLRNM2   ;~RESET SUMMATION TOTAL
       RTS



; CHECK IF THE % ERROR IS GREATER THAN LIMIT.
;
CHECKALARMLIMIT:
       TST    MANATO   ;~SYSTEM IN AUTO ?
       BEQ    XITERC   ;~EXIT ERROR CHECK
       TST    STRUPF   ;
       BNE    XITERC   ;

;       CHECK IF ERROR IS UNDER LIMIT

       LDX     #AWTMERAC
       JSR     FTDREG           ; RESTORE VALUE.
       JSR     FPTINC           ; CONVERT TO AN INTEGER.
       LDD     MATHDREG+3       ; CHECK %
       BNE     SETERROR         ;~EXP.=0; ERROR TOO LARGE
       LDAB    CONTROLPER             ; CHECK DECIMAL POINT
       JSR     CHECKPERLIM     ; CHECK % LIMIT.
       CMPB    EREG            ; COMPARE TO ACTUAL ERROR
       BHI     ERROK            ; ERROR OK.

SETERROR:

       LDAA     MBPROALRM       ;
       ANDA     #CONTROLALARMBIT ; WAS THE ALARM THERE THE LAST TIME.
       BNE      ALRSET          ; BIT IS ALREADY SET.

       LDAA   #CONTROLALARM     ;$40              ;~M.S. NIBBLE = 4 FOR CONTROL ALARM
       JSR    PTALTB            ;~PUT ALARM INTO ALARM TABLE, WITH TIME
ALRSET BRA    XITERC

ERROK:
       LDAA     MBPROALRM       ;
       ANDA     #CONTROLALARMBIT ; WAS THE ALARM THERE THE LAST TIME.
       LBEQ      XITERC
       LDAA   #CONTROLALARM     ;$40              ;~M.S. NIBBLE = 4 FOR CONTROL ALARM
       JSR    RMALTB            ;~PUT ALARM INTO ALARM TABLE, WITH TIME
XITERC RTS

       END
       END
*/

