//////////////////////////////////////////////////////////
// editor.h
//
// Holds the data structures and definitions used by the
// on screen text/data editor.
//
// M.Parks								17-02-2000
// Loosely based on the DOS Sigmat 9000 code.
//////////////////////////////////////////////////////////
// M.McKiernan							24-10-2000
//			bEntryChangeFlag added to DataEntryStruct - for data change notification

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __EDITOR_H__
#define __EDITOR_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************


// Maximum possible number of editor fields
#define	MAX_EDITOR_FIELDS		200

// Supported data formats
#define	EDITOR_DATA_INT			0
#define	EDITOR_DATA_FLOAT		1
#define	EDITOR_DATA_ASCII		2
#define	EDITOR_DATA_TOGGLE		3

#define	EDITOR_DATA_NONE		-1


// Maximum number of characters that can be entered in an
// edit field.    The logical limit is 40 since this is the
// width of the screen but 20 allows for a description in
// front of the edit field.   Change it if you want but the
// editor doesn't currently support multi-line entry for a
// single field so 40 is the max.
#define	MAX_ENTRY_LENGTH		20

//
// Union for holding the various data types
//
typedef union {
	int		nValue;						// Used for the toggle entries
	long	lValue;						// Used for the integers
	float	fValue;						// Used for the floats
	char	cValue[MAX_ENTRY_LENGTH];	// Used for strings
	} unionEditorData;


//
// general format parameters for the editors
//
typedef	struct	{
	// Variables set by the programmer
	int		nTopRow;			// top editing row on the screen in pixels
	int		nDisplayRows;		// number of display rows on the screen
	int		nFieldCount;		// number of display/edit fields
	Bool	bAllowMenuKeys;
	Bool	bAllowMenuKey5Skip;	// Menu key 5 becomes a skip or advance key.
//	int		nLeft;				// leftmost display column
//	int		nRight;				// rightmost display column
//	int		(*function)(void);	// function specific to the data being edited

	// Variables used by the code
	int		nCurrentRow;		// current edit row
	int		nFirstField;		// current top field - changes if we scroll
	int		nLastField;			// current bottom field - changes if we scroll
	int		nCurrentField;		// current edit field
	int		nCharCount;			// Number of characters typed by the user
	char	cEntryData[MAX_ENTRY_LENGTH];	// Buffer for the user entered data 
	} EditFormatStruct;

//
// data entry structure
//
typedef	struct	{
    Bool	bDisplayOnly;		// 0 = edit and display, 1 = display only
	Bool	bHiddenEntry;		// 0 = show data, 1 = Hide data
	int		nIndex;				// only advance to the next line if the index increases
	int		nFormat;			// data format eg, 2 = ASCII
	int		nDecimals;			// number of decimal places
	int		nSize;				// maximum number of characters before the decimal place
								// or in the case of a toggle entry the maximum number of choices
//	int		nUnits;				// conversion factor for imperial units
	int		nOldCol;			// current data display
	int		nNewCol;			// new data display and entry
	int		nTextCol;			// starting column for the text string
//	int		nUpdate;			// 0 = always update, 1 = update only if changed
	Bool	bTextAlloc;			// 1 = memory was allocated to hold the string and must be freed up before the structure
	char	*pText;				// descriptive text for the entry
//	char	*pData;				// pointer to the data in the data structure
	unionEditorData	uNewData;	// holds the previously displayed data value
	char	*far pToggleText[10];	// array of pointers to text strings for the toggle values
	Bool		bEntryChangeFlag;  // flag to show individual data change
//	void	(*function)(void);	// special function related to this field. It is passed the field number
	} DataEntryStruct;


// Function declarations

Bool	Editor( void );

int		DisplayEditorFields( int nStartField );

void	BlankField( int nRow, int nColumn, int nLength );

void	PositionCursor( DataEntryStruct *far pDataEntry, const int nRow );

void	ScrollDown( int *nIndex );

void	CheckEntryPosition( DataEntryStruct *far pDataEntry );


#endif	// __EDITOR_H__