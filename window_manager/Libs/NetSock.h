// NetSock.h
//////////////////////////////////////////////////////////////////////////

#ifndef _NETSOCK_H_
#define _NETSOCK_H_

#include "CritSect.h"
#include "Thread.h"

#define MAX_SERVER_NAME		256
#define RECV_BUF_SIZE		65536

#define THREAD_ID_SOCK		10

#include "arc4.h"

class CNetSock : public CThreadClient
{
	arc4_context m_EncryptRecvCtx;
	arc4_context m_EncryptSendCtx;
	BOOL m_bEncryption;

	SOCKET		m_hSocket;

public:
	CNetSock();
	virtual ~CNetSock();
	
	BOOL AttachSocket(SOCKET hSocket, SOCKADDR_IN *pClientAddress);
	BOOL CreateLink(LPCTSTR ServerName, int ServerPort);

	BOOL SendSock(char *pBuf, int Size);

	CString GetPeerAddr();
	
	void SetStopLink();
	BOOL StopLink();

	void SelectWrite(){m_bSelectWrite=TRUE;}

	SOCKADDR_IN GetAddr()							{return m_Addr;}

	BOOL EnableEncryption(const char* password, int len);
	void DisableEncryption();

protected:
	virtual void OnSockConnect()					{}
	virtual void OnSockRecv(char *pBuf, int Size)	{}
	virtual void OnSockSend()						{}
	virtual void OnSockClose()						{}

protected:
	CCritSect	m_CritSect;

	SOCKADDR_IN	m_Addr;
	
	BOOL		m_bAttached;
	BOOL		m_bSelectWrite;
	
	CString		m_ServerName;
	int			m_ServerPort;

	char		*m_pTempBuf;
	int			m_TempBufSize;
	
	int			m_Error;

	CThread		m_Thread;

	// CThreadClient
	virtual UINT OnThread(int ThreadNum);

	void OnThreadSocket();
};

#endif //_NETSOCK_H_
