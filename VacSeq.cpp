//////////////////////////////////////////////////////
// VacSeq.c
//
// Group of functions for Vacuum Loading control/sequencing
//
// M.McKiernan                          09-12-2009
// First Pass
//
// M.McKiernan                          07-09-2009
// Added ClearAllLoaderAlarms( void ); see new 7.Sept.2009
// New functions - VacLoaderSystemEnterManualMode(  );  VacLoaderSystemExitManualMode( );
// called from VacTenHzPrograms().
// In SetUpVAC8OutputWords() - use manual commands when in Manual mode - g_bVacLoaderSystemInManual
// Renamed m_nDumpDelayTime -> m_nDischargeTime.  -- calibration parameter
// Renamed m_nDumpDelay -> m_nDischargeTmr.  - loader variable.
// Edited SetUpVAC8OutputWords( )  to cater for loaders (outputs) on 2nd VAC8IO.
// Edited UpdateLoaderDataFromVAC8IO to cater for loaders (inputs) on 2nd & 3rd VAC8IO cards
// Added function - CheckForLoaderOutputOn(); use in safety check
// Safety check - if pump on, and Atm. valve closed, if no loader ON, then open A.V.
// Added LOADER_STATUS_UNUSED - g_sLoader[i].m_nStatusByte - for unused loaders.

//
// M.McKiernan                          15-09-2009
//  - When a unit just been turned off - nUnitSwitchedOffIndex
//  leave fill valve on while overlap timer > 0.  - g_sLoader[i].m_nOverLapTimer
// note use fixed O.L. time (LOADER_VALVE_OVERLAP_TIME) in case A.V. overlap = 0.
//	See g_sLoader[nFUIndex].m_nOverLapTimer = LOADER_VALVE_OVERLAP_TIME * 10;
// corrected 3 =='s which should have been =.
// Count the total no. of alarms - g_nVacLoaderAlarmsTotal
// Fixed a bug at 3rd card section in SetUpVAC8OutputWords( ), was using Card2Mask, instead of Card3Mask.
//
// P.Smith                          15/10/09
// use g_nVaccumLoadingVAC8IOCards instead of g_nVAC8IOCards to for
// vacuuming stuff. g_nVAC8IOCards includes all cards including expansion.
//
// P.Smith                          15/10/09
// problem seen where the vaccuming stops when the blow back is activated in the loader.
// the flap opens for a second and the software thinks that the loader is full and switches
// off the solenoid.
//////////////////////////// ////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <Stdio.h>
#include <ctype.h>
#include <iosys.h>

#include "InitialiseHardware.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"
#include "string.h"

#include "BatchCalibrationFunctions.h"
#include "BatchMBIndices.h"
#include "ConfigurationFunctions.h"
#include "Foreground.h"
#include "16R40C.h"
#include    "FiftyHz.h"
#include    "BatVars.h"
#include    "BatVars2.h"
#include    "Pause.h"
#include    "MBProSum.h"
#include    "MBProgs.h"
#include "Cleanio.h"
#include    "TenHz.h"
#include    "FGOneHz.h"
#include "SetpointFormat.h"
#include    "MBCyDiag.h"
#include "Alarms.h"
#include "PrntDiag.h"
#include "Exstall.h"
#include "Conalg.h"
#include "TimeDate.h"
#include "TSMPeripheralsMBIndices.h"
#include "License.h"
#include <basictypes.h>
#include <string.h>
#include "I2CFuncs.h"
#include "TimeDate.h"
#include "Lspeed.h"
#include "Multiblend.h"
#include "Expansion.h"
#include "CPIRRobin.h"
#include "OptimisationMB.h"
#include "Optimisation.h"
#include "Sdcard.h"
#include "Screw.h"
#include "LiWVars.h"
#include "LSpeed.h"
#include "Blrep.h"
#include "MBSHand.h"
#include "MBPanelSNBBHand.h"
#include "Debug.h"
#include "VacVars.h"
#include "VacSeq.h"

/* for  putting into VacSeq.h

*/

BYTE   g_VacLevelSimulatorTime[8] = {23,34,43,22,33,65,51,70};
int    g_nLoaderFillInProgressTime = 0;
extern   structVacLoaderData g_sLoader[];
extern   structVacCalib   g_VacCalibrationData;

extern   int g_arrnWriteVAC8IOMBTable[MAX_VAC8IO_CARDS][VAC8IO_TABLE_WRITE_SIZE];
extern   int g_arrnReadVAC8IOMBTable[MAX_VAC8IO_CARDS][VAC8IO_TABLE_READ_SIZE];

// Locally declared global variables

/*******************************************************************
*   Function Name:      InitialiseVacSystem()                       *
*   Return Value:       void                                        *
*   Parameters:         none                                        *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Initialises all data and o/p's for VAC system *
*                                                                   *
********************************************************************/
void InitialiseVacSystem( void )
{

 // ToDo   testonly - fix some calibration data.
//   HardCodeVacCalibrationData();
   //should be LoadVacCalibrationDataFromEEPROM();

   InitialiseVacVars();

   SetUpVAC8OutputWords( TRUE );    // Set up o/p's for sending to hardware. (VAC8 -
                              // will setup g_nWriteVAC8WordOps1 & g_nWriteVAC8WordOps2)

}

/*******************************************************************
*   Function Name:      AddToFillTable()                              *
*   Return Value:       void                                        *
*   Parameters:         none                                        *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Currently just adds unit into fill table at *
*                       first empty position.                       *                                                    *
*                       ToDo - account for priority                 *
********************************************************************/
void AddToFillTable( BYTE nLoaderNo )
{
 int i;
  bool bFound = FALSE;

// Check if already in table
	for(i=0; i<TO_FILL_TABLE_SIZE; i++)
	{
		if( g_nToFillTable[i] == nLoaderNo)
		 bFound = TRUE;
//			n=1;
	}

// if not found add to table.
//	if(n==0)
   if(!bFound)
	{
		// find first empty (0) in table.
		i=0;
		while( g_nToFillTable[i] !=0 )
		{
			i++;
		}

		g_nToFillTable[i] = nLoaderNo; 	// add at first empty location in table.
	}

}

