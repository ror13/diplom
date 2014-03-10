
#include "StdAfx.h"
#include "RemoteThread.h"

#include "Commands.h"
#include "SessionInfo.h"
#include "SessionInfoDef.h"
#include "OptionsMap.h"

CRemoteThread::CRemoteThread(SOCKET hSocket, int Port, CStationID id, int ConnectID, ISendMsg *msgSend)
{
	m_hSocket = hSocket;
	m_Port = Port;
	m_dwStationID = id;
	m_ConnectID = ConnectID;
	m_msgSend = msgSend;
	m_Data = NULL;
	m_DataLen = 0;
	m_DataMessageID = 0;
}

CRemoteThread::~CRemoteThread()
{
	m_Thread.WaitStopThread();
	if(m_DataLen > 0)
		free(m_Data);
}

int CRemoteThread::GetConnectID()
{
	return m_ConnectID;
}

BOOL CRemoteThread::IsActive()
{
	return m_Thread.IsActive();
}

void CRemoteThread::Create()
{
	m_Thread.Create(this);
}

void CRemoteThread::Stop()
{
	if(m_Thread.IsActive())
		m_Thread.SetStop();
}

CStationID CRemoteThread::GetStationID()
{
	return m_dwStationID;
}

void CRemoteThread::SetData(char *data, int len)
{
	m_DataMutex.Lock();
	if(m_Data)
		m_Data = (char *)realloc(m_Data, len + m_DataLen);
	else
		m_Data = (char *)malloc(len);
	memcpy(m_Data + m_DataLen, data, len);
	m_DataLen = m_DataLen + len;
	m_DataMutex.Unlock();
}

void CRemoteThread::OnRecvRequest(CRequest &req)
{
	m_DataMutex.Lock();
	TRACE(_T("CRemoteThread::OnRecvRequest\n"));
	switch (req.m_iCommand)
	{
	case MESSAGE_TYPE_TCPSTREAM_OPEN:
		TRACE(_T("MESSAGE_TYPE_TCPSTREAM_OPEN\n"));
		break;
	case MESSAGE_TYPE_TCPSTREAM_OPEN_SUCCESS:
		break;
	case MESSAGE_TYPE_TCPSTREAM_OPEN_ERROR:
	case MESSAGE_TYPE_TCPSTREAM_CLOSE:
		TRACE(_T("CRemoteThread::OnRecvRequest MESSAGE_TYPE_TCPSTREAM_CLOSE\n"));
		if(m_Thread.IsActive())
		{
			m_Thread.SetStop();
		}
		break;
	case MESSAGE_TYPE_TCPSTREAM_CLOSE_SUCCESS:
	case MESSAGE_TYPE_TCPSTREAM_CLOSE_ERROR:
	case MESSAGE_TYPE_TCPSTREAM_DATA_SUCCESS:
	case MESSAGE_TYPE_TCPSTREAM_DATA_ERROR:
		break;
	case MESSAGE_TYPE_TCPSTREAM_DATA:
		TRACE(_T("CRemoteThread::SetData MESSAGE_TYPE_TCPSTREAM_DATA\n"));
		void *buf = NULL;
		int len;
		if(req.GetBinaryData(&buf, &len))
		{
			SetData((char*)buf, len);

			TRACE(_T("CRemoteThread::SetData DATALEN = %d\n"), len);

// #ifdef 0//_DEBUG
#if 0
			{
				int nlenHEX, nlenASCII;
				nlenHEX = nlenASCII = len;
				int nposHEX, nposASCII;
				nposHEX = nposASCII = 0;
				unsigned char* _buff = (unsigned char*)buf;
				while (nlenHEX)
				{
					for (int i=0; i<16; i++)
					{
						if (nlenHEX)
						{
							TRACE(_T("%.2X "), _buff[nposHEX]);
						}
						else
						{
							TRACE(_T("   "));
						}
						nposHEX++;
						nlenHEX--;
					}
					TRACE(_T(" "));
					for (i=0; i<16; i++)
					{
						if (nlenASCII)
						{
							if (isprint(_buff[nposASCII]))
							{
								TRACE(_T("%c"), _buff[nposASCII]);
							}
							else
							{
								TRACE(_T("."));
							}
						}
						else
						{
							TRACE(_T("   "));
						}
						nposASCII++;
						nlenASCII--;
					}
					TRACE(_T("\n"));
				}
			}
#endif
		}

		COptionsMap map;
		map.FromString(req.GetDataAsString());
		int messageID = map.GetOptionInt(KEY_CLI_REMOTE_TCP_MESSAGE_ID);

		CRequest reqSend;
		COptionsMap reqMap;
		
		reqSend.m_iCommand = MESSAGE_TYPE_TCPSTREAM_DATA_SUCCESS;
		reqSend.m_dwStationId = m_dwStationID;
		reqSend.m_iConnectType = CONNECT_TYPE_STATION_MNGR;
		reqMap.SetOption(KEY_CLI_REMOTE_TCP_CONNECT_ID, m_ConnectID);
		reqMap.SetOption(KEY_CLI_REMOTE_TCP_MESSAGE_ID, messageID);
		reqSend.SetData(reqMap.ToString());
		m_msgSend->SendMsg(reqSend);

		TRACE(_T("CRemoteThread::OnRecvRequest m_dwStationId %d\n"), m_dwStationID);
		TRACE(_T("CRemoteThread::OnRecvRequest m_ConnectID %d\n"), m_ConnectID);
		TRACE(_T("CRemoteThread::OnRecvRequest messageID %d\n"), messageID);

		break;
	}
	m_DataMutex.Unlock();
}

