//////////////////////////////////////////////////////
// RWSEI.C.
//
//
// P.Smith                              11-02-2005
// First Pass at SEI data distribution
// 
//
// P.Smith                              7-3-05
// Added SEIAutoLampOn(),SEIAutoLampOff(),SEIIncreaseDecreaseLampsOnOff()
// SEIIncreaseLampOn(),SEIIncreaseLampOff(),SEIDecreaseLampOn(),SEIDecreaseLampOff()
// SEIIncreaseLampOnFlag(),SEIIncreaseLampOffFlag(),SEIIncreaseLampOnFlag(),
// SEIDecreaseLampOffFlag(), SEILocalIncreaseLampOnFlag(),SEILocalIncreaseLampOffFlag()
// SEIRemoteIncreaseLampOnFlag(),SEIRemoteIncreaseLampOffFlag()
// SEIVoltageFollowerOn(),SEIVoltageFollowerOff()
//
// P.Smith                              8-3-05
// Added ProcessSEIInputData() 
//

// P.Smith                              10-03-05
// uncomment AutoManualToggle()

// P.Smith                              30-03-05
// uncomment AverageAToDReading()
//
// P.Smith                              31-03-05
// copy the voltage follower reading to a/d storage, not tempory storage.
//
// P.Smith                              7-04-2005
// Added FrontRollChange / BackRollChange
// uncomment set of front roll change / back roll change bit set
// if g_bIncreaseDecreaseCalibrationRead set, read calibration data for increase /decease
//
// P.Smith                              4/10/05
// removed "todo make the blender do a read here" data is already being read.
// CalculateScrewSpeed implemented
//
// P.Smith                              25/11/05
// correct check of g_CalibrationData.m_nControlType for CONTROL_TYPE_INC_DEC
//
// 
// P.Smith                      16/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
// #include "Conalg.h"
// changed unions IntsAndLong to WordAndDWord
// changed unions CharsAndInt to CharsAndWord
//#include "NBBGpio.h"
// 
// P.Smith                      13/11/06
// name change from g_nAlarmOnFlag -> g_bAlarmOnFlag g_nAlarmOnTransitionFlag -> g_bAlarmOnTransitionFlag
// remove unused commented out code.
// 
// P.Smith                      6/2/07
// remove printfs
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      17/9/07 
// Correct SEI reset, check power up hours and minutes, 
// set g_nAllowReadOfControlInputsCtr to READINHIBITFORCONTROLINPUTS on auto, increase ,decrease, local mode
// this stops the reading of the inputs until and command has been downloaded to the sei
// module. when the counter goes to zero, (3 seconds) the command is reset to register is 
// set to ff00.
// remove switch to manual on sei reset.
//
// P.Smith                          13/11/07
// Modified control mode format implemented
// allow for new bit set up for control  mode.                     ;
//
// P.Smith                          15/11/07
// in CalculateScrewSpeed, only run the screw speed calculation if signal type is tacho
//
// P.Smith                          19/12/07
// When reading calibration data from the SEI, set the write sei buffers to the same
// value so that when the following write takes place that the correct data will
// be written down.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          1/3/08
// check g_bReadDToFromSEIOnReset after averaging has been done to allow the 
// a/d reading to be available for storage, this is used as the initial reference 
// for increase /decrease control
// in StoreDToAReading, set g_nExtruderControlSetpointIncDec  to the tacho reading.
//
// P.Smith                          12/3/08
// if in increase /decrease mode and exiting auto inc/dec set generate an alarm
// and switch the blender to manual
// set clear inc/dec auto alarm in command word to clear the alarm.
// set g_bSEIAutoStatus to TRUE for  SEI_STATUS_AUTO_MANUAL_STATUS set in g_nSEIAlarmState 
//
// P.Smith                          12/3/08
// this was not working initially due to the fact that the reset command was not being initiated correctly.
// g_nAllowReadOfControlInputsCtr set to READINHIBITFORCONTROLINPUTS
//
// P.Smith                          17/7/08
// added AddBlenderToManualSEIForcedExitEventToLog & copy of g_nExtruderDAValue to BATCH_SUMMARY_EXTRUDER_AD
// copy extruder d/a value to BATCH_SUMMARY_EXTRUDER_AD
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms ,g_PowerUpTime,g_CurrentTime extern 
// 
// P.Smith                          23/4/09
// set the increase / decrease on time to 5 was 3 originally
 //////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "BatVars.h"
#include "BatVars2.h"
#include "ConversionFactors.h"       
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"   
#include "Alarms.h"
#include "TSMPeripheralsMBIndices.h"
#include "RwSEI.h"
#include "TimeDate.h"
#include "Batvars2.h"
#include "Mbprogs.h"
#include "Conalg.h"
#include "NBBGpio.h"
#include <basictypes.h>
#include <stdio.h>
#include "SerialStuff.h"
#include "Eventlog.h"

extern  CalDataStruct   g_CalibrationData;                                                                
extern  int g_nProtocol;
extern  structControllerData    *arrpDeviceList[];
   

 
 //////////////////////////////////////////////////////
// ReadSEIData( void )         from ASM = RWSEI
// Read SEI data
// 
//
//////////////////////////////////////////////////////