/*******************************************************************
*   Function Name:      AddToFillTableWithPriority()                *
*   Return Value:       void                                        *
*   Parameters:         none                                        *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Adds loader into fill table                 *
*                       accounts for priority                       *
*                       Priority 1 is highest, 7 lowest             *
********************************************************************/
void AddToFillTableWithPriority( BYTE nLoaderNo )
{
 int i,j,nTi;
 bool bFound = FALSE;
 bool bAdded = FALSE;
 BYTE nNum;
 BYTE nCurrentIndex,nNewIndex;
 BYTE nCurrentPriority,nNewPriority;

// Check if  table empty.
   if( g_nToFillTable[0] == 0 )
   {
      g_nToFillTable[0] = nLoaderNo;   // if fill table empty, add loader in first place.
//      if(fdnet>0)
//         printf("\nAToF.>..%d", nLoaderNo);

      return;                           // and exit.
   }
// Check if already in table
	for(i=0; i<TO_FILL_TABLE_SIZE; i++)
	{
		if( g_nToFillTable[i] == nLoaderNo)
		{
		    bFound = TRUE;
          return;                           // exit if already in table.
      }
//			n=1;
	}

// if not found add to table.
   nNewIndex = nLoaderNo-1;            // index for loader to be added.
   nNewPriority = g_VacCalibrationData.m_nPriority[nNewIndex];


   if(!bFound)
	{

		i=TO_FILL_TABLE_SIZE-1;     // work from bottom up

		while( g_nToFillTable[i] == 0 && i>0 )
		{
			i--;        // find first non-empty (0) in table.
		}
		while( i>=0 && !bAdded)
		{
		    nNum = g_nToFillTable[i];    // loader no. at this table positiion
		    nCurrentIndex = nNum-1;        // loader indices are 0 based. (loaders start at 1)
		    nCurrentPriority = g_VacCalibrationData.m_nPriority[nCurrentIndex];
		    // new loader's priority higher (lower priority no) than current - keep searching.
		       //  higher or same priority found.
		    if( nNewPriority >= nCurrentPriority || i==0 )
		    {
		          if( i==0 && nNewPriority < nCurrentPriority )
		                nTi=0;     // add at top. nT=newTableIndex
		          else
		             nTi=i+1;      // add in just below current entry.

         		// move all entries down by 1.
         		j=TO_FILL_TABLE_SIZE-1;        // start at  last position in table
         		while(j<nTi)
         		{
         			g_nToFillTable[j] = g_nToFillTable[j-1];  // move entries down by 1
         			j--;
         		}
         		// now add in loader at empty position
		          g_nToFillTable[nTi] = nLoaderNo;
		          bAdded = TRUE;

		    }
		    else
		       i--;

		}
		if(!bAdded)  // hasnt been added - This should never arise.
		{
         		// no higher or same priority loader found, therefore add at top, index 0.
		          g_nToFillTable[0] = nLoaderNo;
		}

	}

}


/*******************************************************************
*   Function Name:      PopFromToFillTable()                              *
*   Return Value:       void                                        *
*   Parameters:         none                                        *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:                                                    *                                                    *
********************************************************************/
unsigned char PopFromToFillTable(void )
{
 int i;
 unsigned char n=0;

 	if( g_nToFillTable[0] == 0)
		return(0);
	else
	{
		n = g_nToFillTable[0];		// take first entry in table
		// move all entries up by 1.
		i=0;
		while(i<TO_FILL_TABLE_SIZE-2)
		{
			g_nToFillTable[i] = g_nToFillTable[i+1];
			i++;

		}
		return( n );
	}

}

/*******************************************************************
*   Function Name:      CheckToFillTable()                          *
*   Return Value:       void                                        *
*   Parameters:         none                                        *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:     Checks the "To Fill" table and removes units    *
*                    that are either off or full                     *
********************************************************************/
void CheckToFillTable(void )
{
 int i,j;
 BYTE nIndex;
 		i=0;
 		while( g_nToFillTable[i] != 0)
 		{
 		   nIndex = g_nToFillTable[i] - 1;     // loader index is 0 based. (loader no. - 1)

 		   // check that this loader has not been switched off or become full.  or filling.
 		   if(g_sLoader[nIndex].m_bLevelFlag == FULL || g_VacCalibrationData.m_bOnFlag[nIndex] == OFF || g_sLoader[nIndex].m_bHiLevelSensor == COVERED || g_sLoader[nIndex].m_nFillTimer)
 		   {
 		      // remove from fill table.
// 		      if(fdnet>0)
// 		         printf("\nRemoving loader no %d............\n", g_nToFillTable[i] );
 		      j=i;
 		      while(j<TO_FILL_TABLE_SIZE-2)
      		{
      			g_nToFillTable[j] = g_nToFillTable[j+1];
      			j++;
      		}

 		   }

 		   i++;
 		}

}


