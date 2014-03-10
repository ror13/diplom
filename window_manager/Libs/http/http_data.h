// http_data.h
//////////////////////////////////////////////////////////////////////////

#ifndef _HTTP_DATA_H_
#define _HTTP_DATA_H_

#include "http_request.h"

class CHTTPData
{
public:
	CString		m_strTitleBase;
	CString		m_strRoot;
	CString		m_strServer;
	CTime		m_timeStarted;
	UINT		m_uPort;
	int			m_nSvrName;
	BOOL		m_bAllowListing;
	BOOL		m_bListIcon;

	virtual BOOL HttpClientStartSvrApp(CHTTPRequest *pRequest) = 0;
};

#endif //_HTTP_DATA_H_
