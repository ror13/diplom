// http_reqsock.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "http_reqsock.h"
#include "http_srv_res.h"
#include "fileoperation.h"

SECURITY_ATTRIBUTES g_sa = {sizeof(SECURITY_ATTRIBUTES),NULL,TRUE};

CHTTPRequestSocket::CHTTPRequestSocket( void )
{
}

CHTTPRequestSocket::CHTTPRequestSocket(CHTTPData* pDoc)
{
#ifdef IMPL_CGI
	m_pThread = NULL;
	m_pCancel = NULL;
#endif // IMPL_CGI
	m_bKilled = FALSE;
	m_nRefs = 1;
	m_reqStatus = REQ_REQUEST;
	m_pDoc = pDoc;

	// new request....
	m_bKeepOpen = m_bWantKeepOpen = FALSE;
}

CHTTPRequestSocket::~CHTTPRequestSocket( void )
{
	// JIC....
#ifdef IMPL_CGI
	if ( m_pCancel )
	{
		if ( m_pThread )
		{
			DWORD dwCode;
			// signal a cancel if still running....
			if ( ::GetExitCodeThread( m_pThread->m_hThread, &dwCode )
				&& dwCode == STILL_ACTIVE )
			{
				// signal a cancel....
				m_pCancel->SetEvent();
				// wait for the thread to die....
				WaitForSingleObject( m_pThread->m_hThread, INFINITE );
			}
			// kill the object...
			delete m_pThread;
		}
		delete m_pCancel;
	}
#endif // IMPL_CGI

	// release our hold on the request object....
	m_Request.m_bDone = TRUE;
}

void CHTTPRequestSocket::OnSockRecv(char *pBuf, int Size)
{
	int ndx = 0;
	switch ( m_reqStatus )
	{
	case REQ_REQUEST:
	case REQ_HEADER:
		while( GetLine( pBuf, Size, ndx ) == TRUE )
		{
			if ( !m_strLine.IsEmpty() )
				ProcessLine();
			else
			{
				m_reqStatus = REQ_BODY;
				break;
			}
		}
		// break if we're not looking for the body....
		if ( m_reqStatus != REQ_BODY )
			break;

		// stop if no body sent....
		if ( !IsBodySent() )
		{
			m_reqStatus = REQ_DONE;
			break;
		}
		// else fall through....
	case REQ_BODY:
		AddToBody(pBuf, Size, ndx );
		break;
	}
	if ( m_reqStatus == REQ_DONE )
	{
		m_Request.m_buf.SetSize(0);
		if( !StartResponse() )
			SelectWrite();
	}
}

void CHTTPRequestSocket::OnSockSend()
{
// 	PBYTE Buf = m_buf.GetData();
// 	SaveBinFileFromBuf(_T("TEST.txt"), Buf, m_cbOut);
	BOOL bRes = SendSock((LPSTR)m_Request.m_buf.GetData(), m_Request.m_cbOut);
	if (!bRes)
	{
		if(m_Error != WSAEWOULDBLOCK )
		{
			SetStopLink();
			m_bKilled = TRUE;
		}
		else
			SelectWrite();
	}
	else
	{
		// adjust the bytes-sent value for the request....
		m_Request.m_cbSent += m_Request.m_cbOut;
		// if we're not done with the file....
		if( m_Request.m_hFile != INVALID_HANDLE_VALUE )
		{
			DWORD dwRead = 0;
			// read next chunk....
			ReadFile( m_Request.m_hFile, m_Request.m_buf.GetData(),
				m_Request.m_buf.GetSize(), &dwRead, NULL );
			if( dwRead > 0 )
				m_Request.m_cbOut = dwRead;
			else
			{
				// no more data to send....
				CloseHandle( m_Request.m_hFile );
				m_Request.m_hFile = INVALID_HANDLE_VALUE;
			}
		}
		// see if we need to keep going....
		if( m_Request.m_hFile != INVALID_HANDLE_VALUE )
			SelectWrite();
		else
		{
			// eh, we're outta here....
			SetStopLink();
		}
	}
}

void CHTTPRequestSocket::OnSockClose()
{
	m_bKilled = TRUE;
}

BOOL CHTTPRequestSocket::GetLine( const char* bytes, int nBytes, int& ndx )
{
	BOOL bLine = FALSE;
	while ( bLine == FALSE && ndx < nBytes )
	{
		char ch = (char)bytes[ndx];
		switch( ch )
		{
		case '\r': // ignore
			break;
		case '\n': // end-of-line
			bLine = TRUE;
			break;
		default:   // other....
			m_strLine += ch;
			break;
		}
		++ndx;
	}
	return bLine;
}

