//////////////////////////////////////////////////////
// 
// P.Smith                      2/5/07
// Blender Mapping definitions pass #1
// 
// P.Smith                      28/5/07
// added mapping for 650 4 and 6 components,these are the same at the minute.
// change the array to allow for 20 source outputs.
// 
// P.Smith                      22/6/07
// added mapping for TSM1000
// M.McKiernan                   06/09/2007
// added TSM3000_8C_3CleansMapping
// 
// P.Smith                      11/10/07
// added IsKWHBlender
// 
// P.Smith                      24/10/07
// document mapping
// 
// P.Smith                      5/11/07
// added 350 mapping
// 
// P.Smith                      10/1/08
// corrected tsm 350 mapping
//////////////////////////////////////////////////////


/*
    Mapping of valves.
    the mapping works as follows
const   int  TSM650Mapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

The first line is the first source output
In the above case the first source output is mapped to source o/p 1.
If more than one output is required to be on, then the second position in 
the line should be set to the output that should be switched on.
eg
        {1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        this would switch outputs 1 and 3 on.
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        if set to 0, then this source outputs is switched off and is not used.
        even if it is called.
        dummy outputs 17,18,19,20 are used for cleaning top,right,left, and mixer

for example,

        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Top Clean
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Right Corner Clean
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Left Corner Clean
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Mixer Clean
       
source output no 5 is activated when the top clean source (17) is activated.
source output no 9 is activated when the right clean source (18) is activated.
source output no 6 is activated when the left clean source (19) is activated.
source output no 10 is activated when the mixer clean source (20) is activated.

*/

#ifndef __MAPPING_H__
#define __MAPPING_H__

#include <basictypes.h>
#include "General.h"

void LoadOutputMapping( void );
BOOL IsKWHBlender( void );

const   int  TSM350Mapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        };


const   int  TSM650Mapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        };

        
const   int  TSM650Mapping4ComponentsWithCleaning[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        };

        
const   int  TSM650Mapping6ComponentsWithCleaning[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Top Clean
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Right Corner Clean
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Left Corner Clean
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Mixer Clean
        };

const   int  TSM3000KWHMapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //  Top Clean
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Right Corner Clean
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Left Corner Clean
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //  Mixer Clean
        };
const   int  TSM3000_8C_3CleansMapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  // 11 - Bypass - uses dedicated bypass o/p
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  // 12 - Offline - uses dedicated offline o/p SRC12
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  // 13 - Alarm o/p
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  // 14 - Mixer clean - uses CLEAN1
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  // 15 - Weigh hopper
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 17 - Top Clean uses SRC 10 
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 18 - Right Corner Clean uses SRC 9
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 19 - Left corner clean uses SRC 9
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //  20 - Mixer Clean - uses CLEAN1, SRC14
        };


const   int  TSM1000Mapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        };

       
 const   int  TSMDefaultMapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO] = {
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // cleaning
        };
        
        
       
       
  
 #endif   // __MAPPING_H__
 
