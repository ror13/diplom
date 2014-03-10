// ConnectSock.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ConnectSock.h"

BOOL CConnectSock::m_bIsSocketCreated = FALSE;

CConnectSock::CConnectSock()
{
	m_hConnectSocket = INVALID_SOCKET;
	m_pClient = NULL;

	m_bConnected = FALSE;
	m_ConnectToAddr = 0;

	m_bIsSocketCreated = FALSE;
	m_bIsConnectCreated = FALSE;
}

CConnectSock::~CConnectSock()
{
	StopServer();
}

BOOL CConnectSock::MakeConnect(CSockClient *pClient, int iPort, ULONG ConnectToAddr, int ConnectID)
{
	m_pClient = pClient;
	
	m_Port = iPort;
	m_ConnectToAddr = ConnectToAddr;
	m_Error = 0;

	m_hConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hConnectSocket == INVALID_SOCKET)
	{
		m_Error = WSAGetLastError();
		return FALSE;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(iPort);
	addr.sin_addr.S_un.S_addr = ConnectToAddr;
	
	int iRes = connect(m_hConnectSocket, (SOCKADDR*)&addr, sizeof(addr));
	if(iRes == SOCKET_ERROR)
	{
		m_Error = WSAGetLastError();
		closesocket(m_hConnectSocket);
		TRACE(_T("CConnectSock::MakeConnect connect error [%d]\n"), m_Error);
		m_hConnectSocket = INVALID_SOCKET;
		return FALSE;
	}
	
//	return m_Thread.Create(this);
	BOOL bRet = m_Thread.Create(this);
 	OnSockConnect(m_hConnectSocket, &addr, ConnectID);

	m_bIsConnectCreated = TRUE;
	
	return bRet;
}

BOOL CConnectSock::CreateServer(CSockClient *pClient, int iPort, ULONG BindAddr)
{
	if(!m_bIsSocketCreated)
		m_bIsSocketCreated = TRUE;
	else
		return TRUE;

	m_pClient = pClient;
	
	m_Port = iPort;
	m_Error = 0;

	m_hConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hConnectSocket == INVALID_SOCKET)
	{
		m_Error = WSAGetLastError();
		return FALSE;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(iPort);
	addr.sin_addr.S_un.S_addr = BindAddr;
	
	int iRes = bind(m_hConnectSocket, (SOCKADDR*)&addr, sizeof(addr));
	if(iRes == SOCKET_ERROR)
	{
		m_Error = WSAGetLastError();
		closesocket(m_hConnectSocket);
		m_hConnectSocket = INVALID_SOCKET;
		return FALSE;
	}
	
	iRes = listen(m_hConnectSocket, SOMAXCONN);
	if(iRes == SOCKET_ERROR)
	{
		m_Error = WSAGetLastError();
		closesocket(m_hConnectSocket);
		m_hConnectSocket = INVALID_SOCKET;
		return FALSE;
	}
	
	return m_Thread.Create(this);
}

BOOL CConnectSock::StopServer()
{
	if(m_hConnectSocket != INVALID_SOCKET)
	{
		closesocket(m_hConnectSocket);
		m_hConnectSocket = INVALID_SOCKET;
	}
	m_Thread.WaitStopThread();

	m_pClient = NULL;
	
	m_bConnected = FALSE;
	m_ConnectToAddr = 0;
	
	m_bIsSocketCreated = FALSE;
	m_bIsConnectCreated = FALSE;

	return TRUE;
}

UINT CConnectSock::OnThread(int ThreadNum)
{
	int nCount;
	fd_set readfds;
	timeval TimeOut = {0, 500000};

	while(!m_Thread.IsStopped())
	{
		if (m_bIsSocketCreated)
		{
			FD_ZERO(&readfds);
			FD_SET(m_hConnectSocket, &readfds);

			nCount = select(m_hConnectSocket+1, &readfds, NULL, NULL, &TimeOut);
			
			if(nCount == SOCKET_ERROR)
				break;					// Error - break thread

			if(nCount == 0)
				continue;				// select timeout

			if(FD_ISSET(m_hConnectSocket, &readfds))	// new client
			{
				SOCKADDR_IN AcceptAddress;
				int fromlen = sizeof(AcceptAddress);
				SOCKET AcceptSocket = accept(m_hConnectSocket, (SOCKADDR*)&AcceptAddress, &fromlen);

				OnSockAccept(AcceptSocket, &AcceptAddress);
			}
		}
	}
	return 0;
}

void CConnectSock::OnSockAccept(SOCKET hSocket, SOCKADDR_IN *pAddrFrom)
{
	if(m_pClient)
		m_pClient->OnSockAccept(m_Port, hSocket, pAddrFrom);
	else
		closesocket(hSocket);
}

void CConnectSock::OnSockConnect(SOCKET hSocket, SOCKADDR_IN *pAddrFrom, int ConnectID)
{
	if(m_pClient)
		m_pClient->OnSockConnect(m_Port, hSocket, pAddrFrom, ConnectID);
	else
		closesocket(hSocket);
}