void CHTTPRequestSocket::ProcessLine( void )
{
	int ndx;
	switch( m_reqStatus )
	{
	case REQ_REQUEST:
		ndx = m_strLine.Find( ' ' );
		if ( ndx != -1 )
		{
			m_Request.m_strMethod = m_strLine.Left( ndx );
			m_strLine = m_strLine.Mid( ndx+1 );
			m_strLine.TrimLeft();
			ndx = m_strLine.Find( ' ' );
			if ( ndx == -1 )
			{
				m_Request.m_strURL = m_strLine;
				m_Request.m_strURL.TrimRight();
				m_reqStatus = REQ_SIMPLE;
			}
			else
			{
				m_Request.m_strURL = m_strLine.Left( ndx );
				m_Request.m_strVersion = m_strLine.Mid( ndx+1 );
				m_Request.m_strVersion.TrimLeft();
			}
			// check for execution arguments....
			ndx = m_Request.m_strURL.Find( '?' );
			if ( ndx != -1 )
			{
				// yup; save the args....
				m_Request.m_strArgs = m_Request.m_strURL.Mid( ndx+1 );
				int ind_eq;
				int ind_end = 0;
				CString strArgs = Decode(m_Request.m_strArgs, TRUE);
				while(ind_end >= 0 && (ind_eq = strArgs.Find(_T('='), ind_end)) >= 0)
				{
					CString strName = strArgs.Mid(ind_end, ind_eq - ind_end);
					CString strValue;

					ind_end = strArgs.Find(_T('&'), ind_eq);
					if(ind_end >= 0)
					{
						strValue = strArgs.Mid(ind_eq + 1, ind_end - ind_eq-1);
						ind_end++;
					} else
						strValue = strArgs.Mid(ind_eq + 1);

					m_Request.m_listGets.push_back(std::pair<CString, CString>(strName, strValue));
				}
				// strip from file name....
				m_Request.m_strURL = m_Request.m_strURL.Left( ndx );
				m_Request.m_bExecutable = TRUE;
			}

			// change any "%xx"s to the appropriate char....
			m_Request.m_strURL = Decode( m_Request.m_strURL );
		}
		m_reqStatus = REQ_HEADER;
		break;
	case REQ_HEADER:
		ndx = m_strLine.Find( ':' );
		if ( ndx != -1 )
		{
			CString strName = m_strLine.Left( ndx );
			CString strValue = m_strLine.Mid( ndx+1 );
			strName.MakeLower();
			strValue.TrimLeft();
			m_Request.m_mapHeaders.SetAt( strName, strValue );
		}
		break;
	};
	m_strLine.Empty();
}

BOOL CHTTPRequestSocket::IsBodySent( void )
{
	BOOL bSent = FALSE;
	CString strValue = m_Request.GetHeaderValue( "Content-Length" );
	if ( !strValue.IsEmpty() )
	{
		m_Request.m_cbBody = _ttoi( strValue );
		m_Request.m_cbBodyRecv = 0;
		bSent = TRUE;
	}
	return bSent;
}

void CHTTPRequestSocket::AddToBody( const char* bytes, int nBytes, int ndx )
{
	// save the buffer size....

	char *data = NULL;

	data = new char[nBytes - ndx + 1];
	memcpy(data, &bytes[ndx], nBytes - ndx);
	data[nBytes - ndx] = 0;

	int nOldSize = m_Request.m_buf.GetSize();
	// get rid of old stuff; append to body data....
	m_Request.m_buf.SetSize( nBytes - ndx );
	memcpy(m_Request.m_buf.GetData(), &bytes[ndx], nBytes - ndx);
	m_Request.m_strBody = m_Request.m_strBody + CString(data);
	delete[] data;

	m_Request.m_baBody.Append( m_Request.m_buf );
	// restore the buffer size....
	m_Request.m_buf.SetSize( nOldSize );
	// see if we're done....
	if ( m_Request.m_baBody.GetSize() >= m_Request.m_cbBody )
//	if (m_Request.m_strBody.GetLength() == m_Request.m_cbBody)
	{
		m_Request.m_baBody.SetSize( m_Request.m_cbBody );
		if (m_Request.m_strMethod == "POST")
		{
			m_Request.m_strArgs = m_Request.m_strBody;
			m_Request.m_strArgs.TrimRight();
			int ind_eq;
			int ind_end = 0;
			CString strArgs = Decode(m_Request.m_strArgs, TRUE);
			while(ind_end >= 0 && (ind_eq = strArgs.Find(_T('='), ind_end)) >= 0)
			{
				CString strName = strArgs.Mid(ind_end, ind_eq - ind_end);
				CString strValue;

				ind_end = strArgs.Find(_T('&'), ind_eq);
				if(ind_end >= 0)
				{
					strValue = strArgs.Mid(ind_eq + 1, ind_end - ind_eq-1);
					ind_end++;
				} else
					strValue = strArgs.Mid(ind_eq + 1);
				m_Request.m_listPosts.push_back(std::pair<CString, CString>(strName, strValue));
			}
			m_Request.m_bExecutable = TRUE;
		}
		m_reqStatus = REQ_DONE;
	}
}

