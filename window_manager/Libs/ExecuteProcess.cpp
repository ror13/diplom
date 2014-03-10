//////////////////////////////////////////////////////////////
// File:		// ExecuteProcess.cpp
// File time:	// 10.08.2004	18:47
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"

#include "CommonApi.h"

#include "ApplicaInfo.h"

#include <Psapi.h>
#include <tlhelp32.h>
#pragma comment (lib, "psapi.lib")

#include "ExecuteProcess.h"

#include "LogHelper.h"


CString GetProcessInfo( HANDLE hProcess )
{
	const DWORD id = MyGetProcessId(hProcess);
	CString s;
	TCHAR szExe[MAX_PATH] = _T("");

	GetModuleFileNameEx( hProcess, NULL, szExe, MAX_PATH );
	s.Format( _T("handle: %X, id: 0x%X (%d), image: \"%s\""),
		hProcess, id, id, szExe );
	return s;
}

/////////////////////////////////////////////////////////////////////////////

CString ProcessURLPrefixes( const CString& url)
{
	CString arg, argEmpty;
	argEmpty = _T("");

	CString urlTrim = url;

	urlTrim.TrimLeft();
	urlTrim.TrimRight();


	if( urlTrim.GetLength() < 2 )
		return argEmpty;
		
	CString prefix;
	BOOL bPrefix = FALSE;
	BOOL bProtocolPrefix = FALSE;
	
	for( int i=0; i<urlTrim.GetLength(); i++ )
	{
		TCHAR ch = urlTrim[i];
		
	
		if( ch == _T('.') )
		{
			bPrefix = TRUE;
			break;
		}
		if(
			( ch >= _T('a') && ch <= _T('z') ) || // exactly
			( ch >= _T('A') && ch <= _T('Z') ) || // exactly
			( ch >= _T('0') && ch <= _T('9') ) || // exactly
			ch == _T('_') || ch == _T('-') || ch == _T('!') || // '_' and '-' exactly, other - just in case
			ch == _T('@') || ch == _T('$') || ch == _T('#') || // just in case
			ch == _T('%') || ch == _T('<') || ch == _T('>') || // just in case
			ch == _T('(') || ch == _T(')') || ch == _T(',') || // just in case
			FALSE )
		{
			prefix += ch;
			continue;
		}
		else
		{
			if( ch == _T(':') )
			{
				if( urlTrim.Mid( i+1, 2 ) == _T("//") ||
					urlTrim.Mid( i+1, 2 ) == _T("\\\\") )
				{
					TRACE(_T("Protocol http have set in urlTrim"));
					bProtocolPrefix = TRUE;
				}
			}
			break;
		}
	}

	TRACE (_T("Prefix = %s \n"), prefix);
	if( bProtocolPrefix )
	{
		return urlTrim;
	}
	else if (bPrefix)
	{   
		const CString strRegPrefixurlTrim = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\urlTrim\\Prefixes");
		TCHAR szPrefixValue[200];
		BOOL bUserPrefix = FALSE;
		BOOL bMachinePrefix = FALSE;
		CString ret;

		BOOL res;

		res = regReadRegString( REG_HKCU, strRegPrefixurlTrim, prefix,
				szPrefixValue,
				sizeof(szPrefixValue)/sizeof(*szPrefixValue) );
		if( !res )
		{
			TRACE(_T("No register key"));
			res = regReadRegString( REG_HKLM, strRegPrefixurlTrim, prefix,
					szPrefixValue,
					sizeof(szPrefixValue)/sizeof(*szPrefixValue) );
			if (!res)
			{
				_tcscpy( szPrefixValue, _T("http://") );
			}
		}
		ret.Format( _T("%s%s"), szPrefixValue, urlTrim );
		return ret;
	}
	
	return argEmpty;
}

/////////////////////////////////////////////////////////////////////////////

