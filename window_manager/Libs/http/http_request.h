// http_request.h
//////////////////////////////////////////////////////////////////////////

#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <list>

#define CRLF		"\x0d\x0a"

enum APP_TYPE {
	APP_TYPE_NONE,
	APP_TYPE_CGI,
	APP_TYPE_ISAPI,
	APP_TYPE_LUA
};

class CHTTPRequest
{
public:
	// Construction....
	CHTTPRequest();
	~CHTTPRequest();

	// Attributes....
	CString             m_strPathTranslated;
	CString             m_strPathInfo;
	CString             m_strHost; // host's address
	CString             m_strMethod; // GET, HEAD or POST
	CString             m_strURL;
	CString             m_strVersion; // HTTP/1.0
	CMapStringToString  m_mapHeaders;
	
	std::list< std::pair<CString,CString> > m_listGets;
	std::list< std::pair<CString,CString> > m_listPosts;

	CByteArray          m_baBody;
	CString             m_strBody;
	int                 m_cbBody;
	int                 m_cbBodyRecv;
	CString             m_strFullPath;
	DWORD               m_dwAttr;
	DWORD               m_bExecutable;
	CString             m_strArgs; // string after '?'
	UINT                m_uStatus;
	BOOL                m_bDone;
	UINT                m_cbSent;
	CTime               m_timeReq; // time of request
	HANDLE				m_hFile;
	CByteArray			m_buf;
	int					m_cbOut;

	// access routines....
	CString GetHeaderValue( CString strName );

	BOOL CheckExt(const CString& strExt, CString& strAvail, DWORD dwType);
	APP_TYPE GetSvrAppType();

	int StuffStatus( const CString& strStatus );
	int StuffString( const CString& );
	int StuffString( UINT uId );
	int StuffStatus( UINT uMsg );
	int StuffError( UINT uMsg );
	int StuffHeader( CString strName, CString strValue );
	int StuffHeader( CString strName, int nValue );

	void StuffFileType( void );

	BOOL IfModSince( const CTime& timeIfMod );
	CString GetHttpDate( LPFILETIME pft = NULL );
};

#endif //_HTTP_REQUEST_H_
