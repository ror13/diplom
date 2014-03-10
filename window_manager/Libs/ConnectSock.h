// ConnectSock.h
//////////////////////////////////////////////////////////////////////////

#ifndef _ConnectSock_H_
#define _ConnectSock_H_

#include "Thread.h"
#include "SockClient.h"

class CConnectSock : public CThreadClient
{
public:
	CConnectSock();
	~CConnectSock();

	BOOL MakeConnect(CSockClient *pClient, int iPort, ULONG ConnectToAddr, int ConnectID);
	BOOL CreateServer(CSockClient *pClient, int iPort, ULONG BindAddr = INADDR_ANY);
	BOOL StopServer();

	int			m_Port;

protected:
	CSockClient	*m_pClient;
	
	SOCKET		m_hConnectSocket;
	int			m_Error;
	
	CThread		m_Thread;
	static BOOL	m_bIsSocketCreated;
	BOOL		m_bIsConnectCreated;

	BOOL		m_bConnected;
	ULONG		m_ConnectToAddr;

	// CThreadClient
	virtual UINT OnThread(int ThreadNum);
	
	virtual void OnSockAccept(SOCKET hSocket, SOCKADDR_IN *pAddrFrom);
	virtual void OnSockConnect(SOCKET hSocket, SOCKADDR_IN *pAddrFrom, int ConnectID);
};

#endif //_ConnectSock_H_