UINT CRemoteThread::OnThread(int ThreadNum)
{
	int nCount;
	fd_set readfds, writefds;
	
	CRequest reqSend;
	COptionsMap reqMap;

	reqSend.m_iCommand = MESSAGE_TYPE_TCPSTREAM_OPEN;
	reqSend.m_dwStationId = m_dwStationID;
	reqSend.m_iConnectType = CONNECT_TYPE_STATION_MNGR;
	reqMap.SetOption(KEY_CLI_REMOTE_TCP_CONNECT_ID, m_ConnectID);
	reqMap.SetOption(KEY_CLI_REMOTE_TCP_PORT, m_Port);
	reqSend.SetData(reqMap.ToString());
	m_msgSend->SendMsg(reqSend);

	while(!m_Thread.IsStopped())
	{
		timeval timeOut = {0, 1};

		FD_ZERO(&writefds);
		FD_ZERO(&readfds);
		FD_SET(m_hSocket, &readfds);

		if(m_DataLen > 0)
		{
			FD_SET(m_hSocket, &writefds);
		}
		
		nCount = select(m_hSocket + 1, &readfds, &writefds, NULL, &timeOut);
		
		if(nCount == SOCKET_ERROR)
		{
			TRACE(_T("CRemoteThread::OnThread error in select [%d]\n"), WSAGetLastError());
			break;					// Error - break thread
		}
		
		if(nCount == 0)
			continue;				// select timeout
		
		if(FD_ISSET(m_hSocket, &readfds))
		{
			// reading from socket
			char buf[16000];
			
			int res = recv(m_hSocket, buf, sizeof(buf), 0);
			if(res == SOCKET_ERROR || res == 0)
			{
				int error = WSAGetLastError();
				TRACE(_T("CRemoteThread::OnThread error in recv [%d]\n"), WSAGetLastError());
				CRequest req;
				req.m_iCommand = MESSAGE_TYPE_TCPSTREAM_CLOSE;
				req.m_dwStationId = m_dwStationID;
				req.m_iConnectType = CONNECT_TYPE_STATION_MNGR;
				reqMap.SetOption(KEY_CLI_REMOTE_TCP_CONNECT_ID, m_ConnectID);
				reqMap.SetOption(KEY_CLI_REMOTE_TCP_PORT, m_Port);
				req.SetData(reqMap.ToString());
				m_msgSend->SendMsg(req);
				break;
			}

			CRequest req;
			m_DataMessageID++;
			req.m_iCommand = MESSAGE_TYPE_TCPSTREAM_DATA;
			req.m_dwStationId = m_dwStationID;
			req.m_iConnectType = CONNECT_TYPE_STATION_MNGR;
			reqMap.SetOption(KEY_CLI_REMOTE_TCP_CONNECT_ID, m_ConnectID);
			reqMap.SetOption(KEY_CLI_REMOTE_TCP_MESSAGE_ID, m_DataMessageID);

			req.SetDataWithExtra(reqMap.ToString(), buf, res);
			m_msgSend->SendMsg(req);
		}

		if(FD_ISSET(m_hSocket, &writefds))
		{
			// writing to socket
			m_DataMutex.Lock();
			int res = send(m_hSocket, m_Data, m_DataLen, 0);

			if (res == SOCKET_ERROR)
			{
				TRACE(_T("CRemoteThread::OnThread error in send [%d]\n"), WSAGetLastError());
			}

			if(res != m_DataLen)
			{
				m_DataLen = m_DataLen - res;
				memmove(m_Data, m_Data + res, m_DataLen);
			}
			else
			{
				m_DataLen = 0;
			}

			m_DataMutex.Unlock();			
		}
	}

	TRACE(_T("CRemoteThread::OnThread exit\n"));

	reqSend.m_iCommand = MESSAGE_TYPE_TCPSTREAM_CLOSE;
	reqSend.m_dwStationId = m_dwStationID;
	reqSend.m_iConnectType = CONNECT_TYPE_STATION_MNGR;
	reqMap.SetOption(KEY_CLI_REMOTE_TCP_CONNECT_ID, m_ConnectID);
	reqMap.SetOption(KEY_CLI_REMOTE_TCP_PORT, m_Port);
	reqSend.SetData(reqMap.ToString());
	m_msgSend->SendMsg(reqSend);

	closesocket(m_hSocket);
	return 0;
}