BOOL Execute(
			 IN		LPCTSTR	pszCommandLine2,
			 IN		LPCTSTR	pszOpenVerb,
			 IN		BOOL	bDisableUI,
			 IN		CWnd*	pParent,
			 IN		int		nShowWindow,
			 IN		BOOL	bURL,
			 OUT	PHANDLE	pProcessHandle
			 )
{
	const CString strCommandLine = pszCommandLine2;
	CString name, arg;

	if( !bURL )
	{
		int pos;
		if( strCommandLine.GetLength()>2 && strCommandLine[0]==_T('\"') ) // name is in quotas
		{
			pos = strCommandLine.Find( _T('\"'), 1 );
			if( pos != -1 )
			{
				name	= strCommandLine.Left( pos+1 );
				arg		= strCommandLine.Mid( pos+1 );
			}
			else
				name	= strCommandLine;
		}
		else // name is not in quotas
		{
			pos = strCommandLine.Find( _T(' '), 0 );
			if( pos != -1 )
			{
				name	= strCommandLine.Left( pos );
				arg		= strCommandLine.Mid( pos );
			}
			else
				name	= strCommandLine;
		}
		arg.TrimLeft();
		arg.TrimRight();
	}
	else // URL:
	{
		arg = ProcessURLPrefixes(strCommandLine);
		name = PRESENTER_BROWSER;
	}

	TRACE( _T("Execute: \ncommand:  '%s'\nargument: '%s'\n"), name, arg );

	SHELLEXECUTEINFO shei;
	ZeroMemory( &shei, sizeof(shei) );
	shei.cbSize = sizeof( shei );
	shei.fMask =
		SEE_MASK_FLAG_DDEWAIT | // wait for possible DDE conversation
		( pProcessHandle != NULL ? SEE_MASK_NOCLOSEPROCESS : 0 ) |
		( bDisableUI ? SEE_MASK_FLAG_NO_UI : 0 );
	shei.hwnd = ( pParent ? pParent->m_hWnd : HWND_DESKTOP );
	shei.lpVerb = pszOpenVerb; //NULL; // _T("open");
	shei.lpFile = name;
	shei.lpParameters = arg.IsEmpty() ? NULL : (LPCTSTR)arg;
	shei.nShow = nShowWindow; //SW_SHOW;
	shei.hProcess = NULL;

	SetLastError( ERROR_SUCCESS );

	BOOL	bShellRes = ShellExecuteEx( &shei );

	const LONG lShellLastError = GetLastError();

	if( pProcessHandle != NULL )
	{
		*pProcessHandle = shei.hProcess;
	}

	if( !bShellRes || shei.hProcess == NULL || shei.hProcess == INVALID_HANDLE_VALUE
		//|| lShellLastError != ERROR_SUCCESS
		|| ((DWORD)shei.hInstApp) <= 32
		)
	{
		TRACE( _T("ShellExecuteEx: command: '%s', argument: '%s' -> %s code=%d %s"),
			name, arg,
			(bShellRes ? _T("OK") : _T("Error") ),
			shei.hInstApp,
			GetProcessInfo( shei.hProcess ) );

		HWND hwnd = ( pParent ? pParent->m_hWnd : HWND_DESKTOP );
		TRACE( _T("Can't ShellExecuteEx (err=%d, 0x%X): %s \"%s\", disableUI=%d, parentHwnd=%X"),
			lShellLastError, lShellLastError, pszOpenVerb,
			strCommandLine, bDisableUI, hwnd );
	}

	return bShellRes;
}

//////////////////////////////////////////////////////////////

BOOL StartProgram( LPCTSTR pszCommandLine, int nShowWindow )
{
	CString strCommandLine = pszCommandLine;
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = nShowWindow;

	SetLastError( ERROR_SUCCESS );
	BOOL res = CreateProcess( NULL, strCommandLine.GetBuffer(strCommandLine.GetLength()+1),
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
	LONG err = GetLastError();
	strCommandLine.ReleaseBuffer();

	if( !res || err != ERROR_SUCCESS )
	{
		TRACE( _T("Can't start process (err=%d, 0x%X): \"%s\", %s, %s"),
			err, err, strCommandLine,
			(res ? _T("OK") : _T("Error") ),
			GetProcessInfo( pi.hProcess ) );
	}

	if( res )
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	return res;
}

//////////////////////////////////////////////////////////////

HANDLE StartProgram( LPCTSTR pszCommandLine)
{
	CString strCommandLine = pszCommandLine;

	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );

	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags = 0;
	si.wShowWindow = 0;
	
	SetLastError( ERROR_SUCCESS );
	
	BOOL res = CreateProcess( NULL, (LPTSTR)(LPCTSTR)strCommandLine,
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );

	LONG err = GetLastError();
	
	if( !res || err != ERROR_SUCCESS )
	{
		TRACE( _T("Can't start process (err=%d, 0x%X): \"%s\", %s, %s"),
			err, err, strCommandLine,
			(res ? _T("OK") : _T("Error") ),
			GetProcessInfo( pi.hProcess ) );
	}
	
	if( res )
	{
		CloseHandle(pi.hThread);
		return pi.hProcess;
	} else
		return INVALID_HANDLE_VALUE;
}

