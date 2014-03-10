//////////////////////////////////////////////////////////////
// File:		// ExecuteProcess.h
// File time:	// 10.08.2004	18:47
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _EXECUTEPROCESS_H_UID0000016BC1CF9B8C
#define _EXECUTEPROCESS_H_UID0000016BC1CF9B8C

#define  PRESENTER_BROWSER   _T("PRBrowser.exe")
#define  PRESENTER_BROWSER_CLASS   _T("PRIEFrame")

//  use only for tests
// #define  PRESENTER_BROWSER   _T("iexplore.exe")
// #define  PRESENTER_BROWSER_CLASS   _T("IEFrame")

#define  PRESENTER_PREVIEWER	_T("Previewer.exe")

BOOL Execute(
			 IN		LPCTSTR	pszCommandLine,
			 IN		LPCTSTR	pszOpenVerb,	//	= NULL,
			 IN		BOOL	bDisableUI,		//	= FALSE,
			 IN		CWnd*	pParent,		//	= NULL,
			 IN		int		nShowWindow,	//	= SW_SHOWNORMAL,
			 IN		BOOL	bURL,			//	= FALSE,
			 OUT	PHANDLE	pProcessHandle	//	= NULL
			 );

CString ProcessURLPrefixes( const CString& url );

BOOL StartProgram( LPCTSTR pszCommandLine, int nShowWindow );

HANDLE StartProgram( LPCTSTR pszCommandLine);

#define TA_FAILED			0
#define TA_SUCCESS_CLEAN	1
#define TA_SUCCESS_KILL		2
#define TA_SUCCESS_16		3

BOOL SafeTerminateProcess(
						  HANDLE hProcess, // handle to the process
						  UINT uExitCode   // exit code for the process
						  );

#endif //ifndef _EXECUTEPROCESS_H_UID0000016BC1CF9B8C
