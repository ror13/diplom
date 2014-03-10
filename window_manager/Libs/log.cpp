// log.cpp
#include "StdAfx.h"

#include "FileOperation.h"


void  WriteLogToFile ( const CString& strMessage, LPCTSTR szFileSuffix )
{
	const CString strFolder = GetExeFolder();
	FILE* f = NULL;
	CString strLogName;

	strLogName.Format( _T("%sCommon%s.log"),
		strFolder, szFileSuffix );

	while( NULL == ( f = _tfopen( strLogName, _T("ab+") ) ) )
	{
		Sleep(0);
	}

	USES_CONVERSION;
	LPCSTR p = T2CA( (LPCTSTR)strMessage );
	fwrite( p, strlen(p), sizeof(p[1]), f );
	fclose(f);
	f = NULL;
}

//////////////////////////////////////////////////////////////

void  WriteLogToTrace ( const CString& strMessage )
{
	OutputDebugString( strMessage );
}

//////////////////////////////////////////////////////////////

void NormalizeNewLines( CString& strMessage )
{
	strMessage.Replace( _T("\r\n"), _T("\n") );
	strMessage.Replace( _T('\r'), _T('\n') );
	int l = strMessage.GetLength();
	if( strMessage[l-1] == '\n' )
		strMessage.Delete( l-1 );
	strMessage.Replace( _T("\n"), _T("\r\n") );
	strMessage += _T("\r\n");
}

//////////////////////////////////////////////////////////////

void  WritePresenterLog ( LPCTSTR szMessage, LPCTSTR szFileSuffix = _T("") )
{
	LARGE_INTEGER tick;
	QueryPerformanceCounter( &tick );

	CString s = szMessage;

	CString strName = _T("Client");
/*	switch( mode )
	{
		case MODE_AGENT:
			strName =
				//_T("Client on ") + 
				g_info.GetCurrentStationName();
			break;

		case STATION_MANAGER:
			strName = _T("Station manager");
			break;

		case TASK_MANAGER:
			strName = _T("Task manager");
			break;

		case SPECIAL_ACTION:
			strName = _T("Special action");
			break;

		default:
			strName = _T("UNKNOWN!!!");
			TRACE( _T("ERROR: Unknown station mode!\n") );
	}
*/
	CString d;
	d.Format( _T("%-16s %08X%08X> "), 
		strName,
		tick.HighPart, tick.LowPart );

	s = d + s;

	NormalizeNewLines( s );

#ifdef _DEBUG
	WriteLogToTrace( _T("DEBUG") + CString(szFileSuffix) + _T("> ") + s );
#endif
	WriteLogToFile( s, szFileSuffix );
}

//////////////////////////////////////////////////////////////

void  WriteLog ( LPCTSTR f, ... )
{
	CString s;
	{
		va_list marker;
		va_start( marker, f );
		s.FormatV( f, marker );
		va_end( marker );
	}

	WritePresenterLog( s );
}

//////////////////////////////////////////////////////////////

void  WriteDevLog ( LPCTSTR szFileSuffix, LPCTSTR f, ... )
{
	CString s;
	{
		va_list marker;
		va_start( marker, f );
		s.FormatV( f, marker );
		va_end( marker );
	}

	WritePresenterLog( s, szFileSuffix );
}
