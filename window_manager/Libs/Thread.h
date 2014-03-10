// Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREAD_H__82E3AAC7_30E7_4556_861C_BC085BE8ACB8__INCLUDED_)
#define AFX_THREAD_H__82E3AAC7_30E7_4556_861C_BC085BE8ACB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CThreadClient
{
public:
	virtual UINT OnThread(int ThreadNum) = 0;
};

class CThread  
{
public:
	CThread();
	virtual ~CThread();

	void SetName(TCHAR* n);
	BOOL Create(CThreadClient* pClient, int ThreadNum = 0, int nPriority = THREAD_PRIORITY_NORMAL);
	BOOL IsStopped() {return m_bStopThread;} 
	BOOL IsActive();
	void WaitStopThread(int TimeOut = 10000);
	BOOL WaitUntilDone(int TimeOut = INFINITE);
	void SetStop();
	void Suspend();
	void Resume();
	
	enum WAIT_STATUS
	{
		WAIT_STATUS_THREAD_STOPPED = -1,
		WAIT_STATUS_ON_EVENT = 0,
		WAIT_STATUS_TIMEOUT,
		WAIT_STATUS_ERROR
	};
	// Wait while call SetEvent() or Stop() (For use only in parent OnThread()). TimeOut in milliseconds.
	// Return: -1 - stop thread, 0 - on call SetEvent(), 1 - timeout
	WAIT_STATUS WaitEvent(UINT TimeOut = INFINITE);
	void SetEvent();
	void ResetEvent();
	void ProcessException(TCHAR *pThreadName, DWORD dwExceptCode);
	
	operator HANDLE() {return m_hEvent;}

	BOOL			m_bStopThread;
	HANDLE			m_hEvent;
	TCHAR			m_Name[100];
	
protected:
	HANDLE			m_hThread;
	HANDLE			m_hOnExitEvent;
	CThreadClient*	m_pClient;
	int				m_ThreadNum;
	DWORD			m_nThreadID;

	static UINT __stdcall ThreadFunc(LPVOID pContext);
};
#endif // !defined(AFX_THREAD_H__82E3AAC7_30E7_4556_861C_BC085BE8ACB8__INCLUDED_)
