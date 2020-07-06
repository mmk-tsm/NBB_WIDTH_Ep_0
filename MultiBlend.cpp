//////////////////////////////////////////////////////
// P.Smith                      25/4/07
// added  MultiBlendCycle
//
// P.Smith                      25/4/07
// added PauseMultiblend & RestartMultiblend
//
// P.Smith                      30/4/07
// Correct issue with the queue not being de queued when the flap status dis appears, one
// of the braces was in the wrong place, it was inside the check for enable/disable
// Put in facility for queue timer, this only allows queueing every 7 seconds, not implemented yet.
// allow 
//
// P.Smith                      15/5/07
// Remove printfs
//
// P.Smith                      2/8/07
// only check for flap empty if single recipe mode
// put back in g_nQueueOneSecondTimer operation, this stops the flaps from being checked too often.
// remove printfs
//
// P.Smith                      27/8/07
// do not add extruder to the queue if g_bMultiBlendRecipeIsZero[g_unCurrentExtrNo] set to true
// No timer on entry of this function, if empty, reaction is immediately.
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          17/5/08
// remove set/clr of g_bMultiBlendisPaused, if true no multiblend activated.
// it is already being checked before addition to queue
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern 
//////////////////////////////////////////////////////


#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"

#include <basictypes.h>
#include <string.h>
#include "NBBgpio.h"
#include "ConversionFactors.h"
#include "MultiblendMB.h"
#include "Mtttymux.h"

extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_TempRecipe; 

//////////////////////////////////////////////////////
// MultiBlendCycle( void )           
// 
//
//////////////////////////////////////////////////////
void MultiBlendCycle( void )
{
    unsigned int i,j,nThisRecipe;
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)  // single recipe mode ?
    {
	    g_unCurrentFlapStatus1 = g_nLowLevelSensorInputState;
	    g_unInProgressStatus = g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_STATUS];

	    for(i=0; i<MULTIBLEND_EXTRUDER_NO; i++)
	    {
	        if (g_unCurrentFlapStatus1 & 0x0001)
			g_baCurrentFlapStatus[i] = TRUE;
		    else
			g_baCurrentFlapStatus[i] = FALSE;
			
		    if (g_unInProgressStatus & 0x0001)
			g_baInProgress[i] = TRUE;
		    else
			g_baInProgress[i] = FALSE;
		
        g_unCurrentFlapStatus1 >>= 1;
	    g_unInProgressStatus >>= 1;
	    }

		
        if(g_bClearHistory)
        {
            g_bClearHistory = FALSE;
            for(i=0; i<DOWNLOADHISTORYNO; i++)
            {
                g_unaDownLoadHistory[i] = 0;
            }
         }

//        if(g_nQueueOneSecondTimer == 0)
        if(1)
        {
            g_nQueueOneSecondTimer = QUEUEINGINTERVAL;
	        g_unCurrentFlapStatus1 = g_nLowLevelSensorInputState;
	        g_unInProgressStatus = g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_STATUS];
			/*Loop through all 15 bits and see if the corresponding extruders need to be added to the queue*/
			    for(i=0; i<MULTIBLEND_EXTRUDER_NO; i++){
				/*If the current flap bit is set and the current inProgress Bit is NOT set, 
				  check if in queue already, if not, add to queue
				  i = BIT number
				  i+1 = Extruder number for bit i
				*/
				    if ((g_unCurrentFlapStatus1 & 0x0001) && !(g_unInProgressStatus & 0x0001))
                    {
					    g_bRan=true;
					    g_nRanx=i;
					/*Reset the existsInQueue bool incase set*/
					    g_bExistsInQueue = FALSE;
					
					    for(j=0; j<MULTIBLEND_EXTRUDER_NO; j++)
                        {
						    if(g_unaCurrentQueue[j] == (i+1))
							g_bExistsInQueue = TRUE;
					    }
					
					/*If the current Extruder Number wasn't found in the queue, add it to the queue at last position*/
					    if(g_bExistsInQueue == FALSE)
                        {
						    g_unaCurrentQueue[g_nEndOfQueue] = (i+1);
						    g_nEndOfQueue++;	
					    }
				   }
				
				   g_unCurrentFlapStatus1 >>= 1;
				   g_unInProgressStatus >>= 1;
			    }
			/*Bit checking for adding to queue now complete*/
			
						
			/*Check if the blender is paused. We can't download if its paused*/
