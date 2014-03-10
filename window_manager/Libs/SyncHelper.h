// SyncHelper.h: interface for the CSyncConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCHELPER_H__321ED8CC_5F67_42AD_B206_5335F089403E__INCLUDED_)
#define AFX_SYNCHELPER_H__321ED8CC_5F67_42AD_B206_5335F089403E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "product.h"

#include "SerializeStruct.h"
#include "SyncConfig.h"

#include "UDPSocket.h"

// maximum time for wait ont sync id from master
#define WAIT_FOR_EVENT_TIMEOUT	10000
// count for one our for wait master sync event
#define WAIT_FOR_EVENT_COUNTS	(1*60*60/(WAIT_FOR_EVENT_TIMEOUT/1000))
// maximum time for wait ont sync id from master in OnUpdate
#define WAIT_FOR_EVENT_TIMEOUT_IN_ON_UPDATE	500

// max wait time on master for slave sync
#define WAIT_FOR_SLAVE_SYNC_ON_MASTER	5

#define STR_SYNC_PORT_LEN	8
#define MIN_SYNC_PORT	20010
#define MAX_SYNC_PORT	20099

int GetSystemSyncClientMode();
BOOL SendUDPTo(PBYTE Data, ULONG DataLength, ULONG port, struct in_addr to_sin_addr);
BOOL SendUDPBroadcast(PBYTE Data, ULONG DataLength, ULONG port);
BOOL SendMasterUDPStart(CString SyncID);
BOOL SendSlaveUDPStart(CString SyncID, struct in_addr master_sin_addr);
BOOL RecvMasterUDPStart(struct in_addr *sin_addr, BOOL *bCorrectMessage);
BOOL RecvSlaveUDPStart(struct in_addr *sin_addr, BOOL *bCorrectMessage);


// class CSyncNetClient : public CUDPSocketClient
// {
// public:
// 	virtual void OnUDPRecv(int UDPSockNum, char *pBuf, int BufSize, sockaddr_in *pFromAddr);
// };
// 


#endif // !defined(AFX_SYNCHELPER_H__321ED8CC_5F67_42AD_B206_5335F089403E__INCLUDED_)
