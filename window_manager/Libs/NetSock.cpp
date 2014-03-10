// NetSock.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NetSock.h"

CNetSock::CNetSock()
{
	m_ServerPort = 0;

	m_hSocket = INVALID_SOCKET;
	m_bAttached = FALSE;
	m_bSelectWrite = FALSE;
	m_Error = 0;
	m_bEncryption = FALSE;

	m_TempBufSize = 0;
	m_pTempBuf = new char[RECV_BUF_SIZE];
}

CNetSock::~CNetSock()
{
	StopLink();
	delete []m_pTempBuf;
}


BOOL CNetSock::EnableEncryption(const char* password, int len)
{
	if (len > 16 || len <= 0)
		return FALSE;

	m_bEncryption = TRUE;
	arc4_setup( &m_EncryptRecvCtx, (unsigned char*)password, len );
	arc4_setup( &m_EncryptSendCtx, (unsigned char*)password, len );
	return TRUE;
}

void CNetSock::DisableEncryption()
{
	m_bEncryption = FALSE;
}

BOOL CNetSock::AttachSocket(SOCKET hSocket, SOCKADDR_IN *pClientAddress)
{
	StopLink();
	
	m_hSocket = hSocket;
	m_Addr = *pClientAddress;
	
	m_bAttached = TRUE;
	
	m_Thread.SetName(_T("NetSock_AttachSocket"));
	return m_Thread.Create(this, THREAD_ID_SOCK);
}

BOOL CNetSock::CreateLink(LPCTSTR ServerName, int ServerPort)
{
	StopLink();
	
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hSocket == INVALID_SOCKET)
		return FALSE;

//	int flag = 1;
//	setsockopt(m_hSocket,
//							IPPROTO_TCP,
//							TCP_NODELAY,
//							(char *) &flag,
//							sizeof(int));

	m_bEncryption = FALSE;
	m_ServerName = ServerName;
	m_ServerPort = ServerPort;
	
	m_Thread.SetName(_T("NetSock_CreateLink"));
	return m_Thread.Create(this, THREAD_ID_SOCK);
}

BOOL CNetSock::SendSock(char *pBuf, int Size)
{
	int iRes;
	if(m_hSocket == INVALID_SOCKET)
		return FALSE;

	if (m_bEncryption)
	{
		unsigned char* tmp = (unsigned char*)malloc(Size);
		memcpy(tmp, pBuf, Size);
		arc4_crypt(&m_EncryptSendCtx, tmp, Size);
		iRes = send(m_hSocket, (const char*)tmp, Size, 0);
		free(tmp);
	} 
	else 
	{
		iRes = send(m_hSocket, pBuf, Size, 0);
	}

	if(iRes == SOCKET_ERROR)
	{
		TRACE(_T("Error send!\n"));
		m_Error = WSAGetLastError();
		TRACE(_T("Error %d (size = %d)!\n"), m_Error, Size);
		
		return FALSE;
	}

	return TRUE;
}

BOOL CNetSock::StopLink()
{
	m_Thread.SetStop();
	if(m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	m_Thread.WaitStopThread();
	m_bAttached = FALSE;
	return TRUE;
}

CString CNetSock::GetPeerAddr()
{
	CString str;
	str.Format(_T("%03d.%03d.%03d.%03d"),
		m_Addr.sin_addr.s_net, m_Addr.sin_addr.s_host,
		m_Addr.sin_addr.s_lh, m_Addr.sin_addr.s_impno);
	return str;
}

void CNetSock::SetStopLink()
{
	m_Thread.SetStop();
}

UINT CNetSock::OnThread(int ThreadNum)
{
	switch(ThreadNum)
	{
	case THREAD_ID_SOCK:
		OnThreadSocket();
		break;
	}
	return 0;
}

void CNetSock::OnThreadSocket()
{
	fd_set readfds;
	fd_set writefds;
	timeval TimeOut={0,100000};
	int nCount;

	while(!m_Thread.IsStopped())
	{
		if(!m_bAttached)
		{
			memset(&m_Addr, 0, sizeof(m_Addr));
			m_Addr.sin_family = AF_INET;
			m_Addr.sin_port = htons(m_ServerPort);

			USES_CONVERSION;
			LPSTR server_name = T2A(m_ServerName);

			ULONG a = inet_addr(server_name);

			if(a == INADDR_NONE)
			{
				LPHOSTENT lphost;
				lphost = gethostbyname(server_name);
				if(lphost == NULL)
				{
					m_Thread.WaitEvent(500);
					continue;
				}
				m_Addr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			} else
				m_Addr.sin_addr.s_addr = a;
			
			if(m_Thread.IsStopped())
				break;

			int res = connect(m_hSocket, (sockaddr*)&m_Addr, sizeof(m_Addr));
			if(res != 0)
			{
				m_Thread.WaitEvent(500);
				continue;
			}
		}

		OnSockConnect();

		while(!m_Thread.IsStopped())
		{
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_SET(m_hSocket, &readfds);
			if(m_bSelectWrite)
				FD_SET(m_hSocket, &writefds);

			nCount = select(FD_SETSIZE, &readfds, &writefds, NULL, &TimeOut);
			
			if(nCount == SOCKET_ERROR)	//  on error - stop thread
			{
				m_Thread.SetStop();
				break;
			}
			
			if(nCount == 0) continue;	// timeout

			if(FD_ISSET(m_hSocket, &readfds))
			{
				nCount = recv(m_hSocket, m_pTempBuf, RECV_BUF_SIZE, 0);
				if(nCount <= 0)
				{						// socket is closed
					closesocket(m_hSocket);
					m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if(m_hSocket == INVALID_SOCKET)
						m_Thread.SetStop();
					break;
				} 
				else
				{
					if (m_bEncryption)
					{
						arc4_crypt(&m_EncryptRecvCtx, (unsigned char*)m_pTempBuf, nCount);
					}
					OnSockRecv(m_pTempBuf, nCount);
				}
			}
			if(FD_ISSET(m_hSocket, &writefds))
			{
				m_bSelectWrite = FALSE;
				OnSockSend();
			}
		}

		OnSockClose();
		
		if(m_bAttached)
			break;
	}
	if(m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

