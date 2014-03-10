
#include "ISendMsg.h"
#include "Thread.h"

#include "StationInfo.h"

class CRemoteThread: public CThreadClient
{
	int			m_Error;
	CThread		m_Thread;
	SOCKET		m_hSocket;

	int			m_Port;
	CStationID	m_dwStationID;
	int			m_ConnectID;

	char		*m_Data;
	int			m_DataLen;
	CMutex		m_DataMutex;

	ISendMsg	*m_msgSend;

	int			m_DataMessageID;

public:
	CRemoteThread(SOCKET hSocket, int RemotePort, CStationID id, int ConnectID, ISendMsg *msgSend);
	~CRemoteThread();

	void OnRecvRequest(CRequest &req);
	int GetConnectID();
	void Create();
	void Stop();
	CStationID GetStationID();
	void SetData(char *data, int len);
	BOOL IsActive();

protected:
	virtual UINT OnThread(int ThreadNum);
};