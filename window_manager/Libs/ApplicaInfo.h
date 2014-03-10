//////////////////////////////////////////////////////////////
// File:		// ApplicaInfo.h
// File time:	// 14.07.2004	20:13
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _APPLICAINFO_H_UID00000065DA4272AD
#define _APPLICAINFO_H_UID00000065DA4272AD

#define WAIT_FOR				20

// Common Win API superfunctions:

HMODULE ModuleFromAddress(PVOID pv);
DWORD MyGetProcessId( HANDLE hProcess );
void MySleep ( DWORD dwMilliseconds ); // for better debugging

// Applica specific function (info for Presenter):

LPCTSTR GetApplicaRegistry();
LPCTSTR GetApplicaProductName();
LPCTSTR GetApplicaHostDevice();
LPCTSTR GetServiceName();
LPCTSTR GetMailslotFmt();

CString GetStationName( int i );

BOOL IsIniChanged(); // if changed then Presenter will exit

#define CHECK_INI_CHANGE \
	if( IsIniChanged() ) \
	{ \
		DestroyWindow(); \
		return; \
	}

#endif //ifndef _APPLICAINFO_H_UID00000065DA4272AD
