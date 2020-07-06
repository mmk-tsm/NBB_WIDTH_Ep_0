/*
    P.Smith                         17th April 2007
    First pass at save of config

    P.Smith                         9/05/07
    Remove printfs

    P.Smith                         9/05/07
	remove cval[90]
*/
//
// M.McKiernan                      1/12/09
// added predef.h

#include "predef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <basictypes.h>
//#include <unistd.h>
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>

#include "iniparser.h"
#include "iniexample.h"

#include <effs_fat/fat.h>
#include <math.h>
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "Sdcard.h"
#include <ucosmcfc.h>
#include "Batvars2.h"
#include "Copycalibrationdatatoinifile.h"

extern CalDataStruct    g_CalibrationData;



void Update_Batch_ini_file(char * ini_name)
{
	dictionary	*	ini ;
	g_bSDCardSPIActive = TRUE;
    SPI_Select_SD ();
//    InitMMC();
    InitialisePinsUsedBySDCard();
//   iprintf( "\r\n===== Starting ini file update=====\r\n" );
   // Select either SD/MMC or Compact Flash

    ini = iniparser_load(ini_name);

	if (ini==NULL) {
//		iprintf("\n cannot parse file [%s]", ini_name);
	}
    else
    {
//	    iprintf("\n can parse file");
    }
//    UpDateIniFileWithCalibrationData(ini);
//     sprintf(cval, "%1d", 7 );
//    dictionary_set(ini, "hopper:weight averaging", cval);
//   iniparser_dump(ini, stderr);

        F_FILE * pFile = f_open(ini_name, "w");
    if( pFile )
    {
//      iprintf("\n OUtput.ini opened\n");
      iniparser_dump_ini(ini, pFile );
      f_close( pFile );
//      iprintf("\n file closed");
    }

	iniparser_freedict(ini);

      ReInitialisePinsUsedBySDCard();
     g_bSDCardSPIActive = FALSE;

}

