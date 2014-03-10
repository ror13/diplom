// LogHelper.cpp

#include "stdafx.h"
#include "LogHelper.h"

//////////////////////////////////////////////////////////////
TCHAR* GetExeFolder() // With trailing '\'
{
	static TCHAR szExe[MAX_PATH] = _T("");
	GetModuleFileName( NULL, szExe, MAX_PATH );
	LPTSTR p = _tcsrchr( szExe, _T('\\') );
	if( p != NULL )
	{
		p[1] = _T('\0');
	}
	
	return szExe;
}

//////////////////////////////////////////////////////////////
void  WriteLogToFile ( TCHAR* strMessage, LPCTSTR szFileSuffix )
{
	TCHAR* strFolder = GetExeFolder();
	FILE* f = NULL;
	TCHAR strLogName[MAX_PATH];
	TCHAR strCrLf[] = _T("\r\n");
	
	_stprintf(strLogName, 
		_T("%sCommon%s.log"),
		strFolder, szFileSuffix );
	
	while( NULL == ( f = _tfopen( strLogName, _T("ab+") ) ) )
	{
		Sleep(0);
	}
	
	// 	USES_CONVERSION;
	// 	LPCSTR p = T2CA( (LPCTSTR)strMessage );
	
    /* Use strftime to build a customized time string. */
    TCHAR tmpbuf[128];
    time_t ltime;
    struct tm *today;
	
	time( &ltime );
	today = localtime( &ltime );
	
    _tcsftime( tmpbuf, sizeof(tmpbuf),
		_T("%Y.%m.%d %H:%M.%S "), today );
	fwrite( (void*)tmpbuf, _tcslen(tmpbuf), sizeof(TCHAR), f );
	
	_stprintf(tmpbuf, _T("process Id %.8X (%d) "), GetCurrentProcessId(), GetCurrentProcessId());
	fwrite( (void*)tmpbuf, _tcslen(tmpbuf), sizeof(TCHAR), f );
	
	
	fwrite( (void*)strMessage, _tcslen(strMessage), sizeof(TCHAR), f );
	fwrite( (void*)strCrLf, _tcslen(strCrLf), sizeof(TCHAR), f );
	fclose(f);
	f = NULL;
}
