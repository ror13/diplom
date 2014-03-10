#include "stdafx.h"

#include "RecvRequestHelper.h"

#include <tchar.h>

#include "Commands.h"

extern "C" const TCHAR * get_message_string(int m);

void _hlpTRACERecvRequest( CString Title, CRequest req)
{
#ifndef TRACE_PING_PONG
	if (req.m_iCommand == MESSAGE_TYPE_PING)
	{
		return;
	}
#endif	
	TRACE(_T("%s\n"), Title);
	TRACE(_T("\tm_Prefix %.2X%.2X\n"), req.m_Prefix[0], req.m_Prefix[1]);
	TRACE(_T("\tm_iPacketLength %d\n"), req.m_iPacketLength);
	TRACE(_T("\tm_iConnectType %d\n"), req.m_iConnectType);
	TRACE(_T("\tm_dwStationId %d\n"), req.m_dwStationId);
	TRACE(_T("\tm_iCommand %s\n"), get_message_string(req.m_iCommand));
	TRACE(_T("\tm_iDataLen %d\n"), req.m_iDataLen);

	/*
	BYTE	m_Prefix[2];		m_Prefix[2];
	int		m_iPacketLength;m_iPacketLength;
	int		m_iConnectType;m_iConnectType;
	DWORD	m_dwStationId;m_dwStationId;
	int		m_iCommand;m_iCommand;
	int		m_iDataLen;m_iDataLen;
	*/
}