BOOL CHTTPRequestSocket::StartResponse( void )
{
	BOOL bWait = FALSE;
	// save the host address....
	m_Request.m_strHost = GetPeerAddr();
	// switch on the method....
	if ( m_Request.m_cbBody == 0 &&
		m_Request.m_strMethod.CompareNoCase( _T("GET") ) == 0 )
	{
		FindTarget();
		if( m_Request.m_uStatus == 0 )
		{
			if ( m_Request.GetSvrAppType() == APP_TYPE_NONE)
			{
				if ( StuffHeading() )
					StartTargetStuff();
			} else
				bWait = StartSvrApp();
		}
	}
	else if ( m_Request.m_cbBody == 0 && m_reqStatus == REQ_DONE &&
		m_Request.m_strMethod.CompareNoCase( _T("HEAD") ) == 0 )
	{
		FindTarget();
		if( m_Request.m_uStatus == 0 )
		{
			if ( m_Request.m_bExecutable )
				bWait=StartSvrApp();
			else
			{
				StuffHeading();
				// we don't send the file for HEAD reqs....
				if ( m_Request.m_hFile != INVALID_HANDLE_VALUE)
				{
					CloseHandle( m_Request.m_hFile );
					m_Request.m_hFile = INVALID_HANDLE_VALUE;
				}
			}
		}
	}
	else if ( m_Request.m_cbBody > 0 && m_reqStatus == REQ_DONE &&
		m_Request.m_strMethod.CompareNoCase( _T("POST") ) == 0 )
	{
		// assume an executable....
		m_Request.m_bExecutable = TRUE;
		FindTarget();
		if ( m_Request.m_uStatus == 0 )
		{
			bWait=StartSvrApp();
		}
	}
	else
		m_Request.StuffError( IDS_STATUS_NOTIMPL );

	return bWait;
}

BOOL CHTTPRequestSocket::FindTarget()
{
	CString strFile = m_Request.m_strURL;
	// change from URL to local file system path....
	if ( !URLtoPath( strFile ) )
	{
		m_Request.StuffError( IDS_STATUS_BADREQUEST );
		return FALSE;
	}

	m_Request.m_dwAttr = GetFileAttributes( strFile );
	if ( m_Request.m_dwAttr != -1 )
	{
		// strip any trailing SEPCHAR....
		if ( strFile.GetAt( strFile.GetLength()-1) == SEPCHAR )
			m_Request.m_strFullPath = strFile.Left( strFile.GetLength()-1 );
		else
			m_Request.m_strFullPath = strFile;

		// if it's a folder, see if we can redirect to
		// on of the default docs or apps....
		if ( m_Request.m_dwAttr & FILE_ATTRIBUTE_DIRECTORY )
		{
			// check for existence of a default doc or app....
			if( CheckDefault( IDS_DEFAULTDOC, FALSE ) )
				return TRUE;
			if( CheckDefault( IDS_DEFAULTAPP, TRUE ) )
				return TRUE;
		}

		if(m_Request.m_bExecutable && m_Request.GetSvrAppType() == APP_TYPE_NONE)
		{
			m_Request.StuffError( IDS_STATUS_BADREQUEST );
			return FALSE;
		}

		return TRUE;
	} 

	m_Request.StuffError( IDS_STATUS_NOTFOUND );
	
	return FALSE;
}

BOOL CHTTPRequestSocket::URLtoPath( CString& strFile )
{
	BOOL bLegal = FALSE;
	CString& strRoot = m_pDoc->m_strRoot;

	// start with the root, append the abs path....
	CString strTemp = strRoot + strFile;
	// now canonicalize it....
	DWORD dwSize = GetFullPathName( strTemp, MAX_PATH, strFile.GetBuffer(MAX_PATH+1), NULL );
	strFile.ReleaseBuffer();

	// get the full path okay?
	if ( dwSize )
	{
		int cchRoot = strRoot.GetLength();
		int cchFile = strFile.GetLength();
		// must be the same or longer than the root....
		if ( cchRoot == cchFile )
		{
			// must be exactly the same....
			if ( strRoot.Compare( strFile ) == 0 )
				bLegal = TRUE;
		}
		else if ( cchRoot < cchFile )
		{
			// must have the root as the base....
			if ( strRoot.Compare( strFile.Left(cchRoot) ) == 0
				&& strFile.GetAt( cchRoot ) == SEPCHAR )
				bLegal = TRUE;
		}
	}

	return bLegal;
}

BOOL CHTTPRequestSocket::PathToURL( CString& strFile )
{
	int ndx;
	// a local reference to the root....
	CString& strRoot = m_pDoc->m_strRoot;
	// change all SEPCHARs to forward slashes....
	while ( (ndx=strFile.Find( SEPCHAR )) != -1 )
		strFile = strFile.Left( ndx ) + '/' + strFile.Mid(ndx+1);
	// now add the prefix and server, and cut the root....
	CString strPort;
	UINT uPort = m_pDoc->m_uPort;
	if ( uPort != PORT_HTTP )
		strPort.Format( _T(":%d"), uPort );

	strFile = CString("http://")
		+ m_pDoc->m_strServer
		+ strPort
		+ strFile.Mid(strRoot.GetLength());

	return TRUE;
}