void ReadSEIData( void )
{                     
    union WordAndDWord uValue;
    
        g_nSEIDToAReadState = g_arrnReadSEIMBTable[MB_SEI_CURRENT_DA]; // Read current d/a value
        if(g_bReadDAValueFlag)
        {
            g_bReadDAValueFlag = FALSE;        
            if((g_CalibrationData.m_nControlMode & CONTROL_MODE_VOLTAGE_FOLLOWER) != 0)
            {
                if(!g_bManualAutoFlag)  // only used read d/a in manual
                {
                    if(g_nSEIDToAReadState != g_nExtruderDAValue) // change in d/a reading ?                
                    {
                        g_bRunEst = TRUE;
                        g_bCCVsFlag = TRUE;  // allow control % calculation to run
                    }
                
                }
            }
            
            StoreDToAReading();
        }
        
        g_nSEIAlarmState = g_arrnReadSEIMBTable[MB_SEI_STATUS];  // read alarm state from SEI
        g_nSEIInputState = g_arrnReadSEIMBTable[MB_SEI_INPUTS];
        g_nSEIDToAOutputState = g_arrnReadSEIMBTable[MB_SEI_CURRENT_DA];
        uValue.nValue[0] = g_arrnReadSEIMBTable[MB_SEI_LSPD_ACCUM_MSW]; // accumulated pulses
        uValue.nValue[1] = g_arrnReadSEIMBTable[MB_SEI_LSPD_ACCUM_LSW]; // 
        g_lSEIPulses = uValue.lValue;                    
        g_nSEIPulsesPerSecond = g_arrnReadSEIMBTable[MB_SEI_LSPD_PPS];
        g_nVoltageFollowerReading = g_arrnReadSEIMBTable[MB_SEI_AD]; // voltage follower reading
        g_nExtruderTachoReading = g_nVoltageFollowerReading;        // copy to sei a/d reading store
        AverageAToDReading();  // ASM = AVERAGEADREADING
        if(g_bReadDToFromSEIOnReset)
        {
            g_bReadDToFromSEIOnReset = FALSE;    
            StoreDToAReading();
        }
        if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
        {
            if(g_nSEIAlarmState & SEI_STATUS_FORCED_EXIT_FROM_AUTO)
            {
                PutAlarmTable(SEIRESETALARM,  0 );     // SEI reset //PTALTB
                ToManual();
                AddBlenderToManualSEIForcedExitEventToLog();
                g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_CLR_INC_DEC_AUTO_ALARM ;  
                g_nAllowReadOfControlInputsCtr = READINHIBITFORCONTROLINPUTS;             

            }
        }
        
        if((g_nSEIAlarmState & SEI_STATUS_AUTO_MANUAL_STATUS) != 0)
        {
            g_bSEIAutoStatus = TRUE;
       }
        else
        {
            g_bSEIAutoStatus = FALSE;
        }
        g_bSEIPulsesReady = TRUE; // indicate the pulses are available
        g_nLineSpeedFractionCounter = 0;    // reset line speed fraction counter
        ProcessSEIInputData();              // ASM = SEIINPUTSPROCESS  process SEI input process data.                            
        g_nSEIIncreaseDecreaseState = g_arrnReadSEIMBTable[MB_SEI_INC_DEC_REG]; // increase /decrease status
        ProcessIncreaseDecrease();      
        
        if(g_bREadIncDecCalibrationData)
        {
            g_bREadIncDecCalibrationData = FALSE;
            g_CalibrationData.m_nControlLag = g_arrnReadSEIMBTable[MB_SEI_CONTROL_LAG]; // control lag 
            g_CalibrationData.m_nIncreaseRate =  INCDEC_COUNTSPERSECOND_CONVERSION * g_arrnReadSEIMBTable[MB_SEI_INC_RATE]; // Increase rate 
            g_CalibrationData.m_nDecreaseRate =  INCDEC_COUNTSPERSECOND_CONVERSION * g_arrnReadSEIMBTable[MB_SEI_DEC_RATE]; // Decrease rate 
            g_CalibrationData.m_nControlDeadband = g_arrnReadSEIMBTable[MB_SEI_DEADBAND]; // Deadband 
            g_CalibrationData.m_nFineIncDec =  g_arrnReadSEIMBTable[MB_SEI_FINE_BAND]; // Fine Inc /Dec band 
        
            g_arrnWriteSEIMBTable[MB_SEI_CONTROL_LAG] = g_CalibrationData.m_nControlLag;
            g_arrnWriteSEIMBTable[MB_SEI_INC_RATE] = g_CalibrationData.m_nIncreaseRate / INCDEC_COUNTSPERSECOND_CONVERSION;
            g_arrnWriteSEIMBTable[MB_SEI_DEC_RATE] = g_CalibrationData.m_nDecreaseRate /INCDEC_COUNTSPERSECOND_CONVERSION;
            g_arrnWriteSEIMBTable[MB_SEI_DEADBAND] = g_CalibrationData.m_nControlDeadband;
            g_arrnWriteSEIMBTable[MB_SEI_FINE_BAND] = g_CalibrationData.m_nFineIncDec;

//            SetupMttty();
//            iprintf("\n control lag is %d",g_arrnReadSEIMBTable[MB_SEI_CONTROL_LAG]); 
//            iprintf("\n inc rate is %d",g_arrnReadSEIMBTable[MB_SEI_INC_RATE]); 
//            iprintf("\n dec rate is %d",g_arrnReadSEIMBTable[MB_SEI_DEC_RATE]); 
//            iprintf("\n deadband is %d",g_arrnReadSEIMBTable[MB_SEI_DEADBAND]); 
//            iprintf("\n fine deadband is %d",g_arrnReadSEIMBTable[MB_SEI_FINE_BAND]); 

            CopySystemConfigurationToMB();  // copy system configuration back into MB table.

        }

        if(g_bWriteIncreaseDecreaseDataToSEI)
        {
//           SetupMttty();
//           iprintf("\n writing inc /dec to sei");

            g_bWriteIncreaseDecreaseDataToSEI = FALSE;
            g_arrnWriteSEIMBTable[MB_SEI_CONTROL_LAG] = g_CalibrationData.m_nControlLag;
            g_arrnWriteSEIMBTable[MB_SEI_INC_RATE] = g_CalibrationData.m_nIncreaseRate / INCDEC_COUNTSPERSECOND_CONVERSION;
            g_arrnWriteSEIMBTable[MB_SEI_DEC_RATE] = g_CalibrationData.m_nDecreaseRate /INCDEC_COUNTSPERSECOND_CONVERSION;
            g_arrnWriteSEIMBTable[MB_SEI_DEADBAND] = g_CalibrationData.m_nControlDeadband;
            g_arrnWriteSEIMBTable[MB_SEI_FINE_BAND] = g_CalibrationData.m_nFineIncDec;
            g_nDoIncDecReadInNSeconds = 4;                  /* will read them back in 4 seconds asm INITIATEINCDECWRRD */
        }
}





 //////////////////////////////////////////////////////
// StoreDToAReading ( void )       asm = NOCHANGE   
// 
//
// P.Smith                          31-03-2005
//////////////////////////////////////////////////////

void StoreDToAReading ( void )
{                  
    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        g_nExtruderDAValue = g_nExtruderTachoReading;
        g_nExtruderControlSetpointIncDec = g_nExtruderTachoReading; // nbb--todo should this be removed
        g_bCCVsFlag = TRUE;  // allow control % calculation to run
    }
    else
    {
        g_nExtruderDAValue = g_nSEIDToAReadState; // read d/a normally.
        g_bCCVsFlag = TRUE;  // allow control % calculation to run
    }
}





 //////////////////////////////////////////////////////
// SEIAutoLampOn( void )          ASM = SEIAMLAMPON
// 
//
//////////////////////////////////////////////////////

void SEIAutoLampOn( void )
{                     
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_AM_LAMP_BIT; // set auto lamp bit 
}



 //////////////////////////////////////////////////////
// SEIAutoLampOff( void )          ASM = SEIAMLAMPOFF
// 
//
//////////////////////////////////////////////////////

void SEIAutoLampOff( void )
{                     
int nTemp;
    nTemp = SEI_CMD_AM_LAMP_BIT ^ 0xFFFF;      // get complement 
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp;  // clear the a/m bit.
}


 //////////////////////////////////////////////////////
// SEIIncreaseDecreaseLampsOnOff( void )          ASM = SWITCHSEILAMPONOFF
// 
// Switches increase lamp on /off (to SEI),called on 10 hz
// P.Smith
//
//////////////////////////////////////////////////////

void SEIIncreaseDecreaseLampsOnOff( void )
{                     
    if(g_nIncreaseLampStatus & INCREASEDECREASELAMPSTATUSBITPOS)
    {
        SEIIncreaseLampOn(); // switch increase lamp on
    }
    else
    {
        SEIIncreaseLampOff(); // switch increase lamp off

        if(g_nDecreaseLampStatus & INCREASEDECREASELAMPSTATUSBITPOS)
        {
            SEIDecreaseLampOn(); // switch decrease lamp on
        }
        else
        {
            SEIDecreaseLampOff(); // switch decrease lamp off
        }   
    }   
}


//////////////////////////////////////////////////////
// SEIIncreaseLampOn( void )          ASM = SEIINCLAMPON
// 
// Switches increase lamp on 
//
//////////////////////////////////////////////////////

// --review-- should the bit be checked, if already set, do not set again
void SEIIncreaseLampOn( void )
{
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_INC_OP_BIT; // set increase lamp bit 
}



 //////////////////////////////////////////////////////
// SEIIncreaseLampOff( void )          ASM = SEIINCLAMPOFF
// 
// Switches increase lamp off 
//
//////////////////////////////////////////////////////

void SEIIncreaseLampOff( void )
{
int nTemp;
    nTemp = SEI_CMD_INC_OP_BIT ^ 0xFFFF;      // get complement 
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp; // clear increase lamp bit 
}


 //////////////////////////////////////////////////////
// SEIDecreaseLampOn( void )          ASM = SEIDECLAMPON
// 
// Switches decrease lamp on 
//
//////////////////////////////////////////////////////

// --review-- should the bit be checked, if already set, do not set again

void SEIDecreaseLampOn( void )
{
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_DEC_OP_BIT; // set increase lamp bit 
}




 //////////////////////////////////////////////////////
// SEIDereaseLampOff( void )          ASM = SEIDECLAMPOFF
// 
// Switches increase lamp off 
//
//////////////////////////////////////////////////////

void SEIDecreaseLampOff( void )
{
int nTemp;
    nTemp = SEI_CMD_DEC_OP_BIT ^ 0xFFFF;      // get complement 
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp; // clear increase lamp bit 
}



//////////////////////////////////////////////////////
// SEIIncreaseLampOnFlag( void )          ASM = SEIINCLAMPONF
// 
// Sets increase lamp on bit in IncreaseLampStatus 
//
//////////////////////////////////////////////////////

void SEIIncreaseLampOnFlag( void )
{
    g_nIncreaseLampStatus |= LOCALINCREASEDECREASEBITPOS;
}


//////////////////////////////////////////////////////
// SEIIncreaseLampOff( void )          ASM = SEIINCLAMPOFFF
// 
// Clears increase lamp flag bit 
//
//////////////////////////////////////////////////////

void SEIIncreaseLampOffFlag( void )
{
int nTemp;
    nTemp = LOCALINCREASEDECREASEBITPOS ^ 0xFFFF;      // get complement 
    g_nIncreaseLampStatus &= nTemp;
}



//////////////////////////////////////////////////////
// SEIDecreaseLampOnFlag( void )          ASM = SEIDECLAMPONF
// 
// Sets decrease lamp on bit in DecreaseLampStatus 
//
//////////////////////////////////////////////////////

void SEIDecreaseLampOnFlag( void )
{
    g_nDecreaseLampStatus |= LOCALINCREASEDECREASEBITPOS;
}


