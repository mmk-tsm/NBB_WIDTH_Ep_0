//////////////////////////////////////////////////////
// 16R40C.h
//
// Implementation class for handling all the standard
// TSM 16 by 40 LCD display functionality
//
// M.Parks									16-07-1999
// M.McKiernan									30-03-2001
// DisplayMessageWithReset
// M.McKiernan									14-10-2003
// Define attribute changed mask bits, which use bit b6.
//
// M.McKiernan									26-01-2005
// Added masks for remote (serial comms panel) text and attribute changes. - see REM_TEXT_CHANGED_MASK etc
// Added names for user defined chars. e.g. CE_CHAR to DEC_SYMBOL
// 
// P.Smith                      24/1/06
// first pass at netburner hardware conversion.
// commented out functions for now
// commented out #include <stdtypes.h>
//////////////////////////////////////////////////////


//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __16ROW40COLUMNDISPLAY_H__
#define __16ROW40COLUMNDISPLAY_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nb #include <stdtypes.h>


// Flag used to indicate that a value in the display buffer has changed
// This is OR'ed with the display or attribute byte
/// 		#define	CHANGED_FLAG			0x80
// Mask for clearing the changed flag
/// 		#define	CLEAR_CHANGED_MASK		0x7f

#define	LOCAL_TEXT_CHANGED_MASK		0x80		//RENAMED FROM CHANGED_FLAG
#define	LOCAL_CLEAR_TEXT_CHANGED_MASK	0x7F		// RENAMED FROM CLEAR_CHANGED_FLAG

#define	LOCAL_ATTRIBUTE_CHANGED_MASK		0x40
#define	LOCAL_CLEAR_ATTRIBUTE_CHANGED_MASK	0xBF

#define	REM_TEXT_CHANGED_MASK		0x20		//Remote (panel) text CHANGED
#define	REM_CLEAR_TEXT_CHANGED_MASK	0xDF		// 

#define	REM_ATTRIBUTE_CHANGED_MASK		0x10	//Remote (panel) attribute CHANGED
#define	REM_CLEAR_ATTRIBUTE_CHANGED_MASK	0xEF

// main masks will set the local and remote change bits simultaneously.
#define	TEXT_CHANGED_MASK		( LOCAL_TEXT_CHANGED_MASK | REM_TEXT_CHANGED_MASK )		//RENAMED FROM CHANGED_FLAG
#define	ATTRIBUTE_CHANGED_MASK		( LOCAL_ATTRIBUTE_CHANGED_MASK | REM_ATTRIBUTE_CHANGED_MASK )


// Specific details for this display
#define		ROWS					16		
#define		COLUMNS					40
#define		MAX_DISPLAY_INDEX		(ROWS * COLUMNS)
#define		GRAPHICS_BUFFER_SIZE	5120
#define		X_PIXELS_LIMIT			240
#define		Y_PIXELS_LIMIT			128
#define		PIXELS_PER_BYTE			6		// Based on an 8x6 character giving 40 columns
#define		ASPECT_RATIO		(X_PIXELS_LIMIT / Y_PIXELS_LIMIT)
// calculate the spacing for the menu text
#define		MENU_KEY_WIDTH		(COLUMNS / 5)
#define		MENU_KEY1_SPACE		(MENU_KEY_WIDTH / 2)
#define		MENU_KEY2_SPACE		(MENU_KEY1_SPACE + MENU_KEY_WIDTH)
#define		MENU_KEY3_SPACE		(MENU_KEY1_SPACE + (2 * MENU_KEY_WIDTH))
#define		MENU_KEY4_SPACE		(MENU_KEY1_SPACE + (3 * MENU_KEY_WIDTH))
#define		MENU_KEY5_SPACE		(MENU_KEY1_SPACE + (4 * MENU_KEY_WIDTH))


// DisplayMode
#define		DISPLAY_MODE_TEXT		0
#define		DISPLAY_MODE_GRAPHICS	1
#define		DISPLAY_MODE_BOTH		2