BOOL CHTTPRequestSocket::StuffHeading( void )
{
	BOOL bContinue = FALSE;
	if ( m_Request.m_dwAttr & FILE_ATTRIBUTE_HIDDEN )
	{
		// never show hidden files....
		m_Request.StuffError( IDS_STATUS_FORBIDDEN );
	}
	else if ( m_Request.m_dwAttr & FILE_ATTRIBUTE_DIRECTORY )
	{
		if ( m_pDoc->m_bAllowListing )
		{
			// create a directory listing....
			m_Request.StuffStatus( IDS_STATUS_OK );
			m_Request.StuffString( CRLF );
			bContinue = TRUE;
		}
		else
			m_Request.StuffError( IDS_STATUS_FORBIDDEN );
	}
	else if ( m_Request.m_hFile != INVALID_HANDLE_VALUE )
	{
		// cgi's output file will be opened already....
		CString strStatus, strHeaders;
		// loop until we find a blank line....
		DWORD dwRead = 0;
		CByteArray baFile;
		baFile.SetSize( 1024 );
		// read next chunk....
		BOOL bRead = ReadFile( m_Request.m_hFile, baFile.GetData(),
			baFile.GetSize(), &dwRead, NULL );
		while ( dwRead > 0 )
		{
			int ndx = 0;
			while( GetLine( (char*)baFile.GetData(), dwRead, ndx ) == TRUE )
			{
				BOOL bSave = TRUE;
				// stuff any non-empty lines.....
				if ( m_strLine.IsEmpty() )
				{
					// we found our empty line;
					// back up to where we left off....
					DWORD dwPos = SetFilePointer( m_Request.m_hFile,
						ndx - dwRead,
						NULL, FILE_CURRENT );

					// and we're off....
					bContinue = TRUE;
					break;
				}
				else
				{
					int nPos = m_strLine.Find( ':' );
					if ( nPos != -1 )
					{
						CString strName = m_strLine.Left( nPos );
						strName.TrimLeft();
						strName.TrimRight();
						CString strVal  = m_strLine.Mid( nPos+1 );
						strVal.TrimLeft();
						strVal.TrimRight();
						if ( strName.CompareNoCase(_T("Status")) == 0 )
						{
							strStatus = strVal;
							bSave = FALSE;
						}
						else if ( strName.CompareNoCase(_T("Location")) == 0 )
						{
							strStatus.LoadString( IDS_STATUS_MOVEDTEMP );
						}
					}
				}

				// save the header (if we want to)....
				if ( bSave )
					strHeaders += m_strLine + CRLF;

				m_strLine.Empty();
			}
			// read next chunk if we're not done....
			if ( bContinue )
				break;
			else
				ReadFile( m_Request.m_hFile, baFile.GetData(),
					baFile.GetSize(), &dwRead, NULL );
		}
		if ( strStatus.IsEmpty() )
			m_Request.StuffStatus( IDS_STATUS_OK );
		else
			m_Request.StuffStatus( strStatus );

		// stuff the headers....
		m_Request.StuffString( strHeaders );
		// stuff the blank line....
		m_Request.StuffString( CRLF );
	}
	else
	{
		// open the file....
		m_Request.m_hFile = CreateFile( m_Request.m_strFullPath,
			GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL );
		if ( m_Request.m_hFile != INVALID_HANDLE_VALUE )
		{
			if ( m_reqStatus != REQ_SIMPLE )
			{
				CTime timeIfMod;
				CString strIfMod = m_Request.GetHeaderValue( "If-Modified-Since" );
				if ( strIfMod.GetLength() > 0 &&
					FromHttpTime( strIfMod, timeIfMod ) &&
					!m_Request.IfModSince( timeIfMod ) )
				{
					// eh, it hasn't been modified....
					m_Request.StuffStatus( IDS_STATUS_NOTMODIFIED );
					// don't need it anymore....
					CloseHandle( m_Request.m_hFile );
					m_Request.m_hFile = INVALID_HANDLE_VALUE;
				}
				else
				{
					// send it off....
					m_Request.StuffStatus( IDS_STATUS_OK );
					// any other header info....
					m_Request.StuffFileType();
					m_Request.StuffHeader( "Content-length", GetFileSize( m_Request.m_hFile, NULL ) );
					// get the last modified time....
					FILETIME ft;
					if ( GetFileTime( m_Request.m_hFile, NULL, NULL, &ft ) )
					{
						m_Request.StuffHeader( "Last-Modified", m_Request.GetHttpDate( &ft ) );
					}
					bContinue = TRUE;
				}
				// blank line....
				m_Request.StuffString( CRLF );
			}
			else
				bContinue = TRUE;
		}
		else
		{
			// couldn't open; try again later....
			m_Request.StuffError( IDS_STATUS_SVCUNAVAIL );
		}
	}
	return bContinue;
}