// AV open if  nAVOverLapTimer or  nAVOpenTimer
/********************************************************************
*   Function Name:      VACLoaderLogic()                          *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Logic for a stand alone VAC8 controller     *
********************************************************************/
#define MIN_LOADER_ON_TIME (2)         // minimum on time before checking for full.
void VACLoaderLogic(void)
{
//char i,AlarmCtr=0;
//unsigned char Pri,Poi,Sti,ABi;
//unsigned char Prn,Pon,Stn,ABn;
//unsigned char nPassOn = 0;   // pass on fill rights
//unsigned char n, nLoader;
   int i;
   unsigned char nNext;
   BYTE nFUIndex = g_nFillingUnit-1;       // note index is 0 based.
   BYTE nUnitSwitchedOffIndex = 255;       // will be index of loader being switched off if any.

   // set dump delay time continuously for a loader that's filling.
   g_sLoader[nFUIndex].m_nDischargeTmr = g_VacCalibrationData.m_nDischargeTime;

   int nIndex;


	// any unit currently filling???
	if( g_nFillingUnit != 0)
	{
      g_nLoaderFillInProgressTime++;

      g_nSystemIdleTimer = 0;       // clear system idle timer, i.e not idle.
		//yes..
		if(g_nFillingTime > 0)
			g_nFillingTime--;				// decrement filling time
      if( g_sLoader[nFUIndex].m_bHiLevelSensor == COVERED && g_nFillingTime > g_VacCalibrationData.m_nPostFillTime[nFUIndex] ) // hi level reached??
         g_nFillingTime = g_VacCalibrationData.m_nPostFillTime[nFUIndex]; // set time to post fill time.

		//Need to stop loading??
		//    fill time up??   or  turned off                 or full             or Hi Lev Sensor covered            or Override request
		if( g_nFillingTime == 0 || g_VacCalibrationData.m_bOnFlag[nFUIndex] == OFF || ( g_sLoader[nFUIndex].m_bLevelFlag == FULL && (g_nLoaderFillInProgressTime > MIN_LOADER_ON_TIME)) || g_sLoader[nFUIndex].m_bInhibit
             || g_nOverRideRequest )
		{
			g_nFillingTime = 0;
			g_nFillingUnit = 0;
			g_sLoader[nFUIndex].m_nFillTimer = 0;
//			g_sLoader[nFUIndex].m_nOverLapTimer = g_VacCalibrationData.m_nAVOverLapTime*10;  // / 10ths of second. (note this could be 0)
   // note use fixed O.L. time in case A.V. overlap = 0.
			g_sLoader[nFUIndex].m_nOverLapTimer = LOADER_VALVE_OVERLAP_TIME * 10;
			nUnitSwitchedOffIndex = nFUIndex;   // save index of loader being switched off
			// is there any unit requesting a fill
			if( g_nToFillTable[0] != 0)
			{
				nNext = PopFromToFillTable();   // get next unit requesting a fill.
				g_nFillingUnit = nNext;
				nIndex = nNext-1;
				g_nFillingTime = g_VacCalibrationData.m_nSuctionTime[nIndex] + g_VacCalibrationData.m_nPostFillTime[nIndex];  //
				// do we need to need AV open for a time?
				// AV open if  nAVOverLapTimer or  nAVOpenTimer
				if(g_VacCalibrationData.m_nAVOverLapTime > 0)
					g_nAVOverLapTimer = 10*g_VacCalibrationData.m_nAVOverLapTime;	 	// decremented on 10Hz.
		   	g_nAVOpenTimer = 0;

			}
			else // i.e. no other request
			{
				// enter idle period.
				if( g_nSystemIdleTimer < SYSTEM_IDLE_TIMER_MAX)
					g_nSystemIdleTimer++;
				if( g_nAVOpenTimer < AV_OPEN_TIMER_MAX )
					g_nAVOpenTimer++;			// make sure AV is open.

			}

		}
		else
		{
			// continue loading...
			g_sLoader[nFUIndex].m_nFillTimer = g_nFillingTime;
			g_sLoader[nFUIndex].m_nOverLapTimer = 0;		//

		}

	}
	else  // no unit filling at moment
	{
	  g_nLoaderFillInProgressTime = 0;          // no fill in progress, so clear.
		// is there any unit requesting a fill
		if( g_nToFillTable[0] != 0)
		{
		    g_nSystemIdleTimer = 0;

		   // Yes there is a loader waiting to be filled.  Must wait till pump ready.
		  if(g_bPumpReady)
		  {
   			nNext = PopFromToFillTable();
   			nIndex = nNext-1;                // loader index is 0 based.
   			g_nFillingUnit = nNext;
   			g_nFillingTime = g_VacCalibrationData.m_nSuctionTime[nIndex] + g_VacCalibrationData.m_nPostFillTime[nIndex];
   			// do we need to need AV open for a time?
   			// AV open if  nAVOverLapTimer or  nAVOpenTimer
   			if(g_VacCalibrationData.m_nAVOverLapTime > 0)
   				g_nAVOverLapTimer = 10*g_VacCalibrationData.m_nAVOverLapTime;	 	// decremented on 10Hz.
   		   g_nAVOpenTimer = 0;

   			g_sLoader[nIndex].m_nFillTimer = g_nFillingTime;
   			g_sLoader[nIndex].m_nOverLapTimer = 0;		//
			}
		}
		else // no request
		{
			if( g_nSystemIdleTimer < SYSTEM_IDLE_TIMER_MAX)
				g_nSystemIdleTimer++;
//			if( g_nAVOpenTimer < AV_OPEN_TIMER_MAX )
//				g_nAVOpenTimer++;			// make sure AV is open.
         g_bAtmValveFlag = OPEN;	   // make sure AV is open.

		}

	}
// turn on/off valves
// Maybe call this on 10Hz.
   for(i=0; i < MAX_LOADERS; i++)
	{
//        if( (OnFlag[i] && FillTimer[i]  && (ImmFillOffFlag[i] == 0) && (FillTimer[i] < (MBTable[Sti] + MBTable[Poi])) ) || ToStopFillFlag[i] )
		  if( g_VacCalibrationData.m_bOnFlag[i] && ( g_sLoader[i].m_nOverLapTimer || g_sLoader[i].m_nFillTimer) )
        {
                g_sLoader[i].m_bFillValve = ON;       // TURN on fill relay
//                g_sLoader[i].m_nStatusByte |= 0x10;   //status.4 = 1;  set bit in mb status byte
        }
        else if( i==nUnitSwitchedOffIndex && g_sLoader[i].m_nOverLapTimer ) // has this unit just been turned off??
        {   // if so leave on while overlap timer > 0.
                g_sLoader[i].m_bFillValve = ON;       // TURN on fill relay
        }
        else
        {
                g_sLoader[i].m_bFillValve = OFF;       // TURN off fill relay
//                g_sLoader[i].m_nStatusByte &= 0xEF; //.4 = 0;  //
        }
   }
// Atm Valve on or off??
   if( g_nAVOverLapTimer || g_nAVOpenTimer)
      g_bAtmValveFlag = OPEN;
   else
      g_bAtmValveFlag = CLOSED;
}
//
/*******************************************************************
*   Function Name:      PumpControl()                              *
*   Return Value:       void                                        *
*   Parameters:         none                                        *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:  Call on 1Hz                                                   *                                                    *
********************************************************************/
void PumpControl(void)
{
// if any unit filling OR any unit on & empty OR PumpHoldOnTime>0
//    Turn on pump.
// else
//    ensure AtmValveOpen
//	 Pump running??
   if( g_nPumpRunTimer > PUMP_STARTUP_TIME )
   {
      g_bPumpReady = TRUE;

//      if( g_bFilterSequenceInProgress ) // this is called on 10Hz now.
//      {
//         ; //todo - take care of filter sequence.
//      }
         //idling??
      if( g_nSystemIdleTimer > g_VacCalibrationData.m_nDischargeTime+2 )   //make sure to allow time for single filling unit to refill.
      {
         //ToDo
         // Time for Dust filter clean??
         if( g_VacCalibrationData.m_nFilterType == 0)     // note 0 is No Dust Filter present
         {
            g_bFilterFlag = OFF; // TURN  off filter.
            g_bFilterSequenceInProgress = FALSE;
            g_bFilterSequenceFinished = TRUE;      //
         }
         else if( g_nSystemIdleTimer > IDLE_TIME_TO_FILTER_CLEAN && !g_bFilterSequenceInProgress && !g_bFilterSequenceFinished )
         {
            // start filter clean sequence.
            g_bFilterFlag = ON; // TURN  on filter.
            g_nFilterOnTimer = FILTER_ON_TIME;
            g_bFilterSequenceInProgress = TRUE;
            g_bFilterSequenceFinished = FALSE;      //
            g_nFilterRepetitionsCounter = 0;
         }

         // should it be switched off??  idle for calibration time???, not in Filter Sequence.
         if( g_nSystemIdleTimer > g_VacCalibrationData.m_nPumpIdleTime && !g_bFilterSequenceInProgress )
         {
            if(g_nAVIsOpenTimer > AV_OPEN_TO_PUMP_ON_TIME*10 )   // aTM VALVE OPEN long enough??
               g_bPumpFlag = OFF;                            // set flag to turn on pump.
            else
               g_bPumpFlag = ON;  // leave on.

         }
      }
      else  // not idling
      {
         g_bPumpFlag = ON;    // leave on
         g_bFilterSequenceInProgress = FALSE;
         g_bFilterSequenceFinished = FALSE;

      }

   }
   else // pump not running for startup min
   {
      g_bPumpReady = FALSE;

      if( g_nPumpRunTimer == 0)  // pump off, should we switch it on??
      {
//            if(fdnet>0)
//               printf("\n pump off");

         // does it need to run???  Any loaders in the "To Fill" table??
         if( g_nToFillTable[0] !=0 || g_nFillingUnit || g_nFillingTime )
         {
//            if(fdnet>0)
//               printf("\n TFT != 0");
            // yes, turn on pump. note AV is open timer in 10ths of second.
            if(g_nAVIsOpenTimer > (AV_OPEN_TO_PUMP_ON_TIME*10) )   // aTM VALVE OPEN long enough??
               g_bPumpFlag = ON;                            // set flag to turn on pump.
//            if(fdnet>0)
//               printf("\n Sw pump on");
         }
         else
         {
            // leave pump off
            g_bPumpFlag = OFF;
//            if(fdnet>0)
//               printf("\n Switching pump offff");
         }
      }
   }
// new 11.Sept.2009
// Safety check - if pump on, and Atm. valve closed, if no loader ON, then open A.V.
   if( g_bPumpStatus == ON && g_bAtmValveStatus == CLOSED)
   {
      if( CheckForLoaderOutputOn() == FALSE )
         g_bAtmValveFlag = OPEN;
   }

   SetUpVAC8OutputWords(TRUE);
}

