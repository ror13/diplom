//////////////////////////////////////////////////////////////
// File:		// WorkingThread.cpp
// File time:	// 1.02.2006	17:18
// Description: //////////////////////////////////////////////
// 
//

#include "stdafx.h"
//#include <windows.h>
#include "WorkingThread.h"

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

CWorkingThread::CWorkingThread( int MaxThreads )
{
	m_iMaxThreads = 0;
	m_bClosing = FALSE;
	m_pThreads = NULL;

	Init( MaxThreads );
}

//////////////////////////////////////////////////////////////

CWorkingThread::~CWorkingThread()
{
	UninitializeThreads();
	delete[] m_pThreads;
	m_pThreads = NULL;
}

//////////////////////////////////////////////////////////////

void CWorkingThread::Init( int MaxThreads )
{
	if( MaxThreads == e_LATER_INIT ||
		MaxThreads == e_INITIALIZED_AT_CONSTRUCTOR )
	{
		return;
	}

	delete[] m_pThreads;
	m_iMaxThreads = 0;
	m_pThreads = new THREAD_INFO[MaxThreads];

	if( m_pThreads != NULL )
	{
		m_iMaxThreads = MaxThreads;

		for( int i=0; i<m_iMaxThreads; i++ )
		{
			THREAD_INFO& info = m_pThreads[i];
			info.m_that = this;
			info.m_iIndex = i;
			info.m_hThread = NULL;
			info.m_dwThreadId = 0;
		}
	}
}

//////////////////////////////////////////////////////////////

LONG CWorkingThread::InitializeThreads( int MaxThreads )
{
	StopWorkingThread();
	Init( MaxThreads );

	for( int i=0; i<m_iMaxThreads; i++ )
	{
		THREAD_INFO& info = m_pThreads[i];

		DWORD id = 0;
		info.m_hThread = CreateThread( NULL, 0, WorkingThreadStatic, &info, 0, &id );
		if( info.m_hThread == NULL )
		{
			//TRACE( _T("CreateThread(WorkingThreadStatic) failed with error %d!\n"), GetLastError() );
			return GetLastError();
		}

		info.m_dwThreadId = id;
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////

LONG CWorkingThread::UninitializeThreads()
{
	StopWorkingThread();
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////

unsigned long __stdcall CWorkingThread::WorkingThreadStatic( PVOID pArg )
{
	unsigned long res = 0;
	THREAD_INFO* pInfo = (THREAD_INFO*) pArg;

	res = pInfo->m_that->WorkingThread( pInfo->m_iIndex );

	return res;
}

//////////////////////////////////////////////////////////////

void CWorkingThread::StopWorkingThread()
{
	if( m_pThreads == NULL )
	{
		return;
	}

	m_bClosing = TRUE;

	for( int i=0; i<m_iMaxThreads; i++ )
	{
		THREAD_INFO& info = m_pThreads[i];

		if( info.m_hThread != NULL )
		{
			WaitForSingleObject( info.m_hThread, INFINITE );
			CloseHandle( info.m_hThread );
			info.m_hThread = NULL;
		}
	}

	m_bClosing = FALSE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
