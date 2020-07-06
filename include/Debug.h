//////////////////////////////////////////////////////
//
// P.Smith                      8/6/06
// first pass at netburner hardware conversion.
//#include <basictypes.h>
//
// P.Smith                      5/2/08
// add m_bPrintfDebug to debug
//
// P.Smith                      13/2/09
// added  bools m_bUSBPrinterDebug & m_bUSBStickDebug;
//
// P.Smith                      22/7/09
// added m_bPulsingDebug
//
// P.Smith                      25/8/09
// added m_bFlowRateDebug to debug structure
//
// P.Smith                      19/11/09
// added m_bAToDStabilityDebug to debug structure
//
// M.McKiernan					12.02.2010
// Added m_bEIPDebug

// M.Mckiernan					22.4.2020
// Added BOOL m_bUltrasonicsDebug;
//////////////////////////////////////////////////////

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <basictypes.h>


typedef	struct {
                BOOL	m_bAtoD;
                BOOL	m_bNetworkComms;
 	            BOOL	m_bMasterComms;
 	            BOOL	m_bRealTimeClock;
 	            BOOL m_bCycleDiagnostics;
 	            BOOL m_bPrintfDebug;
 	            BOOL m_bUSBPrinterDebug;
 	            BOOL m_bUSBStickDebug;
 	            BOOL m_bPulsingDebug;
 	            BOOL m_bLoaderDebug;
 	            BOOL m_bFlowRateDebug;
 	            BOOL m_bAToDStabilityDebug;
 	            BOOL m_bEIPDebug;				// Ethernet/IP.
 	            BOOL m_bUltrasonicsDebug;
 	            } StructDebug;

#endif	//__DEBUG_H__