//////////////////////////////////////////////////////
// SEIDecreaseLampOff( void )          ASM = SEIDECLAMPOFFF
// 
// Clears increase lamp flag bit 
//
//////////////////////////////////////////////////////

void SEIDecreaseLampOffFlag( void )
{
int nTemp;
    nTemp = LOCALINCREASEDECREASEBITPOS ^ 0xFFFF;      // get complement 
    g_nDecreaseLampStatus &= nTemp;
}

//////////////////////////////////////////////////////
// SEIRemoteIncreaseLampOnFlag( void )          ASM = SEIINCLAMPONF
// 
// Sets remote decrease lamp on bit in DecreaseLampStatus 
//
//////////////////////////////////////////////////////

void SEIRemoteIncreaseLampOnFlag( void )
{
    g_nIncreaseLampStatus |= REMOTEINCREASEDECREASEBITPOS;
}


//////////////////////////////////////////////////////
// SEIRemoteIncreaseLampOnFlag( void )          ASM = SEIINCLAMPONF
// 
// Sets remote decrease lamp on bit in DecreaseLampStatus 
//
//////////////////////////////////////////////////////

void SEIRemoteDecreaseLampOnFlag( void )
{
    g_nDecreaseLampStatus |= REMOTEINCREASEDECREASEBITPOS;
}


//////////////////////////////////////////////////////
// SEIRemoteIncreaseLampOff( void )          ASM = REMINCLAMPOFFF
// 
// Clears increase lamp flag bit 
//
//////////////////////////////////////////////////////

void SEIRemoteIncreaseLampOffFlag( void )
{
int nTemp;
    nTemp = REMOTEINCREASEDECREASEBITPOS ^ 0xFFFF;      // get complement 
    g_nDecreaseLampStatus &= nTemp;
}



//////////////////////////////////////////////////////
// SEIRemoteDecreaseLampOff( void )          ASM = REMDECLAMPOFFF
// 
// Clears increase lamp flag bit 
//
//////////////////////////////////////////////////////

void SEIRemoteDecreaseLampOffFlag( void )
{
int nTemp;
    nTemp = REMOTEINCREASEDECREASEBITPOS ^ 0xFFFF;      // get complement 
    g_nDecreaseLampStatus &= nTemp;
}



//////////////////////////////////////////////////////
// SEIVoltageFollowerOn( void )          ASM = SEIVOLFOLON
// 
// Set SEI voltage follower mode
// In voltage follower mode, 0 - 10 volts is read from the SEI a/d and
// sent back down to the SEI via the blender, hence the term voltage follower 
//
//////////////////////////////////////////////////////

void SEIVoltageFollowerOn( void )
{
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_GO_TO_VFOLLOWER_BIT; // set increase lamp bit 

}




//////////////////////////////////////////////////////
// SEIVoltageFollowerOff( void )          ASM = SEIVOLFOLOFF
// 
// Resets voltage follower bit sent to SEI
//
//////////////////////////////////////////////////////

void SEIVoltageFollowerOff( void )
{
int nTemp;
    nTemp = SEI_CMD_GO_TO_VFOLLOWER_BIT ^ 0xFFFF;      // get complement 
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp; // Reset bit 
}




//////////////////////////////////////////////////////
// SEIIncreaseDecreaseModeOn( void )          ASM = SEIINCDECON
// 
// Sets Increase / Decrease mode 
//
//////////////////////////////////////////////////////

void SEIIncreaseDecreaseModeOn( void )
{
int nTemp;
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_EXTR_CTRL_MODE_BIT1;//SEI_CMD_GO_TO_INCDEC_BIT; // 
    nTemp = SEI_CMD_EXTR_CTRL_MODE_BIT0 ^ 0xFFFF;      // get complement   set lsb to 0
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp;//SEI_CMD_GO_TO_INCDEC_BIT; // 

}


//////////////////////////////////////////////////////
// SEIIncreaseDecreaseModeOff( void )          ASM = SEIINCDECOFF
// 
// Sets Increase / Decrease mode off - 00 written to mode
//
//////////////////////////////////////////////////////

void SEIIncreaseDecreaseModeOff( void )
{
int nTemp;
    nTemp = SEI_CMD_EXTR_CTRL_MODE_BIT0 ^ 0xFFFF;       // get complement   ASM = GOTOINCDECBITPOS
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp;     //  
    nTemp = SEI_CMD_EXTR_CTRL_MODE_BIT1 ^ 0xFFFF;       // get complement   
    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp;     //  
}


//////////////////////////////////////////////////////
// ProcessSEIInputData( void )          ASM = SEIINPUTSPROCESS
// 
// Processes data read from SEI
//
//////////////////////////////////////////////////////

void ProcessSEIInputData( void )
{
int nTemp;
    g_nSEITimeOutCounter = 0;      // reset time out counter
    if(g_bSEIHasTimedOut)          // SEI timed out.
    {
        if(g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS)   // sei enabled
        {
            if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & SEICOMMSALARMBIT)) // SEI alarm bit set??
            {  //
                    RemoveAlarmTable( SEICOMMSALARM,  0 );     // indicate alarm, //PTALTB
            }
        }
        g_bSEIHasTimedOut = FALSE;
    }
    if((g_CalibrationData.m_nControlMode & CONTROL_MODE_VOLTAGE_FOLLOWER) != 0)
    {
        if((g_nSEIAlarmState & SEI_STATUS_VOLTAGE_FOLLOWER_BIT) == 0) // voltage follower mode set ?
        {
                SEIVoltageFollowerOn();                        
        }
             
    }
        else // not voltage follower
        {
            if(g_nSEIAlarmState & SEI_STATUS_VOLTAGE_FOLLOWER_BIT) // voltage follower mode set ?
            {
                SEIVoltageFollowerOff();                        
            }
    
        }            

        if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE

        {            
            if((g_nSEIAlarmState & SEI_STATUS_INCDEC_BIT) == 0) // increase /decrease mode?
            {
                SEIIncreaseDecreaseModeOn();                        
            }
        }
        else // not increase /decrease mode
        {
            if(g_nSEIAlarmState & SEI_STATUS_INCDEC_BIT) // increase /decrease mode?
            {
            SEIIncreaseDecreaseModeOff();                        
            }
        }
            
        if(g_nSEIAlarmState & SEI_STATUS_RESET_BIT) // ?
        {
            g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_CLR_RESET_BIT; // clear reset bit in SEI
            if((g_PowerUpTime[TIME_HOUR] != g_CurrentTime[TIME_HOUR]) || ((g_PowerUpTime[TIME_HOUR] == g_CurrentTime[TIME_HOUR]) && (g_PowerUpTime[TIME_MINUTE] != g_CurrentTime[TIME_MINUTE])))  // has the blender just powered up ?
            {
                PutAlarmTable(SEIRESETALARM,  0 );     // SEI reset //PTALTB
                g_bPrintSEIResetOccurred = TRUE;               // indicate SEI reset occurred
            } 
        }
        else // not SEI reset ASM = NORESET
        {
            if(g_nSEIAlarmState & SEI_STATUS_LOCALMODE_BIT) // local mode ?
            {
                g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_GO_TO_REMOTE_BIT; // set remote bit                
                g_nAllowReadOfControlInputsCtr = READINHIBITFORCONTROLINPUTS;             
            }

            if(((g_nSEIAlarmState & SEI_STATUS_AUTO_MAN_BIT)!= 0) && (g_nAllowReadOfControlInputsCtr == 0)) // auto?
            {
                g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_STATUS_AUTO_MAN_BIT; // set a/m reset bit
                AutoManualToggle();  //ASM = AMKEY   
                g_nAllowReadOfControlInputsCtr = READINHIBITFORCONTROLINPUTS;             
            }
            if(((g_nSEIAlarmState & SEI_STATUS_FRC_BIT)!= 0) && (g_nAllowReadOfControlInputsCtr == 0)) // front roll change ?  NOAM in ASM
            {
                g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_CLR_FRC_BIT ; // set front roll change reset bit  ASM = CLEARFRCBITPOS
                FrontRollChange();  // ASM = FRONTROLLCHANGE
                g_nAllowReadOfControlInputsCtr = READINHIBITFORCONTROLINPUTS;    
            }
            if(((g_nSEIAlarmState & SEI_STATUS_BRC_BIT) != 0) && (g_nAllowReadOfControlInputsCtr == 0)) // front roll change ?  NOFRC in ASM
            {
                g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_CLR_BRC_BIT ; // set back roll change reset bit
                BackRollChange();  // ASM = BACKROLLCHANGE
                g_nAllowReadOfControlInputsCtr = READINHIBITFORCONTROLINPUTS;             
             }
            if(g_bAlarmOnFlag != g_bAlarmOnTransitionFlag)
            {
                if(g_bAlarmOnFlag)
                {
                    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] |= SEI_CMD_ALARM_RLY_BIT ; // set alarm
                }            
                else
                {   
                    nTemp = SEI_CMD_ALARM_RLY_BIT ^ 0xFFFF;      // get complement                    
                    g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= nTemp ; // cleart alarm
               }
                g_bAlarmOnTransitionFlag = g_bAlarmOnFlag;                                        
            }
        }
}



