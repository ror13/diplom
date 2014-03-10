//////////////////////////////////////////////////////////////
// File:		// ApplicaInfo.cpp
// File time:	// 14.07.2004	20:13
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "ApplicaInfo.h"
#include "FileOperation.h"

/////////////////////////////////////////////////////////////////////////////////

#include "..\Inc\CommonApi.h"
#include "..\Inc\Applica.h"

///////////////////////////////////////////////////////////////////////////////

// Returns the HMODULE that contains the specified memory address
HMODULE ModuleFromAddress(PVOID pv)
{
   MEMORY_BASIC_INFORMATION mbi;
   return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) 
      ? (HMODULE) mbi.AllocationBase : NULL);
}

//////////////////////////////////////////////////////////////

//#include <Winternl.h>
#pragma comment (lib, "ntdll.lib")

typedef LONG NTSTATUS;

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
    ProcessWow64Information = 26
} PROCESSINFOCLASS;

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    DWORD PebBaseAddress;
    PVOID Reserved2[2];
    DWORD UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

extern "C" NTSTATUS NTAPI NtQueryInformationProcess (
    IN HANDLE ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

//////////////////////////////////////////////////////////////

DWORD MyGetProcessId( HANDLE hProcess )
{
	//return GetProcessId( hProcess );
	PROCESS_BASIC_INFORMATION basic;
	ZeroMemory( &basic, sizeof(basic) );
	ULONG	size = 0;
	NTSTATUS res = NtQueryInformationProcess( hProcess,
		ProcessBasicInformation, &basic, sizeof(basic), &size );

//	ASSERT( res>=0 );

	return basic.UniqueProcessId;
}

/////////////////////////////////////////////////////////////////////////////

void MySleep ( DWORD dwMilliseconds ) // for better debugging
{
	Sleep( dwMilliseconds );
}

/////////////////////////////////////////////////////////////////////////////

static CString GetLine( CString& strString )
{
	CString s;
	int pos = strString.Find( _T('\n') );
	if( pos==-1 )
	{
		s = strString;
		strString.Empty();
	}
	else
	{
		s = strString.Left( pos );
		strString = strString.Mid( pos+1 );
	}

	if( s.GetLength() && s.GetAt(s.GetLength()-1) == _T('\r') )
		s = s.Left( s.GetLength()-1 );
	return s;
}

/////////////////////////////////////////////////////////////////////////////

static BOOL ParseLine( const CString& strString, CString& token, CString& value )
{
	int pos = strString.Find( _T('=') );
	if( pos==-1 )
	{
		return FALSE;
	}
	else
	{
		token = strString.Left( pos );
		value = strString.Mid( pos+1 );
		token.TrimLeft();
		token.TrimRight();
		value.TrimLeft();
		value.TrimRight();

		return !token.IsEmpty();
	}
}

//////////////////////////////////////////////////////////////

TCHAR g_szApplicaRegistry[100]			= _T("");
TCHAR g_szApplicaHostDevice[30]			= _T("");
TCHAR g_szApplicaServiceName[30]		= _T("");
TCHAR g_szMailslotFmt[30]				= _T("");
TCHAR g_szApplicaProductName[100]			= _T("");

static BOOL g_bInfoLoaded = FALSE;

///////////////////////////////////////////////////////////////////////////////

static CString GetIniName()
{
	HMODULE hMod = ModuleFromAddress( GetIniName );
	TCHAR b[MAX_PATH] = _T("");
	GetModuleFileName( hMod, b, MAX_PATH );
	CString strIniName = b;
	int pos = strIniName.ReverseFind( _T('\\') );
	strIniName = strIniName.Left(pos+1);
	strIniName += _T("CloseApplicationOnChange.flg");
	return strIniName;
}

///////////////////////////////////////////////////////////////////////////////

static BOOL UpdateInfo()
{
	if( g_bInfoLoaded )
		return TRUE;

	BOOL result = TRUE;
	BOOL res;

	res = regReadRegString( REG_HKLM, HKLM_INFO_REG_KEY_T, REG_BASE_REG_VALUE_T,
		g_szApplicaRegistry, SIZEOF_ARRAY(g_szApplicaRegistry) );
	if( !res )
		result = FALSE;

	res = regReadRegString( REG_HKLM, HKLM_INFO_REG_KEY_T, REG_PRODUCT_NAME_VALUE_T,
		g_szApplicaProductName, SIZEOF_ARRAY(g_szApplicaProductName) );
	if( !res )
		result = FALSE;

	res = regReadRegString( REG_HKLM, HKLM_DRIVER_HOST_REG_KEY_T, REG_DRIVER_NAME_VALUE_T,
		g_szApplicaHostDevice, SIZEOF_ARRAY(g_szApplicaHostDevice) );
	if( !res )
		result = FALSE;

	res = regReadRegString( REG_HKLM, HKLM_SERVICES_MAIN_SERVICE_T, REG_SERVICE_NAME_VALUE_T,
		g_szApplicaServiceName, SIZEOF_ARRAY(g_szApplicaServiceName) );
	if( !res )
		result = FALSE;

	res = regReadRegString( REG_HKLM, HKLM_KERNEL_OBJECTS_MAILSLOTS_T, REG_STATION_MAILSLOT_FMT_T,
		g_szApplicaServiceName, SIZEOF_ARRAY(g_szApplicaServiceName) );
	if( !res )
		result = FALSE;

	if( _tcslen(g_szApplicaRegistry) == 0 )
		return FALSE;
	if( _tcslen(g_szApplicaProductName) == 0 )
		return FALSE;
	if( _tcslen(g_szApplicaHostDevice) == 0 )
		return FALSE;
	if( _tcslen(g_szApplicaServiceName) == 0 )
		return FALSE;

	if( result )
	{
		g_bInfoLoaded = TRUE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////

LPCTSTR GetApplicaRegistry()
{
	if( !UpdateInfo() )
		return NULL;
	return g_szApplicaRegistry;
}

LPCTSTR GetApplicaProductName()
{
	if( !UpdateInfo() )
		return NULL;
	return g_szApplicaProductName;
}

LPCTSTR GetApplicaHostDevice()
{
	if( !UpdateInfo() )
		return NULL;
	return g_szApplicaHostDevice;
}

LPCTSTR GetServiceName()
{
	if( !UpdateInfo() )
		return NULL;
	return g_szApplicaServiceName;
}

LPCTSTR GetMailslotFmt()
{
	if( !UpdateInfo() )
		return NULL;
	return g_szMailslotFmt;
}

//////////////////////////////////////////////////////////////

CString GetStationName( int i )
{
	CString s;
	if( i == HOST_STATION_ID )
		s = _T("Server Host");
	else
		s.Format( _T("Station %d"), i );
	return s;
}

//////////////////////////////////////////////////////////////

BOOL IsIniChanged() // if changed then application will exit
{
	static BOOL bFirst = TRUE;
	static CString static_old;
	CString ini;

	CString strIniName = GetIniName();
	if(!LoadAnsiFileToString( strIniName, ini ))
		return FALSE;

	if( bFirst )
	{
		bFirst = FALSE;
		static_old = ini;
	}

	return ini != static_old;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
