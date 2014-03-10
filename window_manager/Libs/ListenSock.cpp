// ListenSock.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ListenSock.h"

BOOL CListenSock::m_bIsSocketCreated = FALSE;

CListenSock::CListenSock()
{
	m_hListenSocket = INVALID_SOCKET;
	m_pClient = NULL;
}

CListenSock::~CListenSock()
{
	StopServer();
}

BOOL CListenSock::CreateServer(CSockClient *pClient, int iPort, ULONG BindAddr)
{
	if(!m_bIsSocketCreated)
		m_bIsSocketCreated = TRUE;
	else
		return TRUE;

	m_pClient = pClient;
	
	m_Port = iPort;
	m_Error = 0;

	m_hListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hListenSocket == INVALID_SOCKET)
	{
		m_Error = WSAGetLastError();
		return FALSE;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(iPort);
	addr.sin_addr.S_un.S_addr = BindAddr;
	
	int iRes = bind(m_hListenSocket, (SOCKADDR*)&addr, sizeof(addr));
	if(iRes == SOCKET_ERROR)
	{
		m_Error = WSAGetLastError();
		closesocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		return FALSE;
	}
	
	iRes = listen(m_hListenSocket, SOMAXCONN);
	if(iRes == SOCKET_ERROR)
	{
		m_Error = WSAGetLastError();
		closesocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		return FALSE;
	}
	
	return m_Thread.Create(this);
}

BOOL CListenSock::StopServer()
{
	if(m_hListenSocket != INVALID_SOCKET)
	{
		closesocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
	}
	m_Thread.WaitStopThread();

	return TRUE;
}

UINT CListenSock::OnThread(int ThreadNum)
{
	int nCount;
	fd_set readfds;
	timeval TimeOut = {0, 500000};

	while(!m_Thread.IsStopped())
	{
		FD_ZERO(&readfds);
		FD_SET(m_hListenSocket, &readfds);

		nCount = select(m_hListenSocket+1, &readfds, NULL, NULL, &TimeOut);
		
		if(nCount == SOCKET_ERROR)
			break;					// Error - break thread

		if(nCount == 0)
			continue;				// select timeout

		if(FD_ISSET(m_hListenSocket, &readfds))	// new client
		{
			SOCKADDR_IN AcceptAddress;
			int fromlen = sizeof(AcceptAddress);
			SOCKET AcceptSocket = accept(m_hListenSocket, (SOCKADDR*)&AcceptAddress, &fromlen);

			OnSockAccept(AcceptSocket, &AcceptAddress);
		}
	}
	return 0;
}

void CListenSock::OnSockAccept(SOCKET hSocket, SOCKADDR_IN *pAddrFrom)
{
	if(m_pClient)
		m_pClient->OnSockAccept(m_Port, hSocket, pAddrFrom);
	else
		closesocket(hSocket);
}

