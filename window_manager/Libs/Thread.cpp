// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thread.h"

#include <process.h>

#ifndef TRACE
#define TRACE printf
#endif // TRACE

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CDebugLog g_LogThreads;

CThread::CThread()
{
	m_hThread = NULL;
	m_pClient = NULL;
	m_ThreadNum = 0;
	m_bStopThread = TRUE;
	m_nThreadID = 0;
	_tcscpy(m_Name, _T("unnamed"));

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hOnExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CThread::~CThread()
{
	WaitStopThread();
	CloseHandle(m_hEvent);
	CloseHandle(m_hOnExitEvent);
	if(m_nThreadID == GetCurrentThreadId())
	{
		TRACE(_T("\nThread %s self delete! Destroy othre objects may by don't complete\n"), 
			m_Name);
		_endthread();
	}
}

void CThread::SetName(TCHAR* n)
{
#ifdef _DEBUG
	_tcscpy(m_Name, n);
#endif
}

UINT __stdcall CThread::ThreadFunc(LPVOID pContext)
{
	CThread* pThis = (CThread*)pContext;

	UINT res;

	res = pThis->m_pClient->OnThread(pThis->m_ThreadNum);

// 	__try
// 	{
// 		res = pThis->m_pClient->OnThread(pThis->m_ThreadNum);
// 	}__except(EXCEPTION_EXECUTE_HANDLER)
// 	{
// 		DWORD dwExceptCode = GetExceptionCode();		
// 		pThis->ProcessException(pThis->m_Name, dwExceptCode);
// 	}
	
	pThis->m_bStopThread = TRUE;
	TRACE(_T("\nThread %s exit!\n"), pThis->m_Name);
	::SetEvent(pThis->m_hOnExitEvent);	
	return res;
}

BOOL CThread::Create(CThreadClient* pClient, int ThreadNum, int nPriority)
{
	if(pClient == NULL)
		return FALSE;

	WaitStopThread();
	
	m_pClient = pClient;
	m_ThreadNum = ThreadNum;

	m_bStopThread = FALSE;
	::ResetEvent(m_hEvent);
	::ResetEvent(m_hOnExitEvent);


	m_hThread = (HANDLE)_beginthreadex(NULL, 0x2000, ThreadFunc,
		(LPVOID)this, 0, (unsigned*)&m_nThreadID);
	
	if(m_hThread == NULL)
		return FALSE;
	
	TRACE(_T("START THREAD: ThreadNum=%d, m_hTread=0x%X, ThreadID=0x%X, Name=%s\n"),
		m_ThreadNum, m_hThread, m_nThreadID, m_Name);
	
	BOOL bSetPrior;
	bSetPrior = SetThreadPriority(m_hThread, nPriority);

	return bSetPrior;
}

BOOL CThread::IsActive()
{
	DWORD ExitCode = 0;
	if(m_hThread && GetExitCodeThread(m_hThread, &ExitCode))
	{
		if(ExitCode == STILL_ACTIVE)
			return TRUE;
	}
	return FALSE;
}

void CThread::SetStop()
{
	m_bStopThread = TRUE;
	::SetEvent(m_hEvent);
}

void CThread::WaitStopThread(int TimeOut)
{
	m_bStopThread = TRUE;
	if(m_hThread && m_nThreadID != GetCurrentThreadId())
	{
		::SetEvent(m_hEvent);
		int res = WaitForSingleObject(m_hOnExitEvent, TimeOut);
		if(res == WAIT_TIMEOUT)
		{
			TRACE(_T("\n************************* Thread %s must die! (timeout=%d)\n"), m_Name, TimeOut);
			TerminateThread(m_hThread, -10);
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

CThread::WAIT_STATUS CThread::WaitEvent(UINT TimeOut)
{
    DWORD res = WaitForSingleObject(m_hEvent, TimeOut);

	if(m_bStopThread)
		return WAIT_STATUS_THREAD_STOPPED;

	if(res == WAIT_OBJECT_0)
		return WAIT_STATUS_ON_EVENT;
	else if(res == WAIT_TIMEOUT)
		return WAIT_STATUS_TIMEOUT;

	return WAIT_STATUS_ERROR;
}


BOOL CThread::WaitUntilDone(int TimeOut)
{
	DWORD res;
	res = WaitForSingleObject(m_hOnExitEvent, TimeOut);
	return res == WAIT_OBJECT_0;
}

void CThread::Suspend()
{
	SuspendThread(m_hThread);
}

void CThread::Resume()
{
	ResumeThread(m_hThread);
}

void CThread::SetEvent()
{
	::SetEvent(m_hEvent);
}

void CThread::ResetEvent()
{
	::ResetEvent(m_hEvent);
}

void CThread::ProcessException(TCHAR *pThreadName, DWORD dwExceptCode)
{
	DWORD dwThreadId = GetCurrentThreadId();
	switch (dwExceptCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		{
			TRACE(_T("!!!!!!! EXCEPTION_ACCESS_VIOLATION:Id = %X; %s !!!!!!!\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_BREAKPOINT:
		{
			TRACE(_T("!!!!!!! EXCEPTION_BREAKPOINT: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		{
			TRACE(_T("!!!!!!! EXCEPTION_DATATYPE_MISALIGNMENT: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_SINGLE_STEP:
		{
			TRACE(_T("!!!!!!! EXCEPTION_SINGLE_STEP: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		{
			TRACE(_T("!!!!!!! EXCEPTION_ARRAY_BOUNDS_EXCEEDED: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		{
			TRACE(_T("!!!!!!!) EXCEPTION_FLT_DENORMAL_OPERAND: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		{
			TRACE(_T("!!!!!!! EXCEPTION_FLT_DIVIDE_BY_ZERO: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		{
			TRACE(_T("!!!!!!! EXCEPTION_FLT_INEXACT_RESULT: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		{
			TRACE(_T("!!!!!!! EXCEPTION_FLT_INVALID_OPERATION: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_FLT_OVERFLOW:
		{
			TRACE(_T("!!!!!!! EXCEPTION_FLT_OVERFLOW: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_FLT_STACK_CHECK:
		{
			TRACE(_T("!!!!!!! EXCEPTION_FLT_STACK_CHECK:Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_FLT_UNDERFLOW:
		{
			TRACE(_T("!!!!!!! EXCEPTION_FLT_UNDERFLOW: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		{
			TRACE(_T("!!!!!!! EXCEPTION_INT_DIVIDE_BY_ZERO: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_INT_OVERFLOW:
		{
			TRACE(_T("!!!!!!! EXCEPTION_INT_OVERFLOW: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_PRIV_INSTRUCTION:
		{
			TRACE(_T("!!!!!!! EXCEPTION_PRIV_INSTRUCTION: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		{
			TRACE(_T("!!!!!!! EXCEPTION_NONCONTINUABLE_EXCEPTION: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	default:
		{
			TRACE(_T("!!!!!!! UNKNOWN EXCEPTION CODE: Id = %X; %s\r\n!!!!!!!\r\n"), dwThreadId, pThreadName);
		}break;
	}
}

