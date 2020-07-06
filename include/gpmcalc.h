//////////////////////////////////////////////////////
// GPMCalc.h
//
// Grams per meter calculations
//
// P.Smith                          2-3-2005
//
// P.Smith                          13/11/07
// added void CalculateSetGramsPerMeterFromGramsPerSqMeter( void )  
//////////////////////////////////////////////////////

#ifndef __GPMCALC_H__
#define __GPMCALC_H__

void BatchSecondsCalculation( void );
void CalculateKgPerHour( void );
void CalculateKgHRoundRobin( void );
void ResetKgHRoundRobin( void );
void CopyKghToMB( void );
void CopyGPMToModbus( void );
void CalculateGPMRoundRobin( void );
void BatchGPMCalc( void );
void ResetGPMRoundRobin( void );
void CalculateGramsPerSquareMeter( void );  
void CalculateSetGramsPerMeter( void );  
void CalculateSetGramsPerMeterFromGramsPerSqMeter( void );  


#endif  // __GPMCALC_H__


