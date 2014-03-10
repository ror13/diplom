//////////////////////////////////////////////////////////////
// File:		// WorkingThread.h
// File time:	// 1.02.2006	17:18
// Description: //////////////////////////////////////////////
// 
//

#ifndef _WORKINGTHREAD_H_UID0000009B5C5BAA3C
#define _WORKINGTHREAD_H_UID0000009B5C5BAA3C

//////////////////////////////////////////////////////////////

//#include "Synchro.h"

//////////////////////////////////////////////////////////////

class CWorkingThread
{
public:

	enum { e_LATER_INIT = -1, e_INITIALIZED_AT_CONSTRUCTOR = -1 };
	
	CWorkingThread( int MaxThreads = e_LATER_INIT );
	virtual ~CWorkingThread();

	LONG InitializeThreads( int MaxThreads = e_INITIALIZED_AT_CONSTRUCTOR );
	LONG UninitializeThreads();

//	CInterlockedScalar<BOOL>	m_bClosing;
//	CWhenZero<BOOL>				m_bClosing;
	volatile BOOL	m_bClosing;

protected:

	virtual int WorkingThread( int ThreadIndex ) = 0;
	virtual void StopWorkingThread();

private:
	static unsigned long __stdcall WorkingThreadStatic( PVOID pArg );
	void Init( int MaxThreads );

	typedef struct _THREAD_INFO
	{
		CWorkingThread*	m_that;
		int		m_iIndex;
		HANDLE	m_hThread;
		DWORD	m_dwThreadId;
	} THREAD_INFO, *PTHREAD_INFO;

	int				m_iMaxThreads;
	PTHREAD_INFO	m_pThreads;
};

//////////////////////////////////////////////////////////////

#endif //ifndef _WORKINGTHREAD_H_UID0000009B5C5BAA3C
