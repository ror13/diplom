//////////////////////////////////////////////////////////////
// File:		// CommonApi.h
// File time:	// 27.08.2004	14:52
// Renamed:  	// 23.11.2004	15:26
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _COMMONAPI_H_UID0000003D193F4A71
#define _COMMONAPI_H_UID0000003D193F4A71

/////////////////////////////////////////////////////////////////////////////////

#ifndef NO_SPECIALIZED_PROTECTION
#include "Product.h"
#endif

/////////////////////////////////////////////////////////////////////////////////

#ifdef APGINA_PRJ
	void  WriteLog ( LPCTSTR f, ... ); // APGinaCommon.cpp
#	define TraceDrv(x)
#	define TraceUser(x) WriteLog x
#endif

#ifdef CSLS_PRJ
#	include "..\Csls\Log.h"
#	define TraceDrv(x)
#	define TraceUser(x) WriteLog x
#endif

#ifdef IPMAPPER_PRJ
#	define TraceDrv(x)
#	define TraceUser(x) ::AfxTrace x
#endif

#ifdef DHCP_CLIENT_PRJ
#	include <stdio.h>
#	define TraceDrv(x)
#	define TraceUser(x) printf x
#endif

#ifdef IP_SERVICE_PRJ
#	include "../IpManagementService/NTService.h"
#	define TraceDrv(x)
#	define TraceUser(x) CNTService::DebugMsg x
#endif

#ifdef DRIVER_INSTALLER_PRJ
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef CSLS_INSTALL_PRJ
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef TCPFILTER_PRJ
#	define TraceDrv(x) DebugPrint(x)
#	define TraceUser(x)
#endif

#ifdef LOGON_CONFIG_PRJ
	void  WriteLog ( LPCTSTR f, ... ); // LogonConfig.cpp
#	define TraceDrv(x)
#	define TraceUser(x) WriteLog x
#endif

#ifdef PRESENTER_CLIENT_CODE
//#define WRITE_LOG(x)	WriteLog(_T("%s|%s %s\n"), __FILE__, __LINE__, x);
//#define WRITE_LOG(x)	WriteLog(_T("%s\n"), x);
#define WRITE_LOG(x)	WriteLog(_T("%S|%d %s\n"), __FILE__, __LINE__, x);
	void  WriteLog ( LPCTSTR f, ... ); // LogonConfig.cpp
	
//#	define TraceDrv(x)
//#	define TraceUser(x) WriteLog x
#endif
	
#ifdef PRESENTER_PRJ	
#define WRITE_LOG(x)	WriteLog(_T("%S|%d %s\n"), __FILE__, __LINE__, x);
	void  WriteLog ( LPCTSTR f, ... ); // LogonConfig.cpp
#endif

#ifdef STARTER_SERVICE_PRJ
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef APPLICA_SERVICE_PRJ
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef MESSAGES_PRJ
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef APPLICAMMC
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef PRESENTER_PRJ
#	include "..\Presenter.Engine\StationInfo.h"
#	define TraceDrv(x)
#	define TraceUser(x) TRACE x
#endif

#ifdef APPLICA_PRJ
#	include "Debug.h"
#	define TraceDrv(x) //KdPrint(x)
#	define TraceUser(x)
#endif

#ifdef ACSN_PRJ
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef APPLICASN_PRJ
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

#ifdef PROTECTION_TOOL_PRJ
#	define TraceDrv(x)
#	define TraceUser(x) TRACE x
#endif

#ifdef CONFIG_WIZARD_PRJ
#	define TraceDrv(x)
#	define TraceUser(x) TRACE x
#endif

#ifdef CREEPING_LINE_PRJ
#	define TraceDrv(x)
#	define TraceUser(x) TRACE x
#endif



#if !defined(TraceDrv) || !defined(TraceUser)
//#error "Unknown Project"
#	define TraceDrv(x)
#	define TraceUser(x)
#endif

/////////////////////////////////////////////////////////////////////////////////

#ifdef DRIVER_XXX
#	if DBG
#		define VISIBLE volatile
#	else
#		define VISIBLE
#	endif
#else
#	define VISIBLE
#endif

/////////////////////////////////////////////////////////////////

#ifdef DRIVER_XXX
#define REG_HKLM L"\\Registry\\Machine\\"
#define REG_HKCU L"\\Registry\\User\\"
#else
#define REG_HKLM HKEY_LOCAL_MACHINE
#define REG_HKCU HKEY_CURRENT_USER
#define REG_HKCR HKEY_CLASSES_ROOT
#endif

/////////////////////////////////////////////////////////////////

#ifndef SIZEOF_ARRAY
    #define SIZEOF_ARRAY(ar)        (sizeof(ar)/sizeof((ar)[0]))
#endif // !defined(SIZEOF_ARRAY)

#define USE_VAR(x) x

/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////

#include <tchar.h>

#ifndef ASSERT
#  include <assert.h>
#  define ASSERT assert
#endif

#ifdef _UNICODE
#ifndef UNICODE
#error UNICODE not defined with _UNICODE!
#endif
#else
#ifdef UNICODE
#error UNICODE defined without _UNICODE!
#endif
#endif

#ifdef UNICODE
#define UTCHAR	USHORT
#else
#define UTCHAR	UCHAR
#endif

