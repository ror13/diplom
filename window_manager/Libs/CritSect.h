// CritSect.h: interface for the CCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITSECT_H__C41DF892_1BF9_11D2_B323_00609766FAEF__INCLUDED_)
#define AFX_CRITSECT_H__C41DF892_1BF9_11D2_B323_00609766FAEF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCritSect
{
public:
	CCritSect();
	~CCritSect();

	int					m_Check;
	HANDLE				m_Mutex;
//	CRITICAL_SECTION m_CriticalSection;
	
	BOOL Lock(int Timeout = INFINITE);
	BOOL Unlock();

	operator HANDLE(){return m_Mutex;}
};

BOOL MultiLock(CCritSect **Sections, int Count, int Timeout = INFINITE);
BOOL MultiUnLock(CCritSect **Sections, int Count);

class CCritSectLock
{
public:
	CCritSectLock(CCritSect* pCritSect);
	~CCritSectLock();

	CCritSect* m_pCritSection;
};

#endif // !defined(AFX_CRITSECT_H__C41DF892_1BF9_11D2_B323_00609766FAEF__INCLUDED_)
