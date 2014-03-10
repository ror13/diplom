// http_reqsock.h
//////////////////////////////////////////////////////////////////////////

#ifndef _TTP_REQSOCK_H_
#define _TTP_REQSOCK_H_

#include "http_data.h"
#include "http_request.h"

#include "NetSock.h"

#define SEPCHAR     '\\'
#define PORT_HTTP	80

class CHTTPRequestSocket : public CNetSock
{
	enum REQSTATUS
	{
		REQ_REQUEST=0, REQ_HEADER, REQ_BODY, REQ_SIMPLE, REQ_DONE
	};
public:
	CHTTPRequestSocket();
	CHTTPRequestSocket(CHTTPData* pDoc);
	~CHTTPRequestSocket();

	BOOL        m_bKilled;
	
protected:
	CHTTPData*	m_pDoc;
	BOOL CheckExt( const CString& strExt, CString& strAvail, DWORD dwType );
	BOOL CheckDefault( UINT uList, BOOL bExecute );
	CString StripLast( CString& strPath );

#ifdef IMPL_CGI
	BOOL        CGIStart( void );
	CWinThread* m_pThread;
	CEvent*     m_pCancel;
#endif // IMPL_CGI

	BOOL		CGIOpenOutFile();
	void        CGIDone( void );
	
protected:
	BOOL GetLine( const char* bytes, int nBytes, int& ndx );
	void ProcessLine( void );
	
	BOOL IsBodySent( void );
	void AddToBody( const char* bytes, int nBytes, int ndx );
	
	BOOL StartResponse( void );
	
	BOOL FindTarget();
	
	BOOL StuffHeading( void );
	void StartTargetStuff( void );
	
	// methods not supported by Mac version....
	void StuffListing( void );
	int  StuffListingFile( WIN32_FIND_DATA* fd, const CString& strDir, BOOL bIcons );
	
	BOOL URLtoPath( CString& strFile );
	BOOL PathToURL( CString& strFile );
	
	BOOL StartSvrApp( void );
	BOOL FromHttpTime( const CString& strHttp, CTime& timeHttp );
	
protected:
	virtual void OnSockRecv(char *pBuf, int Size);
	virtual void OnSockSend();
	virtual void OnSockClose();
	
	friend UINT CGIThread( LPVOID );

	
protected:
	BOOL m_bKeepOpen;
	BOOL m_bWantKeepOpen;
	
	CHTTPRequest	m_Request; // request object
	
	CString     m_strLine;
	REQSTATUS   m_reqStatus;
	int         m_nRefs;
};

void AddFile( CString& strPath, UINT uStr );
void AddFile( CString& strPath, const CString& strFile );
CString Decode( const CString& str, BOOL bQuery = FALSE );

#endif //_TTP_REQSOCK_H_
