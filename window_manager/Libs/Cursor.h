//////////////////////////////////////////////////////////////
// File:		// Cursor.h
// File time:	// 28.03.2006	15:19
// Description: //////////////////////////////////////////////
// 
//

#ifndef _CURSOR_H_UID000000492EAA5A3C
#define _CURSOR_H_UID000000492EAA5A3C

//////////////////////////////////////////////////////////////

BOOL LoadCursorFromFileMy( LPCTSTR szCursorFile, HCURSOR& hCursor,
						  CBitmap& bmp, CPalette* pPalette,
						  CBitmap& bmpMask, CPalette* pPaletteMask );
BOOL SaveCursorToFile( HCURSOR hCursor, LPCTSTR szCursorFile );

//////////////////////////////////////////////////////////////

#endif //ifndef _CURSOR_H_UID000000492EAA5A3C