/********************************************************************
*   Function Name:      SetUpVAC8OutputWords()                      *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Setup o/p words for sending to VAC8         *
*   Note: Assumes Loader o/p's 1-8 go to A1 to A8 on Vac8            *
*                 Alarm  o/p's 1-8 go to B1 to B8 on VAC8           *
*                 Pump o/p goes to H1, AV -> H3, Filter -> H4       *
********************************************************************/
void SetUpVAC8OutputWords( bool bDoLoaders )
{
 int i,j;

   WORD nWordTemp = 0x0000;
   WORD nWordTemp2 = 0x0000;
   WORD nMask = 0x0001;
   WORD nMaskA = 0x0100;   // mask for Alarm o/p's.  uses M.s. byte
// relating to 2nd card.
   WORD nCard2WordTemp = 0x0000;
//   WORD nCard2WordTemp2 = 0x0000;
   WORD nCard2Mask = 0x0001;
   WORD nCard2MaskA = 0x0100;   // mask for Alarm o/p's.  uses M.s. byte
// relating to 3rd card uf present
   WORD nCard3WordTemp = 0x0000;
//   WORD nCard3WordTemp2 = 0x0000;
   WORD nCard3Mask = 0x0001;
   WORD nCard3MaskA = 0x0100;   // mask for Alarm o/p's.  uses M.s. byte


   if( bDoLoaders )
   {
      for(i=0; i<g_VacCalibrationData.m_nLoaders  && i<8; i++)    //take care of loaders on first VAC8IO card
      {
         // new 7.Sept.2009
         if( g_bVacLoaderSystemInManual )
         {  // Manual or test mode.....use the manual command.
            if(g_sLoader[i].m_bManualCommand == ON)
               nWordTemp |= nMask;
         }
         else  // normal operation.
         {
            if(g_sLoader[i].m_bFillValve == ON)
               nWordTemp |= nMask;
         }
         if(g_sLoader[i].m_bAlarm )
            nWordTemp |= nMaskA;

         nMask <<= 1;
         nMaskA <<=1;
      }
      g_nWriteVAC8WordOps1 = nWordTemp;

// 2nd card - loaders 9-16
      if(g_VacCalibrationData.m_nLoaders > 8)
      {
         for(i=8; i<g_VacCalibrationData.m_nLoaders  && i<16; i++)    //take care of loaders on 2nd VAC8IO card
         {
            // new 7.Sept.2009
            if( g_bVacLoaderSystemInManual )
            {  // Manual or test mode.....use the manual command.
               if(g_sLoader[i].m_bManualCommand == ON)
                  nCard2WordTemp |= nCard2Mask;
            }
            else  // normal operation.
            {
               if(g_sLoader[i].m_bFillValve == ON)
                  nCard2WordTemp |= nCard2Mask;
            }
            if(g_sLoader[i].m_bAlarm )
               nCard2WordTemp |= nCard2MaskA;

            nCard2Mask <<= 1;
            nCard2MaskA <<=1;
         }

         g_nWriteVAC8Card2WordOps1 = nCard2WordTemp;

      }
// end loaders 9-16

// 3rd card - loaders 17-24
      if(g_VacCalibrationData.m_nLoaders > 16)
      {
         for(i=16; i<g_VacCalibrationData.m_nLoaders  && i<24; i++)    //take care of loaders on 3rd VAC8IO card
         {
            // new 7.Sept.2009
            if( g_bVacLoaderSystemInManual )
            {  // Manual or test mode.....use the manual command.
               if(g_sLoader[i].m_bManualCommand == ON)
                  nCard3WordTemp |= nCard3Mask;
            }
            else  // normal operation.
            {
               if(g_sLoader[i].m_bFillValve == ON)
                  nCard3WordTemp |= nCard3Mask;
            }
            if(g_sLoader[i].m_bAlarm )
               nCard3WordTemp |= nCard3MaskA;

            nCard3Mask <<= 1;
            nCard3MaskA <<=1;
         }
         g_nWriteVAC8Card3WordOps1 = nCard3WordTemp;

      }
// end loaders 17-24


//      g_nWriteVAC8Card2WordOps2

//      j=0;
/*      // testonly  - checking the transmission down to VAC8.
      if( g_nSecondCounter & 0x0001)
         nWordTemp = 0x0001;
      else
         nWordTemp = 0x0000;
 */
      if(g_nVaccumLoadingVAC8IOCards >0)
         g_arrnWriteVAC8IOMBTable[0][MB_VAC8IO_COMMAND1] = nWordTemp;   //1st card
      if(g_nVaccumLoadingVAC8IOCards >1  && MAX_VAC8IO_CARDS > 1)
         g_arrnWriteVAC8IOMBTable[1][MB_VAC8IO_COMMAND1] = nCard2WordTemp;   //2nd card.
      if(g_nVaccumLoadingVAC8IOCards >2 && MAX_VAC8IO_CARDS > 2 )
         g_arrnWriteVAC8IOMBTable[2][MB_VAC8IO_COMMAND1] = nCard3WordTemp;   //3rd card.

   }

// new 7.Sept.2009
   if( g_bVacLoaderSystemInManual || g_bVacLoaderSystemEnterManual || g_bVacLoaderSystemExitManual )
   {  // in test or manual mode.
      // note, Atm. valve must be open to turn pump on.
      if( g_bPumpManualCommand == ON  && g_nAVIsOpenTimer > AV_OPEN_TO_PUMP_ON_TIME*10  ) //only turn pump on if AV open.
         nWordTemp2 |= 0x0001;      // pump uses l.s. bit
         //
      if( g_bAtmValveManualCommand == OPEN  )
         nWordTemp2 |= 0x0004;      // Atm Valve uses b2
      if( g_bFilterManualCommand == ON )
         nWordTemp2 |= 0x0008;      // Atm Valve uses b3

   }
   else  // normal (not manual or test) operation.
   {
      // pump AV & filter.
      if( g_bPumpFlag == ON )
         nWordTemp2 |= 0x0001;      // pump uses l.s. bit
      if( g_bAtmValveFlag == OPEN )
         nWordTemp2 |= 0x0004;      // Atm Valve uses b2
      if( g_bFilterFlag == ON )
         nWordTemp2 |= 0x0008;      // Atm Valve uses b3
   //   if( g_bClearVac8ioReset )   // ToDo.
      if( 1 )
        nWordTemp2 |= 0x0010;      // b4 clears VAC8IO reset flag

      if( g_nVacLoaderAlarms1 )      // any alarms on VAC8IO #1??
         nWordTemp2 |= 0x0020;    // Alarm LED & relay on VAC8 - uses b5.
   }
   g_nWriteVAC8WordOps2 = nWordTemp2;

   // Assuming pump station on Card g_nVAC8IOPumpCardNo.
   j=g_nVAC8IOPumpCardNo-1;               //
   g_arrnWriteVAC8IOMBTable[j][MB_VAC8IO_COMMAND2] = nWordTemp2;   //todo -
}

