// http_request.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "http_request.h"
#include "http_srv_res.h"
#include "fileoperation.h"

CHTTPRequest::CHTTPRequest( void )
{
	m_bDone = FALSE;
	m_bExecutable = 0;
	m_cbBody = 0;
	m_dwAttr = 0;
	m_uStatus = 0;
	m_cbSent = 0;
	m_timeReq = CTime::GetCurrentTime();
	m_hFile = INVALID_HANDLE_VALUE;
	m_buf.SetSize( 1024 );
	m_cbOut = 0;
}

CHTTPRequest::~CHTTPRequest()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
}

CString CHTTPRequest::GetHeaderValue( CString strName )
{
	CString strValue;
	strName.MakeLower();
	m_mapHeaders.Lookup( strName, strValue );
	return strValue;
}

BOOL CHTTPRequest::CheckExt( const CString& strExt, CString& strAvail, DWORD dwType )
{
	BOOL bMatch = FALSE;
	CString strPossible;
	// loop through all possible exts....
	while( !strAvail.IsEmpty() )
	{
		int ndx = strAvail.ReverseFind('\n');
		if ( ndx == -1 )
		{
			strPossible = strAvail;
			strAvail.Empty();
		}
		else
		{
			strPossible = strAvail.Mid( ndx+1 );
			strAvail = strAvail.Left( ndx );
		}
		if ( strExt.CompareNoCase( strPossible ) == 0 )
		{
			bMatch = TRUE;
			break;
		}
	}
	return bMatch;
}


APP_TYPE CHTTPRequest::GetSvrAppType( void )
{
	int ndx = m_strFullPath.ReverseFind( '.' );
	if ( ndx != -1 )
	{
		CString strExt = m_strFullPath.Mid( ndx+1 );
		CString strAvail;
		// check if CGI app....
		strAvail.LoadString( IDS_APP_CGI );
		if(CheckExt( strExt, strAvail, APP_TYPE_CGI ))
			return APP_TYPE_CGI;

		strAvail.LoadString( IDS_APP_ISAPI );
		if(CheckExt( strExt, strAvail, APP_TYPE_ISAPI ))
			return APP_TYPE_ISAPI;

		strAvail.LoadString( IDS_APP_LUA );
		if(CheckExt( strExt, strAvail, APP_TYPE_LUA ))
			return APP_TYPE_LUA;
	}

	return APP_TYPE_NONE;
}

int CHTTPRequest::StuffString( const CString& strData )
{
	CUTF8String strUTF8(strData);
	
	int nLen = strlen(strUTF8);
	// make sure there's enough room....
	if( m_cbOut + nLen > m_buf.GetSize() )
	{
		int nChunks = nLen / 1024 + 1;
		m_buf.SetSize( m_cbOut + nChunks * 1024 );
	}
	// copy the data....
	MoveMemory(m_buf.GetData() + m_cbOut, strUTF8, nLen);
	
	m_cbOut += nLen;
	
	// return amount of space left....
	return (m_buf.GetSize() - m_cbOut);
}

int CHTTPRequest::StuffString( UINT uId )
{
	CString str;
	str.LoadString( uId );
	return StuffString( str );
}

int CHTTPRequest::StuffStatus( const CString& strStatus )
{
	CString strVer = _T("HTTP/1.0 ");
	StuffString( strVer );
	StuffString( strStatus );
	StuffString( CRLF );

	// stuff the server name....
	CString strServer;
	if ( strServer.LoadString( IDS_SERVER_NAME ) && !strServer.IsEmpty() )
		StuffHeader( _T("Server"), strServer );

	// stuff the date....
	return StuffHeader( _T("Date"), GetHttpDate() );
}

int CHTTPRequest::StuffStatus( UINT uStatus )
{
	CString strStatus;
	strStatus.LoadString( uStatus );
	// save the status for this request....
	m_uStatus = uStatus;
	// stuff the HTTP status line....
	return StuffStatus( strStatus );
}

int CHTTPRequest::StuffError( UINT uMsg )
{
	StuffStatus( uMsg );
	
	CString strStatus;
	strStatus.LoadString( uMsg );

	StuffHeader( "Content-length",  strStatus.GetLength());
	StuffString( CRLF );
	
	return StuffString(strStatus);
}

int CHTTPRequest::StuffHeader( CString strName, CString strValue )
{
	StuffString( strName );
	StuffString( ": " );
	StuffString( strValue );
	return StuffString( CRLF );
}

int CHTTPRequest::StuffHeader( CString strName, int nValue )
{
	CString strValue;
	StuffString( strName );
	StuffString( ": " );
	strValue.Format( _T("%d"), nValue );
	StuffString( strValue );
	return StuffString( CRLF );
}

void CHTTPRequest::StuffFileType( void )
{
	// get the extension....
	CString strExt = m_strFullPath.Mid(
		m_strFullPath.ReverseFind(_T('.')) );
	// find it in the registry....
	HKEY hKey = NULL;
	if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, strExt,
		0, KEY_READ, &hKey ) == ERROR_SUCCESS )
	{
		DWORD dwSize = 0;
		// see how long the data is....
		if ( RegQueryValueEx( hKey, _T("Content Type"), NULL, NULL,
			NULL, &dwSize ) == ERROR_SUCCESS )
		{
			CString strType;
			LONG lRet = RegQueryValueEx( hKey, _T("Content Type"), NULL, NULL,
				(LPBYTE)strType.GetBuffer( dwSize ), &dwSize );
			strType.ReleaseBuffer();
			if ( lRet == ERROR_SUCCESS )
				StuffHeader( _T("Content-type"), strType );
		}
		RegCloseKey( hKey );
	}
}

CString CHTTPRequest::GetHttpDate( LPFILETIME pft )
{
	SYSTEMTIME st;
	if ( pft )
		FileTimeToSystemTime( pft, &st );
	else
		GetSystemTime( &st );

	CTime timeHttp( st );
	return timeHttp.Format( IDS_HTTPTIME );
}

BOOL CHTTPRequest::IfModSince( const CTime& timeIfMod )
{
	// assume it has been modified....
	BOOL bOk = TRUE;
	FILETIME ft;
	if ( GetFileTime( m_hFile, NULL, NULL, &ft ) )
	{
		SYSTEMTIME st;
		if ( FileTimeToSystemTime( &ft, &st ) )
		{
			CTime timeFile( st );
			if ( timeFile <= timeIfMod )
				bOk = FALSE;
		}
	}
	return bOk;
}
