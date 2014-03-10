// NetLink.cpp: implementation of the CNetLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetLink.h"
#include "CommonApi.h"
#include "RecvRequestHelper.h"

#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif


#define THREAD_ID_SEND					1


//////////////////////////////////////////////////////////////////////////

#define USER_TCP_PORT_POOL_BEGIN	10000
#define USER_TCP_PORT_POOL_END		11000

int GetFreeTCPPort()
{
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
	if(Socket == INVALID_SOCKET)
		return -1;
	
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	BOOL PortIsBusy = TRUE;
	int port = USER_TCP_PORT_POOL_BEGIN;

	do
	{
		port++;
		addr.sin_port = htons(port);
		int BindRes = bind(Socket, (sockaddr FAR *)&addr, sizeof(addr));
		PortIsBusy = (BindRes == SOCKET_ERROR);
	} while(PortIsBusy && port < USER_TCP_PORT_POOL_END);

	closesocket(Socket);
	
	if(PortIsBusy)
		port = -1;

	return port;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetLink::CNetLink()
{
	m_bConnected = FALSE;

	m_pWaitRecvRequest = NULL;
	m_pMsgTypesForWait = NULL;

	m_hRequestReaded = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hConnect = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_TempBufSize = 0;
//	m_pTempBuf = (char*)&m_RecvRequest;
	m_pTempBuf = new char[MAX_DATA_SIZE*2];
	m_pTempRequestData = NULL;
	m_iTempRequestDataRecv = 0;
	m_bIsRequestHeaderReceived = FALSE;
}

CNetLink::~CNetLink()
{
	delete []m_pTempBuf;
	if (m_pTempRequestData)
	{
		free(m_pTempRequestData);
		m_pTempRequestData = NULL;
	}
	CloseHandle(m_hConnect);
	CloseHandle(m_hRequestReaded);
}

BOOL CNetLink::RemoveFileMsgs()
{
	//m_SendReqBuf.RemoveSpecifiedCommand(MESSAGE_TYPE_FILE);
	m_SendReqBuf.RemoveAll();
	//m_LoPrioritySendReqBuf.RemoveSpecifiedCommand(MESSAGE_TYPE_FILE);
	m_LoPrioritySendReqBuf.RemoveAll();

	return TRUE;
}

BOOL CNetLink::SendMsg(CRequest& req)
{
	TRACERecvRequest(_T("CNetLink::SendMsg"), req);
	m_SendReqBuf.Push(req);
	return TRUE;
}

BOOL CNetLink::Send(CRequest& req)
{
	//	TRACE(_T("Send:%d\n"), req.m_iCommand);
	CCritSectLock lock(&m_CritSect);

	if(!m_bConnected)
		return FALSE;

	// get packet in new memory
	PCHAR pPacket = (PCHAR)req.GetPacket();

	// send it
	BOOL bRet = SendSock(pPacket , req.GetPacketSize());
	// and free it
	free(pPacket);

	return bRet;
	//return SendSock((char*)req.GetPacket(), req.GetPacketSize());
}

BOOL CNetLink::SendMsgLoPriority(CRequest& req)
{
	return m_LoPrioritySendReqBuf.PushAndWaitEmpty(req);
}

BOOL CNetLink::SendMsgAndWaitReply(CRequest& reqSend, CRequest& reqRecv, int *WaitMsg, int Timeout)
{
	m_pMsgTypesForWait = WaitMsg;
	m_pWaitRecvRequest = &reqRecv;

	ResetEvent(m_hRequestReaded);

	SendMsg(reqSend);

	TRACE(_T("Wait: "));
	for(int i = 0; WaitMsg[i] != -1; i++)
		TRACE(_T("%d "), WaitMsg[i]);
	TRACE(_T("for %d ms\n"), Timeout);

	BOOL res = WaitForSingleObject(m_hRequestReaded, Timeout) == WAIT_OBJECT_0;

	TRACE(_T("Wait res:%d\n"), res);

	return res;
}

BOOL CNetLink::WaitLinkConnect(int Timeout)
{
	if(m_bConnected)
		return TRUE;
	return WaitForSingleObject(m_hConnect, Timeout) == WAIT_OBJECT_0;
}


BOOL CNetLink::StopLink()
{
	m_SendThread.SetStop();
	CNetSock::StopLink();
	m_SendThread.WaitStopThread();
	return TRUE;
}

UINT CNetLink::OnThread(int ThreadNum)
{
	switch(ThreadNum)
	{
	case THREAD_ID_SEND:
		return OnThreadSendToClient();
	default:
		return CNetSock::OnThread(ThreadNum);
	}
}


UINT CNetLink::OnThreadSendToClient()
{
#define SEND_TO_CLIENT_THREAD_TIMEOUT 100
	
	while(!m_SendThread.IsStopped())
	{
		HANDLE ev[] = {m_SendReqBuf, m_LoPrioritySendReqBuf, m_SendThread.m_hEvent};
		if(WaitForMultipleObjects(sizeof(ev)/sizeof(HANDLE), ev, FALSE,
			SEND_TO_CLIENT_THREAD_TIMEOUT) != WAIT_TIMEOUT)
		{
			CRequest req;
			
			while(m_SendReqBuf.Pop(req))
				Send(req);
			
			CRequest *preq;
			if(m_LoPrioritySendReqBuf.Pop(&preq))
			{
				Send(*preq);
				m_LoPrioritySendReqBuf.ReleaseBuf();
			}
		}
	}

	return 0;
}

void CNetLink::OnSockConnect()
{
	m_SendThread.SetName(_T("NetLinkSend"));
	m_SendThread.Create(this, THREAD_ID_SEND);
	m_bConnected = TRUE;
	SetEvent(m_hConnect);
	
	if(m_pCallback)
		m_pCallback->OnLinkConnect();
	
	m_RecvRequest.Clear();
}

void CNetLink::OnSockRecv(char *pBuf, int Size)
{
	//TRACE(_T("CNetLink::OnSockRecv size=%d\n"), Size);
	Lock();


/*
+Copy received data to temp buffer

Check if we got request header. if we got copy from temp to our data and delete from temp.
If we dont get full data return

else if we dont got request header Check if request header in temp buffer 
if get it copy in to temp recv request else
return

parse the whole request

*/
	if(Size > MAX_DATA_SIZE*2)
	{
		TRACE(_T("+++++++ CNetLink::OnSockRecv Size > MAX_DATA_SIZE +++++++\n"));
	}

	if(m_TempBufSize + Size > MAX_DATA_SIZE*2)
	{
		TRACE(_T("+++++++ CNetLink::OnSockRecv buffer overflow +++++++\n"));
		m_TempBufSize = 0;
	}
	memcpy(m_pTempBuf+m_TempBufSize, pBuf, Size);
	m_TempBufSize += Size;

	while(m_TempBufSize)
	{
		// if we not have request header
		if (!m_bIsRequestHeaderReceived)
		{
			//TRACE(_T("No header\n"));
			int packetLength = 0;
			char* pPacket = m_RecvRequest.FindPacketStartAndLength(m_pTempBuf, m_TempBufSize, &packetLength);
			if(pPacket == NULL)
			{
				//TRACE(_T("0 0 1 m_TempBufSize %d\n"), m_TempBufSize);
				Unlock();
				return;
			}

			// now we got the header!
			
			//TRACE(_T("now we got the header!\n"));
			// copy received request header
			REQUEST_HEADER* pRequestHeader = (REQUEST_HEADER*)m_pTempBuf;
			memcpy(&m_tempRequestHeader, pRequestHeader, REQUEST_HEADER_SIZE);
			m_bIsRequestHeaderReceived = TRUE;
			
			// allocate request data buffer
			m_pTempRequestData = (PBYTE)realloc(m_pTempRequestData, pRequestHeader->m_iDataLen);
			m_iTempRequestDataRecv = 0;
			
			if (REQUEST_HEADER_SIZE < m_TempBufSize)
			{
				// remove from temp buffer processed bytes
				memmove(m_pTempBuf, m_pTempBuf + REQUEST_HEADER_SIZE, m_TempBufSize - REQUEST_HEADER_SIZE);
				m_TempBufSize -= REQUEST_HEADER_SIZE;
			} 
			else
			{
				// we process all bytes from temp buffer
				m_TempBufSize = 0;
			}
			// continue with while
			continue;
		}

		// if we already got the request header
		if (m_bIsRequestHeaderReceived)
		{
			//TRACE(_T("Using header\n"));
			// we only copy request data
			int iRestDataBytes = m_tempRequestHeader.m_iDataLen - m_iTempRequestDataRecv;
			int iProcessedBytes = min(iRestDataBytes, m_TempBufSize);
			memcpy(m_pTempRequestData + m_iTempRequestDataRecv, m_pTempBuf, iProcessedBytes);

			if (iProcessedBytes < m_TempBufSize)
			{
				// remove from temp buffer processed bytes
				memmove(m_pTempBuf, m_pTempBuf + iProcessedBytes, m_TempBufSize - iProcessedBytes);
				m_TempBufSize -= iProcessedBytes;
			} 
			else
			{
				// we process all bytes from temp buffer
				m_TempBufSize = 0;
			}

			m_iTempRequestDataRecv += iProcessedBytes;
			if (m_tempRequestHeader.m_iDataLen > 0 &&
				m_iTempRequestDataRecv < m_tempRequestHeader.m_iDataLen)
			{
				//TRACE(_T("no whole data\n"));
				Unlock();
				return;
			}
		}

		//TRACE(_T("waiting postfix\n"));
		if (m_TempBufSize < REQ_POSTFIX_SIZE)
		{
			//TRACE(_T("no postfix\n"));
			Unlock();
			return;
		}
		// check postfix and delete it
		if (memcmp(m_pTempBuf, REQ_POSTFIX, REQ_POSTFIX_SIZE) != 0)
		{
			//TRACE(_T("bad postfix\n"));
			Unlock();
			return;
		}

		if (REQ_POSTFIX_SIZE < m_TempBufSize)
		{
			// remove from temp buffer processed bytes
			memmove(m_pTempBuf, m_pTempBuf + REQ_POSTFIX_SIZE, m_TempBufSize - REQ_POSTFIX_SIZE);
			m_TempBufSize -= REQ_POSTFIX_SIZE;
		} 
		else
		{
			// we process all bytes from temp buffer
			m_TempBufSize = 0;
		}

		//TRACE(_T("using with whole data\n"));
		// now we can process the header
		memcpy(&m_RecvRequest, &m_tempRequestHeader, REQUEST_HEADER_SIZE);
		m_RecvRequest.SetData(m_pTempRequestData, m_tempRequestHeader.m_iDataLen);
		memcpy(&m_RecvRequest.m_Reserve, &REQ_POSTFIX, REQ_POSTFIX_SIZE);

		m_bIsRequestHeaderReceived = FALSE;
		m_iTempRequestDataRecv = 0;
		
		BOOL msg_for_wait = FALSE;
		int i = 0;
		while(m_pMsgTypesForWait && m_pMsgTypesForWait[i] != -1)
		{
			if(m_RecvRequest.m_iCommand == m_pMsgTypesForWait[i++])
			{
				msg_for_wait = TRUE;
				break;
			}
		}
		
		if(m_pCallback)
			m_pCallback->OnRecvRequest(m_RecvRequest);
		
		if(msg_for_wait || m_pMsgTypesForWait == NULL)
		{
			if(m_pWaitRecvRequest)
			{
				*m_pWaitRecvRequest = m_RecvRequest;
				
				m_pWaitRecvRequest = NULL;
				m_pMsgTypesForWait = NULL;
				
				SetEvent(m_hRequestReaded);
			}
		}
		
		//TRACE(_T("CNetLink: on recv msg: %d\n"), m_RecvRequest.m_iCommand);
		
		m_RecvRequest.Clear();
	}

	Unlock();
}

void CNetLink::OnSockClose()
{
	m_bConnected = FALSE;
	ResetEvent(m_hConnect);
	
	if(m_pCallback)
		m_pCallback->OnLinkDisconnect();
	
	m_SendThread.SetStop();
}

//////////////////////////////////////////////////////////////////////////
// CNetLinkTest

#ifdef _DEBUG

class CNetLinkTest: public CppUnit::TestFixture
{
public:
	void setUp()
	{
	}
	
	void testGetFreeTCPPort()
	{
		int free_port1 = GetFreeTCPPort();
		int free_port2 = GetFreeTCPPort();
		CPPUNIT_ASSERT(free_port1 == free_port2);

		SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
		CPPUNIT_ASSERT(Socket != INVALID_SOCKET);
		
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(free_port1);
		
		int BindRes = bind(Socket, (sockaddr FAR *)&addr, sizeof(addr));
		CPPUNIT_ASSERT(BindRes != SOCKET_ERROR);

		int free_port3 = GetFreeTCPPort();
		CPPUNIT_ASSERT(free_port1 != free_port3);

		closesocket(Socket);

		int free_port4 = GetFreeTCPPort();
		CPPUNIT_ASSERT(free_port1 == free_port4);
	}

	void testSendRecvRequest()
	{
		int iPort = GetFreeTCPPort();

		SOCKET hSrvSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		CPPUNIT_ASSERT(hSrvSock != INVALID_SOCKET);

		sockaddr_in adr;
		memset(&adr, 0, sizeof(adr));
		adr.sin_family = AF_INET;
		adr.sin_port = htons(iPort);
		adr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);

		int iRes = bind(hSrvSock, (sockaddr*)&adr, sizeof(adr));
		CPPUNIT_ASSERT(iRes != SOCKET_ERROR);
		
		iRes = listen(hSrvSock, SOMAXCONN);
		CPPUNIT_ASSERT(iRes != SOCKET_ERROR);

		CNetLink link;
		link.CreateLink(_T("127.0.0.1"), iPort);

		class CLink2Client: public CLinkPointCallback
		{
		public:
			CString strTest;
			CRequest req;
			CRequest req2;

			HANDLE hRecvEvent;

			CLink2Client()
			{
				strTest = _T("String for test");
				req.m_iCommand = 1;
				req.SetData(strTest);

				hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			}
			~CLink2Client()
			{
				CloseHandle(hRecvEvent);
			}
			BOOL WaitRecv(int Timeout)
			{
				return WaitForSingleObject(hRecvEvent, Timeout) == WAIT_OBJECT_0;
			}

			virtual void OnLinkConnect()
			{
				TRACE(_T("Connect\n"));
			}
			virtual void OnRecvRequest(CRequest &req)
			{
				req2 = req;
				SetEvent(hRecvEvent);
			}
			virtual void OnLinkDisconnect()
			{
				TRACE(_T("Disconnect\n"));
			}
		} link2Clinet;

		CNetLink link2;
		link2.SetCallback(&link2Clinet);

		sockaddr_in AcceptAddress;
		int fromlen = sizeof(AcceptAddress);
		SOCKET AcceptSocket = accept(hSrvSock, (SOCKADDR *)&AcceptAddress, &fromlen);

		// work with accepted connection
		CPPUNIT_ASSERT(link2.AttachSocket(AcceptSocket, &AcceptAddress));

		CPPUNIT_ASSERT(link.WaitLinkConnect(1000));
		TRACE(_T("Send\n"));
		CPPUNIT_ASSERT(link.SendMsg(link2Clinet.req));

		TRACE(_T("Wait\n"));
		BOOL wait_req_res = link2Clinet.WaitRecv(1000);
		link2.StopLink();
		link.StopLink();
		CPPUNIT_ASSERT(wait_req_res);

		//CPPUNIT_ASSERT(link2Clinet.strTest == link2Clinet.req2.GetDataAsString());
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CNetLinkTest);
	CPPUNIT_TEST(testGetFreeTCPPort);
	CPPUNIT_TEST(testSendRecvRequest);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CNetLinkTest);

#endif // _DEBUG