BOOL SendToProcessThreads( DWORD dwOwnerPID ) 
{ 
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32; 
	
	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hThreadSnap == INVALID_HANDLE_VALUE ) 
		return( FALSE ); 
	
	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32 ); 
	
	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if( !Thread32First( hThreadSnap, &te32 ) ) 
	{
		//printError( TEXT("Thread32First") );  // Show cause of failure
		CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
		return( FALSE );
	}
	
	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do 
	{ 
		if( te32.th32OwnerProcessID == dwOwnerPID )
		{
			PostThreadMessage(
				te32.th32ThreadID,	// thread identifier
				WM_QUIT,			// message
				0,  // first message parameter
				0   // second message parameter
				);

// 			_tprintf( TEXT("\n     THREAD ID      = 0x%08X"), te32.th32ThreadID ); 
// 			_tprintf( TEXT("\n     base priority  = %d"), te32.tpBasePri ); 
// 			_tprintf( TEXT("\n     delta priority = %d"), te32.tpDeltaPri ); 

		}
	} while( Thread32Next(hThreadSnap, &te32 ) );
	
	//_tprintf( TEXT("\n"));
	
	//  Don't forget to clean up the snapshot object.
	CloseHandle( hThreadSnap );
	return( TRUE );
}


BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam )
{
	DWORD dwID ;
	
	GetWindowThreadProcessId(hwnd, &dwID) ;
	
	if(dwID == (DWORD)lParam)
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0) ;
	}
	
	return TRUE ;
}

DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout )
{
	HANDLE   hProc ;
	DWORD   dwRet ;
	
	// If we can't open the process with PROCESS_TERMINATE rights,
	// then we give up immediately.
	hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,
		dwPID);
	
	if(hProc == NULL)
	{
		return TA_FAILED ;
	}
	
	// TerminateAppEnum() posts WM_CLOSE to all windows whose PID
	// matches your process's.
	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM) dwPID) ;
	
	// Wait on the handle. If it signals, great. If it times out,
	// then you kill it.
	if(WaitForSingleObject(hProc, dwTimeout)!=WAIT_OBJECT_0)
		dwRet=(TerminateProcess(hProc,0)?TA_SUCCESS_KILL:TA_FAILED);
	else
		dwRet = TA_SUCCESS_CLEAN ;
	
	CloseHandle(hProc) ;
	
	return dwRet ;
}

//////////////////////////////////////////////////////////////
BOOL SafeTerminateProcess(
					  HANDLE hProcess, // handle to the process
					  UINT uExitCode   // exit code for the process
					  )
{
	DWORD ProcessId = GetProcessId(hProcess);

	//SendToProcessThreads(ProcessId);

	DWORD   dwRet;
	dwRet = TerminateApp(ProcessId, 1000);

	TCHAR tmpbuf[128];
	_stprintf(tmpbuf, _T("SafeTerminateProcess Id %.8X (%d) TerminateApp %d"), ProcessId, ProcessId, dwRet);

	WriteLogToFile(tmpbuf, _T("ExecProcess"));

	return TRUE;

// 	BOOL fOk;
// 	
// 	PROCESSENTRY32 pe = { sizeof(pe) };
// 	
// 	HANDLE hSnapshot= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0 );
// 	
// 	if(hSnapshot == INVALID_HANDLE_VALUE)
// 		return FALSE;
// 	
// 	int iCount = 0;
// 	for ( fOk = Process32First(hSnapshot,&pe); fOk ; fOk = Process32Next(hSnapshot,&pe) )
// 	{
// 		iCount++;
// 
// 		if (ProcessId == pe.th32ProcessID)
// 		{
// 			SendToProcessThreads(ProcessId);
// 		}
// 	}
// 	
// 	CloseHandle(hSnapshot);
// 	

/*	
	LRESULT SendMessage(
		HWND hWnd,      // handle to destination window
		UINT Msg,       // message
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);

	BOOL PostThreadMessage(
		DWORD idThread, // thread identifier
		UINT Msg,       // message
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);
*/
}

//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CScheduleArrayTest

#ifdef _DEBUG

class CUtilTest: public CppUnit::TestFixture
{
public:
	void setUp()
	{
	}
	
	void testProcessURLPrefixes()
	{	
		CString name, arg;

		name = "applica.com";
		arg = ProcessURLPrefixes(name);
		CPPUNIT_ASSERT(arg == "http://applica.com");
		name = "mail.ru";
		arg = ProcessURLPrefixes(name);
		CPPUNIT_ASSERT(arg == "http://mail.ru");
		name = " google.com   ";
		arg = ProcessURLPrefixes(name);
		CPPUNIT_ASSERT(arg == "http://google.com");
		name = "go ogle.com   ";
		arg = ProcessURLPrefixes(name);
		CPPUNIT_ASSERT(arg == "");

	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CUtilTest);
	CPPUNIT_TEST(testProcessURLPrefixes);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CUtilTest);

#endif // _DEBUG