#ifdef UNICODE
inline BYTE TcharToByte( WCHAR ch )
{
	return (BYTE)(UCHAR)(USHORT)ch;
}
#else
inline BYTE TcharToByte( CHAR ch )
{
	return (BYTE) ch;
}
#endif

inline void zeromemory( LPVOID pBuffer, DWORD dwSize )
{
	void *start = pBuffer;

	while (dwSize--)
	{
		*(char *)start = 0;
		start = (char *)start + 1;
	}
}

inline bool my_isalpha( TCHAR Char )
{
	return 
		( Char>=_T('A') && Char<=_T('Z') ) ||
		( Char>=_T('a') && Char<=_T('z') );
}

inline bool my_isalnum( TCHAR Char )
{
	return 
		( Char>=_T('0') && Char<=_T('9') ) ||
		( Char>=_T('A') && Char<=_T('Z') ) ||
		( Char>=_T('a') && Char<=_T('z') );
}

inline int my_isasciiA (char ch)
{
	int res = ch >= 0x00 && ch <= 0x7F;
	return res;
}

inline TCHAR my_tolower( TCHAR Char )
{
	if( Char>=_T('A') && Char<=_T('Z') )
		return TCHAR( Char - _T('A') + _T('a') );
	return Char;
}

inline TCHAR my_toupper( TCHAR Char )
{
	if( Char>=_T('a') && Char<=_T('z') )
		return TCHAR( Char - _T('a') + _T('A') );
	return Char;
}

inline CHAR my_toupperA( CHAR Char )
{
	if( Char>=('a') && Char<=('z') )
		return CHAR( Char - ('a') + ('A') );
	return Char;
}

inline bool ishex( TCHAR Char )
{
	return 
		( Char>=_T('0') && Char<=_T('9') ) ||
		( Char>=_T('A') && Char<=_T('F') ) ||
		( Char>=_T('a') && Char<=_T('f') );
}

inline BYTE fromhex( TCHAR Char )
{
	if( Char>=_T('0') && Char<=_T('9') )
		return TcharToByte( TCHAR( Char - _T('0') ) );
	if( Char>=_T('A') && Char<=_T('F') )
		return TcharToByte( TCHAR( Char - _T('A') + 10 ) );
	if( Char>=_T('a') && Char<=_T('f') )
		return TcharToByte( TCHAR( Char - _T('a') + 10 ) );
	return 0;
}

inline BYTE fromhexA( char Char )
{
	if( Char>='0' && Char<='9' )
		return BYTE( Char - _T('0') );
	if( Char>='A' && Char<='F' )
		return BYTE( Char - _T('A') + 10 );
	if( Char>='a' && Char<='f' )
		return BYTE( Char - 'a' + 10 );
	return 0;
}

inline TCHAR tohex( BYTE b )
{
	if( b < 10 )
		return TCHAR( BYTE ( _T('0') + b ) );
	if( b < 16 )
		return TCHAR( BYTE ( _T('A') + b - 10 ) );
	return _T('z');
}

inline int my_atoi( LPCTSTR szStr )
{
	int res = 0;

	while( TRUE )
	{
		const TCHAR ch = *szStr;
		if( ch < _T('0') || ch > _T('9') )
			break;
		res *= 10;
		res += ch - _T('0');
		szStr++;
	}
	return res;
}

#define	MY_ITOA_BUFFER_LENGTH	(11+1)	// for the following function:

inline void my_itoa( int n, LPTSTR szStr, int min_chars = -1 ) // number is padded by zeros!
{
	// tested OK!
	int chars = 1;
	int t;
	LPTSTR szBegin = szStr;

	if( n < 0 )
	{
		szBegin++;
		szStr[0] = _T('-');
		n = -n;
		chars++;
	}

	for( t=n; t>=10; t/=10 )
	{
		chars++;
	}

	ASSERT( chars <= 11 );

	if( min_chars != -1 )
	{
		if( min_chars > chars )
			chars = min_chars;
	}

	t = n;
	for( LPTSTR p = szStr+chars-1; p>=szBegin; p-- )
	{
		*p = TCHAR( _T('0') + t % 10 );
		t /= 10;
	}

	szStr[chars] = _T('\0');

}

inline LPTSTR my_strstr (
		LPCTSTR str1,
		LPCTSTR str2
		)
{
	LPTSTR s1, s2;
	if ( *str2 == _T('\0') )
		return (LPTSTR) str1;

	for( LPTSTR cp = (LPTSTR) str1; *cp != _T('\0') ; cp++ )
	{
		s1 = cp;
		s2 = (LPTSTR) str2;

		while ( *s1 == *s2 &&
				*s1 != _T('\0') &&
				*s2 != _T('\0') )
		{
			s1++;
			s2++;
		}

		if( *s2 == _T('\0') )
			return cp;
	}

	return NULL;
}

inline int my_strcmp( LPCTSTR p1, LPCTSTR p2 )
{
	while( TRUE )
	{
		const TCHAR ch1 = *p1;
		const TCHAR ch2 = *p2;
		p1++;
		p2++;
		if( ch1 != ch2 )
			return ch1<ch2 ? -1 : 1;

		if( ch1 == 0 )
			return 0;
	}
}

