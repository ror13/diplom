// UDPSocket.h: interface for the CUDPSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSOCKET_H__81D200B9_6DAA_4A3A_A0F3_A19BA5E14B9C__INCLUDED_)
#define AFX_UDPSOCKET_H__81D200B9_6DAA_4A3A_A0F3_A19BA5E14B9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "thread.h"

class CUDPSocketClient
{
public:
	virtual void OnUDPRecv(int UDPSockNum, char *pBuf, int BufSize, sockaddr_in *pFromAddr) = 0;
};

class CUDPSocket : public CThreadClient
{
public:
	CUDPSocket();
	~CUDPSocket();

	BOOL Create(int nPort, CUDPSocketClient *pClient = NULL, int UDPSockNum = 0, BOOL bBroadcast = FALSE, u_long BindAddr = INADDR_ANY);
	BOOL Stop();

	BOOL Send(sockaddr_in *pToAddr, char *pBuf, int BufSize);
	BOOL Send(u_long ToAddr, int nPort, char *pBuf, int BufSize);
	BOOL Send(LPCTSTR ToAddr, int nPort, char *pBuf, int BufSize);

protected:
	int					m_UDPSockNum;
	CUDPSocketClient	*m_pClient;
	SOCKET				m_hSocket;
	CThread				m_Thread;

	// CThreadClient
	virtual UINT OnThread(int ThreadNum);
};

#endif // !defined(AFX_UDPSOCKET_H__81D200B9_6DAA_4A3A_A0F3_A19BA5E14B9C__INCLUDED_)