// These are the reverse of what is documented in the LCD manual
// but this is because there is a link on the LCD which allows
// you to reverse the sense in hardware.
#define		REVERSE_ATTRIBUTE		0x00
#define		NORMAL_ATTRIBUTE		0x05
#define		REVERSE_BLINK_ATTRIBUTE	0x08
#define		NORMAL_BLINK_ATTRIBUTE	0x0d

#define		JUSTIFY_LEFT			0
#define		JUSTIFY_RIGHT			1
#define		JUSTIFY_CENTER			2

// User defined characters	
#define		DOWN_ARROW				(0x80+0x20)	// DOWN ARROW
#define		UP_ARROW					(0x81+0x20)	// UP ARROW (0x20 KEEPS CODE SAME AS FOR ASCII)
#define		CE_CHAR					(0x82+0x20)	// Polish ce char
#define		UPD_QUESTION_MARK		(0x83+0x20)	// Upside down & backwards question mark.
#define		AM_SYMBOL				(0x84+0x20)	// Auto Man symbol
#define		BACK_ARROW				(0x85+0x20)	// BACK ARROW (RETURN OR ESC)
#define		INC_SYMBOL				(0x86+0x20)	// INCrease symbol (up triangle)
#define		DEC_SYMBOL				(0x87+0x20)	// DECrease symbol (down triangle).

// Commands unique to this display type
#define		GRAPHICS_HOME_ADDR		0x0000
#define		TEXT_HOME_ADDR			0x1000		// TEXT HOME ADDRESS	
#define		ATTRIB_HOME_ADDR		0x0c00		// Start of attribute data in the LCD Display

#define		ADDR_PTR_CMD			0x24		// ADDR. PTR. SET COMMAND
#define		OFFSET_REG_CMD			0x22		// OFFSET REG. SET CMD.
#define		CURSOR_POS_CMD			0x21		// CURSOR POSITION SET COMMAND.

#define		TEXT_HOME_CMD			0x40		// TEXT HOME COMMAND
#define		TEXT_AREA_CMD			0x41		// TEXT AREA COMMAND (NO. OF COLUMNS)
#define		GRAPHICS_HOME_CMD		0x42		// GRAPHIC HOME ADDR. COMMAND
#define		GRAPHICS_AREA_CMD		0x43		// GRAPHIC AREA COM.

#define		GRAPHICS_MODE_SET_CMD	0x80		// CG ROM, Graphic & TEXT 'OR' Mode
#define		MODE_SET_CMD			0x84		// CG ROM, TEXT ONLY

#define		TEXT_GRAPHIC_OFF		0x90
#define		TEXT_CURSOR_OFF			0x95 		// TEXT ON, CURSOR OFF, GRAPHIC OFF
#define		TEXT_CURSOR_ON			0x97		// TEXT ON Blinking CURSOR ON & GRAPHIC OFF
#define		GRAPHIC_CURSOR_OFF		0x98		// GRAPHIC ON, CURSOR OFF, TEXT OFF
#define		GRAPHIC_CURSOR_ON		0x9b		// GRAPHIC ON Blinking CURSOR ON, TEXT OFF
#define		BOTH_CURSOR_OFF			0x9c
#define		BOTH_CURSOR_ON			0x9e

#define		CURSOR_PATTERN			0xa1		// Bits 0 to 2 set pattern B0 is bottom line of 8

#define		DATA_WRITE_NO_INC		0xC4		// DATA WRITE COMMAND - No increment
#define		DATA_READ_NO_INC		0xC5		// DATA READ COMMAND - No increment
#define		DATA_WRITE_INC			0xC0		// DATA WRITE COMMAND - Increment

#define		LCD_STATUS_MASK			0x03		// Mask for checking that the LCD is ready for a write 