/********************************************************************
*   Function Name:      VAC8OneHzProgram                             *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        called on  1Hz for timing stuff             *
********************************************************************/
void VAC8OneHzPrograms( void )
{
int i;
BYTE nAlarms;
      if( g_bPumpStatus == ON )
      {
         if( g_nPumpRunTimer < PUMP_RUN_TIME_MAX )
            g_nPumpRunTimer++;                  // increment pump running timer while pump running - but limit at MAX
      }
      else
         g_nPumpRunTimer = 0;

      // call program to monitor pump once per second.
      PumpControl();

//   if( g_bAutoCycleFlag )
   if( g_bVacSimulatorRunning )
   {
      VacSimulatorOneHz();
   }
   else
   {


   // loader related
      for(i=0; i < MAX_LOADERS; i++)
   	{
   	  if( g_sLoader[i].m_nDischargeTmr )
   	     g_sLoader[i].m_nDischargeTmr--;     // decrement dump delay to zero.

      }

      // Check  the "To Fill" table, remove any units that are off or full
      CheckToFillTable( );
      nAlarms = 0;
      // Add empty loaders to the "to fill" table.
      for(i=0; i< g_VacCalibrationData.m_nLoaders; i++)
      {
         // add if empty & ON &  = 0.
         if( g_sLoader[i].m_bLevelFlag == EMPTY && g_VacCalibrationData.m_bOnFlag[i] )
         {
            // dont add again if filling. or after filling, in dump delay time..
            if(g_sLoader[i].m_nFillTimer == 0 && g_sLoader[i].m_nOverLapTimer == 0 && g_sLoader[i].m_nDischargeTmr == 0
                && g_sLoader[i].m_bHiLevelSensor != COVERED )
            {
   //            AddToFillTable( g_sLoader[i].m_nLoaderNo );
               AddToFillTableWithPriority( g_sLoader[i].m_nLoaderNo );
               // testonly
//               if(fdnet>0)
//                  printf(" ATFT %d", g_sLoader[i].m_nLoaderNo);
            }
         }
		 //03022010 - Only set or clear alarms while discarge time elapsed.
         if( g_sLoader[i].m_nDischargeTmr == 0 )
         {
				//Full or OFF clear fill counter & alarm
			 if( g_sLoader[i].m_bLevelFlag == FULL || g_VacCalibrationData.m_bOnFlag[i] == OFF)
			 {
				g_sLoader[i].m_nFillCounter = 0;
				g_sLoader[i].m_bAlarm = FALSE;
			 }
				//Hi Level sensor covered....clear alarm...treat as full.
			 if( g_sLoader[i].m_bHiLevelSensor == COVERED )
			 {
				g_sLoader[i].m_nFillCounter = 0;
				g_sLoader[i].m_bAlarm = FALSE;
			 }

			 // empty & ON & fill ctr > limit
			 if( g_sLoader[i].m_bLevelFlag == EMPTY && g_sLoader[i].m_nFillCounter > FILLS_TO_ALARM && g_VacCalibrationData.m_bOnFlag[i] )
				   g_sLoader[i].m_bAlarm = TRUE;
         }	//03022010 - end


         if( g_sLoader[i].m_bAlarm )
         nAlarms++;

      }

      g_nVacLoaderAlarmsTotal = nAlarms;

// now count alarms for each card.
      nAlarms = 0;
      for(i=0; i< g_VacCalibrationData.m_nLoaders && i<8; i++)
      {
         if( g_sLoader[i].m_bAlarm )
         nAlarms++;
      }
      g_nVacLoaderAlarms1 = nAlarms;

      nAlarms = 0;
      if( g_VacCalibrationData.m_nLoaders >8 )
      {
         for(i=8; i< g_VacCalibrationData.m_nLoaders && i<16; i++)
         {
            if( g_sLoader[i].m_bAlarm )
            nAlarms++;
         }
      }
      g_nVacLoaderAlarms2 = nAlarms;

      nAlarms = 0;
      if( g_VacCalibrationData.m_nLoaders >16 )
      {
         for(i=16; i<g_VacCalibrationData.m_nLoaders && i<24; i++)
         {
            if( g_sLoader[i].m_bAlarm )
            nAlarms++;
         }
      }
      g_nVacLoaderAlarms3 = nAlarms;

      VACLoaderLogic();

   }
}
/********************************************************************
*   Function Name:      VacTenHzPrograms                             *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        called on 10Hz for timing stuff             *
********************************************************************/
void VacTenHzPrograms( void )
{
int i;
//   if(fdnet>0)
//   {
//      printf("\n nLoaders= %d MAX= %d",  g_VacCalibrationData.m_nLoaders, MAX_LOADERS );
//      printf("\nAVSts= %d AVOpenTmr= %d AVOlpTmr= %d AVFlag= %d #6FTmr = %d #6OlTmr= %d #6FV= %d #6Sts= %d Ctr= %d",g_bAtmValveStatus,  g_nAVOpenTimer, g_nAVOverLapTimer, g_bAtmValveFlag, g_sLoader[5].m_nFillTimer, g_sLoader[5].m_nOverLapTimer, g_sLoader[5].m_bFillValve, g_sLoader[5].m_nStatusByte, g_nForeground50HzCounter );
//        printf("\nA-M= %x LdrAM= %d Pump= %d AV= %d Fltr= %d", g_arrnMBTable[VACUUM_LOADER_MODE_MANUAL], g_bVacLoaderSystemInManual, g_bPumpStatus, g_bAtmValveStatus, g_bFilterStatus );
 //  }

// new 7.Sept.2009
// entry to MANUAL mode.
   if( g_bVacLoaderSystemEnterManual )    // entering manual mode (test mode)
      VacLoaderSystemEnterManualMode();
// Exit from MANUAL mode.
   if( g_bVacLoaderSystemExitManual )    // exiting manual mode (test mode)
      VacLoaderSystemExitManualMode();

// endnew 7.Sept.2009

   // Atm Valve open time
   if(g_bAtmValveStatus == OPEN )
   {
      if(g_nAVIsOpenTimer < 10000 )
         g_nAVIsOpenTimer++;           // increment open timer (10ths of sec) when Atm Valve is open.
   }
   else
      g_nAVIsOpenTimer = 0;

// Atm Valve overlap timer..
   if( g_nAVOverLapTimer )
      g_nAVOverLapTimer--;                // decrement Atm Valve overlap timer.
   // then set valve state.
   if( g_nAVOverLapTimer || g_nAVOpenTimer)
      g_bAtmValveFlag = OPEN;
   else
      g_bAtmValveFlag = CLOSED;

   for(i=0; i < MAX_LOADERS; i++)
	{
      if( g_sLoader[i].m_nOverLapTimer )
      {
         g_sLoader[i].m_nOverLapTimer--;
         //reached 0??
         if(g_sLoader[i].m_nOverLapTimer == 0 )
         {
            g_sLoader[i].m_bFillValve = OFF;    // turn off fill valve.
//            g_sLoader[i].m_nStatusByte &= 0xEF; // clear filling bit in status.
            g_sLoader[i].m_nFillCounter++;   // increment fill counter      //
            g_sLoader[i].m_nDischargeTmr = g_VacCalibrationData.m_nDischargeTime;      //
            //todo - update status.
            //if( g_sLoader[i].m_bLevelFlag == FULL )

         }

      }
    // set up status byte (0= normal, 1=alarm, 2=override, 3= UNUSED
      if( i >= g_VacCalibrationData.m_nLoaders )
         g_sLoader[i].m_nStatusByte = LOADER_STATUS_UNUSED;
      else if( g_sLoader[i].m_bAlarm )
         g_sLoader[i].m_nStatusByte = LOADER_STATUS_IN_ALARM;
      else if(g_sLoader[i].m_nLoaderNo == g_nOverRideRequest )     // is this loader seeking override?
         g_sLoader[i].m_nStatusByte = LOADER_STATUS_OVERRIDE_REQUEST;
      else
         g_sLoader[i].m_nStatusByte = 0;
   }

// filter clean sequence??
   // only happens if sequence is in progress.
   if( g_bFilterSequenceInProgress )
      HandleFilterSequence();



}

