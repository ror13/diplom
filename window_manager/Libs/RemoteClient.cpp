
#include "StdAfx.h"
#include "RemoteClient.h"

CRemoteClient::CRemoteClient()
{
}

CRemoteClient::~CRemoteClient()
{
}

void CRemoteClient::SetRemoteLink(int port, CStationID id, ISendMsg *msgSend)
{
	m_RemotePort = port;
	m_StationID = id;
	m_msgSend = msgSend;
}

void CRemoteClient::StopRemoteThreadsIfClientDisconnect(const CSessionArray &arr)
{
	std::list<CRemoteThread *>::iterator i;
	int kol;
	for (i = m_lstRemoteThreads.begin(); i != m_lstRemoteThreads.end(); i++)
	{
		kol = 0;
		for(int j = 0; j < arr.GetSize(); j++)
		{
			if((*i)->GetStationID() == arr[j].m_StationId)
				break;
			kol++;
		}

		if(kol == arr.GetSize())
			(*i)->Stop();
	}
}

BOOL CRemoteClient::OnRecvRequest(CRequest &req)
{
	switch (req.m_iCommand)
	{
	case MESSAGE_TYPE_TCPSTREAM_OPEN_SUCCESS:
	case MESSAGE_TYPE_TCPSTREAM_OPEN_ERROR:
	case MESSAGE_TYPE_TCPSTREAM_CLOSE:
	case MESSAGE_TYPE_TCPSTREAM_CLOSE_SUCCESS:
	case MESSAGE_TYPE_TCPSTREAM_CLOSE_ERROR:
	case MESSAGE_TYPE_TCPSTREAM_DATA:
	case MESSAGE_TYPE_TCPSTREAM_DATA_SUCCESS:
	case MESSAGE_TYPE_TCPSTREAM_DATA_ERROR:
		switch (req.m_iCommand)
		{
		case MESSAGE_TYPE_TCPSTREAM_CLOSE:
			TRACE(_T("CRemoteClient::OnRecvRequest MESSAGE_TYPE_TCPSTREAM_CLOSE\n"));
			break;
		}
		COptionsMap reqMap;
		CString str = req.GetDataAsString();
		reqMap.FromString(str);
		int id = reqMap.GetOptionInt(KEY_CLI_REMOTE_TCP_CONNECT_ID);

		TRACE(_T("CRemoteClient::OnRecvRequest id = %d\n"), id);
		TRACE(_T("CRemoteClient::OnRecvRequest StationId = %d\n"), req.m_dwStationId);

		std::list<CRemoteThread *>::iterator i;
		for (i = m_lstRemoteThreads.begin(); i != m_lstRemoteThreads.end(); i++)
		{
			TRACE(_T("ConnectID = %d"), (*i)->GetConnectID());
			if ((*i)->GetConnectID() == id)
			{
				(*i)->OnRecvRequest(req);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CRemoteClient::OnSockAccept(int Port, SOCKET hSocket, SOCKADDR_IN *pAddrFrom)
{
	//we got the socket from listening socket
	static int ConnectID = 0;
	ConnectID++;
	CRemoteThread* thr = new CRemoteThread(hSocket, m_RemotePort, m_StationID, ConnectID, m_msgSend);
	std::list<CRemoteThread *>::iterator i;
	for (i = m_lstRemoteThreads.begin(); i != m_lstRemoteThreads.end(); i++)
	{
		if(!(*i)->IsActive())
		{
			delete *i;
			m_lstRemoteThreads.erase(i);
			break;
		}
	}
	m_lstRemoteThreads.push_back(thr);
	thr->Create();
}

void CRemoteClient::OnSockConnect(int Port, SOCKET hSocket, SOCKADDR_IN *pAddrFrom, int ConnectID)
{
	//we got the socket from connected socket
	CRemoteThread* thr = new CRemoteThread(hSocket, m_RemotePort, m_StationID, ConnectID, m_msgSend);
	std::list<CRemoteThread *>::iterator i;
	for (i = m_lstRemoteThreads.begin(); i != m_lstRemoteThreads.end(); i++)
	{
		if(!(*i)->IsActive())
		{
			delete *i;
			m_lstRemoteThreads.erase(i);
			break;
		}
	}
	m_lstRemoteThreads.push_back(thr);
	thr->Create();
}