inline int my_strncmp( LPCTSTR p1, LPCTSTR p2, int bytes )
{
	for( int i=0; i<bytes; i++ )
	{
		const TCHAR ch1 = *p1;
		const TCHAR ch2 = *p2;
		p1++;
		p2++;
		if( ch1 != ch2 )
			return ch1<ch2 ? -1 : 1;

		if( ch1 == 0 )
			return 0;
	}
	return 0;
}

inline int my_strlen( LPCTSTR p )
{
	int res = 0;

	while(*p)
	{
		p++;
		res++;
	}
	return res;
}

inline void my_strcpy( LPTSTR p1, LPCTSTR p2 )
{
	while(TRUE)
	{
		TCHAR ch = *p2;
		*p1 = ch;

		p2++;
		p1++;

		if( ch == _T('\0') )
		{
			return;
		}
	}
}

inline void my_strcpyA( LPSTR p1, LPCSTR p2 )
{
	while(TRUE)
	{
		CHAR ch = *p2;
		*p1 = ch;

		p2++;
		p1++;

		if( ch == ('\0') )
		{
			return;
		}
	}
}

inline void my_strncpy( LPTSTR p1, LPCTSTR p2, int bytes )
{
	for( int i=0; i<bytes; i++ )
	{
		TCHAR ch = *p2;
		*p1 = ch;

		p2++;
		p1++;

		if( ch == _T('\0') )
		{
			return;
		}
	}

}

inline void my_strcat( LPTSTR p1, LPCTSTR p2 )
{
	while( *p1 != _T('\0') )
	{
		p1++;
	}

	while(TRUE)
	{
		TCHAR ch = *p2;
		*p1 = ch;

		p2++;
		p1++;

		if( ch == _T('\0') )
		{
			return;
		}
	}
}

inline void my_strncat( LPTSTR p1, LPCTSTR p2, int bytes )
{
	while( *p1 != _T('\0') )
	{
		p1++;
	}

	for( int i=0; i<bytes; i++ )
	{
		TCHAR ch = *p2;
		*p1 = ch;

		p2++;
		p1++;

		if( ch == _T('\0') )
		{
			return;
		}
	}

}

inline int my_strcmpi( LPCTSTR p1, LPCTSTR p2 )
{
	while( TRUE )
	{
		TCHAR ch1 = *p1;
		TCHAR ch2 = *p2;
		p1++;
		p2++;
		if( my_isalpha(ch1) && my_isalpha(ch2) )
		{
			if( ch1 >= _T('a') )
				ch1 &= 0x00DF;		// clear bit #5 (32) - to uppercase
			if( ch2 >= _T('a') )
				ch2 &= 0x00DF;		// clear bit #5 (32) - to uppercase
		}

		if( ch1 != ch2 )
			return ch1<ch2 ? -1 : 1;

		if( ch1 == 0 )
			return 0;
	}
}

inline int my_strncmpi( LPCTSTR p1, LPCTSTR p2, int bytes )
{
	for( int i=0; i<bytes; i++ )
	{
		TCHAR ch1 = *p1;
		TCHAR ch2 = *p2;
		p1++;
		p2++;

		if( my_isalpha(ch1) && my_isalpha(ch2) )
		{
			if( ch1 >= _T('a') )
				ch1 &= 0x00DF;		// clear bit #5 (32) - to uppercase
			if( ch2 >= _T('a') )
				ch2 &= 0x00DF;		// clear bit #5 (32) - to uppercase
		}

		if( ch1 != ch2 )
			return ch1<ch2 ? -1 : 1;

		if( ch1 == 0 )
			return 0;
	}

	return 0;
}

inline void * my_memcpy (
        void * dst,
        const void * src,
        size_t count
        )
{
	register char* s = (char*) src;
	register char* d = (char*) dst;
	// copy from lower addresses to higher addresses
	for( register size_t i = 0; i<count; i++ )
	{
		*d = *s;
		d++;
		s++;
	}
	return dst;
}

inline void * my_memmove (
        void * dst,
        const void * src,
        size_t count
        )
{
	if( dst <= src || (char *)dst >= ((char *)src + count) )
	{
		// Non-Overlapping Buffers
		// copy from lower addresses to higher addresses
		register char* s = (char*) src;
		register char* d = (char*) dst;

		for( register size_t i = 0; i<count; i++ )
		{
			*d = *s;
			d++;
			s++;
		}
	}
	else
	{
		// Overlapping Buffers
		// copy from higher addresses to lower addresses
		register char* s = (char*) src + count - 1;
		register char* d = (char*) dst + count - 1;

		for( register size_t i = 0; i<count; i++ )
		{
			*d = *s;
			d--;
			s--;
		}
	}

	return dst;
}

//////////////////////////////////////////////////////////////

#ifdef DRIVER_XXX
typedef HANDLE	HREGKEY;
typedef LPCWSTR	REGROOTTYPE;
typedef WCHAR	REGCHAR;
#define REGTEXT(x) L##x
#define HDEVICE HANDLE //PDEVICE_OBJECT

#else
typedef HKEY	HREGKEY;
typedef HKEY	REGROOTTYPE;
typedef TCHAR	REGCHAR;
#define REGTEXT _T
#define HDEVICE HANDLE

#endif

typedef REGCHAR* LPREGCHAR;
typedef REGCHAR* PREGCHAR;
typedef const REGCHAR* LPCREGCHAR;