/********************************************************************
*   Function Name:      UpdateLoaderData                            *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Updates loader data after read from VAC8    *
********************************************************************/
void UpdateLoaderDataFromVAC8IO( void )
{
 int i,j;
 // ToDo this will be from the Comms array
   WORD nVAC8Inputs;        // ToDo this will be from the Comms array
   WORD nVAC8OutputsStatus1;
   WORD nVAC8OutputsStatus2;

   WORD nMask = 0x0001;
   WORD nMaskHi = 0x0100;        // Hi Level sensors in M.S. byte.
   WORD nOpsMask = 0x0001;       // used to detect loader outputs state.
   j=0;
//   if(g_bAutoCycleFlag)
   if( g_bVacSimulatorRunning )
      g_bVAC8IOReadDataAvailable[j] = FALSE;

   // card #1
   j=0;
   if(g_nVaccumLoadingVAC8IOCards >0 &&  g_bVAC8IOReadDataAvailable[j] )
   {
      g_bVAC8IOReadDataAvailable[j] = FALSE;

      nVAC8Inputs = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_IPS_STATUS];
      nVAC8OutputsStatus1 = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_OPS_STATUS];
      nVAC8OutputsStatus2 = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_OPS2_STATUS];

//      if(fdnet>0)
//         printf("\n Inputs= %4x Ops1= %4x Ops2= %4x", nVAC8Inputs, nVAC8OutputsStatus1, nVAC8OutputsStatus2);

      for(i=0; i<g_VacCalibrationData.m_nLoaders && i<8; i++)     // note max 8 loaders/VAC8 current setup
      {
         if( nVAC8Inputs & nMask )
         {
            g_sLoader[i].m_bLevelFlag = FULL;
         }
         else
         {
            g_sLoader[i].m_bLevelFlag = EMPTY;
         }

         if( nVAC8Inputs & nMaskHi )
         {
            g_sLoader[i].m_bHiLevelSensor = COVERED;
         }
         else
         {
            g_sLoader[i].m_bHiLevelSensor = UNCOVERED;
         }
// output state
         if( nVAC8OutputsStatus1 & nOpsMask )
         {
            g_sLoader[i].m_bOutputState = ON;
         }
         else
         {
            g_sLoader[i].m_bOutputState = OFF;
         }

         nMask <<= 1;
         nMaskHi <<=1;
         nOpsMask <<=1;
      }

      if( g_nVAC8IOPumpCardNo == 1)    // if pump connected to VAC8io #1
      {
         if( nVAC8OutputsStatus2 & PUMP_STATUS_BIT )
            g_bPumpStatus = ON;
         else
            g_bPumpStatus = OFF;

         if( nVAC8OutputsStatus2 & AV_STATUS_BIT )
            g_bAtmValveStatus = OPEN;
         else
            g_bAtmValveStatus = CLOSED;

         if( nVAC8OutputsStatus2 & FILTER_STATUS_BIT )
            g_bFilterStatus = ON;
         else
            g_bFilterStatus = OFF;
      }
   }

   // Card #2
   j=1;
   if(g_nVaccumLoadingVAC8IOCards >1 &&  g_bVAC8IOReadDataAvailable[j] )
   {
      g_bVAC8IOReadDataAvailable[j] = FALSE;

      nVAC8Inputs = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_IPS_STATUS];
      nVAC8OutputsStatus1 = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_OPS_STATUS];
      nVAC8OutputsStatus2 = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_OPS2_STATUS];

      nMask = 0x0001;
      nMaskHi = 0x0100;        // Hi Level sensors in M.S. byte.
      nOpsMask = 0x0001;