// Macros used by the graphics routines
#define	sign(x)		((x) > 0 ? 1 : ((x) == 0 ? 0 : (-1)))
#define	max(x, y)	(((x) > (y)) ? (x) : (y))

/*nb
//////////////////////////////////////////////////////////////////////
//
// F U N C T I O N   D E C L A R A T I O N S
//
//////////////////////////////////////////////////////////////////////

//@member
void	ClearDisplayRowSection( int nRow, int nColumn, int nLength );
//@member
void	ClearDisplayRow( int Row );
//@member
void	ClearDisplay( void );

//@member
void	GotoRowColumn( int Row, int Column );

//@member
void	CursorOn( void );
//@member
void	CursorOff( void );

//@member
void	DisplayStringData( const char *pdata );
//@member
void	DisplayStringDataWithPosition( const char *pData, int Row, int Column );
//@member
int		DisplayStringDataJustified( const char *pData, int nRow, int nColumn, int nJustification );
//@member

void	DisplayIntegerData( long nData, int nDigits, int nUnits );
//@member
void	DisplayIntegerDataWithPosition( long nData, int nDigits, int Row, int Column, int nUnits );
//@member
int		DisplayIntegerDataJustified( long nData, int nDigits, int Row, int Column, int nJustification, int nUnits );

//@member
void	DisplayFloatData( double fData, int nDigits, int nDecimals, int nUnits );
//@member
void	DisplayFloatDataWithPosition( double fData, int nDigits, int nDecimals, int Row, int Column, int nUnits );
//@member
int		DisplayFloatDataJustified( double fData, int nDigits, int nDecimals, int Row, int Column, int nJustification, int nUnits );

//@member
void	DisplayCharacterData( const char Data );
//@member
void	DisplayCharacterDataWithPosition( const char cData, int nRow, int nColumn );

//@member
void	SetNormalDisplayAttribute( int Row, int Column, int Length );
//@member
void	SetReverseDisplayAttribute( int Row, int Column, int Length );
//@member
void	SetBlinkDisplayAttribute( int Row, int Column, int Length );
//@member
void	SetReverseBlinkDisplayAttribute( int Row, int Column, int Length );

//@member
void	DisplayMessage( const char *pText, Bool bSave );
//mmk
void DisplayMessageBox( const char *pText, Bool bSave );
void DisplayMessageWithReset( const char *pText, Bool bSave );

//@member
void	DisplayWaitMessage( const char *pText );
//@member
void	UpdateWaitMessage( void );
//@member
Bool	YesNoPrompt( const char *pText );
//@member
Bool	YesNoPrompt2( const char *pText );

int YesNoExitPrompt( const char *pText );

//@member
void	UpdateDisplay( void );
//@member
void	RefreshDisplay( void );
//@member
void	TestDisplay( void );

//@member
Bool	IsDisplayPresent( void );
//@member
void	InitialiseDisplay( Bool bStart );
//@member
void	ConfigureDisplayForText( void );
//@member
void	CreateUserCharacters( void );
//@member
void	ConfigureDisplayForGraphics( void );

//@member
void	WriteDisplayCommand( char Command );
//@member
void	WriteDisplayCommandWithDelay( char Command, int Delay );
//@member
void	WriteDisplayData( char Data );
//@member
void	WriteDisplayDataWithDelay( char Data, int Delay );
//@member
void	SetDisplayAttribute( int Row, int Column, int Length, int Attribute );

// Graphics Functions

//@member
void	ClearGraphicsDisplay( void );
//@member
void	TestFunction( void );
//@member
void	TurnOnPixel( int nX, int nY );
//@member
void	DrawLine( int nStartX, int nStartY, int nEndX, int nEndY );
//@member
void	DrawBox( int nTopX, int nTopY, int nWidth, int nHeight );
//@member
void	DrawCircle( int nX, int nY, int nRadius );
//@member
void	Symmetry( int x, int y, int XCentre, int YCentre, double ratio );

*/
#endif	// __16ROW40COLUMNDISPLAY_H__
