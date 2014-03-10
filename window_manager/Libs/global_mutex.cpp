// global_mutex.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "global_mutex.h"

CGlobalMutex::CGlobalMutex()
{
	m_hMutex = NULL;
}

CGlobalMutex::~CGlobalMutex()
{
	CloseMutex();
}

BOOL CGlobalMutex::OpenMutex(LPCTSTR Name)
{
	m_hMutex = CreateMutex(NULL, FALSE, Name);
	return m_hMutex != NULL;
}

BOOL CGlobalMutex::CloseMutex()
{
	if(m_hMutex != NULL)
		return CloseHandle(m_hMutex);
	else
		return FALSE;
}

BOOL CGlobalMutex::Lock(DWORD Timeout)
{
    // Request ownership of mutex.
	DWORD dwWaitResult = WaitForSingleObject(m_hMutex, Timeout);
	return dwWaitResult == WAIT_OBJECT_0;
}

BOOL CGlobalMutex::Unlock()
{
	return ReleaseMutex(m_hMutex);
}