//      if(fdnet>0)
//         printf("\n Inputs= %4x Ops1= %4x Ops2= %4x", nVAC8Inputs, nVAC8OutputsStatus1, nVAC8OutputsStatus2);

      for(i=8; i<g_VacCalibrationData.m_nLoaders && i<16; i++)     // note max 8 loaders/VAC8 current setup
      {
         if( nVAC8Inputs & nMask )
         {
            g_sLoader[i].m_bLevelFlag = FULL;
         }
         else
         {
            g_sLoader[i].m_bLevelFlag = EMPTY;
         }

         if( nVAC8Inputs & nMaskHi )
         {
            g_sLoader[i].m_bHiLevelSensor = COVERED;
         }
         else
         {
            g_sLoader[i].m_bHiLevelSensor = UNCOVERED;
         }
// output state
         if( nVAC8OutputsStatus1 & nOpsMask )
         {
            g_sLoader[i].m_bOutputState = ON;
         }
         else
         {
            g_sLoader[i].m_bOutputState = OFF;
         }


         nMask <<= 1;
         nMaskHi <<=1;
         nOpsMask <<=1;
      }

      if( g_nVAC8IOPumpCardNo == 2)    // if pump connected to VAC8io #2
      {
         if( nVAC8OutputsStatus2 & PUMP_STATUS_BIT )
            g_bPumpStatus = ON;
         else
            g_bPumpStatus = OFF;

         if( nVAC8OutputsStatus2 & AV_STATUS_BIT )
            g_bAtmValveStatus = OPEN;
         else
            g_bAtmValveStatus = CLOSED;

         if( nVAC8OutputsStatus2 & FILTER_STATUS_BIT )
            g_bFilterStatus = ON;
         else
            g_bFilterStatus = OFF;
      }
   }

   // Card #3
   j=2;
   if( g_nVaccumLoadingVAC8IOCards >2 && g_bVAC8IOReadDataAvailable[j] )
   {
      g_bVAC8IOReadDataAvailable[j] = FALSE;

      nVAC8Inputs = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_IPS_STATUS];
      nVAC8OutputsStatus1 = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_OPS_STATUS];
      nVAC8OutputsStatus2 = g_arrnReadVAC8IOMBTable[j][MB_VAC8IO_OPS2_STATUS];

      nMask = 0x0001;
      nMaskHi = 0x0100;        // Hi Level sensors in M.S. byte.
      nOpsMask = 0x0001;

//      if(fdnet>0)
//         printf("\n Inputs= %4x Ops1= %4x Ops2= %4x", nVAC8Inputs, nVAC8OutputsStatus1, nVAC8OutputsStatus2);

      for(i=16; i<g_VacCalibrationData.m_nLoaders && i<24; i++)     // note max 8 loaders/VAC8 current setup
      {
         if( nVAC8Inputs & nMask )
         {
            g_sLoader[i].m_bLevelFlag = FULL;
         }
         else
         {
            g_sLoader[i].m_bLevelFlag = EMPTY;
         }

         if( nVAC8Inputs & nMaskHi )
         {
            g_sLoader[i].m_bHiLevelSensor = COVERED;
         }
         else
         {
            g_sLoader[i].m_bHiLevelSensor = UNCOVERED;
         }
// output state
         if( nVAC8OutputsStatus1 & nOpsMask )
         {
            g_sLoader[i].m_bOutputState = ON;
         }
         else
         {
            g_sLoader[i].m_bOutputState = OFF;
         }

         nOpsMask <<=1;

         nMask <<= 1;
         nMaskHi <<=1;
      }

      if( g_nVAC8IOPumpCardNo == 3)    // if pump connected to VAC8io #3
      {
         if( nVAC8OutputsStatus2 & PUMP_STATUS_BIT )
            g_bPumpStatus = ON;
         else
            g_bPumpStatus = OFF;

         if( nVAC8OutputsStatus2 & AV_STATUS_BIT )
            g_bAtmValveStatus = OPEN;
         else
            g_bAtmValveStatus = CLOSED;

         if( nVAC8OutputsStatus2 & FILTER_STATUS_BIT )
            g_bFilterStatus = ON;
         else
            g_bFilterStatus = OFF;
      }
   }

}