//////////////////////////////////////////////////////////////

#ifdef DRIVER_XXX // Kernel mode:

inline PVOID __cdecl my_malloc (size_t iSize)
{
#ifdef DRIVERNAME
	static LPCSTR szDriverName = DRIVERNAME;
#else
	static LPCSTR szDriverName = "MxUn";
#endif
	static ULONG Tag = *((PULONG)szDriverName);
	if( iSize == 0 )
		iSize = 1;
    PVOID result = ExAllocatePoolWithTag( NonPagedPool, iSize, Tag );

    if (result)
	{
        zeromemory(result,iSize);
    }

    return result;
}

inline void __cdecl my_free(void * pVoid)
{
	if( pVoid == NULL )
		return;
    ExFreePool(pVoid);
}

///////////////////////////////

inline LARGE_INTEGER GetCurrentLocalTime()
{
	LARGE_INTEGER st, lt;
	KeQuerySystemTime( &st );
	ExSystemTimeToLocalTime( &st, &lt );
	return lt;
}

///////////////////////////////

inline void	regCloseRegKey( HREGKEY hKey )
{
	NTSTATUS res = ZwClose (hKey);
}

// See comments below this function!

inline HREGKEY regOpenRegKey( HREGKEY hRoot, LPCREGCHAR pszKeyName, DWORD dwOpenRights )
{
	HREGKEY hKey = NULL;

	UNICODE_STRING Name;
	RtlInitUnicodeString (&Name, pszKeyName);

	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes (&ObjectAttributes, &Name,
		OBJ_CASE_INSENSITIVE, hRoot, NULL);

	NTSTATUS res = ZwOpenKey (&hKey, dwOpenRights, &ObjectAttributes);
	if( !NT_SUCCESS(res) )
	{
		TraceDrv(( "Multixp common: ZwOpenKey('%ws', %X) failed: %X\n",
			szRegKey, dwOpenRights, res ));
		return NULL;
	}
	TraceDrv(( "Multixp common: success ZwOpenKey('%ws', %X) = %X\n",
		szRegKey, dwOpenRights, hKey ));

	return hKey;
}

// Following function is very common to the above one.
// There is no difference between them in USER-mode!!!

inline HREGKEY regOpenRegKey( REGROOTTYPE root, LPCREGCHAR pszKeyName, DWORD dwOpenRights )
{
	const MaxRegName = 260;
	LPREGCHAR szRegKey = (LPREGCHAR) my_malloc( MaxRegName * sizeof(REGCHAR) );
	if( szRegKey == NULL )
	{
		return NULL;
	}

	my_strncpy( szRegKey, root, MaxRegName );
	my_strncat( szRegKey, pszKeyName, MaxRegName - _tcslen(szRegKey) - 1 );

	HREGKEY hKey = regOpenRegKey( (HREGKEY)NULL, szRegKey, dwOpenRights );

	my_free( szRegKey );

	return hKey;
}

inline HREGKEY regCreateRegKey( HREGKEY hRoot, LPCREGCHAR pszKeyName,
							   DWORD dwOpenRights, BOOL bVolatile = FALSE )
{
	HREGKEY hKey = NULL;
	
	UNICODE_STRING Name;
	RtlInitUnicodeString (&Name, pszKeyName);

	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes (&ObjectAttributes, &Name,
		OBJ_CASE_INSENSITIVE, hRoot, NULL);

	ULONG CreateOptions = bVolatile ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE;
	NTSTATUS res = ZwCreateKey (&hKey, dwOpenRights, &ObjectAttributes,
						0, NULL, CreateOptions, NULL );
	if( !NT_SUCCESS(res) )
	{
		TraceDrv(( "Multixp common: ZwCreateKey('%ws', %X) failed: %X\n",
			pszKeyName, dwOpenRights, res ));
		return NULL;
	}
	TraceDrv(( "Multixp common: success ZwCreateKey('%ws', %X) = %X\n",
		pszKeyName, dwOpenRights, hKey ));

	return hKey;
}

inline HREGKEY regCreateRegKey( REGROOTTYPE root, LPCREGCHAR pszKeyName,
							   DWORD dwOpenRights, BOOL bVolatile = FALSE )
{
	const MaxRegName = 260;
	LPREGCHAR szRegKey = (LPREGCHAR) my_malloc( MaxRegName * sizeof(REGCHAR) );
	if( szRegKey == NULL )
	{
		return NULL;
	}

	my_strncpy( szRegKey, root, MaxRegName );
	my_strncat( szRegKey, pszKeyName, MaxRegName - _tcslen(szRegKey) - 1 );

	HREGKEY hKey = regCreateRegKey( (HREGKEY)NULL, szRegKey, dwOpenRights, bVolatile );

	my_free( szRegKey );

	return hKey;
}

inline BOOL regInternalDeleteRegKey( HREGKEY hKey, LPCREGCHAR pszKeyName )
{
	NTSTATUS res = ZwDeleteKey( hKey );

	if( !NT_SUCCESS(res) )
	{
		TraceDrv(( "Multixp common: ZwDeleteKey('%ws') failed: %X\n",
			pszKeyName, res ));
		return FALSE;
	}
	TraceDrv(( "Multixp common: success ZwDeleteKey('%ws')\n",
		pszKeyName ));

	return TRUE;
}

