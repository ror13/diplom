//////////////////////////////////////////////////////////////
// File:		// ScreenSaver.cpp
// File time:	// 19.05.2005	15:08
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "ScreenSaver.h"

//////////////////////////////////////////////////////////////

BOOL EnableScreenSaver( BOOL bNewValue, BOOL bQueryOnly ) // returns old value
{
	BOOL bScreenSaverIsOn = TRUE;
	SystemParametersInfo( SPI_GETSCREENSAVEACTIVE, 0, &bScreenSaverIsOn, SPIF_SENDWININICHANGE );
	if( !bQueryOnly )
	{
		SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, bNewValue, NULL, SPIF_SENDWININICHANGE );
	}
	return bScreenSaverIsOn;
}

//////////////////////////////////////////////////////////////

BOOL CALLBACK KillScreenSaverFunc(HWND hwnd, LPARAM lParam)
{
  if(IsWindowVisible(hwnd))
      PostMessage(hwnd, WM_CLOSE, 0, 0);
  return TRUE;
}

//////////////////////////////////////////////////////////////

BOOL StopScreenSaver()
{
   HDESK hdesk;

   hdesk = OpenDesktop(TEXT("Screen-saver"),
                       0,
                       FALSE,
                       DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS);
   if (hdesk)
   {
      EnumDesktopWindows(hdesk, KillScreenSaverFunc, 0);
      CloseDesktop(hdesk);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
