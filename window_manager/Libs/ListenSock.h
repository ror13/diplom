// ListenSock.h
//////////////////////////////////////////////////////////////////////////

#ifndef _LISTENSOCK_H_
#define _LISTENSOCK_H_

#include "Thread.h"
#include "SockClient.h"

class CListenSock : public CThreadClient
{
public:
	CListenSock();
	~CListenSock();

	BOOL CreateServer(CSockClient *pClient, int iPort, ULONG BindAddr = INADDR_ANY);
	BOOL StopServer();

	int			m_Port;

protected:
	CSockClient	*m_pClient;
	
	SOCKET		m_hListenSocket;
	int			m_Error;
	
	CThread		m_Thread;
	static BOOL	m_bIsSocketCreated;

	// CThreadClient
	virtual UINT OnThread(int ThreadNum);
	
	void OnSockAccept(SOCKET hSocket, SOCKADDR_IN *pAddrFrom);
};

#endif //_LISTENSOCK_H_
