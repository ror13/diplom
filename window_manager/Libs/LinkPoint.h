// LinkPoint.h

#ifndef _LINK_POINT_H_
#define _LINK_POINT_H_

#include "Request.h"


#define SEND_TIMEOUT 3
#define RECV_TIMEOUT 9


class CLinkPointCallback
{
public:
	virtual void OnLinkConnect() = 0;
	virtual void OnRecvRequest(CRequest &req) = 0;
	virtual void OnLinkDisconnect() = 0;
};

class CLinkPoint
{
public:
	CLinkPoint() {m_pCallback = NULL;}

	virtual BOOL SendMsg(CRequest& req) = 0;
	virtual BOOL SendMsgLoPriority(CRequest& req) = 0;
	virtual BOOL SendMsgAndWaitReply(CRequest& reqSend, CRequest& reqRecv, int *WaitMsg = NULL, int Timeout = 30000) = 0;

	void SetCallback(CLinkPointCallback *pCallback) {m_pCallback = pCallback;}

protected:
	CLinkPointCallback		*m_pCallback;
};

#endif //_LINK_POINT_H_