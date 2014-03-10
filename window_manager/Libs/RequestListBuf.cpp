// RequestListBuf.cpp: implementation of the CRequestListBuf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RequestListBuf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRequestListBuf::CRequestListBuf()
{
	m_hMutex = CreateMutex(NULL, FALSE, NULL);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CRequestListBuf::~CRequestListBuf()
{
	CloseHandle(m_hEvent);
	CloseHandle(m_hMutex);

	for(REQ_LIST::iterator i = m_List.begin(); i != m_List.end(); i++)
		delete *i;
	m_List.clear();
}

BOOL CRequestListBuf::RemoveSpecifiedCommand(int iCommand)
{
	Lock();
// 	for(REQ_LIST::iterator i = m_List.begin(); i != m_List.end(); i++)
// 	{
// 		CRequest *pReq = *i;
// 		if (pReq->m_iCommand == iCommand)
// 		{
// 			m_List.erase(i);
// 			delete pReq;
// 		}
// 	}
	REQ_LIST::iterator i = m_List.begin();
	while(i != m_List.end())
	{
		CRequest *pReq = *i;
		if (pReq->m_iCommand == iCommand)
		{
			m_List.erase(i);
			delete pReq;

			i = m_List.begin();
			continue;
		}

		i++;
	}
	Unlock();

	return TRUE;
}

BOOL CRequestListBuf::RemoveAll()
{
	Lock();
	for(REQ_LIST::iterator i = m_List.begin(); i != m_List.end(); i++)
	{
		if (i != NULL)
		{
			CRequest *pReq = *i;
			delete pReq;
		}
	}
	m_List.clear();

	Unlock();

	return TRUE;
}

void CRequestListBuf::Push(CRequest &req)
{
	CRequest *pNewReq = new CRequest;
	*pNewReq = req;

	Lock();
	m_List.push_back(pNewReq);
	Unlock();
	SetEvent(m_hEvent);
}

BOOL CRequestListBuf::Pop(CRequest &req)
{
	BOOL res = FALSE;
	Lock();
	if(m_List.size())
	{
		CRequest *pReq = *m_List.begin();
		req = *pReq;
		delete pReq;
		m_List.pop_front();
		res = TRUE;
	}
	Unlock();
	return res;
}

CRequest* CRequestListBuf::GetFirst()
{
	if(!m_List.empty())
	{
		m_CurItem = m_List.begin();
		return *m_CurItem;
	} else
		return NULL;
}

CRequest* CRequestListBuf::GetNext()
{
	if(++m_CurItem != m_List.end())
		return *m_CurItem;
	else
		return NULL;
}

CRequest* CRequestListBuf::RemoveCurrentAndGetNext()
{
	if(m_CurItem != m_List.end())
	{
		m_CurItem = m_List.erase(m_CurItem);
		if(m_CurItem != m_List.end())
			return *m_CurItem;
		else
			return NULL;
	} else
		return NULL;
}

LONG CRequestListBuf::GetCount()
{
	return m_List.size();
}

BOOL CRequestListBuf::Wait(int Timeout)
{
	return WaitForSingleObject(m_hEvent, Timeout) == WAIT_OBJECT_0;
}

BOOL CRequestListBuf::Lock(int Timeout)
{
	return WaitForSingleObject(m_hMutex, Timeout) == WAIT_OBJECT_0;
}

BOOL CRequestListBuf::Unlock()
{
	return ReleaseMutex(m_hMutex);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CRequestBuf::CRequestBuf()
{
	m_hMutex = CreateMutex(NULL, FALSE, NULL);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEmptyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pItem = NULL;
}

CRequestBuf::~CRequestBuf()
{
	CloseHandle(m_hEmptyEvent);
	CloseHandle(m_hEvent);
	CloseHandle(m_hMutex);
}

BOOL CRequestBuf::RemoveSpecifiedCommand(int iCommand)
{
	ASSERT(FALSE);
/*
	CRequest *ppreq;
	Pop(&ppreq);
*/
	// TODO: !!!
	// Need to write correct delete function

	return TRUE;
}

BOOL CRequestBuf::RemoveAll()
{
	Lock();
	CRequest *preq;
	if(Pop(&preq))
	{
		ReleaseBuf();
	}
	Unlock();

	return TRUE;
}

BOOL CRequestBuf::PushAndWaitEmpty(CRequest &req, DWORD Timeout)
{
	Lock();
	ResetEvent(m_hEmptyEvent);
	m_pItem = &req;
	Unlock();
	SetEvent(m_hEvent);
	return WaitForSingleObject(m_hEmptyEvent, Timeout) == WAIT_OBJECT_0;
}

BOOL CRequestBuf::Pop(CRequest **ppreq)
{
	BOOL res = FALSE;
	if(m_pItem && ppreq)
	{
		Lock();
		*ppreq = m_pItem;
		m_pItem = NULL;
		res = TRUE;
		Unlock();
	}
	return res;
}

BOOL CRequestBuf::ReleaseBuf()
{
	SetEvent(m_hEmptyEvent);
	return Unlock();
}

BOOL CRequestBuf::Wait(int Timeout)
{
	return WaitForSingleObject(m_hEvent, Timeout) == WAIT_OBJECT_0;
}

BOOL CRequestBuf::Lock(int Timeout)
{
	return WaitForSingleObject(m_hMutex, Timeout) == WAIT_OBJECT_0;
}

BOOL CRequestBuf::Unlock()
{
	return ReleaseMutex(m_hMutex);
}

//////////////////////////////////////////////////////////////////////////
// CRequestListBufTest

#ifdef _DEBUG

#include "Thread.h"

class CRequestListBufTest: public CppUnit::TestFixture
{
public:
	void setUp()
	{
	}
	
	void testBuf()
	{
		CRequestListBuf Buf;

		CPPUNIT_ASSERT(Buf.GetFirst() == NULL);

		CRequest req;
		for(int i = 0; i < 10; i++)
		{
			req.m_iCommand = i;
			Buf.Push(req);
		}

		CPPUNIT_ASSERT(Buf.GetFirst() != NULL);

		CRequest *preq = Buf.GetFirst();
		while(preq)
		{
			if(preq->m_iCommand == 1)
				preq = Buf.RemoveCurrentAndGetNext();
			else
				preq = Buf.GetNext();
		}

		CPPUNIT_ASSERT(Buf.Pop(req));
		CPPUNIT_ASSERT_EQUAL(0, req.m_iCommand);
		
		for(i = 2; i < 10; i++)
		{
			CPPUNIT_ASSERT(Buf.Pop(req));
			CPPUNIT_ASSERT_EQUAL(i, req.m_iCommand);
		}
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CRequestListBufTest);
	CPPUNIT_TEST(testBuf);
	CPPUNIT_TEST_SUITE_END();
};

//////////////////////////////////////////////////////////////////////////
// CRequestBufTest

class CRequestBufTest: public CppUnit::TestFixture, public CThreadClient
{
private:
	CThread m_ThreadIn;
	CThread m_ThreadOut;

	int		m_CountPushs;
	int		m_CountPops;

	CRequest m_Req;

	CRequestBuf	m_ReqBuf;

	std::string err_str;
	std::vector<std::string>	m_ErrorStr;

	// CThreadClient
	virtual UINT OnThread(int ThreadNum)
	{
		switch(ThreadNum)
		{
		case 1:
			OnThreadPush();
			break;
		case 2:
			OnThreadPop();
			break;
		}
		return 0;
	}

	void OnThreadPush()
	{
		for(int i = 0; i < 10000 && !m_ThreadIn.IsStopped(); i++)
		{
			m_Req.m_iCommand = m_CountPushs;
			if(m_ReqBuf.PushAndWaitEmpty(m_Req, 1000))
			{
				m_CountPushs++;
			} else
			{
				m_ErrorStr.push_back("Error push");
				break;
			}
		}
	}
	void OnThreadPop()
	{
#define THREAD_TIMEOUT INFINITE
		int Count;
		Count = 0;
		while(!m_ThreadOut.IsStopped())
		{
			HANDLE ev[] = {m_ThreadOut, m_ReqBuf};
			if(WaitForMultipleObjects(sizeof(ev)/sizeof(HANDLE), ev, FALSE,
				THREAD_TIMEOUT) != WAIT_TIMEOUT)
			{
				if(m_ThreadOut.IsStopped())
					break;

				CRequest *pReq;
				if(m_ReqBuf.Pop(&pReq))
				{
					if(pReq->m_iCommand != Count)
					{
						m_ReqBuf.ReleaseBuf();
						
						char buffer[20];
						err_str = std::string("Error count: m_iCommand=") + itoa(pReq->m_iCommand,buffer,10) +
							" Count=" + itoa(Count,buffer,10);
						
						m_ErrorStr.push_back(err_str);
						break;
					} else
						Count++;

					m_CountPops++;
					m_ReqBuf.ReleaseBuf();
				} else
				{
					m_ErrorStr.push_back("Error pop");
					break;
				}
			}
		}
#undef THREAD_TIMEOUT
	}

public:
	void setUp()
	{
		m_CountPushs = 0;
		m_CountPops = 0;
		m_ThreadIn.SetName(_T("Push"));
		m_ThreadOut.SetName(_T("Pop"));
	}
	
	void testBuf()
	{
		m_ThreadIn.Create(this, 1);
		m_ThreadOut.Create(this, 2);

		m_ThreadIn.WaitUntilDone(10000);
		m_ThreadOut.WaitStopThread();

		
		for(std::vector<std::string>::iterator b = m_ErrorStr.begin(); b != m_ErrorStr.end(); b++)
		{
			USES_CONVERSION;
			CPPUNIT_ASSERT_MESSAGE(*b, 0);
		}
		CPPUNIT_ASSERT_EQUAL(m_CountPushs, m_CountPops);
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CRequestBufTest);
	CPPUNIT_TEST(testBuf);
	CPPUNIT_TEST_SUITE_END();
	
};

CPPUNIT_TEST_SUITE_REGISTRATION(CRequestListBufTest);
CPPUNIT_TEST_SUITE_REGISTRATION(CRequestBufTest);

#endif // _DEBUG
