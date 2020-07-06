/*
 P.Smith                                       10/5/07
 Remove parse_Batch_ini_file from here.


   P.Smith                          30/1/08
   correct compiler warnings

   P.Smith                          14/10/08
   remove PUpdate_Batch_ini_file
   set ffWtConstant to 0

*/
// M.McKiernan                      1/12/09
// added predef.h
//
#include "predef.h"				//Eclipse
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
//#include <strlib.h>
#include "strlib.h"
const char * IniFileData = "\n\
#\n\
# This is an example of ini file\n\
#\n\
\n\
[Pizza]\n\
\n\
Ham       = yes ;\n\
Mushrooms = TRUE ;\n\
Capres    = 0 ;\n\
Cheese    = NO ;\n\
\n\
\n\
[Wine]\n\
\n\
Grape     = Cabernet Sauvignon ;\n\
Year      = 1989 ;\n\
Country   = Spain ;\n\
Alcohol   = 12.5  ;\n\
\n\
#\n\
# end of file\n\
#\n";

const char * BatchBlIniFileData = "\n\
#\n\
# This is a prototype of a Batch Blender ini file\n\
#\n\
\n\
[hopper]\n\
\n\
\n\
weight constant = 3.674 ;\n\
tare value      = 77751 ;\n\
double dump     = YES ;\n\
batch weight    = 2.72 ;\n\
\n\
#\n\
# end of file\n\
#\n";

/*
int main(int argc, char * argv[])
{
	int		status ;

	if (argc<2) {
		create_example_ini_file();
		status = parse_ini_file("example.ini");
	} else {
		status = parse_ini_file(argv[1]);
	}
	return status ;
}
*/

/*
void create_example_ini_file(void)
{
	FILE	*	ini ;

	ini = fopen("example.ini", "w");
	fprintf(ini, "\n\
#\n\
# This is an example of ini file\n\
#\n\
\n\
[Pizza]\n\
\n\
Ham       = yes ;\n\
Mushrooms = TRUE ;\n\
Capres    = 0 ;\n\
Cheese    = NO ;\n\
\n\
\n\
[Wine]\n\
\n\
Grape     = Cabernet Sauvignon ;\n\
Year      = 1989 ;\n\
Country   = Spain ;\n\
Alcohol   = 12.5  ;\n\
\n\
#\n\
# end of file\n\
#\n");

	fclose(ini);
}
*/
