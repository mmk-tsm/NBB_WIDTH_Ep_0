//////////////////////////////////////////////////////
// 
// P.Smith                      8/6/07
// removed printfs 
// 
// P.Smith                     11/6/07
// added CheckIfDHCP
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern 
//////////////////////////////////////////////////////


#include "predef.h" 
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h> 
#include <htmlfiles.h>

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
#include "BatchMBIndices.h"
#include <System.h>
#include "Batvars2.h"
#include "NBBip.h"
#include "BSP.h"


void UpdateNBBIPSettings (void)
{
       union CharsAndLong   dIPAddress,dIPMask,dIPGateway,dIPDNS;
       
// ip address
// take data from modbus table and convert to a long.
    dIPAddress.cValue[0] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPADDRESS];
    dIPAddress.cValue[1] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPADDRESS+1];
    dIPAddress.cValue[2] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPADDRESS+2];
    dIPAddress.cValue[3] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPADDRESS+3];

// Network Mask
// take data from modbus table and convert to a long.
    dIPMask.cValue[0] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPMASK];
    dIPMask.cValue[1] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPMASK+1];
    dIPMask.cValue[2] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPMASK+2];
    dIPMask.cValue[3] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPMASK+3];

// Gateway
// take data from modbus table and convert to a long.
    dIPGateway.cValue[0] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPGATEWAY];
    dIPGateway.cValue[1] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPGATEWAY+1];
    dIPGateway.cValue[2] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPGATEWAY+2];
    dIPGateway.cValue[3] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPGATEWAY+3];

// DNS
// take data from modbus table and convert to a long.
    dIPDNS.cValue[0] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPDNS];
    dIPDNS.cValue[1] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPDNS+1];
    dIPDNS.cValue[2] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPDNS+2];
    dIPDNS.cValue[3] = g_arrnMBTable[NETBURNER_CONFIG_SET_IPDNS+3];


    ConfigRecord* newCR=(ConfigRecord*)malloc(sizeof(ConfigRecord));
    memcpy(newCR,&gConfigRec,sizeof(ConfigRecord));
    newCR->ip_Addr = dIPAddress.lValue;
    newCR->ip_Mask = dIPMask.lValue;
    newCR->ip_GateWay = dIPGateway.lValue;
    newCR->ip_DNS_server = dIPDNS.lValue;
    
//    iprintf("setting ip addresses ip is %x",EthernetIP);
    
    
  
//    SetupMttty();
//    iprintf("\nip address is %x",dIPAddress.lValue);
//    iprintf("\nip mask is %x",dIPMask.lValue);
//    iprintf("\nip gateway is %x",dIPGateway.lValue);
//    iprintf("\nip gateway is %x",dIPDNS.lValue);
 
    UpdateConfigRecord (newCR);
    free (newCR);
    CheckIfDHCP();
    ForceReboot();                                                                                                                                                                                                                      

}


void CheckIfDHCP (void)
{
      if (EthernetIP==0)
      {
          if(GetDHCPAddress() == DHCP_OK)
          {
              g_bDHCP = TRUE;
          }
          else
          {
              g_bDHCP = FALSE;
          } 
      }
}



