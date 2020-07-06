// P.Smith                      1/3/07
// LASTLINESPEEDSIGNALTYPE defined
//
// P.Smith                      16/10/08
// added CopyLiquidAdditiveDataToMB
///////////////////////////////////////////////////////

#ifndef __LIQUIDADDITIVE_H__
#define __LIQUIDADDITIVE_H__

void LiquidAdditiveCommand( int nTemp );
void CopyLiquidAdditiveToMB( void );
void LiquidAdditiveOnehz( void );
void SetLiquidAdditiveControlOutput( void );
void EstimateLiquidComponentWeight( void );
void LiquidAdditiveOnTimeExpired( void );
void CopyLiquidAdditiveDataToMB( void );



#endif   // __LIQUIDADDITIVE_H__


