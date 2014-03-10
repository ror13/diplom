// CritSect.cpp: implementation of the CCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CritSect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCritSect::CCritSect()
{
	m_Check	=	0;
//	InitializeCriticalSection(&m_CriticalSection);
	m_Mutex = CreateMutex(NULL, FALSE, NULL);
}

CCritSect::~CCritSect()
{
//	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_Mutex);
}

BOOL CCritSect::Lock(int Timeout)
{
//	EnterCriticalSection(&m_CriticalSection);
	DWORD res = WaitForSingleObject(m_Mutex, Timeout);	
	if(res == WAIT_OBJECT_0)
	{
		m_Check++;
		return TRUE;
	} else
		return FALSE;
}

BOOL CCritSect::Unlock()
{
	if(m_Check <= 0)
		return FALSE;
	m_Check--;
	BOOL result = ReleaseMutex(m_Mutex);
	//	LeaveCriticalSection(&m_CriticalSection);
	return result;
}

CCritSectLock::CCritSectLock(CCritSect* pCritSect)
{
	m_pCritSection = pCritSect;
	pCritSect->Lock();
}

CCritSectLock::~CCritSectLock()
{
	m_pCritSection->Unlock();
}

BOOL MultiLock(CCritSect **Sections, int Count, int Timeout)
{
	DWORD dwThreadId = GetCurrentThreadId();
	int i;
	HANDLE hs[256];
	for(i = 0; i < Count; i++)
		hs[i] = *Sections[i];
	DWORD res = WaitForMultipleObjects(Count, hs, TRUE, Timeout);
	if(res >= WAIT_OBJECT_0 && res < WAIT_OBJECT_0 + Count)
	{
		for(i = 0; i < Count; i++)
			Sections[i]->m_Check++;
		return TRUE;
	} else
		return FALSE;
}

BOOL MultiUnLock(CCritSect **Sections, int Count)
{
	int i;
	for(i = 0; i < Count; i++)
		Sections[i]->Unlock();
	return TRUE;
}