//			    if((g_nPauseFlag & NORMAL_PAUSE_BIT != 0) || g_bMultiBlendPaused )
				/*Blender is paused */
//	            {
//				    g_bMultiBlendisPaused = TRUE;//kwh
//				}
//			    else
//			    {
//				    g_bMultiBlendisPaused = FALSE; //kwh
//				}    
			/*Check the endOfQueue number. If its greater than zero, there is an extruder in the queue
			  Also, if the blender state is "Ready to accept a recipe"(435 bit 9), then we can proceed
			*/
			
			/*We must now get the extruder number at the head of the queue*/
			    if(g_unaCurrentQueue[0] > 0)
				    g_unCurrentExtrNo = g_unaCurrentQueue[0]-1;
					
					
					
			/*Get a count of the number of progress Bits currently set
			  We should only allow 2 to be in progress at once
			*/
			    g_nNoCurrentFlaps = 0;
						
			    for(i=0; i<MULTIBLEND_EXTRUDER_NO; i++){
				    if(g_baInProgress[i] == 1)
					g_nNoCurrentFlaps++;
			    }
                if((g_nEndOfQueue > 0) && !(g_bRecipeWaiting) && (g_bMultiBlendisPaused == 0) && (g_nNoCurrentFlaps < 1) && (g_baInProgress[g_unCurrentExtrNo] == 0) )
            {
//             iprintf("\n add to queue now-----------------------------------");
			/*Again, We must now get the extruder number at the head of the queue*/
			        if(g_unaCurrentQueue[0] > 0)
				    g_unCurrentExtrNo = g_unaCurrentQueue[0]-1;
//				iprintf("\n current extruder no %d",g_unCurrentExtrNo);
				
				/*First we check whether we should run the recipe - is the extruder enabled in the table?
				  and is the extruder currently requesting material */
				    if(g_CalibrationData.m_stMultiblendExtruders[g_unCurrentExtrNo].m_bEnabled && !g_bMultiBlendRecipeIsZero[g_unCurrentExtrNo] && g_baCurrentFlapStatus[g_unCurrentExtrNo] == 1)
                    {
     				/*We must now get the recipe number of the extruder from the extruder look up table*/
					
					    nThisRecipe = g_unCurrentExtrNo+1;
                       StartRecipe(nThisRecipe);

                        for(i=DOWNLOADHISTORYNO-1; i>0; i--)
                        {
						    g_unaDownLoadHistory[i] = g_unaDownLoadHistory[i-1];
					    }	
						    g_unaDownLoadHistory[0] = g_unCurrentExtrNo+1;
						
				    }					/*Even if the recipe wasn't loaded because the extruder is disabled, or 100% check failed,
				  we should de-queue it
				*/
				
				
				/*Once complete, we shift the queue down one place
				NOTE: We only loop as far as 15 to prevent the loop from going out of bounds on i+1 statement
				*/
				    for(i=0; i<MULTIBLEND_EXTRUDER_NO-1; i++)
                    {
					    g_unaCurrentQueue[i] = g_unaCurrentQueue[i+1];
				    }
				/*After exiting the loop above, we should reset the last place in the queue,
				  otherwise it would never get reset
				*/
				    g_unaCurrentQueue[MULTIBLEND_EXTRUDER_NO-1] = 0;
				
				/*Reduce the Queuesize by 1, if the queue size is not equal to zero*/
				   if(g_nEndOfQueue != 0)
					g_nEndOfQueue--;
             }

	    }	
	    else
	   {
            g_nQueueOneSecondTimer--;
       }
    }
}


//////////////////////////////////////////////////////
// ClearMultiblendDownloadHistory( void )           
// 
//
//////////////////////////////////////////////////////
void ClearMultiblendDownloadHistory( void )
{
     g_bClearHistory = TRUE;
}

//////////////////////////////////////////////////////
// PauseMultiblend( void )           
// 
//
//////////////////////////////////////////////////////
void PauseMultiblend( void )
{
    g_bMultiBlendisPaused = TRUE;
}

//////////////////////////////////////////////////////
// RestartMultiblend( void )           
// 
//
//////////////////////////////////////////////////////
void RestartMultiblend( void )
{
    g_bMultiBlendisPaused = FALSE;
}
 