inline BOOL regDeleteRegKey( REGROOTTYPE root, LPCREGCHAR pszKeyName )
{
	HREGKEY hRegKey = regOpenRegKey( root, pszKeyName, DELETE );
	if( hRegKey == NULL )
	{
		TraceDrv(( "Multixp common: regOpenRegKey('%ws', %X) failed!\n",
			pszKeyName, DELETE ));
		return FALSE;
	}

	BOOL res = regInternalDeleteRegKey( hRegKey, pszKeyName );

	return res;
}

inline BOOL regDeleteRegKey( HREGKEY hRoot, LPCREGCHAR pszKeyName )
{
	HREGKEY hRegKey = regOpenRegKey( hRoot, pszKeyName, DELETE );
	if( hRegKey == NULL )
	{
		TraceDrv(( "Multixp common: regOpenRegKey('%ws', %X) failed!\n",
			pszKeyName, DELETE ));
		return FALSE;
	}

	BOOL res = regInternalDeleteRegKey( hRegKey, pszKeyName );

	return res;
}

inline BOOL	regQueryRegValue( HREGKEY hKey, LPCREGCHAR pszValueName, LPDWORD pdwType, LPVOID pData, LPDWORD pdwBytes )
{
	DWORD dwOutDataSize = ( pdwBytes ? *pdwBytes : 0 );
	DWORD dwInfoSize = dwOutDataSize + sizeof(KEY_VALUE_PARTIAL_INFORMATION);
	PKEY_VALUE_PARTIAL_INFORMATION pValuePartialInfo = (PKEY_VALUE_PARTIAL_INFORMATION)
		 ExAllocatePool( NonPagedPool, dwInfoSize );
	if( pValuePartialInfo == NULL )
	{
		TraceDrv(( "Multixp common: ExAllocatePool(NonPagedPool, %d) failed\n",
			dwInfoSize ));
		return FALSE;
	}

	UNICODE_STRING Name;
	RtlInitUnicodeString (&Name, pszValueName);
	DWORD dwCopyedBytes = 0;

	pValuePartialInfo->DataLength = dwOutDataSize;
	
	NTSTATUS status = ZwQueryValueKey (hKey, &Name, KeyValuePartialInformation,
		pValuePartialInfo, dwInfoSize, &dwCopyedBytes );

	if (!NT_SUCCESS(status))
	{
		TraceDrv(( "Multixp common: ZwQueryValueKey(%X, '%ws') failed: %X\n",
			hKey, pszValueName, status ));
		ExFreePool( pValuePartialInfo );
		return FALSE;
	}

	if( pdwBytes )
	{
		*pdwBytes = pValuePartialInfo->DataLength;
	}

	if (pdwType)
	{
		*pdwType = pValuePartialInfo->Type;
	}

	if (pData)
	{
		my_memcpy (pData, pValuePartialInfo->Data, pValuePartialInfo->DataLength);
	}


	if( pValuePartialInfo->Type == REG_SZ || pValuePartialInfo->Type == REG_MULTI_SZ )
	{
		TraceDrv(( "Multixp common: success ZwQueryValueKey(%X, '%ws'): '%ws'\n",
			hKey, pszValueName, pValuePartialInfo->Data ));
	}
	else
	{
		TraceDrv(( "Multixp common: success ZwQueryValueKey(%X, '%ws'): %d bytes\n",
			hKey, pszValueName, pValuePartialInfo->DataLength ));
	}

	ExFreePool( pValuePartialInfo );
	return TRUE;
}

