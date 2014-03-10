// SockClient.h
//////////////////////////////////////////////////////////////////////////

#ifndef _SockClient_H_
#define _SockClient_H_

#include "Thread.h"

class CSockClient
{
public:
	virtual void OnSockAccept(int Port, SOCKET hSocket, SOCKADDR_IN *pAddrFrom) = 0;
	virtual void OnSockConnect(int Port, SOCKET hSocket, SOCKADDR_IN *pAddrFrom, int ConnectID) = 0;
};

#endif //_SockClient_H_