void CHTTPRequestSocket::StartTargetStuff( void )
{
	if ( m_Request.m_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwRead = 0;
		// read the first chunk....
		ReadFile( m_Request.m_hFile, m_Request.m_buf.GetData() + m_Request.m_cbOut,
			m_Request.m_buf.GetSize()-m_Request.m_cbOut, &dwRead, NULL );
		if ( dwRead > 0 )
			m_Request.m_cbOut += dwRead;
		else
		{
			// nothing read.... close the file....
			CloseHandle( m_Request.m_hFile );
			m_Request.m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	else if ( m_Request.m_dwAttr & FILE_ATTRIBUTE_DIRECTORY )
		StuffListing();
	else
		m_Request.StuffString( CRLF );
}

void CHTTPRequestSocket::StuffListing( void )
{
	BOOL bRoot = FALSE;
	BOOL bIcons = m_pDoc->m_bListIcon;
	CString strIcon;
	CString strLine = CString("http://")
		+ m_pDoc->m_strServer
		+ m_Request.m_strURL;
	CString strDir = m_Request.m_strURL;
	CString strMask = m_Request.m_strFullPath;

	// make sure URL ends in a slash....
	if ( strDir.GetAt( strDir.GetLength()-1 ) != '/' )
		strDir += '/';
	// is this the server's root folder?
	else if ( strDir.Compare( _T("/") ) == 0 )
		bRoot = TRUE;

	// create the file search mask....
	AddFile( strMask, IDS_DIRMASK );
	m_Request.StuffString( IDS_CONTENTS_PRE );
	m_Request.StuffString( strLine );
	m_Request.StuffString( IDS_CONTENTS_POST );
	if ( bRoot )
		m_Request.StuffString( IDS_CONTENTS_DESC );

	if ( bIcons )
		strIcon.LoadString( IDS_ICON_BLANK );
	strLine.Format( IDS_CONTENTS_HEADING, strIcon );
	m_Request.StuffString( strLine );

	int nFiles = 0;

	WIN32_FIND_DATA fd;
	// find the first file that matches the mask....
	HANDLE fh = FindFirstFile( strMask, &fd );
	if ( fh != INVALID_HANDLE_VALUE )
	{
		// create a line for the found file....
		nFiles += StuffListingFile( &fd, strDir, bIcons );
		// loop through all other files....
		while ( FindNextFile( fh, &fd ) )
			nFiles += StuffListingFile( &fd, strDir, bIcons );
	}

	if ( nFiles == 0 )
		m_Request.StuffString( IDS_CONTENTS_EMPTY );

	m_Request.StuffString( IDS_CONTENTS_FOOTER );
	// only add the parent link if there is one....
	if ( !bRoot )
	{
		if ( bIcons )
			strIcon.LoadString( IDS_ICON_PARENT );
		strLine.Format( IDS_CONTENTS_PARENT, strIcon );
		m_Request.StuffString( strLine );
	}
	// add the note and end it....
	m_Request.StuffString( IDS_CONTENTS_NOTE );
	m_Request.StuffString( CRLF );
}

int CHTTPRequestSocket::StuffListingFile( WIN32_FIND_DATA* pfd, const CString& strDir, BOOL bIcons )
{
	int nFile = 0;
	// don't include '.', '..' or hidden files....
	if ( lstrcmp( pfd->cFileName, _T(".") ) && lstrcmp( pfd->cFileName, _T("..") )
		&& (pfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 )
	{
		CString strSize, strIcon = "";
		CString strLine, strFile = pfd->cFileName;
		CTime timeFile( pfd->ftLastWriteTime );
		BOOL bFolder = ((pfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
		if ( bIcons && bFolder )
			strIcon.LoadString( IDS_ICON_FOLDER );
		else if ( bIcons )
			strIcon.LoadString( IDS_ICON_FILE );

		// create the link string....
		CString strLink = strDir + strFile;
		// make sure spaces are replaced with '%20'...
		int ndx;
		while ( (ndx=strLink.Find(' ')) != -1 )
			strLink = strLink.Left(ndx) + "%20" + strLink.Mid( ndx+1 );

		// format the size string....
		if ( bFolder )
			strSize = "  Folder";
		else if ( pfd->nFileSizeHigh > 0 )
			strSize = _T("   &gt; 4GB"); // yeah, right.
		else if ( pfd->nFileSizeLow < 1024 )
			strSize = _T("    &lt; 1K");
		else
			strSize.Format( _T("%7dK"), pfd->nFileSizeLow/1024 );

		strLine.Format( IDS_CONTENTS_FORMAT,
			timeFile.Format( IDS_FILETIMEFMT ),
			strSize, strLink, strIcon, strFile );
		m_Request.StuffString( strLine );
		nFile = 1;
	}
	return nFile;
}

BOOL CHTTPRequestSocket::StartSvrApp( void )
{
	if(m_Request.GetSvrAppType() == APP_TYPE_LUA)
	{
		if(CGIOpenOutFile())
		{
			m_pDoc->HttpClientStartSvrApp(&m_Request);
			m_Request.m_cbOut = 0;
			CGIDone();
		}
		return FALSE;
	}
#ifdef IMPL_CGI
	else if ( m_Request.GetSvrAppType() == APP_TYPE_CGI )
	{
		if(CGIOpenOutFile())
			return CGIStart();
		else
			return FALSE;
	} else
	{
		m_Request.StuffError( IDS_STATUS_NOTIMPL );
		return FALSE;
	}
#else //  IMPL_CGI
	m_Request.StuffError( IDS_STATUS_NOTIMPL );
	return FALSE;
#endif // IMPL_CGI
}

BOOL CHTTPRequestSocket::CGIOpenOutFile()
{
	// get the temp path...
	CString strTempPath;
	GetTempPath( MAX_PATH, strTempPath.GetBuffer(MAX_PATH) );
	strTempPath.ReleaseBuffer();
	// create a temporary file for the output....
	CString strTempName;
	GetTempFileName( strTempPath, _T("CGI"), 0, strTempName.GetBuffer(MAX_PATH) );
	strTempName.ReleaseBuffer();
	m_Request.m_hFile = CreateFile( strTempName, GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE, &g_sa,
		CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE, NULL );
	if(m_Request.m_hFile == INVALID_HANDLE_VALUE)
	{
		m_Request.StuffError( IDS_STATUS_SVRERROR );
		return FALSE;
	}
	return TRUE;
}

#ifdef IMPL_CGI
BOOL CHTTPRequestSocket::CGIStart( void )
{
	BOOL bOk = FALSE;
	if ( m_Request.m_hFile != INVALID_HANDLE_VALUE )
	{
		// create the cancel event....
		m_pCancel = new CEvent;
		if ( m_pCancel )
		{
			// make sure the event is reset....
			m_pCancel->ResetEvent();
			// create the CGI thread suspended....
			m_pThread = AfxBeginThread( (AFX_THREADPROC)CGIThread,
				(LPVOID)this, THREAD_PRIORITY_NORMAL, 0,
				CREATE_SUSPENDED, NULL );
			if ( m_pThread )
			{
				// don't self-destruct (we must delete)....
				m_pThread->m_bAutoDelete = FALSE;
				// resume...
				m_pThread->ResumeThread();
				bOk = TRUE;
			}
		}
	}

	if ( bOk == FALSE )
	{
		m_Request.StuffError( IDS_STATUS_SVRERROR );
		if( m_Request.m_hFile != INVALID_HANDLE_VALUE )
		{ // JIC....
			CloseHandle( m_Request.m_hFile );
			m_Request.m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	return bOk;
}

void AddEnvVar( CString& strEnv, CString strName, CString strVal )
{
	// add the name=val pair to the env in alphabetical order....
	strEnv += strName + '=' + strVal + '\a';
}

UINT CGIThread( LPVOID pvParam )
{
	CHTTPRequestSocket* pReqSock = (CHTTPRequestSocket*)pvParam;
	CRequest* pRequest = &pReqSock->m_Request;
	BOOL bOk = FALSE;
	DWORD dwErr;
	HANDLE hWritePipe, hReadPipe;
	// create a pipe we'll use for STDIN....
	if ( CreatePipe( &hReadPipe, &hWritePipe, &g_sa, 0 ) )
	{
		// get the command line together....
		CString strCmdLine = pRequest->m_strFullPath
			+ ' '
			+ Decode( pRequest->m_strArgs, TRUE );
		// get the directory....
		CString strDir = pRequest->m_strFullPath;
		int ndx = strDir.ReverseFind( SEPCHAR );
		// assume we found it....
		strDir = strDir.Left( ndx+1 );

		// create an environment for the CGI process....
		DWORD dwCreateFlags = 0;
#ifdef UNICODE
		dwCreateFlags = CREATE_UNICODE_ENVIRONMENT;
#endif // UNICODE
		CEnvironment cEnv;

		CString strValue;
		strValue.LoadString( IDS_SERVER_NAME );
		cEnv.Add( "SERVER_SOFTWARE", strValue );
		cEnv.Add( "SERVER_NAME", pReqSock->m_pDoc->m_strServer );
		cEnv.Add( "GATEWAY_INTERFACE", "CGI/1.1" );
		cEnv.Add( "SERVER_PROTOCOL", "HTTP/1.0" );
		strValue.Format( "%d", pReqSock->m_pDoc->m_uPort );
		cEnv.Add( "SERVER_PORT", strValue );

		cEnv.Add( "REQUEST_METHOD", pRequest->m_strMethod );
		cEnv.Add( "SCRIPT_NAME", pRequest->m_strURL );
		cEnv.Add( "QUERY_STRING", pRequest->m_strArgs );
		cEnv.Add( "REMOTE_ADDR", pRequest->m_strHost );
		if ( pRequest->m_cbBody > 0 )
		{
			cEnv.Add( "CONTENT_LENGTH", pRequest->GetHeaderValue("Content-Length") );
			cEnv.Add( "CONTENT_TYPE", pRequest->GetHeaderValue("Content-Type") );
		}
		if ( !pRequest->m_strPathInfo.IsEmpty() )
		{
			cEnv.Add( "PATH_INFO", pRequest->m_strPathInfo );
			cEnv.Add( "PATH_TRANSLATED", pRequest->m_strPathTranslated );
		}

		// all the passed headers prefixed with "HTTP_"....
		POSITION pos = pRequest->m_mapHeaders.GetStartPosition();
		while ( pos != NULL )
		{
			// get the name/value pair....
			CString strName, strValue;
			pRequest->m_mapHeaders.GetNextAssoc( pos, strName, strValue );
			HeaderToEnvVar( strName );
			// set the environment variable....
			cEnv.Add( strName, strValue );
		}

		// create the process....
		LPVOID pEnv = (LPVOID)cEnv.GetBlock();
		PROCESS_INFORMATION pi;
		STARTUPINFO si = {0};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdInput = hReadPipe;
		si.hStdOutput = pReqSock->m_hFile;
		si.hStdError = pReqSock->m_hFile;
		bOk = CreateProcess( NULL, strCmdLine.GetBuffer(1),
			NULL, NULL, TRUE,
			dwCreateFlags, pEnv,
			strDir, &si, &pi );
		strCmdLine.ReleaseBuffer();
		// if created....
		if ( bOk )
		{
			// release our hold on the thread....
			CloseHandle( pi.hThread );
			// send the body of the post to the stdin....
			if ( pRequest->m_cbBody > 0 )
			{
				DWORD dwWritten = 0;
				WriteFile( hWritePipe, pRequest->m_baBody.GetData(),
					pRequest->m_cbBody, &dwWritten, NULL );
			}
			// wait for either cancel or process done....
			HANDLE aHandles[2];
			aHandles[0] = pi.hProcess;
			aHandles[1] = pReqSock->m_pCancel->m_hObject;
			if ( WaitForMultipleObjects( 2, aHandles, FALSE, INFINITE ) == WAIT_OBJECT_0 )
			{
				// process finished; notify main thread....
				AfxGetApp()->m_pMainWnd->PostMessage( WSM_CGIDONE, 0, (LPARAM)pReqSock );
			}
			else
			{
				// canceled or some other error....
				bOk = FALSE;
			}
			// close our hold on it....
			CloseHandle( pi.hProcess );
		}
		else
			dwErr = GetLastError();

		// close the stdin pipe....
		CloseHandle( hWritePipe );
		CloseHandle( hReadPipe );
		delete pEnv;
	}
	if ( bOk == FALSE && pReqSock->m_hFile != INVALID_HANDLE_VALUE )
	{ // JIC....
		CloseHandle( pReqSock->m_hFile );
		pReqSock->m_hFile = INVALID_HANDLE_VALUE;
	}

	return (bOk?0:1);
}
#endif // IMPL_CGI

void CHTTPRequestSocket::CGIDone( void )
{
	if ( !m_bKilled )
	{
		// flush the temp file's buffers....
		BOOL bSucceed = FlushFileBuffers( m_Request.m_hFile );
		// go to start of file....
		DWORD dwPos = SetFilePointer( m_Request.m_hFile, 0, NULL, FILE_BEGIN );
		// output the header....
		StuffHeading();
		
		if ( m_Request.m_strMethod.CompareNoCase( _T("HEAD") ) == 0 )
		{
			CloseHandle( m_Request.m_hFile );
			m_Request.m_hFile = INVALID_HANDLE_VALUE;
		} else
			StartTargetStuff();

		SelectWrite();
	}
	else
	{
		CloseHandle( m_Request.m_hFile );
		m_Request.m_hFile = INVALID_HANDLE_VALUE;
	}
}

void HeaderToEnvVar( CString& strVar )
{
	int ndx;
	// make upper case, change '-' to '_', and prefix....
	strVar.MakeUpper();
	while( (ndx = strVar.Find('-')) != -1 )
		strVar = strVar.Left(ndx) + '_' + strVar.Mid(ndx+1);
	strVar = "HTTP_" + strVar;
}

static int IntVal( CString strVal )
{
	int nVal = 0;
	strVal.TrimLeft();

	CUTF8String lzVal(strVal);

	int nSize = strlen(lzVal);
	for( int ndx = 0; ndx < nSize; ++ndx )
		nVal = nVal*10 + lzVal[ndx] - '0';
	
	return nVal;
}

static int MonthFromStr( const CString& str )
{
	LPCTSTR aMonths[] = {
		_T("xxx"), _T("jan"), _T("feb"), _T("mar"), _T("apr"), _T("may"), _T("jun"),
		_T("jul"), _T("aug"), _T("sep"), _T("oct"), _T("nov"), _T("dec") };
	for( int nMonth=1; nMonth <= 12; ++nMonth )
	{
		if ( str.CompareNoCase( aMonths[nMonth] ) == 0 )
			break;
	}

	return nMonth;
}

// Dow, dd Mon year hh:mm:ss GMT
BOOL CHTTPRequestSocket::FromHttpTime( const CString& strHttp, CTime& timeHttp )
{
	// assume we couldn't get a good time conversion....
	BOOL bOk = FALSE;
	SYSTEMTIME st = {0};
	int ndx;
	switch( strHttp.GetAt(3) )
	{
	case ',':
		// read RFC-1123 (preferred)....
		st.wDay = IntVal( strHttp.Mid(5,2) );
		st.wMonth = MonthFromStr( strHttp.Mid(8,3) );
		st.wYear = IntVal( strHttp.Mid(12,4) );
		st.wHour = IntVal( strHttp.Mid(17,2) );
		st.wMinute = IntVal( strHttp.Mid(20,2) );
		st.wSecond = IntVal( strHttp.Mid(23,2) );
		break;
	case ' ':
		// read ANSI-C time format....
		st.wDay = IntVal( strHttp.Mid(8,2) );
		st.wMonth = MonthFromStr( strHttp.Mid(4,3) );
		st.wYear = IntVal( strHttp.Mid(20,4) );
		st.wHour = IntVal( strHttp.Mid(11,2) );
		st.wMinute = IntVal( strHttp.Mid(14,2) );
		st.wSecond = IntVal( strHttp.Mid(17,2) );
		break;
	default:
		if ( (ndx = strHttp.Find( _T(", ") )) != -1 )
		{
			st.wDay = IntVal( strHttp.Mid(ndx+2,2) );
			st.wMonth = MonthFromStr( strHttp.Mid(ndx+5,3) );
			st.wYear = IntVal( strHttp.Mid(ndx+9,2) );
			st.wHour = IntVal( strHttp.Mid(ndx+12,2) );
			st.wMinute = IntVal( strHttp.Mid(ndx+15,2) );
			st.wSecond = IntVal( strHttp.Mid(ndx+18,2) );
			// add the correct century....
			st.wYear += (st.wYear > 50)?1900:2000;
		}
		break;
	}
	// if year not zero, we pulled the info out of the string....
	if ( st.wYear != 0 )
	{
		// assume GMT....
		CTime strTime( st );
		// check to see if the minutes are the same....
		if ( strTime.GetMinute() == st.wMinute )
		{
			// assume it worked....
			timeHttp = strTime;
			bOk = TRUE;
		}
	}
	return bOk;
}

CString CHTTPRequestSocket::StripLast( CString& strPath )
{
	CString strExtra;
	if ( !strPath.IsEmpty() )
	{
		int ndx = strPath.ReverseFind( SEPCHAR );
		if ( ndx < 0 )
			ndx = 0;
		strExtra = strPath.Mid( ndx );
		strPath = strPath.Left( ndx );
	}
	return strExtra;
}

BOOL CHTTPRequestSocket::CheckDefault( UINT uList, BOOL bExecute )
{
	BOOL bFound = FALSE;
	DWORD dwAttr;
	CString strDefault, strDefList;
	strDefList.LoadString( uList );
	while ( !strDefList.IsEmpty() )
	{
		int ndx;
		strDefault = m_Request.m_strFullPath;
		if ( (ndx=strDefList.Find('\n')) == -1 )
		{
			AddFile( strDefault, strDefList );
			strDefList.Empty();
		}
		else
		{
			AddFile( strDefault, strDefList.Left(ndx) );
			strDefList = strDefList.Mid( ndx+1 );
		}
		if ( (dwAttr=GetFileAttributes(strDefault)) != -1 &&
			(dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			bFound = TRUE;
			break;
		}
	}
	if ( bFound )
	{
		m_Request.m_strFullPath = strDefault;
		m_Request.m_bExecutable = bExecute;
		m_Request.m_dwAttr = dwAttr;
	}
	return bFound;
}

void AddFile( CString& strPath, UINT uStr )
{
	CString strFile;
	strFile.LoadString( uStr );
	AddFile( strPath, strFile );
}

void AddFile( CString& strPath, const CString& strFile )
{
	if ( strPath.GetAt( strPath.GetLength()-1 ) != SEPCHAR )
		strPath += SEPCHAR;
	strPath += strFile;
}

CString Decode( const CString& str, BOOL bQuery )
{
	int ndx;
	CUTF8String strSource(str);
	int len = strlen(strSource);

	// special processing or query strings....
	if ( bQuery )
	{
		// change all '+' to ' '....
		int ch = '+';
		LPSTR pCh = strSource;
		while(pCh = strchr(pCh, ch))
			*(pCh++) = ' ';
	}
	
	// first see if there are any %s to decode....
	int ch = '%';
	if ( strchr(strSource, ch ) )
	{
		// iterate through the string, changing %dd to special char....
		for( ndx=0; ndx < len; ndx++ )
		{
			if ( strSource[ndx] == '%' )
			{
				if ( strSource[ndx + 1] == '%' )
				{
					// wanna keep one percent sign....
					memmove(&strSource[ndx], &strSource[ndx + 1], len - (ndx+1));
					len -= 1;
					strSource[len] = '\0';
				}
				else
				{
					BYTE c1 = strSource[ndx+1] >= 'A' ? strSource[ndx+1] - 'A' + 10 : strSource[ndx+1] - '0';
					BYTE c2 = strSource[ndx+2] >= 'A' ? strSource[ndx+2] - 'A' + 10 : strSource[ndx+2] - '0';

					// replace the escape sequence with the char....
					strSource[ndx] = (char)(c1*16 + c2);
					memmove(&strSource[ndx + 1], &strSource[ndx + 3], len - (ndx+3));
					len -= 2;
					strSource[len] = '\0';
				}
			}
		}
	}
	CString strDest = UTF8ToString(strSource);
	
	return strDest;
}