inline BOOL	regSetRegValue( HREGKEY hKey, LPCREGCHAR pszValueName, DWORD dwType, LPCVOID pData, DWORD dwBytes )
{
	UNICODE_STRING Name;
	RtlInitUnicodeString (&Name, pszValueName);
	
	NTSTATUS status = ZwSetValueKey( hKey, &Name, 0, dwType, (LPVOID)pData, dwBytes );
	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////

inline HDEVICE OpenDevice(	LPCTSTR szDeviceName,
					BOOL bRead = TRUE,
					BOOL bWrite = TRUE )
{
	ACCESS_MASK mask =	( bRead ?	GENERIC_READ	: 0 ) |
						( bWrite ?	GENERIC_WRITE	: 0 );

	NTSTATUS status;
	HANDLE hDevice = NULL;
	OBJECT_ATTRIBUTES ObjectAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	LARGE_INTEGER AllocationSize;
	UNICODE_STRING Name;

//	DebugPrint (( DRIVERNAME"OpenDevice '%ws'\n",	szDeviceName ));

	RtlInitUnicodeString (&Name, szDeviceName);

	InitializeObjectAttributes(
		&ObjectAttributes, 
		&Name,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
		);

	status = ZwCreateFile(
		&hDevice,
		mask | SYNCHRONIZE,
		&ObjectAttributes,
		&IoStatusBlock,
		&AllocationSize,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN,
		0,
		NULL,
		0
		);

	if (!NT_SUCCESS(status))
	{
//		DebugPrint (( DRIVERNAME"Unable to open device %ws, Status(%08X)\n",
//			szDeviceName, status ));
		hDevice = NULL; //INVALID_HANDLE_VALUE;
	}

	return hDevice;
}

inline BOOL CloseDevice(	HDEVICE hDevice )
{
	ZwClose( hDevice );
	return TRUE;
}

//
// use the Win32 API instead
//     DeviceIoControl
//
EXTERN_C
NTSTATUS
ZwDeviceIoControlFile (
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength
    );

inline BOOL MyDeviceIoControl(
    HDEVICE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesReturned = NULL,
	LPLONG	pLastError = NULL
    )
{
	IO_STATUS_BLOCK	ioStatus = {0,0};

	if( pLastError != NULL )
		*pLastError = ERROR_SUCCESS;

	if( lpBytesReturned != NULL )
		*lpBytesReturned = 0;

	NTSTATUS status =
		ZwDeviceIoControlFile(hDevice, 
			NULL, NULL, NULL, 
			&ioStatus,
			dwIoControlCode,
			lpInBuffer, nInBufferSize, 
			lpOutBuffer,
			nOutBufferSize );

	if( pLastError != NULL )
		*pLastError = status;

	if( lpBytesReturned != NULL )
		*lpBytesReturned = ioStatus.Information;

    if( !NT_SUCCESS(status) )
	{
        return FALSE;
    }

	return TRUE;
}

///////////////////////////////

#endif // ifdef DRIVER_XXX // Kernel mode END

//////////////////////////////////////////////////////////////

#ifndef DRIVER_XXX // User mode:


inline PVOID __cdecl my_malloc (size_t iSize)
{
	if( iSize == 0 )
		iSize = 1;
	HANDLE hHeap = GetProcessHeap();
	PVOID result = HeapAlloc( hHeap, 0, iSize);
	
	if (result) {
		zeromemory(result,iSize);
	}

	return result;
}

inline void __cdecl my_free(void * pVoid)
{
	if( pVoid == NULL )
		return;
	HANDLE hHeap = GetProcessHeap();
	HeapFree( hHeap, 0, pVoid );
}

///////////////////////////////

inline LARGE_INTEGER GetCurrentLocalTime()
{
	SYSTEMTIME st;
	GetLocalTime( &st );
	FILETIME ft;
	SystemTimeToFileTime( &st, &ft );
	LARGE_INTEGER li;
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	return li;
}

///////////////////////////////

inline void	regCloseRegKey( HREGKEY hKey )
{
	LONG res = RegCloseKey( hKey );
	USE_VAR( res );
}

inline HREGKEY regOpenRegKey( REGROOTTYPE root, LPCREGCHAR pszKeyName, DWORD dwOpenRights )
{
	HKEY hKey = NULL;
	LONG nRet = ::RegOpenKeyEx( root, pszKeyName, 0, dwOpenRights, &hKey );
	if (nRet != ERROR_SUCCESS)
	{
		return NULL;
	}
	return hKey;
}

inline HREGKEY regCreateRegKey( REGROOTTYPE root, LPCREGCHAR pszKeyName,
							   DWORD dwOpenRights, BOOL bVolatile = FALSE )
{
//	TraceUser(( _T("regCreateRegKey()\n") ));

	HKEY hKey = NULL;
	DWORD dwDiposition = 0;
	DWORD dwOptions = bVolatile ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE;
	if( bVolatile )
	{
		TraceUser(( _T("Multixp common: Creating volatile reg key: %s\n"),
				pszKeyName ));
	}
	LONG nRet = ::RegCreateKeyEx( root, pszKeyName,
		0, NULL, dwOptions, dwOpenRights, NULL, &hKey, &dwDiposition);
	if (nRet != ERROR_SUCCESS)
	{
		TraceUser(( _T("Multixp common: RegCreateKeyEx(%s) failed with code %d!\n"),
				pszKeyName, nRet ));

		return NULL;
	}
//	TraceUser(( _T("Multixp common: RegCreateKeyEx(%s) succeeded!\n"),
//			pszKeyName ));

	return hKey;
}

inline BOOL regDeleteRegKey( REGROOTTYPE root, LPCREGCHAR pszKeyName )
{
	LONG nRet = ::RegDeleteKey( root, pszKeyName );
	if (nRet != ERROR_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;
}

inline BOOL	regQueryRegValue( HREGKEY hKey, LPCREGCHAR pszValueName, LPDWORD pdwType, LPVOID pData, LPDWORD pdwBytes )
{
	LONG nRet = RegQueryValueEx( hKey, pszValueName, NULL, pdwType, (BYTE*)pData, pdwBytes );
	if (nRet != ERROR_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;
}

inline BOOL	regSetRegValue( HREGKEY hKey, LPCREGCHAR pszValueName, DWORD dwType, LPCVOID pData, DWORD dwBytes )
{
	LONG nRet = RegSetValueEx( hKey, pszValueName, NULL, dwType, (const BYTE*)pData, dwBytes );
	if (nRet != ERROR_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////

inline HDEVICE OpenDevice(	LPCTSTR szDeviceName,
					BOOL bRead = TRUE,
					BOOL bWrite = TRUE )
{
	TCHAR szUserDevName[200] = _T("");
	LPCTSTR szDosDeviceName = NULL;
	LPCTSTR szKernelName = NULL;
	my_strncpy( szUserDevName, szDeviceName, sizeof(szUserDevName)/sizeof(szUserDevName[0]) );

	BOOL res;
	LPCTSTR szKernelDevStart = _T("\\device\\");
	int iKernelPrefixLen = _tcslen(szKernelDevStart);
	BOOL bKernelPrefix = FALSE;

	if( 0 == my_strncmpi( szDeviceName, szKernelDevStart, iKernelPrefixLen ) )
	{
		LPCTSTR szDosPrefix = _T("\\\\.\\");
		bKernelPrefix = TRUE;
		szKernelName = &szDeviceName[iKernelPrefixLen];
		my_strcpy( szUserDevName, szDosPrefix );
		my_strcat( szUserDevName, _T("device_") );
		my_strcat( szUserDevName, szKernelName );
		//_stprintf( szUserDevName, _T("%sdevice_%s"), szDosPrefix, szKernelName );
		szDosDeviceName = &szUserDevName[ _tcslen(szDosPrefix) ];

		res = DefineDosDevice (DDD_RAW_TARGET_PATH,
			szDosDeviceName, szDeviceName );
		if( !res )
		{
			TraceUser(( _T("Multixp common: Can't define dos device %ls from %ls\n\n"),
					szDosDeviceName, szDeviceName ));
			return NULL;
		}
	}

	HDEVICE h = ::CreateFile( szUserDevName, 
		( bRead ? GENERIC_READ : 0 ) | ( bWrite ? GENERIC_WRITE : 0 ),
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, 
		OPEN_EXISTING, 0, NULL );
	
	LONG le = GetLastError();
	USE_VAR( le );

	if( h == INVALID_HANDLE_VALUE )
		h = NULL;

	if( bKernelPrefix )
	{
		res = DefineDosDevice( DDD_REMOVE_DEFINITION, szDosDeviceName, NULL );
		if( !res )
		{
			LONG le2 = GetLastError();
			USE_VAR( le2 );
			TraceUser(( _T("Multixp common: Can't remove dos device %ls (original: %s), error: %d\n\n"),
					szDosDeviceName, szDeviceName, le2 ));
			if( h != NULL )
			{
				res = CloseHandle( h );
			}
			return NULL;
		}
	}

	if( h == NULL )
	{
		TraceUser(( _T("Multixp common: Error opening device '%s': %X\n"),
			szUserDevName, le ));
	}

	return h;
}

inline BOOL CloseDevice(	HDEVICE hDevice )
{
	BOOL res = CloseHandle( hDevice );

	return res;
}

inline BOOL MyDeviceIoControl(
    HDEVICE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesReturned = NULL,
	LPLONG	pLastError = NULL
    )
{
	DWORD dummy = 0;
	BOOL res = DeviceIoControl( hDevice, dwIoControlCode,
		lpInBuffer, nInBufferSize,
		lpOutBuffer, nOutBufferSize,
		(lpBytesReturned != NULL ? lpBytesReturned : &dummy ),
		NULL );

	if( pLastError != NULL )
		*pLastError = GetLastError();

	return res;
}

///////////////////////////////

#endif // ifndef DRIVER_XXX // User mode END

//////////////////////////////////////////////////////////////

// Emulation byte asm ROR operation
inline BYTE ROR(
	BYTE value, 
	BYTE count
	)
{
	count &= 0x7;
	return BYTE( (value >> count) | (value << (8 - count)) );
}

// Emulation byte asm ROL operation
inline BYTE ROL(
	BYTE value, 
	BYTE count
	)
{
	count &= 0x7;
	return BYTE( (value << count) | (value >> (8 - count)) );
}

//////////////////////////////////////////////////////////////

inline BOOL regReadRegString( REGROOTTYPE hRootKey,
		LPCREGCHAR szSubKey, LPCREGCHAR szValueName,
		LPTSTR szValue, DWORD dwMaxSize )
{
	*szValue = _T('\0');
	// open registry
	HREGKEY hKey = regOpenRegKey( hRootKey,	szSubKey, KEY_READ );
	if( hKey == NULL )
	{
		return FALSE;
	}

	DWORD Type = REG_SZ;
	DWORD Size = dwMaxSize * sizeof(TCHAR);
	BOOL bRet = regQueryRegValue( hKey, szValueName, &Type, szValue, &Size );
	if(!bRet)
	{
		regCloseRegKey(hKey);
		return FALSE;
	}
	
	regCloseRegKey( hKey );
	return TRUE;
}

//////////////////////////////////////////////////////////////

inline BOOL regWriteRegString( REGROOTTYPE hRootKey,
		LPCREGCHAR szSubKey, LPCREGCHAR szValueName,
		LPCTSTR szValue, BOOL bCreateVolatile = FALSE )
{
	// open registry
	HREGKEY hKey = regCreateRegKey( hRootKey, szSubKey, KEY_WRITE, bCreateVolatile );
	if( hKey == NULL )
	{
		return FALSE;
	}

	DWORD Type = REG_SZ;
	DWORD Size = ( _tcslen(szValue) + 1 ) * sizeof(TCHAR);
	BOOL bRet = regSetRegValue( hKey, szValueName, Type, szValue, Size );
	if(!bRet)
	{
		regCloseRegKey(hKey);
		return FALSE;
	}
	
	regCloseRegKey( hKey );
	return TRUE;
}

//////////////////////////////////////////////////////////////

inline BOOL regReadRegDword( REGROOTTYPE hRootKey,
		LPCREGCHAR szSubKey, LPCREGCHAR szValueName,
		LPDWORD pdwValue )
{
	*pdwValue = 0;
	// open registry
	HREGKEY hKey = regOpenRegKey( hRootKey,	szSubKey, KEY_READ );
	if( hKey == NULL )
	{
		return FALSE;
	}

	DWORD Type = REG_DWORD;
	DWORD Size = sizeof(DWORD);
	BOOL bRet = regQueryRegValue( hKey, szValueName, &Type, pdwValue, &Size );
	if(!bRet)
	{
		regCloseRegKey(hKey);
		return FALSE;
	}
	
	regCloseRegKey( hKey );
	return TRUE;
}

//////////////////////////////////////////////////////////////

inline BOOL regWriteRegDword( REGROOTTYPE hRootKey,
		LPCREGCHAR szSubKey, LPCREGCHAR szValueName,
		DWORD dwValue, BOOL bCreateVolatile = FALSE )
{
	// open registry
	HREGKEY hKey = regCreateRegKey( hRootKey, szSubKey, KEY_WRITE, bCreateVolatile );
	if( hKey == NULL )
	{
		return FALSE;
	}

	DWORD Type = REG_DWORD;
	DWORD Size = sizeof(dwValue);
	BOOL bRet = regSetRegValue( hKey, szValueName, Type, &dwValue, Size );
	if(!bRet)
	{
		regCloseRegKey(hKey);
		return FALSE;
	}
	
	regCloseRegKey( hKey );
	return TRUE;
}

//////////////////////////////////////////////////////////////

inline BOOL regReadRegBin( REGROOTTYPE hRootKey,
		LPCREGCHAR szSubKey, LPCREGCHAR szValueName,
		LPVOID pValue, LPDWORD pdwMaxSize )
{
	// open registry
	HREGKEY hKey = regOpenRegKey( hRootKey,	szSubKey, KEY_READ );
	if( hKey == NULL )
	{
		return FALSE;
	}

	DWORD Type = REG_BINARY;
	BOOL bRet = regQueryRegValue( hKey, szValueName, &Type, pValue, pdwMaxSize );
	if(!bRet)
	{
		regCloseRegKey(hKey);
		return FALSE;
	}
	
	regCloseRegKey( hKey );
	return TRUE;
}

//////////////////////////////////////////////////////////////

inline BOOL regWriteRegBin( REGROOTTYPE hRootKey,
		LPCREGCHAR szSubKey, LPCREGCHAR szValueName,
		LPCVOID pValue, DWORD dwSize, BOOL bCreateVolatile = FALSE )
{
	// open registry
	HREGKEY hKey = regCreateRegKey( hRootKey, szSubKey, KEY_WRITE, bCreateVolatile );
	if( hKey == NULL )
	{
		return FALSE;
	}

	DWORD Type = REG_BINARY;
	BOOL bRet = regSetRegValue( hKey, szValueName, Type, pValue, dwSize );
	if(!bRet)
	{
		regCloseRegKey(hKey);
		return FALSE;
	}
	
	regCloseRegKey( hKey );
	return TRUE;
}

//////////////////////////////////////////////////////////////

inline BOOL regReadRegString( HREGKEY hKey, LPCREGCHAR szValueName, LPTSTR szValue, DWORD dwMaxSize )
{
	DWORD Type = REG_SZ;
	DWORD Size = dwMaxSize * sizeof(TCHAR);
	*szValue = _T('\0');
	BOOL bRet = regQueryRegValue( hKey, szValueName, &Type, szValue, &Size );
	return bRet;
}

//////////////////////////////////////////////////////////////

inline BOOL regWriteRegString( HREGKEY hKey, LPCREGCHAR szValueName, LPCTSTR szValue )
{
	DWORD Type = REG_SZ;
	DWORD Size = ( _tcslen(szValue) + 1 ) * sizeof(TCHAR);
	BOOL bRet = regSetRegValue( hKey, szValueName, Type, szValue, Size );
	return bRet;
}

//////////////////////////////////////////////////////////////

inline BOOL regReadRegDword( HREGKEY hKey, LPCREGCHAR szValueName, DWORD* pdwOpen )
{
	*pdwOpen = 0;
	DWORD Type = REG_DWORD;
	DWORD Size = sizeof(DWORD);
	BOOL bRet = regQueryRegValue(hKey, szValueName, &Type, pdwOpen, &Size);
	return bRet;
}

//////////////////////////////////////////////////////////////

inline BOOL regWriteRegDword( HREGKEY hKey, LPCREGCHAR szValueName, DWORD dwOpen )
{
	DWORD Type = REG_DWORD;
	DWORD Size = sizeof(DWORD);
	BOOL bRet = regSetRegValue(hKey, szValueName, Type, &dwOpen, Size);
	return bRet;
}

///////////////////////////////

inline TCHAR GetWindowsDiskSymbol()
{
	REGCHAR szSystemRoot[MAX_PATH] = _T("Z");
	regReadRegString( REG_HKLM,
		_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
		_T("SystemRoot"), szSystemRoot, sizeof(szSystemRoot)/sizeof(*szSystemRoot) );

	return szSystemRoot[0];
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


#endif //ifndef _COMMONAPI_H_UID0000003D193F4A71
