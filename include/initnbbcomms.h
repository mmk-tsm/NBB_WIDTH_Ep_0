//////////////////////////////////////////////////////
// P.Smith                      9/11/06
// added InitialiseMasterCommsSerialPort
//
// P.Smith                      24/1/07
// name change InitialiseU2SerialPort to InitialiseNetworkSerialPort
//
// P.Smith                      5/2/07
// name change NetworkRxOn, NetworkTxOn
//
// P.Smith                      6/2/07
// EnableU2TxB,DisableU2TxB removed.
//
// P.Smith                      27/4/10
// added definitions of baud rate 9600 - 115200
//
// P.Smith                      30/4/10
// changed BAUD_RATE_9600_ID from 0 - 4, was 1-4
//////////////////////////////////////////////////////



#ifndef _INITNBBCOMMS_H
#define _INITNBBCOMMS_H


// exported methods
void EnableNBBTxB( void );

void DisableNBBTxB( void );

void InitialisePanelSerialPort(void);
void InitialiseNetworkSerialPort(void);
void NetworkRxOn( void );
void NetworkTxOn( void );
void InitialiseMasterCommsSerialPort( void );

#define	BAUD_RATE_9600_ID	(0)
#define	BAUD_RATE_19200_ID	(1)
#define	BAUD_RATE_38400_ID	(2)
#define	BAUD_RATE_57600_ID	(3)
#define	BAUD_RATE_115200_ID	(4)

#define	BAUD_RATE_LAST_ID   (BAUD_RATE_115200_ID)
#define	BAUD_RATE_DEFAULT   (BAUD_RATE_9600_ID)


#define	BAUD_RATE_9600	    (9600)
#define	BAUD_RATE_19200	    (19200)
#define	BAUD_RATE_38400	    (38400)
#define	BAUD_RATE_57600	    (57600)
#define	BAUD_RATE_115200    (115200)






// void RxBOn( void );                 //todo - taken from MBSHand.c
#endif // _INITNBBCOMMS_H

