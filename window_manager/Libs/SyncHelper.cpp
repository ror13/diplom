// SyncHelper.cpp: implementation of the CSyncConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SyncHelper.h"

#include "NetworkHelper.h"

#include "PresenterGlobals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined PRESENTER_CLIENT_CODE && !defined PREVIEWER
#include "pnetclient.h"
#include "PresenterClientDlg.h"

int GetSystemSyncClientMode()
{
	CPresenterClientDlg* pdlg = (CPresenterClientDlg*)AfxGetMainWnd();
	if (pdlg)
	{
		return pdlg->m_SyncClientConfig.m_SyncClientMode;
	}

	return SYNC_CLIENT_NONE;
}
#else
int GetSystemSyncClientMode()
{
	return SYNC_CLIENT_NONE;
}
#endif

static DWORD g_SyncPort = MIN_SYNC_PORT;

DWORD GetSyncPort()
{
	g_SyncPort++;
	
	if (g_SyncPort > MAX_SYNC_PORT)
	{
		g_SyncPort = MIN_SYNC_PORT;
	}
	
	return g_SyncPort;
}

TCHAR GetSystemSyncClientModeName()
{
	TCHAR cModeName = SYNC_CLIENT_ERROR_NAME;
	switch (GetSystemSyncClientMode())
	{
	case SYNC_CLIENT_NONE:
		cModeName = SYNC_CLIENT_NONE_NAME;
		break;
	case SYNC_CLIENT_MASTER:
		cModeName = SYNC_CLIENT_MASTER_NAME;
		break;
	case SYNC_CLIENT_SLAVE:
		cModeName = SYNC_CLIENT_SLAVE_NAME;
		break;
	}

	return cModeName;
}

BOOL SendUDPBroadcast(PBYTE Data, ULONG DataLength, ULONG port)
{
// 	int iRet;
// 	struct sockaddr_in si_other;
// 	int s, slen=sizeof(si_other);;
// 	
// 	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
// 	if (s == INVALID_SOCKET)
// 	{
// 		return FALSE;
// 	}
// 
// 	memset((char *) &si_other, 0, sizeof(si_other));
// 	si_other.sin_family = AF_INET;
// 	si_other.sin_port = htons(port);
// 	si_other.sin_addr.s_addr = GetLocalBroadcastIP();
// 	
// 	iRet = sendto(s, (char*)Data, DataLength, 0, (struct sockaddr*)&si_other, slen);
// 	
// 	closesocket(s);
// 
// 	return iRet != SOCKET_ERROR;

	struct in_addr to_sin_addr;
	to_sin_addr.s_addr = GetLocalBroadcastIP();

	return SendUDPTo(Data, DataLength, port, to_sin_addr);
}

BOOL SendUDPTo(PBYTE Data, ULONG DataLength, ULONG port, struct in_addr to_sin_addr)
{
	int iRet;
	struct sockaddr_in si_other;
	int s, slen=sizeof(si_other);;
	
	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		return FALSE;
	}
	
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	si_other.sin_addr = to_sin_addr;
	
	iRet = sendto(s, (char*)Data, DataLength, 0, (struct sockaddr*)&si_other, slen);
	
	closesocket(s);
	
	return iRet != SOCKET_ERROR;
}

BOOL SendMasterUDPStart(CString SyncID)
{
	BOOL bRet;

	//bRet = SendUDPBroadcast((PBYTE)UDP_TASK_START_SYNC_MASTER_MESSAGE, _tcslen(UDP_TASK_START_SYNC_MASTER_MESSAGE), UDP_TASK_START_MASTER_SYNC_PORT);
	//bRet = SendUDPBroadcast((PBYTE)UDP_TASK_START_SYNC_MASTER_MESSAGE, sizeof(UDP_TASK_START_SYNC_MASTER_MESSAGE), UDP_TASK_START_MASTER_SYNC_PORT);
	CString strSendSync;

	strSendSync.Format(_T("%c%.8d%s"), GetSystemSyncClientModeName(), GetSyncPort(), SyncID);
	TRACE(_T("SendMasterUDPStart strSendSync [%s], length [%d]\n"), strSendSync, strSendSync.GetLength());
	bRet = SendUDPBroadcast((PBYTE)((LPCTSTR)strSendSync), (strSendSync.GetLength()+1)*sizeof(TCHAR), UDP_TASK_START_MASTER_SYNC_PORT);
	
	return bRet;
}