//////////////////////////////////////////////////////
// ProcessIncreaseDecrease( void )          ASM = PROCESSINCDEC:
// 
// Processes data read from SEI
//
//////////////////////////////////////////////////////

// --review-- the speed of this should be 5 times a second, at the moment it is only once per second.
// which is insufficient to read the increase /decrease status from the SEI card.

void ProcessIncreaseDecrease( void )
{
    union CharsAndWord nIncDecStatus;

    
    if(g_bManualAutoFlag == MANUAL)
    {
        nIncDecStatus.nValue = g_nSEIIncreaseDecreaseState;
        if(nIncDecStatus.nValue)   //increase active ?
        {
            if(nIncDecStatus.cValue[0])
            {
                g_nCommsIncreaseFlag = 5;   // initiate increas
                SEIIncreaseLampOnFlag();    // set increase lamp on bit             
            }
            else
            if(nIncDecStatus.cValue[1])     // decrease active ?
            {
                g_nCommsDecreaseFlag = 5;                   // initiate increas
                SEIDecreaseLampOnFlag();    // set decrease lamp on bit             
            }            
        }
        else // no increase / decrease
        {
            SEIIncreaseLampOffFlag();       // clear increase lamp off bit             
            SEIDecreaseLampOffFlag();       // clear decrease lamp off bit             
        }

    }
    else  // in manual
    {
        SEIIncreaseLampOffFlag();           // set increase lamp off bit             
        SEIDecreaseLampOffFlag();           // set decrease lamp off bit             
    }
}




//////////////////////////////////////////////////////
// AverageAToDReading( void )          ASM = AverageAToDReading
// 
// Average a/d reading from sei
//
//////////////////////////////////////////////////////



void AverageAToDReading( void )
{
    if(g_nAToDSumCounter >= 4)        // .
    {
        g_nAToDSumCounter = 0;
        g_lAToDSum = 0;
    }
    g_lAToDSum += g_nExtruderTachoReading;     // Add in current raw A/D value
    g_nAToDSumCounter++;
    if(g_nAToDSumCounter == 4)
    {
        g_nExtruderTachoReadingAverage = g_lAToDSum >>2;    // Divide by  4;
        g_nAToDSumCounter = 0;
        g_lAToDSum = 0;
    }
}








/*

PROCESSINCDEC:
                LDAA    MANATO
                BNE     INCDECOFF       ; OUTPUTS SHOULD BE OFF.        
                LDD     SEIINCDECSTATE  ; READ DECREASE STATE                                                                                                           
                BEQ     INCDECOFF       ; INCREASE / DECREASE OFF.
                TSTA                    ; INCREASE ?
                BEQ     NOINC           ; NO
                LDAA    #3
                STAA    COMMSIFLG       ; INITIATE INCREASE                 
                JSR     SEIINCLAMPONF    ; SEI LAMP ON.
                BRA     XITRD
NOINC           TSTB                    ; DECREASE ?
                BEQ     XITRD           ; EXIT.
                LDAB    #3
                STAB    COMMSDFLG       ; INITIATE DECREASE.                 
                JSR     SEIDECLAMPONF    ; SEI LAMP ON.
XITRD           RTS                
INCDECOFF       JSR     SEIINCLAMPOFFF   ; SEI INC LAMP OFF.
                JSR     SEIDECLAMPOFFF   ; SEI DEC LAMP OFF.
                RTS


*/


//////////////////////////////////////////////////////
// FrontRollChange( void )          ASM = FRONTROLLCHANGE
// 
// Front Roll Change
//
// P.Smith                          7-04-2005
//
//////////////////////////////////////////////////////



void FrontRollChange( void )
{
    if(!g_nFrontRollChangeLag) // RCF_LAG  -lag counter = 0 ?
    {
        g_nFrontRollChangeLag = ROLLCHANGEBOUNCETIMEINSECONDS;  // RCF_LAG           
        g_bActivateFrontRollChange = TRUE;          // RCFFLG 
    }
}





//////////////////////////////////////////////////////
// BackRollChange( void )          ASM = BACKROLLCHANGE
// 
// Back Roll Change
//
// P.Smith                          7-04-2005
//
//////////////////////////////////////////////////////

void BackRollChange( void )
{
    if(!g_nBackRollChangeLag) // RCF_LAG  -lag counter = 0 ?
    {
        g_nBackRollChangeLag = ROLLCHANGEBOUNCETIMEINSECONDS;  // RCB_LAG           
        g_bActivateBackRollChange = TRUE;          // RCBFLG 
    }
}



//////////////////////////////////////////////////////
// CalculateScrewSpeed( void )          ASM = SSPEED
// 
// Calculate screw speed
// Screw speed = constant / 1000 * (extruder a/d reading (from SEI) -  screw speed tare counter)
//
// P.Smith                          4-10-2005
//
//////////////////////////////////////////////////////

void CalculateScrewSpeed( void )   //nbb--todolp-- test this function.
{
float   fDiff;

// SSAD_TARE merged inline
    if(g_CalibrationData.m_nScewSpeedSignalType == SCEW_SPEED_SIGNAL_TACHO)
    {
        if(g_nExtruderTachoReadingAverage  > g_CalibrationData.m_nScrewSpeedTareCounts)
        {
            fDiff = g_nExtruderTachoReadingAverage - g_CalibrationData.m_nScrewSpeedTareCounts;  //asm (SEIADREADINGAVG - SSTARE)
        }
        else
        {        
            fDiff = 0.0f;       // reading less than tare value
        }
    //  review if this calculation is correct for the screw speed constant.
        g_fExtruderRPM =  fDiff * (g_CalibrationData.m_fScrewSpeedConstant / 100000);

    // merge CPYERPMMB inline

        if(g_nProtocol == PROTOCOL_MODBUS)
        {
            g_arrnMBTable[BATCH_SUMMARY_EXTRUDER_SPEED] = (int)(g_fExtruderRPM * 100);        //  asm MBERPM.
            g_arrnMBTable[BATCH_SUMMARY_EXTRUDER_AD] = g_nExtruderDAValue;        // asm MBEA_D
            g_arrnMBTable[BATCH_SUMMARY_SCREW_SPEED_ATD] = g_nExtruderTachoReadingAverage;        // asm MBEA_D
        }    
    }
}

                            
/*

;FRONT ROLL CHANGE INITIATION.

FRONTROLLCHANGE:
        TST     RCF_LAG         ; ROLL CHANGE BOUNCING.
        BNE     FBNC            ; YES
        LDAA    #ROLLCHANGEBOUNCE ; ROLL CHANGE BOUNCING.
        STAA    RCF_LAG         ; SET
        LDAA    #1
        STAA    RCFFLG          ; INITIATE RC.
FBNC    RTS

; BACK ROLL INITIATION.


BACKROLLCHANGE:
        TST     RCB_LAG         ; ROLL CHANGE BOUNCING.
        BNE     BBNC            ; YES
        LDAA    #ROLLCHANGEBOUNCE ; ROLL CHANGE BOUNCING.
        STAA    RCB_LAG         ; SET
        LDAA    #1
        STAA    RCBFLG          ; INITIATE RC.
BBNC    RTS



*/

