#ifndef __APPWEBSERVER_H__
#define __APPWEBSERVER_H__

#include "CritSect.h"
#include "Thread.h"

#include "appweb/appweb.h"

//class CPServiceData;

class CAppWebServer: public CThreadClient
{
public:
	CAppWebServer(CPServiceData *pServiceData = NULL);
	virtual ~CAppWebServer();
//	void SetUsersChanged();
protected:
	CCritSect		m_CritSect;
	CPServiceData   *m_pServiceData;
	
	CThread			m_Thread;
	
	// CThreadClient
	virtual UINT OnThread(int ThreadNum);
private:
	MaHttp		*m_http;				// Http service inside our app
	MaServer	*m_server;				// For the HTTP server
	Mpr			m_mpr;					// Initialize the run time 

	int configure(MaHttp *http);

/*	BOOL			UpdateUsers();

	BOOL			m_bIsUsersUpdated;

	MaDir			*m_TaskManagerDir;
	MaDir			*m_UserManagerDir;
	MaDir			*m_StationManagerDir;*/
};

#endif // __APPWEBSERVER_H__

