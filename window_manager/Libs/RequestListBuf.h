// RequestListBuf.h: interface for the CRequestListBuf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTLISTBUF_H__39C6DA52_D378_47A3_A5F6_869C78E9FB46__INCLUDED_)
#define AFX_REQUESTLISTBUF_H__39C6DA52_D378_47A3_A5F6_869C78E9FB46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
//#include "ScheduleServerWorker.h"
#include "Request.h"

typedef std::list<CRequest*> REQ_LIST;

class CRequestListBuf  
{
public:
	CRequestListBuf();
	virtual ~CRequestListBuf();

	void Push(CRequest &req);
	BOOL Pop(CRequest &req);
	
	CRequest* GetFirst();
	CRequest* GetNext();
	CRequest* RemoveCurrentAndGetNext();
	LONG GetCount();

	operator HANDLE() {return m_hEvent;}

	BOOL Wait(int Timeout = 1000);

	BOOL Lock(int Timeout = INFINITE);
	BOOL Unlock();

	BOOL RemoveSpecifiedCommand(int iCommand);
	BOOL RemoveAll();
protected:
	HANDLE		m_hMutex;
	HANDLE		m_hEvent;

	REQ_LIST	m_List;
	REQ_LIST::iterator	m_CurItem;
};

class CRequestBuf  
{
public:
	CRequestBuf();
	virtual ~CRequestBuf();
	
	BOOL PushAndWaitEmpty(CRequest &req, DWORD Timeout = INFINITE);
	BOOL Pop(CRequest **preq);
	BOOL ReleaseBuf();
	
	operator HANDLE() {return m_hEvent;}
	
	BOOL Wait(int Timeout = 1000);
	
	BOOL Lock(int Timeout = INFINITE);
	BOOL Unlock();

	BOOL RemoveSpecifiedCommand(int iCommand);
	BOOL RemoveAll();

protected:
	HANDLE		m_hMutex;
	HANDLE		m_hEvent;
	HANDLE		m_hEmptyEvent;
	
	CRequest	*m_pItem;
};

#endif // !defined(AFX_REQUESTLISTBUF_H__39C6DA52_D378_47A3_A5F6_869C78E9FB46__INCLUDED_)
