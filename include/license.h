//////////////////////////////////////////////////////
// License.h
//
// P.Smith                              20-09-2005
//
// P.Smith                              23-09-2005
// added CheckLicensePowerUpStatus,DecideOnLicenseExpiryWarning
//
//
// P.Smith                      28/6/06
// first pass at netburner hardware conversion.
//
// P.Smith                      5/3/10
// added LicenseFunction,AllLicenseOptionsOn,AllLicenseOptionsOff
// move licensing definitions to here.
//
// P.Smith                      8/3/10
// added LICENSE_ALLOW_BATCH_WEIGHT_CHECK,LICENSE_DISABLE_BATCH_WEIGHT_CHECK              (40)
// added LICENSE_DO_BATCH_WEIGHT_CHECK bit
//
// P.Smith                      25/3/10
// added LICENSE_LINK_NBB_TO_NETBURNER_MODULE
//
// P.Smith                      13/4/10
// added ShowLicenseInfo
//
// P.Smith                      15/4/10
// added CheckForUnRestrictedAccessToLicening,MapMBLicensingOptions
// MapMBLicensingOptionsNo2
// added LICENSE_ALLOW_UNRESTRICED_ACCESS,define UNRESTRICED_ACCESS
// put LICENSE_BLENDER_PERMANENT_RUN_OFF at the end so that
// all the licensing options can be  activated one after the other.
// added LICENSE_ALLOW_UNRESTRICED_ACCESS
// added license definition for mb options.
// this is the format for the licensing options that can be changed via
// modbus when unrestricted access is enabled.
//
// P.Smith                      4/5/10
// pass data to MapMBLicensingOptions & MapMBLicensingOptionsNo2;

//////////////////////////////////////////////////////

#ifndef __LICENSE_H__
#define __LICENSE_H__

void CheckEncryptionEntry( unsigned long ulCode );
void ClearLicenseAlarm( void );
void CheckTemporaryHourlyCounter( void );
void DecideOnLicenseExpiryWarning( void );
void LicenseFunction( unsigned long lValue);
void AllLicenseOptionsOn( void );
void AllLicenseOptionsOff( void );
void CheckOptionsForValidLicense( void );
void ShowLicenseInfo( void );
void CheckForUnRestrictedAccessToLicening( void );
void MapMBLicensingOptions(WORD wOptions );
void MapMBLicensingOptionsNo2( WORD w2Options );




#define     LICENSE_ALL_OPTIONS_ON					 		(1)
#define     LICENSE_ALL_OPTIONS_OFF					 		(2)
#define     LICENSE_BLENDER_PERMANENT_RUN_ON         		(3)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_YEAR     	(4)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_SIX_MONTHS   	(5)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_THREE_MONTHS 	(6)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_MONTH    	(7)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_WEEK     	(8)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_DAY      	(9)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_FIVE_HOURS   	(10)
#define     LICENSE_BLENDER_TEMPORARY_RUN_ON_TWO_HOURS    	(11)
#define     LICENSE_BLENDER_PERMANENT_RUN_OFF        		(12)

#define     LICENSE_VACUUM_LOADING_ON		         		(13)
#define     LICENSE_VACUUM_LOADING_OFF		         		(14)
#define     LICENSE_MODBUS_TCP_ON    		         		(15)
#define     LICENSE_MODBUS_TCP_OFF    		         		(16)
#define     LICENSE_ETHERNET_IP_ON   		         		(17)
#define     LICENSE_ETHERNET_IP_OFF   		         		(18)
#define     LICENSE_KGPH_EXTRUSION_CONTROL_ON	         	(19)
#define     LICENSE_KGPH_EXTRUSION_CONTROL_OFF	          	(20)

#define     LICENSE_GMPM_EXTRUSION_CONTROL_ON	         	(21)
#define     LICENSE_GMPM_EXTRUSION_CONTROL_OFF	          	(22)
#define     LICENSE_CLEANING_ON               	          	(23)
#define     LICENSE_CLEANING_OFF              	          	(24)
#define     LICENSE_LLS_ON                    	          	(25)
#define     LICENSE_LLS_OFF                   	          	(26)
#define     LICENSE_TOPUP_ON                   	          	(27)
#define     LICENSE_TOPUP_OFF                   	        (28)

#define     LICENSE_FULL_CLEAN_ON                 	        (29)
#define     LICENSE_MIXER_CLEAN_ON                 	        (30)
#define     LICENSE_BIN_CLEAN_DRAIN_ON              	    (31)
#define     LICENSE_BIN_CLEAN_DRAIN_OFF             	    (32)
#define     LICENSE_BYASS_MODE_ON                   	    (33)
#define     LICENSE_BYASS_MODE_OFF                  	    (34)