BOOL SendSlaveUDPStart(CString SyncID, struct in_addr master_sin_addr)
{
	BOOL bRet;
	
	//bRet = SendUDPBroadcast((PBYTE)UDP_TASK_START_SYNC_SLAVE_MESSAGE, _tcslen(UDP_TASK_START_SYNC_SLAVE_MESSAGE), UDP_TASK_START_SLAVE_SYNC_PORT);
	//bRet = SendUDPBroadcast((PBYTE)UDP_TASK_START_SYNC_SLAVE_MESSAGE, sizeof(UDP_TASK_START_SYNC_SLAVE_MESSAGE), UDP_TASK_START_SLAVE_SYNC_PORT);
//	bRet = SendUDPBroadcast((PBYTE)(SyncID.GetBuffer(0)), SyncID.GetLength(), UDP_TASK_START_SLAVE_SYNC_PORT);
	CString strSendSync;
	
	strSendSync.Format(_T("%c%s"), GetSystemSyncClientModeName(), SyncID);

	if (master_sin_addr.s_addr == INADDR_BROADCAST)
	{
		bRet = SendUDPBroadcast((PBYTE)((LPCTSTR)strSendSync), (strSendSync.GetLength()+1)*sizeof(TCHAR), UDP_TASK_START_MASTER_SYNC_PORT);
	}
	else
	{
		bRet = SendUDPTo((PBYTE)((LPCTSTR)strSendSync), (strSendSync.GetLength()+1)*sizeof(TCHAR), UDP_TASK_START_MASTER_SYNC_PORT, master_sin_addr);
	}
	
	return bRet;
}

BOOL RecvMasterUDPStart(struct in_addr *sin_addr, BOOL *bCorrectMessage)
{
	int iRet = 0;

	struct sockaddr_in si_me, si_other;
	SOCKET s;
	int slen=sizeof(si_other);
	
	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		return FALSE;
	}

	if (sin_addr)
	{
		RtlZeroMemory(sin_addr, sizeof(struct in_addr));
	}
	if (bCorrectMessage)
	{
		*bCorrectMessage = FALSE;
	}
	
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(UDP_TASK_START_MASTER_SYNC_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(s, (struct sockaddr*)&si_me, sizeof(si_me));
	
	TCHAR Buff[sizeof(UDP_TASK_START_SYNC_MASTER_MESSAGE)];
	RtlZeroMemory(Buff, sizeof(Buff));
	
	fd_set readfds;
	fd_set writefds;
	//timeval TimeOut={0,100000};
	timeval TimeOut={5,0};
	int nCount;
	
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	
	FD_SET(s, &readfds);
	
	nCount = select(FD_SETSIZE, &readfds, &writefds, NULL, &TimeOut);
	
	if(nCount != SOCKET_ERROR && nCount > 0)
	{
		if(FD_ISSET(s, &readfds))
		{
			int datalen;
			
			datalen = recvfrom(s, (char *)Buff, sizeof(Buff)-sizeof(TCHAR), 0, (struct sockaddr*)&si_other, &slen);
			TRACE(_T("Data received. Len %d\n"), datalen);

			if (sin_addr)
			{
				*sin_addr = si_other.sin_addr;
			}
			if (bCorrectMessage)
			{
				*bCorrectMessage = (memcmp(Buff, UDP_TASK_START_SYNC_MASTER_MESSAGE, sizeof(UDP_TASK_START_SYNC_MASTER_MESSAGE)) == 0);
			}

			iRet = datalen;
		}
	}
	else
	{
		TRACE(_T("Data received error. %d\n"), nCount);
	}
	
	closesocket(s);

	return iRet != SOCKET_ERROR;
}

BOOL RecvSlaveUDPStart(struct in_addr *sin_addr, BOOL *bCorrectMessage)
{
	int iRet = 0;
	
	struct sockaddr_in si_me, si_other;
	SOCKET s;
	int slen=sizeof(si_other);
	
	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		return FALSE;
	}
	
	if (sin_addr)
	{
		RtlZeroMemory(sin_addr, sizeof(struct in_addr));
	}
	if (bCorrectMessage)
	{
		*bCorrectMessage = FALSE;
	}
	
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
//	si_me.sin_port = htons(UDP_TASK_START_SLAVE_SYNC_PORT);
	si_me.sin_port = htons(UDP_TASK_START_MASTER_SYNC_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(s, (struct sockaddr*)&si_me, sizeof(si_me));
	
	TCHAR Buff[sizeof(UDP_TASK_START_SYNC_SLAVE_MESSAGE)];
	RtlZeroMemory(Buff, sizeof(Buff));
	
	fd_set readfds;
	fd_set writefds;
	//timeval TimeOut={0,100000};
	timeval TimeOut={5,0};
	int nCount;
	
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	
	FD_SET(s, &readfds);
	
	nCount = select(FD_SETSIZE, &readfds, &writefds, NULL, &TimeOut);
	
	if(nCount != SOCKET_ERROR && nCount > 0)
	{
		if(FD_ISSET(s, &readfds))
		{
			int datalen;
			
			datalen = recvfrom(s, (char *)Buff, sizeof(Buff)-sizeof(TCHAR), 0, (struct sockaddr*)&si_other, &slen);
			TRACE(_T("Data received. Len %d\n"), datalen);
			
			if (sin_addr)
			{
				*sin_addr = si_other.sin_addr;
			}
			if (bCorrectMessage)
			{
				*bCorrectMessage = (memcmp(Buff, UDP_TASK_START_SYNC_SLAVE_MESSAGE, sizeof(UDP_TASK_START_SYNC_SLAVE_MESSAGE)) == 0);
			}
			
			iRet = datalen;
		}
	}
	else
	{
		TRACE(_T("Data received error. %d\n"), nCount);
	}
	
	closesocket(s);
	
	return iRet != SOCKET_ERROR;
}

