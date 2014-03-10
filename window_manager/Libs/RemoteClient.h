
#include <list>
#include "SockClient.h"
#include "RemoteThread.h"

#include "ISendMsg.h"
#include "SessionInfo.h"

class CRemoteClient: public CSockClient
{
public:
	CRemoteClient();
	~CRemoteClient();

	void SetRemoteLink(int port, CStationID id, ISendMsg *msgSend);
	void StopRemoteThreadsIfClientDisconnect(const CSessionArray &arr);
	BOOL OnRecvRequest(CRequest &req);

	virtual void OnSockAccept(int Port, SOCKET hSocket, SOCKADDR_IN *pAddrFrom);
	virtual void OnSockConnect(int Port, SOCKET hSocket, SOCKADDR_IN *pAddrFrom, int ConnectID);

private:
	std::list<CRemoteThread *>	m_lstRemoteThreads;
	int							m_RemotePort;
	CStationID					m_StationID;
	ISendMsg					*m_msgSend;
};