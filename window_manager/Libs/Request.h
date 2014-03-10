// Request.h: interface for the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUEST_H__24C6C3A5_E988_4CEA_AC70_CECAF9E9E4FC__INCLUDED_)
#define AFX_REQUEST_H__24C6C3A5_E988_4CEA_AC70_CECAF9E9E4FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define REQ_PREFIX "##"
#define REQ_PREFIX_SIZE 2

#define REQ_POSTFIX "$$"
#define REQ_POSTFIX_SIZE 2

#pragma pack(push, 1)

struct REQUEST_HEADER
{
	BYTE	m_Prefix[2];
	int		m_iPacketLength;
	int		m_iConnectType;
	DWORD	m_dwStationId;
	int		m_iCommand;
	int		m_iDataLen;
};

#define REQUEST_HEADER_SIZE sizeof(REQUEST_HEADER)

//#ifdef _DEBUG
#define MAX_DATA_SIZE 0xFFFF
//#else // _DEBUG
//#define MAX_DATA_SIZE (1L*1024*1024-1)
//#endif // _DEBUG


struct REQUEST : public REQUEST_HEADER
{
	//BYTE	m_pData[MAX_DATA_SIZE];
	PBYTE	m_pDataMAX_DATA_SIZE;
	BYTE	m_Reserve[2];		// For posfix
};

#pragma pack(pop)

class CRequest : public REQUEST
{
public:
	CRequest();
	CRequest(const CRequest &req);
	CRequest& operator =(CRequest &req);
	BOOL operator ==(CRequest &req);

	~CRequest();

	void Clear();
	void SetDataRAW(LPVOID pData, int len);
	void SetData(LPVOID pData, int len);
	void SetData(CString strToSend);
	void SetDataWithExtra(CString strToSend, void* extra, int extraLen);

	BOOL GetData(PVOID* pdata, int* len);
	BOOL GetBinaryData(PVOID* pdata, int* len);
	CString GetDataAsString();

	char* FindPacketStartAndLength(char* bufData, int len, OUT int* pPacketLength);

// 	BOOL ParseRequest(char* bufRequest, int len);
// 	int ParseRequestHeader(char* bufRequest, int len);
	int CreateRequest(char* bufRequest, int len);
	int CreateHeader(char* bufRequest, int len);
	
	LPVOID GetPacket();
	DWORD GetPacketSize() { return REQUEST_HEADER_SIZE + m_iDataLen + REQ_POSTFIX_SIZE; }

	CString GetErrorDescrition();
};

#endif // !defined(AFX_REQUEST_H__24C6C3A5_E988_4CEA_AC70_CECAF9E9E4FC__INCLUDED_)
