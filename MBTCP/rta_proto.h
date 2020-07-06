/* ********************************************************************
Module:  rta_proto.h
Author:  Jamin D. Wendorf       10/02
         (c)2002 REAL TIME AUTOMATION

This file contains function prototypes used by all RTA applications.
*********************************************************************** */
#ifndef __RTA_PROTO_H__
#define __RTA_PROTO_H__

/* --------------------------------------------------------------- */
/*      UTILITY FUNCTION PROTOTYPES                                */
/* --------------------------------------------------------------- */
void   rta_PutBigEndian16 (uint16 value, uint8 *pos);
void   rta_PutBigEndian32 (uint32 value, uint8 *pos);
void   rta_PutLitEndian16 (uint16 value, uint8 *pos);
void   rta_PutLitEndian32 (uint32 value, uint8 *pos);
uint16 rta_GetBigEndian16 (uint8 *pos);
uint32 rta_GetBigEndian32 (uint8 *pos);
uint16 rta_GetLitEndian16 (uint8 *pos);
uint32 rta_GetLitEndian32 (uint8 *pos);
void   rta_ByteMove       (uint8 *dst, uint8 *src, uint16 len);

#endif /* __RTA_PROTO_H__ */
