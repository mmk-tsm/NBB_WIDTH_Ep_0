///////////////////////////////////////////////////////////
// 
// P.Smith                       13/3/07
// some changed to printing
///////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <basictypes.h>
#include "Batvars2.h"
#include "Batvars.h"
#include "General.h"
#include "init5270.h"
#include <pins.h>
#include "NBBGpio.h"


#include <basictypes.h>


#include "predef.h" 
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h> 
#include <htmlfiles.h>
#include <C:\Nburn\MOD5270\system\sim5270.h>
#include <cfinter.h>
#include "init5270.h"
#include "PITHandler.h"
#include "I2CFuncs.h"
#include "NBBGpio.h"
#include "QSPIconstants.h"
#include "LTC2415.h"
#include "NBBVars.h"
#include "MBSHand.h"
#include "IOMeasure.h"  
#include "UltrasonicRead.h"


void UltrasonicCalculation ( void )
{
    printf("\n batch cycle time is %f",g_fBatchTimeInSeconds);
//    printf("\n level sensor tripped at %f mm",g_fUSDistance);
    g_nCycleSecondNo = 0;
    g_nUSBufferIndex = 0;
}


