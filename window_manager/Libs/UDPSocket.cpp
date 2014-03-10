// UDPSocket.cpp: implementation of the CUDPSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UDPSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUDPSocket::CUDPSocket()
{
	m_UDPSockNum = 0;
	m_pClient = NULL;
	m_hSocket = INVALID_SOCKET;

	m_Thread.SetName(_T("CUDPSocket"));

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
}

CUDPSocket::~CUDPSocket()
{
	Stop();
	WSACleanup();
}

BOOL CUDPSocket::Create(int nPort, CUDPSocketClient *pClient, int UDPSockNum, BOOL bBroadcast, u_long BindAddr)
{
	Stop();

	m_UDPSockNum = UDPSockNum;
	m_pClient = pClient;

	m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_hSocket == INVALID_SOCKET)
		return FALSE;

	BOOL bReuseAddr = TRUE;
	setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuseAddr, sizeof(bReuseAddr));

	// initializing bind address
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.S_un.S_addr = htonl(BindAddr);
	// bind
	int res = bind(m_hSocket, (sockaddr*)&addr, sizeof(addr));
	if(res != 0)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return FALSE;
	}
	if(bBroadcast)
		setsockopt(m_hSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, sizeof(bBroadcast)); 

	return m_Thread.Create(this);
}

BOOL CUDPSocket::Stop()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	m_Thread.WaitStopThread();
	return TRUE;
}

BOOL CUDPSocket::Send(sockaddr_in *pToAddr, char *pBuf, int BufSize)
{
	int res = sendto(m_hSocket, pBuf, BufSize, 0, (sockaddr*)pToAddr, sizeof(sockaddr_in));
	return res != SOCKET_ERROR;
}

BOOL CUDPSocket::Send(u_long ToAddr, int nPort, char *pBuf, int BufSize)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.S_un.S_addr = ToAddr;
	
	return Send(&addr, pBuf, BufSize);
}

BOOL CUDPSocket::Send(LPCTSTR ToAddr, int nPort, char *pBuf, int BufSize)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);

	USES_CONVERSION;
	addr.sin_addr.S_un.S_addr = inet_addr(T2A(ToAddr));

	return Send(&addr, pBuf, BufSize);
}

UINT CUDPSocket::OnThread(int ThreadNum)
{
	int nCount;
	fd_set readfds;
	timeval TimeOut = {0, 500000};
	char buf[65536];

	while(!m_Thread.IsStopped())
	{
		FD_ZERO(&readfds);
		FD_SET(m_hSocket, &readfds);

		nCount = select(m_hSocket + 1, &readfds, NULL, NULL, &TimeOut);

		if(nCount == SOCKET_ERROR)
			break;					// Error - break thread
		if(nCount == 0)
			continue;				// select timeout
		
		if(FD_ISSET(m_hSocket, &readfds))	// recv data
		{
			sockaddr_in Address;
			int fromlen = sizeof(Address);
			int res = recvfrom(m_hSocket, buf, sizeof(buf), 0, (SOCKADDR*)&Address, &fromlen);
			if(res == SOCKET_ERROR)
				continue;					// Error - break thread
			if(m_pClient)
				m_pClient->OnUDPRecv(m_UDPSockNum, buf, res, &Address);
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// CUDPSocketTest

#ifdef _DEBUG

#define COUNT_TEST_BUFS	256
#define TEST_PORT		10010

class CUDPSocketTest: public CppUnit::TestFixture, public CUDPSocketClient
{
private:
	CUDPSocket	m_Sock;
	int			m_TestBufs[COUNT_TEST_BUFS];

	// CUDPSocketClient
	virtual void OnUDPRecv(int UDPSockNum, char *pBuf, int BufSize, sockaddr_in *pFromAddr)
	{
		int num;
		if(sscanf(pBuf, "%d", &num) == 1)
		{
			if(num >= 0 && num < COUNT_TEST_BUFS)
				m_TestBufs[num] = 1;
		}
	}

public:
	void setUp()
	{
		memset(&m_TestBufs, 0, sizeof(m_TestBufs));
	}
	
	void testSendUDPBuf()
	{
		CPPUNIT_ASSERT(m_Sock.Create(TEST_PORT, this, 0, FALSE, INADDR_LOOPBACK));
		Sleep(100);

		CUDPSocket sock;
		CPPUNIT_ASSERT(sock.Create(TEST_PORT+1));

		char buf[256];
		for(int i = 0; i < COUNT_TEST_BUFS; i++)
		{
			sprintf(buf, "%d", i);

			CPPUNIT_ASSERT(sock.Send(_T("127.0.0.1"), TEST_PORT, buf, strlen(buf)+1));
		}
		Sleep(100);
		for(i = 0; i < COUNT_TEST_BUFS; i++)
		{
			CPPUNIT_ASSERT(m_TestBufs[i] == 1);
		}
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CUDPSocketTest);
	CPPUNIT_TEST(testSendUDPBuf);
	CPPUNIT_TEST_SUITE_END();
	
};
CPPUNIT_TEST_SUITE_REGISTRATION(CUDPSocketTest);

#endif // _DEBUG