#define     LICENSE_OFFLINE_MODE_HIGH_LOW_SENSORS_ON        (35)
#define     LICENSE_OFFLINE_MODE_HIGH_LOW_SENSORS_OFF       (36)
#define     LICENSE_OFFLINE_MODE_BLEND_TO_XXKGS_ON          (37)
#define     LICENSE_OFFLINE_MODE_BLEND_TO_XXKGS_OFF         (38)
#define     LICENSE_ALLOW_BATCH_WEIGHT_CHECK         		(39)
#define     LICENSE_DISABLE_BATCH_WEIGHT_CHECK              (40)

#define     LICENSE_LINK_NBB_TO_NETBURNER_MODULE            (41)





#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX50	         	(50)
#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX150        	(51)
#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX250        	(52)
#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX350        	(53)
#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX650        	(54)
#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX1000        	(55)
#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX1500        	(56)
#define     LICENSE_SET_BLENDER_MODEL_OPTIMIX3000        	(57)

#define     LICENSE_SET_BLENDER_MODEL_TSM50             	(58)
#define     LICENSE_SET_BLENDER_MODEL_TSM150             	(59)

#define		LICENSE_ALLOW_UNRESTRICED_ACCESS				(70)

#define     LICENSE_BLENDERRUNOPTIONBIT             0X01 // BLENDERRUNOPTIONBIT EQU     %00000001
//#define     LICENSE_CLEANOPTIONBIT                  0X02 // CLEANOPTIONBIT      EQU     %00000010
#define     LICENSE_KGHROPTIONBIT                   0X04 //
#define     LICENSE_ETHERNETIP                      0X08 // ETHERNET IP

#define     LICENSE_GPMOPTIONBIT                    0X10 // GPMOPTIONBIT        EQU     %00010000
#define     LICENSE_LLSOPTIONBIT                    0X20 // LLSOPTIONBIT        EQU     %00100000
#define     LICENSE_VACUUMLOADING                   0X40 // REPORTOPTIONBIT     EQU     %01000000

#define     LICENSE_MODBUSTCP                       0X0001 // modbus tcp
#define     LICENSE_TOPUPOPTIONBIT                  0X0002 // TOPUP

#define     LICENSE_FULLCLEANOPTIONBIT              0X0004 //
#define     LICENSE_MIXERCLEANOPTIONBIT             0X0008 //
#define     LICENSE_BINCLEANOPTIONBIT               0X0010 //
#define     LICENSE_BYPASSOPTIONBIT                 0X0020 //
#define     LICENSE_OFFLINEHIGHLOWOPTIONBIT         0X0040 //
#define     LICENSE_OFFLINEBLENDTOXXKGSBIT          0X0080 //
#define     LICENSE_DO_BATCH_WEIGHT_CHECK           0X0100 //





#define     LICENSE_ONE_YEAR                        8760 // ID1HOURS            EQU     8760        ; 1 YEAR
#define     LICENSE_SIX_MONTHS                      4380 // ID2HOURS            EQU     4380        ; 6 MONTHS
#define     LICENSE_THREE_MONTHS                    2190 // ID3HOURS            EQU     2190        ; 3 MONTHS
#define     LICENSE_ONE_MONTH                       730  // ID4HOURS            EQU     730         ; 1 MONTH
#define     LICENSE_ONE_WEEK                        168  // ID5HOURS            EQU     168         ; 1 WEEK
#define     LICENSE_ONE_DAY                         24   // ID6HOURS            EQU     24          ; 1 DAY
#define     LICENSE_TEN_HOURS                       10   // ID7HOURS            EQU     10          ;
#define     LICENSE_FIVE_HOURS                      5    // ID8HOURS            EQU     5          ;
#define     LICENSE_TWO_HOURS                       2    // ID9HOURS            EQU     2          ;

#define     UNRESTRICED_ACCESS                       (0xAA)


// definitions of bits for licensing options
//
// 1st word
#define     LICENSEMB_KGHROPTIONBIT                   0X0001 //
#define     LICENSEMB_ETHERNETIP                      0X0002 // ETHERNET IP

#define     LICENSEMB_GPMOPTIONBIT                    0X0004 // GPMOPTIONBIT        EQU     %00010000
#define     LICENSEMB_LLSOPTIONBIT                    0X0008 // LLSOPTIONBIT        EQU     %00100000
#define     LICENSEMB_VACUUMLOADING                   0X0010 // REPORTOPTIONBIT     EQU     %01000000

#define     LICENSEMB_MODBUSTCP                       0X0020 // modbus tcp
#define     LICENSEMB_TOPUPOPTIONBIT                  0X0040 // TOPUP

#define     LICENSEMB_FULLCLEANOPTIONBIT              0X0080 //
#define     LICENSEMB_MIXERCLEANOPTIONBIT             0X0100 //
#define     LICENSEMB_BINCLEANOPTIONBIT               0X0200 //
#define     LICENSEMB_BYPASSOPTIONBIT                 0X0400 //
#define     LICENSEMB_OFFLINEHIGHLOWOPTIONBIT         0X0800 //
#define     LICENSEMB_OFFLINEBLENDTOXXKGSBIT          0X1000 //
#define     LICENSEMB_DO_BATCH_WEIGHT_CHECK           0X2000 //


#endif  // __LICENSE_H__