/********************************************************************
*   Function Name:      HandleFilterSequence                        *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Handles Dust Filter Clean sequence - called *
*                       on 10Hz                                     *
********************************************************************/
void HandleFilterSequence( void )
{
   // only happens if sequence is in progress.
   if( g_bFilterSequenceInProgress )
   {
      if(g_nFilterOnTimer)
      {
         g_nFilterOnTimer--;
         if( g_nFilterOnTimer == 0 )
         {
            g_bFilterFlag = OFF; // TURN  off filter.

            g_nFilterRepetitionsCounter++;
            // have we done enough repetitions or blasts???
            if( g_nFilterRepetitionsCounter >= g_VacCalibrationData.m_nFilterRepetitions )
            {
               // TERMINATE THE FILTER sequence.
               g_bFilterSequenceInProgress = FALSE;
               g_bFilterSequenceFinished = TRUE;      // signal end of sequence..
               g_nFilterRepetitionsCounter = 0;

            }
            else
            {
               g_nFilterOffTimer = FILTER_OFF_TIME;
            }

         }
         else
            g_bFilterFlag = ON;     // leave dust filter ON.
      }
      else if( g_nFilterOffTimer )
      {
         g_nFilterOffTimer--;
         if(g_nFilterOffTimer == 0)
         {
            g_bFilterFlag = ON; // TURN  on filter.
            g_nFilterOnTimer = FILTER_ON_TIME;
         }
         else
            g_bFilterFlag = OFF;     // leave dust filter Off.

      }

      SetUpVAC8OutputWords( FALSE );
   }

   g_bFilterFlag = OFF; // TURN  off filter.


}
// call on 1 Hz.
void VacSimulatorOneHz( void )
{
int i,j,n;
int nFillUnitIndex;
int nEmptyIndex = 0;
bool bFound = FALSE;
bool bEmptyFound = FALSE;
bool bFillingFound = FALSE;
 if( 1 )      // i.e. always....could put something here to disable.
 {
   for(i=0; i<g_VacCalibrationData.m_nLoaders && i<8; i++)
   {
      if( g_sLoader[i].m_nFillTimer )
      {
         bFillingFound = TRUE;
         nFillUnitIndex = i;
      }

   }

   bEmptyFound = FALSE;
   if( !bFillingFound ) // no unit filling
   {

      j=0;
      while( j<8 && !bEmptyFound )
      {
            if(g_sLoader[j].m_nLevelSimulator == 0 && g_VacCalibrationData.m_bOnFlag[j] == ON )   // 0 => empty.
            {
               bEmptyFound = TRUE;
               nEmptyIndex = j;
            }
            j++;

      }

      if( bEmptyFound )
      {
         j = nEmptyIndex;
         g_sLoader[j].m_nFillTimer = g_VacCalibrationData.m_nSuctionTime[j];
//         if(fdnet>0)
//            printf("\n bEm...mmmpty %d", j);
      }
   }

   for(i=0; i<g_VacCalibrationData.m_nLoaders && i<8; i++)
   {
      if(g_sLoader[i].m_nFillTimer)
      {
         if( g_VacCalibrationData.m_bOnFlag[i] == OFF )  // if loader off  clear fill timer.
            g_sLoader[i].m_nFillTimer = 0;
         else
            g_sLoader[i].m_nFillTimer--;

         if( g_sLoader[i].m_nFillTimer == 0 )
         {
            // if loader is on then simulate reaching full
            if(g_VacCalibrationData.m_bOnFlag[i] == ON)
               g_sLoader[i].m_nLevelSimulator = g_VacLevelSimulatorTime[i];   // only "fill" it if it is ON.

            //get next  empty unit to fill. j
            bFound=FALSE;
            j=i;   // next unit.
            for(n=0; n<7 && !bFound; n++) // max 7 loaders (8 - current one)
            {
               j++;  // advance to next unit.
               if( j>=8 || j >= g_VacCalibrationData.m_nLoaders )    // wrap around
                  j=0;
               if(g_sLoader[j].m_nLevelSimulator == 0 && g_VacCalibrationData.m_bOnFlag[j] == ON )   // 0 => empty.
                  bFound = TRUE;
            }

            if( bFound )
               g_sLoader[j].m_nFillTimer = g_VacCalibrationData.m_nSuctionTime[j];

         }
      }

  }
      for(i=0; i<g_VacCalibrationData.m_nLoaders && i<8; i++)
      {
         if( g_sLoader[i].m_nLevelSimulator > 0 )
            g_sLoader[i].m_nLevelSimulator--;

         if( g_sLoader[i].m_nLevelSimulator > 0 )
            g_sLoader[i].m_bLevelFlag = FULL;
         else
            g_sLoader[i].m_bLevelFlag = EMPTY;

      }
  }
}

// new 7.Sept.2009
/********************************************************************
*   Function Name:      ClearAllLoaderAlarms                        *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        called on modbus command                    *
********************************************************************/
void ClearAllLoaderAlarms( void )
{
int i;
      for(i=0; i<MAX_LOADERS; i++)        // note clear for all defined loaders.
      {
            g_sLoader[i].m_nFillCounter = 0;    // fill counter used to generate alarm
            g_sLoader[i].m_bAlarm = FALSE;
      }
}

/********************************************************************
*   Function Name:      VacLoaderSystemEnterManualMode              *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        called  from VacTenHzPrograms() if  g_bVacLoaderSystemEnterManual true *
********************************************************************/
void VacLoaderSystemEnterManualMode( void )
{
int i;
// loaders all off
      for(i=0; i<MAX_LOADERS; i++)
      {
         g_sLoader[i].m_bManualCommand = OFF;    // turn off all loaders
      }

//  Open Atm. Valve.
      g_bAtmValveFlag = OPEN;
      g_bAtmValveManualCommand = OPEN;
         // show state in MB table.
      g_arrnMBTable[VACUUM_LOADER_ATMOSPHERE_VALVE_COMMAND] = g_bAtmValveManualCommand;

// leave pump as is.
      g_bPumpManualCommand = g_bPumpStatus;     // command pump to be as currently is - before entering manual.
      g_arrnMBTable[VACUUM_LOADER_PUMP_COMMAND] = g_bPumpManualCommand;
// can clear flag when A.V. is open. (safe to do), i.e. keep calling until A.V. is open.
      if(g_bAtmValveStatus == OPEN )
         g_bVacLoaderSystemEnterManual = FALSE;
}


/********************************************************************
*   Function Name:      VacLoaderSystemExitManualMode               *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        called from  VacTenHzPrograms() if  g_bVacLoaderSystemExitManual true *
********************************************************************/
void VacLoaderSystemExitManualMode( void )
{
int i;
// loaders all off
      for(i=0; i<MAX_LOADERS; i++)
      {
         g_sLoader[i].m_bFillValve = OFF;    // turn off all loaders
         g_sLoader[i].m_bManualCommand = OFF;
      }
// filter off
      g_bFilterFlag = OFF;
// Atm. Valve open.
      g_bAtmValveFlag = OPEN;
      g_bAtmValveManualCommand = OPEN;

// only clear flag when A.V. known open.
      if(g_bAtmValveStatus == OPEN )
         g_bVacLoaderSystemExitManual = FALSE;    // can clear flag when A.V. is open. (safe to do)

}

/********************************************************************
*   Function Name:      CheckForLoaderOutputOn                            *
*   Return Value:       void                                        *
*   Parameters:                                                     *
*   RAM Usage:                                                      *
*             Global:   0 bytes                                     *
*             Local:    0 bytes                                     *
*   ROM Usage:          ~x words                                    *
*   Description:        Checks that there is a loader on            *
*                       *
********************************************************************/
bool CheckForLoaderOutputOn( void )
{
int i;
bool bAnyLoaderOutputOn = FALSE;

      for(i=0; i<g_VacCalibrationData.m_nLoaders && !bAnyLoaderOutputOn; i++)
      {
         if(g_sLoader[i].m_bOutputState == ON)
            bAnyLoaderOutputOn = TRUE;
      }

      return( bAnyLoaderOutputOn );
}

