// NetLink.h: interface for the CNetLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETLINK_H__937151DA_0AB9_484B_8F1F_35B6C5533A0E__INCLUDED_)
#define AFX_NETLINK_H__937151DA_0AB9_484B_8F1F_35B6C5533A0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Request.h"
#include "LinkPoint.h"
#include "NetSock.h"
#include "Thread.h"

#include "RequestListBuf.h"

//////////////////////////////////////////////////////////////////////////

int GetFreeTCPPort();

//////////////////////////////////////////////////////////////////////////

class CNetLink : public CLinkPoint, public CNetSock
{
public:
	CNetLink();
	virtual ~CNetLink();
	
	// CLinkPoint
	virtual BOOL RemoveFileMsgs();
	virtual BOOL SendMsg(CRequest& req);
	virtual BOOL SendMsgLoPriority(CRequest& req);
	virtual BOOL SendMsgAndWaitReply(CRequest& reqSend, CRequest& reqRecv, int *WaitMsg = NULL, int Timeout = 30000);

	BOOL WaitLinkConnect(int Timeout = 1000);
	BOOL IsConnected(){return m_bConnected;}

	BOOL StopLink();

protected:
	virtual void OnSockConnect();
	virtual void OnSockRecv(char *pBuf, int Size);
	virtual void OnSockSend()						{}
	virtual void OnSockClose();

	BOOL Send(CRequest& req);
	
	BOOL		m_bConnected;
	HANDLE		m_hConnect;

	CRequest	m_RecvRequest;
// 	char		*m_pTempBuf;
// 	int			m_TempBufSize;

	// store m_pData for request
	PBYTE		m_pTempRequestData;
	// length of received m_pData
	int			m_iTempRequestDataRecv;
	// received request header for m_pData
	REQUEST_HEADER	m_tempRequestHeader;
	// if we got request header set it TRUE
	BOOL		m_bIsRequestHeaderReceived;
	
	CRequest	*m_pWaitRecvRequest;

	HANDLE		m_hRequestReaded;
	int			*m_pMsgTypesForWait;

	CRequestListBuf	m_SendReqBuf;
	CRequestBuf		m_LoPrioritySendReqBuf;

	CThread		m_SendThread;

	// CThreadClient
	virtual UINT OnThread(int ThreadNum);

	UINT OnThreadSendToClient();

	CCriticalSection	m_Crit;
	void	Lock() { m_Crit.Lock(); };
	void	Unlock() { m_Crit.Unlock(); };
};

#endif // !defined(AFX_NETLINK_H__937151DA_0AB9_484B_8F1F_35B6C5533A0E__INCLUDED_)