/*


; COMMANDS.

; COMMANDS.
GOTOINCDECBITPOS        EQU     %01000000
GOTOREMOTEBITPOS        EQU     %00000010
CLEARRESETBITPOS        EQU     %00000001
CLEARFRCBITPOS          EQU     %10000000       ; FRONT RC RESET.
CLEARBRCBITPOS          EQU     %01000000       ; BACK RC RESET.
CLEARAMBITPOS           EQU     %00010000       ; A/M RESET.


#define SEI_CMD_CLR_RESET_BIT                   0x0001  // ASM = CLEARRESETBITPOS        EQU     %00000001
#define SEI_CMD_GO_TO_REMOTE_BIT                0x0002  // ASM = GOTOREMOTEBITPOS        EQU     %00000010
#define SEI_CMD_CLR_PULSE_ACCUM_BIT             0x0004
#define SEI_CMD_CLR_NOT_USED_BIT                0x0008  // Not used
#define SEI_CMD_CLR_AUTO_MAN_BIT                0x0010  // ASM = CLEARAMBITPOS           EQU     %00010000       ; A/M RESET.
#define SEI_CMD_GO_TO_VFOLLOWER_BIT             0x0020  // ASM = GOTOVFMODEBITPOS        EQU     %00100000       ; VOLTAGE FOLLOWER BIT POSITION.
#define SEI_CMD_GO_TO_INCDEC_BIT                0x0040  // ASM = GOTOINCDECBITPOS        EQU     %01000000




#define  MB_SEI_INPUTS                              11  // SEI inputs
#define  MB_SEI_CURRENT_DA                          12  // D/A value being output by SEI
#define  MB_SEI_LSPD_ACCUM_MSW                      13  // Line speed pulses accumulator m.s.word
#define  MB_SEI_LSPD_ACCUM_LSW                      14  // Line speed pulses accumulator l.s.word
#define  MB_SEI_LSPD_PPS                            15  // Line speed pulses/SEC 
#define  MB_SEI_LSPD_PPS_2                          16  // Line speed pulses/SEC (T = 2sec)

#define  MB_SEI_AD                              17  // A/D value

#define  MB_SEI_INC_RATE_MEASURED               18  // measure inc. rate in a/d cts per 0.1s
#define  MB_SEI_DEC_RATE_MEASURED               19  // measure dec. rate in a/d cts per  0.1s



BANK1PROGS:        SECTION



POLLSEI TST     COMMS1COMPLETE          ; COMMS TO #1 
        LBEQ     XITEIP                  ; WAIT FOR  REMOTE DISPLAY TO FINISH.
        TST     COMMS2COMPLETE          ; HAS MESSAGE COMPLETED.
        BNE     CHKCMD                  ; CHECK FOR ANY COMMANDS.
        LDD     WATCHDOGCOMMS2          ; CHECK WATCH DOG COUNTER
        CPD     #50
        BHI     RETRYSEI
        BRA     XITEIP                  ; EXIT 
RETRYSEI:
        LDAB    #$01
        STAB    COMMS2COMPLETE          ; COMMUNICATION TO #1, COMPLETION INDICATED.
        TST     COMMS2STARTED           ; COMMS STARTED ?
        BEQ     XITEIP                  ; EXIT.
        LDX      #TIMEOUTCTR2
        JSR      INCBCD3                ; INCREMENT TIMEOUT CTR
        BRA     XITEIP               



CHKCMD  LDAA    DEVICETOPOLL            ; WHICH DEVICE TO POLL.
        CMPA    #POLLSEIID
        BNE     XITEIP      
        LDX     #SEICMDWRF               ; WRITE COMMAND
        TST     0,X
        BEQ     NOTSEC                  ; NO SEI COMMAND.
        JSR     SETSEICMD               ; WRITE COMMAND.
        BRA     XITEIP                  ; EXIT POLLING.
NOTSEC  LDX     #SEIPOLLINPUTSF          ; READ INPUTS ?
        TST     0,X
        BEQ     NOINR                   ; NO
        JSR     SEIREADINPUTS           ; INITIATE READ OF INPUTS.
        BRA     XITEIP                  ; EXIT POLLING.
NOINR   LDX     #SEIPOLLINCDECF         ; INCREASE / DECREASE
        TST     0,X
        BEQ     NOID                    ; NO
        JSR     READSEIINCDEC           ; INITIATE READ OF FWORD 13.
        BRA     XITEIP                  ; EXIT POLLING.
NOID    LDX     #READDAVALUEF           ; READ D/A VALUE FROM SEI.
        TST     0,X
        BEQ     NORDDA                  ; NO
        JSR     SEIREADDA               ; READ D/A
        BRA     XITEIP                  ; EXIT POLLING.
NORDDA  LDX     #WRITEDAVALUEF          ; WRITE D/A VALUE ?
        TST     0,X
        BEQ     NODAWR                  ; NO
        JSR     SETSEIDA                ; SET D/A VALUE.
        BRA     XITEIP                  ; EXIT POLLING.


NODAWR  LDX     #WRITEINCDECF           ; WRITE INCREASE DECREASE ?
        TST     0,X
        BEQ     NOINDE                  ; NO
        JSR     SETINCREASEDECREASEWTRITE ; SET D/A VALUE.
        BRA     XITEIP                  ; EXIT POLLING.

NOINDE  LDX     #READINCDECCAL           ; WRITE INCREASE DECREASE ?
        TST     0,X
        BEQ     NOINDEC                  ; NO
        JSR     INCDECCALDATAREAD ; SET D/A VALUE.
NOINDEC:



XITEIP  RTS                             ; GET OUT OF HERE.
                
;       COMMANDS NECESSARY FOR SERIAL EI.
;
; 1. SET D/A VALUE MODBUS LOCATION 0


SETSEIDA:
       STX     CMDINITFLAGPR     ; COMMAND INITIATION FLAG
       LDAA     #SEIDAWR        ; SEI D/A READ
       STAA     EIMBSOURCE      ; MODBUS SOURCE
       LDAB     #WRITENWORD
       JSR      GENHEADER       ; GENERATE HEADER.
       LDD     #EIDAPOS ; ADDRESS OF D/A WORD POSITION.
       STD     0,X
       AIX     #2
       LDD     #ONEWORD
       STD     0,X
       AIX     #2              ; INC TO END OF BUFFER
       LDAA    #TWOBYTES       ; BYTES TO WRITE
       STAA    0,X
       AIX     #1
       LDD      DA1TMP        ; READ D/A VALUE.
       STD      0,X            ; SET VALUE
       AIX      #2             ;
       JSR     STARTSEITXD     ; START TRANSMISSION.
       RTS

; WRITE COMMAND TO SEI AND CLEAR SAME.

SETSEICMD:
       STX     CMDINITFLAGPR     ; COMMAND INITIATION FLAG
       LDAA     #SEICMDWR        ; 
       STAA     EIMBSOURCE       ; MODBUS SOURCE
       LDAB     #WRITENWORD
       JSR      GENHEADER       ; GENERATE HEADER.
       LDD     #SEICMDPOS       ; ADDRESS OF D/A WORD POSITION.
       STD     0,X
       AIX     #2
       LDD     #ONEWORD
       STD     0,X
       AIX     #2              ; INC TO END OF BUFFER
       LDAA    #TWOBYTES       ; BYTES TO WRITE
       STAA    0,X
       AIX     #1
       LDD      SEICOMMAND     ; SEI COMMAND        
       STD      0,X            ; SET VALUE
       CLR      SEICOMMAND+1   ; COMMAND WRITTEN ONCE.
       AIX      #2             ;
       JSR     STARTSEITXD     ; START TRANSMISSION.
       RTS


; WRITE INCREASE / DECREASE INFO.

SETINCREASEDECREASEWTRITE:
       STX     CMDINITFLAGPR     ; COMMAND INITIATION FLAG
       LDAA     #SEICMDWR        ; 
       STAA     EIMBSOURCE       ; MODBUS SOURCE
       LDAB     #WRITENWORD
       JSR      GENHEADER       ; GENERATE HEADER.
       LDD     #SEIINCDECPOS       ; ADDRESS OF D/A WORD POSITION.
       STD     0,X
       AIX     #2
       LDD     #5              ; 5 WORDS
       STD     0,X
       AIX     #2              ; INC TO END OF BUFFER
       LDAA    #10             ; BYTES TO WRITE
       STAA    0,X
       AIX     #1

       LDD      CONTROLLAG     ; INCREASE RATE       
       JSR      BCDHEX2        ; BCD TO HEX.
       STD      0,X            ; 
       AIX     #2              ; INC TO END OF BUFFER
        
       LDD      INCREASERATE   ; INCREASE RATE       
       LSRD    
       LSRD    
       LSRD    
       LSRD    
       JSR      BCDHEX2        ; BCD TO HEX.
       STD      0,X            ; 
       AIX     #2              ; INC TO END OF BUFFER
       LDD      DECREASERATE   ; DECREASE RATE       
       LSRD    
       LSRD    
       LSRD    
       LSRD    
       JSR      BCDHEX2        ; BCD TO HEX.
       STD      0,X            ; 
       AIX     #2              ; INC TO END OF BUFFER
       LDD      DEADBAND       ; DEADBAND       
       JSR      BCDHEX2        ; BCD TO HEX.
       STD      0,X            ; 
       AIX     #2              ; INC TO END OF BUFFER

       LDD      FINEINCDEC   ; DECREASE RATE       
       JSR      BCDHEX2        ; BCD TO HEX.
       STD      0,X            ; 
       AIX      #2             ;
       JSR     STARTSEITXD     ; START TRANSMISSION.
       RTS

        

;       READ ALARM/STATUS,INPUTS,AND OUTPUT D/A.
;
SEIREADINPUTS:
        STX     CMDINITFLAGPR     ; COMMAND INITIATION FLAG
        LDAA     #SEIINPUTSRD    ; SEI INPUTS READ
        STAA     EIMBSOURCE      ; MODBUS SOURCE
        LDAB     #READNWRDCMD
        JSR     GENHEADER       ; GENERATE HEADER.
        LDD     #EIINPUTPOS ; ADDRESS OF 1ST WORD
        STD     0,X
        AIX     #2
        LDD     #EIGHTWORDS
        STD     0,X
        AIX     #2              ; INC TO END OF BUFFER
        JSR     STARTSEITXD        ; START TRANSMISSION.
        RTS

; READ REMOTE INCREASE / DECREASE FROM SEI CARD. 

READSEIINCDEC:
        STX     CMDINITFLAGPR     ; COMMAND INITIATION FLAG
        LDAA     #SEIINCDECRD    ; SEI INPUTS READ
        STAA     EIMBSOURCE      ; MODBUS SOURCE
        LDAB     #READNWRDCMD
        JSR     GENHEADER       ; GENERATE HEADER.
        LDD     #EIINCDECPOS ; ADDRESS OF 1ST WORD
        STD     0,X
        AIX     #2
        LDD     #ONEWORD
        STD     0,X
        AIX     #2              ; INC TO END OF BUFFER
        JSR     STARTSEITXD     ; START TRANSMISSION.
        RTS

; READ D/A VALUE FROM SEI

SEIREADDA:
        STX     CMDINITFLAGPR     ; COMMAND INITIATION FLAG
        LDAA     #SEIDARD        ; SEI INPUTS READ
        STAA     EIMBSOURCE      ; MODBUS SOURCE
        LDAB     #READNWRDCMD
        JSR     GENHEADER       ; GENERATE HEADER.
        LDD     #EIDARDPOS      ; ADDRESS OF 1ST WORD
        STD     0,X
        AIX     #2
        LDD     #ONEWORD
        STD     0,X
        AIX     #2              ; INC TO END OF BUFFER
        JSR     STARTSEITXD     ; START TRANSMISSION.
        RTS



; READ INC/DEC VALUES FROM SEI

INCDECCALDATAREAD:
        STX     CMDINITFLAGPR     ; COMMAND INITIATION FLAG
        LDAA     #SEIINCDECCALRD  ; INC/DEC CALIBRATION  DATA READ
        STAA     EIMBSOURCE      ; MODBUS SOURCE
        LDAB     #READNWRDCMD
        JSR     GENHEADER        ; GENERATE HEADER.
        LDD     #EIINCDECCALPOS  ; ADDRESS OF 1ST WORD
        STD     0,X
        AIX     #2
        LDD     #SIX            ; READ SIX WORDS
        STD     0,X
        AIX     #2              ; INC TO END OF BUFFER
        JSR     STARTSEITXD     ; START TRANSMISSION.
        RTS

; GENERATE THE MODBUS HEADER
        
GENHEADER:
       LDAA     #EIADDRESS
       STAA   IVRADD    ; ADDRESS STORED
       STAB   COMANDSE  ;~SAVE THE COMMAND FOR LATER
       JSR    GENHDRSE  ;~GENERATE THE MESSAGE HEADER
       RTS


SEIINPUTSPROCESS:
        CLR     SEITIMEOUTCTR   ; SEI COUNTER CLEARED.
        TST     SEITIMEOUTFLAG  ; TIMEOUT ?
        BEQ     NOTO            ; NO
        LDAA    EIOMITTED               ;
        ANDA    #SEIENBITPOS        
        BEQ     NOSEIC
        LDAA    MBPROALRM+1             ; MODBUS ALARMS.
        ANDA    #SEICOMMSALARMBIT       ; SEI COMMS ALARM ?
        BEQ     NOSEIC                  ; ALREADY CLEAR
        LDAA    #SEICOMMSALARM          ; SEI COMMS ALARM
        JSR     RMALTB                  ; INDICATE ALARM.
NOSEIC  LDAA    #1
        STAA    READDAVALUEF        
        CLR     SEITIMEOUTFLAG  ; RESET TIMEOUT FLAG.
NOTO    LDAA    CRLMODE         ; CHECK MODE
        CMPA    #VOLTAGEFOLLOWER
        BNE     NOTVFM          ; NOT VF MODE
        LDD     SEIALARMSTATE
        ANDD    #VOLFOLBITPOS    ; LOCAL BIT POSITION.
        BNE     CON1              ; IS IN VF MODE
        JSR     SEIVOLFOLON
        BRA     CON1

             
NOTVFM  LDD     SEIALARMSTATE
        ANDD    #VOLFOLBITPOS    ; LOCAL BIT POSITION.
        BEQ     CON1             ; IS NOT IN VF MODE
        JSR     SEIVOLFOLOFF

CON1    LDAA    CONTROLTYPE        ; CHECK MODE
        CMPA    #INCREASEDECREASE  ; INCREASE / DECREASE
        BNE     NOTIND            ; NOT INCREASE /DECREASE
        LDD     SEIALARMSTATE
        ANDD    #INCDECBITPOS    ; LOCAL BIT POSITION.
        BNE     CON2             ; 
        JSR     SEIINCDECON      ; SET INCREASE / DECREASE MODE ON.
        BRA     CON2
             
NOTIND  LDD     SEIALARMSTATE
        ANDD    #INCDECBITPOS    ; .
        BEQ     CON2             ; 
        JSR     SEIINCDECOFF     ; SET INCREASE / DECREASE MODE ON.
CON2    LDD     SEIALARMSTATE
        ANDD    #RESETBITPOS    ; RESET ACTIVE
        BEQ     NORESET         ; NO RESET.
        LDAA    #CLEARRESETBITPOS; .
        STAA    SEICOMMAND+1    ; COMMAND.
        LDAA    #1
        STAA    SEICMDWRF       ; INITIATE COMMAND.
        JSR     TOMANUAL        ; SWITCH BLENDER TO MANUAL

        LDX     #TIMEBUF
        LDAA    HOUR,X
        CMPA    RESETHOUR
        BNE     SIGRES          ; SIGNAL RESET.
        LDAA    MIN,X
        CMPA    RESETMIN
        BEQ     FSTRES
SIGRES  LDAA    #1
        STAA    PRINTTOMANUAL   ; PRINT THAT THE SEI HAS RESET.
        LDAA    #SEIRESETALARM          ; SEI COMMS ALARM
        JSR     PTALTB                  ; INDICATE ALARM.
FSTRES  LBRA     XITINP          ; EXIT.
NORESET LDD     SEIALARMSTATE
        ANDD    #LOCALBITPOS    ; LOCAL BIT POSITION.
        BEQ     NOTLOC          ; NOT LOCAL.
        LDAA    #GOTOREMOTEBITPOS; BIT POSITION.
        STAA    SEICOMMAND+1    ; COMMAND.
        LDAA    #1
        STAA    SEICMDWRF       ; INITIATE COMMAND.
        LBRA     XITINP          ; EXIT.
NOTLOC  LDD     SEIALARMSTATE
        TDE     
        ANDD    #AMFBITPOS      ; AUTO MANUAL TOGGLE.
        BEQ     NOAM            ; NO A/M
        LDAA    #CLEARAMBITPOS  ; CLEAR A/M COMMAND.
        STAA    SEICOMMAND+1    ; INITIATE COMMAND.
        LDAA    #1
        STAA    SEICMDWRF       ; INITIATE COMMAND.
        JSR     AMKEY           ; AUTO/MANUAL HANDLER
        BRA     XITINP          ; EXIT.
NOAM    TED                     ; RECOVER STATUS
        ANDD    #FRCBITPOS      ; FRONT RC ?
        BEQ     NOFRC           ; NO
        LDAA    #CLEARFRCBITPOS ; CLEAR FRC COMMAND.
        STAA    SEICOMMAND+1    ; INITIATE COMMAND.
        LDAA    #1
        STAA    SEICMDWRF       ; INITIATE COMMAND.
        JSR     FRONTROLLCHANGE ;
        BRA     XITINP          ; EXIT.
NOFRC   TED                     ; RECOVER STATUS
        ANDD    #BRCBITPOS      ; BACK RC ?
        BEQ     NOBRC           ; NO
        LDAA    #CLEARBRCBITPOS ; CLEAR BACK RC COMMAND.
        STAA    SEICOMMAND+1    ; INITIATE COMMAND.
        LDAA    #1
        STAA    SEICMDWRF       ; INITIATE COMMAND.
        JSR     BACKROLLCHANGE  ; BACK ROLL CHANGE
        BRA     XITINP          ; EXIT.
NOBRC   LDAA    ALARMONFLAG     ; INDICATES THAT ALARM IS ON.
        CMPA    ALARMONTRFLAG   ; TRANSITION INDICATION FOR COMMUNICATIONS.
        BEQ     XITINP          ; NO CHANGE
        TST     ALARMONFLAG
        BEQ     ALARMOFF
        LDAA    #ALARMBITPOS    ; CLEAR BACK RC COMMAND.
        ORAA    SEICOMMAND
        STAA    SEICOMMAND      ; INITIATE COMMAND.
        BRA     SETCMDFA        ; SET COMMAND FOR ALARM.
ALARMOFF:
        LDAA    #ALARMBITPOS ; CLEAR BACK RC COMMAND.
        COMA    
        ANDA    SEICOMMAND
        STAA    SEICOMMAND   ; INITIATE COMMAND.
SETCMDFA:
        LDAA    ALARMONFLAG
        STAA    ALARMONTRFLAG
        LDAA    #1
        STAA    SEICMDWRF       ; INITIATE COMMAND.
XITINP  RTS   

; CHECK FOR SEI TIMEOUT.

CHKFORSEITO:
        INC     SEITIMEOUTCTR           ; INCREMENT WATCHFOG TIMER.
        LDAA    SEITIMEOUTCTR
        CMPA    #SEITIMEOUTMAX+1        ; SEI TIMEOUT
        BLO     NOSEIT                  ; NO
        LDAA    EIOMITTED               ;
        ANDA    #SEIENBITPOS        
        BEQ     NOSEI
        LDAA    MBPROALRM+1             ; MODBUS ALARMS.
        ANDA    #SEICOMMSALARMBIT       ; SEI COMMS ALARM ?
        BNE     NOSEIT                  ; ALREADY SET
        LDAA    #SEICOMMSALARM          ; SEI COMMS ALARM
        JSR     PTALTB                  ; INDICATE ALARM.
NOSEI   LDAA    #1
        STAA    SEITIMEOUTFLAG          
NOSEIT  RTS        


; READ WORD COMMAND FOR SERIAL EI.
;
RWSEI           LDAA    EIMBSOURCE      ; READ MB SOURCE.
                CMPA    #SEIDARD        ; D/A READ
                BNE     NODARD          ; NO
                LDE     RXBUFFSE+3      ; READ DATA.
                STE     SEIDARDSTATE    ; INDICATE SEI READ.
                LDAA    CRLMODE         ; CHECK MODE
                CMPA    #VOLTAGEFOLLOWER ; VOLTAGE FOLLOWER
                BNE     NOCHANGE        ; NO
                TST     MANATO                  ; AUTO MODE
                BNE     NODARD        ; NO READ IF IN AUTO.
                CPE     DA1TMP          ; CHECK FOR CHANGE.
                BEQ     NOCHANGE        ; NO
                INC     CCVSFLG         ; CALCULATE CONTROL SETTINGS
                LDAA    #1
                STAA    RUNEST          ; RUN ESTIMATE.
NOCHANGE:       LDAA    CONTROLTYPE
                CMPA    #INCREASEDECREASE
                BNE     NOIDAD          ; NO
                LDE     SEIADREADING
NOIDAD          STE     DA1TMP       
                LDAA    #1
                STAA    CCVSFLG
                LBRA     XITRW           ; EXIT.
NODARD          CMPA    #SEIINPUTSRD    ; READ INPUTS ?
                BNE     NOINRD          ; NO

; READ ALARM STATUS, INPUT STATE, D/A OUTPUT STATE,LINE SPEED PULSE ACC,PULSES / SECOND.

                LDD     RXBUFFSE+3      ; READ INPUTS
                STD     SEIALARMSTATE   ; ALARM STATUS.
                LDD     RXBUFFSE+5      ; 
                STD     SEIINPUTSTATE   ; INPUT STATE
                LDD     RXBUFFSE+7
                STD     SEIDAOPSTATE    ; INC / DEC STATE.
                LDD     RXBUFFSE+9      ; PULSE INFO.
                STD     SEIPULSEACC     ; SEI PULSE
                LDD     RXBUFFSE+11     ; PULSE INFO.
                STD     SEIPULSEACC+2   ; SEI PULSE
                LDD     RXBUFFSE+13     ; READ PULSES PER SECOND.
                STD     SEIPULSEPSEC    ; PULSES PER SECOND.
 ; PULSES PER SECOND 2 SECOND AVERAGE.
 ; A/D 16 BITS
 

                LDD     RXBUFFSE+17     ; READ PULSES PER SECOND.
                STD     VOLFOLREADING   ; PULSES PER SECOND.
                STD     SEIADREADING    ; SEI AD READING STORED.
                JSR     AVERAGEADREADING ; AVERAGE READING FOR USEAGE.
                STD     HCAD0RES        ;~RESULT STORE FOR A/D DISPLAY.
 
 
                LDAA    #1
                STAA    SEIPULSESREADY  ; INDICATES PULSE READY.       
                CLRW    LSPDFRACTIONCTR ; RESET FRACTION CTR.
                JSR     SEIINPUTSPROCESS; PROCESS INPUTS
                BRA     XITRW           ; EXIT.               
; READ INCREASE / DECREASE STATUS.

NOINRD          CMPA    #SEIINCDECRD    ; READ INCREASE / DECREASE.
                BNE     NOINCDEC        ; NO
                LDD     RXBUFFSE+3      ; 
                STD     SEIINCDECSTATE  ; INCREASE / DECREASE.
                JSR     PROCESSINCDEC   ; PROCESS INCREASE /DECREASE.
NOINCDEC        NOP
                CMPA     #SEIDAWR        ; SEI D/A READ
                BNE     NOTDAWR         ; NO:

NOTDAWR:        CMPA    #SEIINCDECCALRD  ; INC DEC CALIBRATION DATA READ
                BNE     NOTINDRD        ; NO
                LDX     RXBUFFSE+3      ; START UP VALUE FOR RELAY
                LDD     RXBUFFSE+5      ; CONTROL LAG 
                JSR     HEXBCD2         ; 
                STE     CONTROLLAG      ;
                LDD     RXBUFFSE+7      ; INCREASE RATE 
                JSR     HEXBCD2         ; 
                ASLE
                ASLE
                ASLE
                ASLE
                STE     INCREASERATE    ;
                LDD     RXBUFFSE+9      ; DECREASE RATE 
                JSR     HEXBCD2         ; 
                ASLE
                ASLE
                ASLE
                ASLE
                STE     DECREASERATE    ;
                LDD     RXBUFFSE+11     ; DEADBAND 
                JSR     HEXBCD2         ; 
                STE     DEADBAND        ;
                LDD     RXBUFFSE+13     ; FINEBAND READ 
                JSR     HEXBCD2         ; 
                STE     FINEINCDEC    ;
NOTINDRD:

XITRW           RTS                     ;        



PROCESSINCDEC:
                LDAA    MANATO
                BNE     INCDECOFF       ; OUTPUTS SHOULD BE OFF.        
                LDD     SEIINCDECSTATE  ; READ DECREASE STATE                                                                                                           
                BEQ     INCDECOFF       ; INCREASE / DECREASE OFF.
                TSTA                    ; INCREASE ?
                BEQ     NOINC           ; NO
                LDAA    #3
                STAA    COMMSIFLG       ; INITIATE INCREASE                 
                JSR     SEIINCLAMPONF    ; SEI LAMP ON.
                BRA     XITRD
NOINC           TSTB                    ; DECREASE ?
                BEQ     XITRD           ; EXIT.
                LDAB    #3
                STAB    COMMSDFLG       ; INITIATE DECREASE.                 
                JSR     SEIDECLAMPONF    ; SEI LAMP ON.
XITRD           RTS                
INCDECOFF       JSR     SEIINCLAMPOFFF   ; SEI INC LAMP OFF.
                JSR     SEIDECLAMPOFFF   ; SEI DEC LAMP OFF.
                RTS
                                                                                  
SEIINCLAMPONF   LDAA    #%00000001
                ORAA    INCLAMPSTATUS
                STAA    INCLAMPSTATUS
                RTS

SEIINCLAMPOFFF  LDAA    #%00000001
                COMA
                ANDA    INCLAMPSTATUS
                STAA    INCLAMPSTATUS
                RTS
                                   
SEIDECLAMPONF   LDAA    #%00000001
                ORAA    DECLAMPSTATUS
                STAA    DECLAMPSTATUS
                RTS

SEIDECLAMPOFFF  LDAA    #%00000001
                COMA
                ANDA    DECLAMPSTATUS
                STAA    DECLAMPSTATUS
                RTS

REMINCLAMPONF   LDAA    #%00000010
                ORAA    INCLAMPSTATUS
                STAA    INCLAMPSTATUS
                RTS

REMINCLAMPOFFF  LDAA    #%00000010
                COMA
                ANDA    INCLAMPSTATUS
                STAA    INCLAMPSTATUS
                RTS
                                   
REMDECLAMPONF   LDAA    #%00000010
                ORAA    DECLAMPSTATUS
                STAA    DECLAMPSTATUS
                RTS

REMDECLAMPOFFF  LDAA    #%00000010
                COMA
                ANDA    DECLAMPSTATUS
                STAA    DECLAMPSTATUS
                RTS
                

;
;       SEI TO VOLTAGE FOLLLOWER MODE.
;
SEIVOLFOLON:
                LDAA    #GOTOVFMODEBITPOS
                ORAA    SEICOMMAND
                STAA    SEICOMMAND      ; COMMAND.
                LDAA    #1
                STAA    SEICMDWRF            
                RTS

                                                                            
; SWITHC INCREASE LAMP ON.

SEIINCLAMPON:
                LDAA    SEICOMMAND
                ANDA    #INCLAMPBITPOS  ; INC LAMP ON ?
                BNE     IISON
                LDAA    #INCLAMPBITPOS
                ORAA    SEICOMMAND
                STAA    SEICOMMAND      ; COMMAND.
                LDAA    #1
                STAA    SEICMDWRF            
IISON           RTS
                
SEIDECLAMPON:
                LDAA    SEICOMMAND
                ANDA    #DECLAMPBITPOS  ; DEC LAMP ON ?
                BNE     DISON
                LDAA    #DECLAMPBITPOS
                ORAA    SEICOMMAND
                STAA    SEICOMMAND      ; COMMAND.
                LDAA    #1
                STAA    SEICMDWRF            
DISON           RTS
                
SEIINCLAMPOFF:
                LDAA    SEICOMMAND
                ANDA    #INCLAMPBITPOS  ; DEC LAMP OFF ?
                BEQ     IISOFF
                LDAA    #INCLAMPBITPOS  ; 
                COMA                    ; DEC LAMP OFF
                ANDA    SEICOMMAND      ;
                STAA    SEICOMMAND      ;                
                LDAA    #1
                STAA    SEICMDWRF            
IISOFF          RTS

SEIDECLAMPOFF:
                LDAA    SEICOMMAND
                ANDA    #DECLAMPBITPOS  ; DEC LAMP OFF ?
                BEQ     DISOFF
                LDAA    #DECLAMPBITPOS  ; 
                COMA                    ; DEC LAMP OFF
                ANDA    SEICOMMAND      ;
                STAA    SEICOMMAND      ;                
                LDAA    #1
                STAA    SEICMDWRF            
DISOFF          RTS                                                




; A/M LAMP ON.
SEIAMLAMPON:
                LDAA    #AMLAMPBITPOS
                ORAA    SEICOMMAND
                STAA    SEICOMMAND      ; COMMAND.
                LDAA    #1
                STAA    SEICMDWRF            
                RTS

; A/M LAMP OFF
SEIAMLAMPOFF:
                LDAA    #AMLAMPBITPOS
                COMA    
                ANDA    SEICOMMAND
                STAA    SEICOMMAND      ; COMMAND.
                LDAA    #1
                STAA    SEICMDWRF            
                RTS
                     
SEIVOLFOLOFF:
                LDAA    #GOTOVFMODEBITPOS
                COMA    
                ANDA    SEICOMMAND
                STAA    SEICOMMAND      ; COMMAND.
                LDAA    #1
                STAA    SEICMDWRF            
                RTS




STARTSEIDAWR:
       LDAA     #1
       STAA     WRITEDAVALUEF           ; INITIATE D/A WRITE.       
       RTS


SWITCHSEILAMPONOFF:
        LDAA    INCLAMPSTATUS
        ANDA    #%00000011
        BEQ     ILAMPOF
        JSR     SEIINCLAMPON            ; INC LAMP ON.
        RTS
ILAMPOF JSR     SEIINCLAMPOFF           ; INC LAMPOFF
        LDAA    DECLAMPSTATUS           ; CHECK STATUS.        
        ANDA    #%00000011
        BEQ     DLAMPOF
        JSR     SEIDECLAMPON            ; DEC LAMP ON.
        RTS
DLAMPOF JSR     SEIDECLAMPOFF           ; DEC LAMP OFF.
        RTS        
        
        
        
INITIATEINCDECWRRD:
        LDAA    #1
        STAA    WRITEINCDECF
        LDAA    #3
        STAA    DOREADINNSECONDS

        RTS
        
        
        


;       SEI INCREASE /DECREASE MODE 


SEIINCDECON:
           LDAA    #GOTOINCDECBITPOS
           ORAA    SEICOMMAND
           STAA    SEICOMMAND      ; COMMAND.
           LDAA    #1
           STAA    SEICMDWRF            
           RTS



;       SEI INCREASE /DECREASE MODE 


SEIINCDECOFF:
           LDAA    #GOTOINCDECBITPOS
           ANDA    SEICOMMAND
           STAA    SEICOMMAND      ; COMMAND.
           LDAA    #1
           STAA    SEICMDWRF            
           RTS


                        
mce12-15

;
; SSPEED IS A ROUTINE TO CALCULATE MAIN SCREW'S SPEED IN RPM
;        SSRPM = SS A/D CTS * SS CONST.

SSAD_TARE:
       LDD    SEIADREADINGAVG
       JSR    HEXBCD2           ; HEX TO BCD 
       STAB   HCAD0BCD
       STE    HCAD0BCD+1          
       LDD      SEIADREADING
       CPD      SSTARE
       BHI      CALDIF          ; CALCULATE DIFFERENCE.
       LDE      #0
       CLRB
       BRA      STRDIF          ; STORE DIFFERENCE
CALDIF LDD    SEIADREADINGAVG
       SUBD   SSTARE            ; SUBTRACT TARE VALUE
       JSR    HEXBCD2           ; HEX TO BCD 
STRDIF CLRW   AREG              ; RESET A REGISTER.
       STAB   AREG+2
       STE    AREG+3            ; STORE TO A REGISTER.       
SSPEED2 RTS



SSPEED:
;       TST    SSSTYPE            ; TACHO(0) SIGNAL?
;       LBNE   SSPEEDC            ; GO TO SCREW SPEED ON COMMs
       JSR    SSAD_TARE          ; A/D - TARE COUNTS
       JSR    CLCREG           ;~CLEAR CREG
       LDX    SSCON1            ;~SCREW SPEED CONST (4 DIGITS)
       STX    MATHCREG+3        ;~PUT IN L.S. BYTES OF CREG
       JSR    MUL               ;~SS A/D CTS * CONST.
       LDX    #EREG
       LDAA   #3
       JSR    GPRND1            ;~ROUND ANSWER TO 3 DIGITS
       LDX    #EREG
       LDAB   #5
       LDAA   #3
       JSR    GPSFTR            ;~MOVE 3 DIGITS RIGHT
       LDX    #SSRPM2D          ;~RPM TO 2 DECIMAL PLACES
       JSR    EMOVE             ;~MOVE TO RPM STORAGE

       LDX    #EREG
       LDAA   #2
       JSR    GPRND1            ;~ROUND A FURTHER 2 DIGITS
       LDX    #EREG
       LDAB   #5
       LDAA   #2
       JSR    GPSFTR            ;~MOVE 2 DIGITS RIGHT

       LDX    #SSRPM                    ;~RESULT RPM STORAGE
       JSR    EMOVE             ;~STORE VALUE
SSPEEDC:
       JSR    CPYERPMMB         ; COPY EXT. RPM -> MODBUS TABLE       
       LDX      SSRPM
       BNE      SSOK
       NOP
SSOK:              
       RTS



;* COPY EXTRUDER RPM  INTO MODBUS TABLE PROD. SUMMMARY DATA 
CPYERPMMB:
        TST     PROTOCOL
        BPL     CPYERPMX
        
        LDX     #SSRPM2D        ; RPM TO 2DP
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD  TO HEX        
        STE     MBERPM          ; MAX VALUE EXPECTED = 655.35 RPM

        LDD     HCAD0RES        ; A/D COUNTS
        STD     MBEA_D
CPYERPMX RTS        


*/
