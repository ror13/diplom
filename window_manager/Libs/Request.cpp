// Request.cpp: implementation of the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Request.h"

#include "OptionsMap.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#pragma setlocale( "Russian_Russia.1251" )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRequest::CRequest()
{
	m_Prefix[0] = '#';
	m_Prefix[1] = '#';
	m_iPacketLength = 0;
	m_iConnectType = 0;
	m_dwStationId = 0;
	m_iCommand = 0;
	m_iDataLen = 0;
	m_pDataMAX_DATA_SIZE = NULL;
	m_Reserve[0] = '$';
	m_Reserve[1] = '$';
}

CRequest::~CRequest()
{
	if (m_pDataMAX_DATA_SIZE)
	{
		free(m_pDataMAX_DATA_SIZE);
		m_pDataMAX_DATA_SIZE = NULL;
		m_iDataLen = 0;
	}
	m_Prefix[0] = 0;
	m_Prefix[1] = 0;
	m_Reserve[0] = 0;
	m_Reserve[1] = 0;
}

CRequest::CRequest(const CRequest &req)
{
	m_Prefix[0] = '#';
	m_Prefix[1] = '#';
	m_iPacketLength = req.m_iPacketLength;
	m_iConnectType = req.m_iConnectType;
	m_dwStationId = req.m_dwStationId;
	m_iCommand = req.m_iCommand;
	
	m_iDataLen = req.m_iDataLen;
	m_pDataMAX_DATA_SIZE = NULL;
	if(m_iDataLen)
	{
//		m_bDoLocalFreeOnDataPtr = TRUE;
//		m_pData = (LPBYTE)LocalAlloc(LPTR, m_iDataLen);
		//		WriteLog(_T("Alloc"));
		m_pDataMAX_DATA_SIZE = (PBYTE)malloc(m_iDataLen);
		if (req.m_pDataMAX_DATA_SIZE)
		{
			memcpy(m_pDataMAX_DATA_SIZE,  req.m_pDataMAX_DATA_SIZE, m_iDataLen);
		}
	}
	m_Reserve[0] = '$';
	m_Reserve[1] = '$';
}

CRequest& CRequest::operator =(CRequest &req)
{
	m_Prefix[0] = '#';
	m_Prefix[1] = '#';
	m_iPacketLength = req.m_iPacketLength;
	m_iConnectType = req.m_iConnectType;
	m_dwStationId = req.m_dwStationId;
	m_iCommand = req.m_iCommand;
	
	m_iDataLen = req.m_iDataLen;
	m_pDataMAX_DATA_SIZE = NULL;
	if(m_iDataLen)
	{
//		m_bDoLocalFreeOnDataPtr = TRUE;
//		m_pData = (LPBYTE)LocalAlloc(LPTR, m_iDataLen);
		//		WriteLog(_T("Alloc"));
		m_pDataMAX_DATA_SIZE = (PBYTE)realloc(m_pDataMAX_DATA_SIZE, m_iDataLen);
		if (req.m_pDataMAX_DATA_SIZE)
		{
			memcpy(m_pDataMAX_DATA_SIZE,  req.m_pDataMAX_DATA_SIZE, m_iDataLen);
		}
	}
	m_Reserve[0] = '$';
	m_Reserve[1] = '$';
	
	return *this;
}

BOOL CRequest::operator ==(CRequest &req)
{
	BOOL res = 
	m_Prefix[0] == req.m_Prefix[0] &&
	m_Prefix[1] == req.m_Prefix[1] &&
	m_iConnectType == req.m_iConnectType &&
	m_dwStationId == req.m_dwStationId &&
	m_iCommand == req.m_iCommand &&
	m_Reserve[0] == req.m_Reserve[0] &&
	m_Reserve[1] == req.m_Reserve[1] &&
	m_iDataLen == req.m_iDataLen;
	if(res && m_iDataLen)
	{
		//		m_bDoLocalFreeOnDataPtr = TRUE;
		//		m_pData = (LPBYTE)LocalAlloc(LPTR, m_iDataLen);
		//		WriteLog(_T("Alloc"));
		res = memcmp(m_pDataMAX_DATA_SIZE,  req.m_pDataMAX_DATA_SIZE, m_iDataLen) == 0;
	}
	
	return res;
}

void CRequest::Clear()
{
	m_iDataLen = 0;
	m_pDataMAX_DATA_SIZE = (PBYTE)realloc(m_pDataMAX_DATA_SIZE, 0);
}

void CRequest::SetDataRAW(LPVOID pData, int len)
{
	REQUEST_HEADER* pRequestHeader = (REQUEST_HEADER*)pData;

	memcpy(m_Prefix,  pRequestHeader->m_Prefix, REQ_PREFIX_SIZE);
	m_iPacketLength = pRequestHeader->m_iPacketLength;
	m_iConnectType = pRequestHeader->m_iConnectType;
	m_dwStationId = pRequestHeader->m_dwStationId;
	m_iCommand = pRequestHeader->m_iCommand;
	m_iDataLen = pRequestHeader->m_iDataLen;


	m_pDataMAX_DATA_SIZE = (PBYTE)realloc(m_pDataMAX_DATA_SIZE, m_iDataLen);
	memcpy(m_pDataMAX_DATA_SIZE,  ((PBYTE)pData)+REQUEST_HEADER_SIZE, m_iDataLen);
	memcpy(m_Reserve,  ((PBYTE)pData)+REQUEST_HEADER_SIZE+m_iDataLen, REQ_POSTFIX_SIZE);
}

void CRequest::SetData(LPVOID pData, int len)
{
	m_iDataLen = len;
	
	if(len)
	{
		m_pDataMAX_DATA_SIZE = (PBYTE)realloc(m_pDataMAX_DATA_SIZE, m_iDataLen);
		memcpy(m_pDataMAX_DATA_SIZE,  pData, m_iDataLen);
	}
}

int GetNecessaryWideCharToMultiByteBufferLength(CString strToSend)
{
	USES_CONVERSION;

	int iDataLen = WideCharToMultiByte(CP_UTF8, 0, T2W((LPTSTR)(LPCTSTR)strToSend), -1,
        NULL, 0, NULL, NULL );

	return iDataLen;
}

void CRequest::SetData(CString strToSend)
{
	USES_CONVERSION;

	// Get necessary buffer length 
	int iDataLen = GetNecessaryWideCharToMultiByteBufferLength(strToSend);

	m_pDataMAX_DATA_SIZE = (PBYTE)realloc(m_pDataMAX_DATA_SIZE, iDataLen);

	m_iDataLen = WideCharToMultiByte(CP_UTF8, 0, T2W((LPTSTR)(LPCTSTR)strToSend), -1,
        (LPSTR)m_pDataMAX_DATA_SIZE, iDataLen, NULL, NULL );
}

void CRequest::SetDataWithExtra(CString strToSend, void* extra, int extraLen)
{
	if (extraLen == 0) 
	{
		SetData(strToSend);
		return;
	}
	USES_CONVERSION;
	
	// Get necessary buffer length 
	int iDataLen = GetNecessaryWideCharToMultiByteBufferLength(strToSend) + extraLen;

	m_pDataMAX_DATA_SIZE = (PBYTE)realloc(m_pDataMAX_DATA_SIZE, iDataLen);

	m_iDataLen = WideCharToMultiByte(CP_UTF8, 0, T2W((LPTSTR)(LPCTSTR)strToSend), -1,
        (LPSTR)m_pDataMAX_DATA_SIZE, iDataLen, NULL, NULL );
	memcpy((LPSTR)m_pDataMAX_DATA_SIZE + m_iDataLen, extra, extraLen);
	m_iDataLen += extraLen;

}


static int GetNecessaryMultiByteToWideCharBufferLength(LPCSTR strToSend, int len)
{
	USES_CONVERSION;
	
	int iDataLen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strToSend, len, NULL, 0);
	
	return iDataLen;
}

/**
  * Return pointer to internal data storage
  *
  * @pdata pointer copy to
  * @len size of buffer
  *
  * return TRUE on success
  */
BOOL CRequest::GetData(PVOID* pdata, int* len)
{
	*len = m_iDataLen;
	*pdata = m_pDataMAX_DATA_SIZE;
	return TRUE;
}

/**
  * Get Binary part of message (one thet follow \0 if any)
  */
BOOL CRequest::GetBinaryData(PVOID* pdata, int* len)
{
	int i;
	for (i = 0; i< m_iDataLen; i++) 
	{
		if ((LPCSTR)m_pDataMAX_DATA_SIZE[i] == 0)
		{
			break;
		}
	}
	if (i == m_iDataLen)
	{
		*len = 0;
		return FALSE;
	}
	*len = m_iDataLen - i - 1;
	*pdata = (void*)((LPCSTR)m_pDataMAX_DATA_SIZE  + i + 1);
	return TRUE;
}

CString CRequest::GetDataAsString()
{
	CString strResult;
	int iDataLen;
	int i = 0;

	if(m_iDataLen)
	{
		USES_CONVERSION;
		//WCHAR wszBuffer[MAX_DATA_SIZE];

		for (i = 0; i< m_iDataLen; i++) 
		{
			if ((LPCSTR)m_pDataMAX_DATA_SIZE[i] == 0)
			{
				break;
			}
		}
		iDataLen = GetNecessaryMultiByteToWideCharBufferLength((LPCSTR)m_pDataMAX_DATA_SIZE, i);

		PWCHAR pwszBuffer = (PWCHAR)malloc((iDataLen+1)*sizeof(WCHAR));
 
		//m_pDataMAX_DATA_SIZE[m_iDataLen] = '\0';
		//int Count = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)m_pDataMAX_DATA_SIZE, m_iDataLen, wszBuffer, m_iDataLen);
		int Count = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)m_pDataMAX_DATA_SIZE, i, pwszBuffer, iDataLen);
		pwszBuffer[Count] = 0;
		strResult = W2T(pwszBuffer);
		free(pwszBuffer);
	}
	return strResult;
}


char* CRequest::FindPacketStartAndLength(char* bufData, int len, OUT int* pPacketLength)
{
	//const int prefLen = REQ_PREFIX_SIZE + sizeof(int); // prefix + length of packet
	const int headerLen = REQUEST_HEADER_SIZE; // prefix + length of packet

	if(len < headerLen) return NULL;
	
	while(len > headerLen)
	{
		if(memcmp(bufData, REQ_PREFIX, REQ_PREFIX_SIZE) == 0)
		{
			if (pPacketLength)
				*pPacketLength = *(int*)(bufData + REQ_PREFIX_SIZE);
			
			//TRACE(_T("CRequest::FindPacketStartAndLength return bufData;\n"));
			return bufData;
		}
		
		// continue
		bufData++;
		len--;
	}
	
	return NULL;
}

int CRequest::CreateRequest(char* bufRequest, int len)
{
	// Request: _prefix_ProductType_StationId_dataLen_data_postfix

	// Calculate request length
	int calcLength = REQUEST_HEADER_SIZE;
	calcLength += m_iDataLen;
	calcLength += REQ_POSTFIX_SIZE;

	m_iPacketLength = calcLength;

	if (NULL == bufRequest)
		return calcLength;
	if (len < calcLength)
		return -1;
	
	int PostfixLen = REQ_POSTFIX_SIZE;
	
	//memcpy(bufRequest, (REQUEST_HEADER*)this, calcLength - PostfixLen);
	// Copy header ...
	memcpy(bufRequest, (REQUEST_HEADER*)this, REQUEST_HEADER_SIZE);
	// ... and then copy data
	memcpy(bufRequest + REQUEST_HEADER_SIZE, m_pDataMAX_DATA_SIZE, m_iDataLen);

	// at last copy postfix
	char* pBuffer = bufRequest + calcLength - PostfixLen;
	memcpy(pBuffer, REQ_POSTFIX, PostfixLen);

	return calcLength;
}

int CRequest::CreateHeader(char* bufRequest, int len)
{
	// Request: _prefix_ProductType_StationId_dataLen_data_postfix

	// Calculate request length
	int calcLength = REQUEST_HEADER_SIZE;

	m_iPacketLength = calcLength + m_iDataLen + REQ_POSTFIX_SIZE;

	if (bufRequest == NULL)
		return calcLength;
	if (len < calcLength)
		return -1;
	
	memcpy(bufRequest, (REQUEST_HEADER*)this, calcLength);

	return calcLength;
}

LPVOID CRequest::GetPacket()
{
	//m_iPacketLength = REQUEST_HEADER_SIZE + m_iDataLen + REQ_POSTFIX_SIZE;
	//memcpy(m_pData + m_iDataLen, REQ_POSTFIX, REQ_POSTFIX_SIZE);
	//return (REQUEST*)this;
	
	// new version of GetPacket returns new allocated memory
	m_iPacketLength = REQUEST_HEADER_SIZE + m_iDataLen + REQ_POSTFIX_SIZE;
	PBYTE pPacket = (PBYTE)malloc(m_iPacketLength);
	// header
	memcpy(pPacket, this, REQUEST_HEADER_SIZE);
	// data
	memcpy(pPacket + REQUEST_HEADER_SIZE, m_pDataMAX_DATA_SIZE, m_iDataLen);
	// postfix
	memcpy(pPacket + REQUEST_HEADER_SIZE + m_iDataLen, m_Reserve, REQ_POSTFIX_SIZE);

	return pPacket;
}

CString CRequest::GetErrorDescrition()
{
	CString strError = GetDataAsString();
	COptionsMap map;
	map.FromString(strError);
	
	if(map.IsOptionExists(KEY_ERROR_DESCRIPTION))
		return map.GetOption(KEY_ERROR_DESCRIPTION);
	else
		return strError;
}

//////////////////////////////////////////////////////////////////////////
// CRequestTest

#ifdef _DEBUG

class CRequestTest: public CppUnit::TestFixture
{
private:
public:
	void setUp()
	{
	}
	
	void testCopyConstructor()
	{
		CRequest req;
		req.m_iConnectType = 10;
		req.m_dwStationId = 11;
		req.m_iCommand = 12;
		req.m_iDataLen = 13;

		CRequest req2(req);

		CPPUNIT_ASSERT_EQUAL(req2.m_iConnectType, 10);
		CPPUNIT_ASSERT_EQUAL(req2.m_dwStationId, (DWORD)11);
		CPPUNIT_ASSERT_EQUAL(req2.m_iCommand, 12);
		CPPUNIT_ASSERT_EQUAL(req2.m_iDataLen, 13);
	}
	
	void testOperatorEq()
	{
		CRequest req;
		req.m_iConnectType = 10;
		req.m_dwStationId = 11;
		req.m_iCommand = 12;
		req.m_iDataLen = 13;
		
		CRequest req2;
		req2 = req;
		
		CPPUNIT_ASSERT_EQUAL(req2.m_iConnectType, 10);
		CPPUNIT_ASSERT_EQUAL(req2.m_dwStationId, (DWORD)11);
		CPPUNIT_ASSERT_EQUAL(req2.m_iCommand, 12);
		CPPUNIT_ASSERT_EQUAL(req2.m_iDataLen, 13);
	}

	void testSetData()
	{
		CRequest req;
		char *Buf1 = "testString";
		req.SetData(Buf1, strlen(Buf1)+1);

		CRequest req2;
		CString Buf2(_T("testString"));
		req2.SetData(Buf2);

		CPPUNIT_ASSERT(req.m_iDataLen == req2.m_iDataLen);
		CPPUNIT_ASSERT(memcmp(req.m_pDataMAX_DATA_SIZE, req2.m_pDataMAX_DATA_SIZE, req.m_iDataLen) == 0);

		Buf1 = "СЃС‚СЂРѕРєР° СЂСѓСЃСЃРєРѕРіРѕ С‚РµРєСЃС‚Р°";
		Buf2 = _T("строка русского текста");

		req.SetData(Buf2);
		CPPUNIT_ASSERT_EQUAL((int)strlen(Buf1)+1, req.m_iDataLen);
		CPPUNIT_ASSERT(memcmp(req.m_pDataMAX_DATA_SIZE, Buf1, req.m_iDataLen) == 0);
	}
	
	void testGetDataAsString()
	{
		CRequest req;
		CString Buf;

		Buf = _T("testStringEngl");
		req.SetData(Buf);

		CPPUNIT_ASSERT_MESSAGE("Error set / get engl string", req.GetDataAsString() == Buf);

		Buf = _T("тест конвертации русского текста");
		req.SetData(Buf);
		
		CPPUNIT_ASSERT_MESSAGE("Error set / get rus string", req.GetDataAsString() == Buf);
	}

	void testParseRequest()
	{
		BYTE Buf[] = {
			'#','#',		// REQ_PREFIX
			28,0,0,0,		// packetLength
			10,0,0,0,		// m_iConnectType
			11,0,0,0,		// m_dwStationId
			12,0,0,0,		// m_iCommand
			4,0,0,0,		// m_iDataLen
			't','e','s','t',// m_pData
			'$','$'			// REQ_POSTFIX
		};

		CPPUNIT_ASSERT_EQUAL(28, (int)sizeof(Buf));

		char *Buf2 = "test";

		//CRequest &req = *(CRequest*)Buf;
		CRequest req;
		req.SetDataRAW(Buf, sizeof(Buf));

		CPPUNIT_ASSERT_EQUAL(req.m_iConnectType, 10);
		CPPUNIT_ASSERT_EQUAL(req.m_dwStationId, (DWORD)11);
		CPPUNIT_ASSERT_EQUAL(req.m_iCommand, 12);
		CPPUNIT_ASSERT_EQUAL(req.m_iDataLen, 4);
		CPPUNIT_ASSERT(memcmp(req.m_pDataMAX_DATA_SIZE, Buf2, req.m_iDataLen) == 0);
	}

	void testParseRequestHeader()
	{
		BYTE Buf[] = {
			'#','#',		// REQ_PREFIX
			28,0,0,0,		// packetLength
			10,0,0,0,		// m_iConnectType
			11,0,0,0,		// m_dwStationId
			12,0,0,0,		// m_iCommand
			4,0,0,0,		// m_iDataLen
		};
		//CRequest &req = *(CRequest*)Buf;
		CRequest req;
		req.SetDataRAW(Buf, sizeof(Buf));
		
		CPPUNIT_ASSERT_EQUAL(req.m_iConnectType, 10);
		CPPUNIT_ASSERT_EQUAL(req.m_dwStationId, (DWORD)11);
		CPPUNIT_ASSERT_EQUAL(req.m_iCommand, 12);
		CPPUNIT_ASSERT_EQUAL(req.m_iDataLen, 4);
	}

	void testCreateRequest()
	{
		BYTE Buf[] = {
			'#','#',		// REQ_PREFIX
			29,0,0,0,		// packetLength
			10,0,0,0,		// m_iConnectType
			11,0,0,0,		// m_dwStationId
			12,0,0,0,		// m_iCommand
			5,0,0,0,		// m_iDataLen
			't','e','s','t',// m_pData
			0,
			'$','$'			// REQ_POSTFIX
			};

		CRequest req;

		req.m_iConnectType = 10;
		req.m_dwStationId = 11;
		req.m_iCommand = 12;
		req.SetData(_T("test"));


		CPPUNIT_ASSERT(req.CreateRequest(NULL, 0) == sizeof(Buf));

		BYTE Buf2[256];
		CPPUNIT_ASSERT(req.CreateRequest((char*)Buf2, sizeof(Buf2)) == sizeof(Buf));

		CPPUNIT_ASSERT(memcmp(Buf, Buf2, sizeof(Buf)) == 0);
	}
	
	void testCreateRequest_LowBuffer()
	{
		CRequest req;
		req.SetData(_T("test"));
		
		BYTE Buf2[25];
		
		CPPUNIT_ASSERT(req.CreateRequest((char*)Buf2, sizeof(Buf2)) == -1);
	}

	
	void testCreateHeader()
	{
		BYTE Buf[] = {
			'#','#',		// REQ_PREFIX
			29,0,0,0,		// packetLength
			10,0,0,0,		// m_iConnectType
			11,0,0,0,		// m_dwStationId
			12,0,0,0,		// m_iCommand
			5,0,0,0,		// m_iDataLen
			};
		
		CRequest req;
		
		req.m_iConnectType = 10;
		req.m_dwStationId = 11;
		req.m_iCommand = 12;
		req.SetData(_T("test"));
		
		
		CPPUNIT_ASSERT(req.CreateHeader(NULL, 0) == sizeof(Buf));
		
		BYTE Buf2[256];
		CPPUNIT_ASSERT(req.CreateHeader((char*)Buf2, sizeof(Buf2)) == sizeof(Buf));
		
		CPPUNIT_ASSERT(memcmp(Buf, Buf2, sizeof(Buf)) == 0);
	}
	
	void testCreateHeader_LowBuffer()
	{
		CRequest req;
		req.SetData(_T("test"));
		
		BYTE Buf2[20];
		
		CPPUNIT_ASSERT(req.CreateHeader((char*)Buf2, sizeof(Buf2)) == -1);
	}
	
	void testGetPacket()
	{
		BYTE Buf[] = {
			'#','#',		// REQ_PREFIX
				29,0,0,0,		// packetLength
				10,0,0,0,		// m_iConnectType
				11,0,0,0,		// m_dwStationId
				12,0,0,0,		// m_iCommand
				5,0,0,0,		// m_iDataLen
				't','e','s','t',// m_pData
				0,
				'$','$'			// REQ_POSTFIX
		};
		
		CRequest req;
		
		req.m_iConnectType = 10;
		req.m_dwStationId = 11;
		req.m_iCommand = 12;
		req.SetData(_T("test"));
		
		CPPUNIT_ASSERT(req.GetPacketSize() == sizeof(Buf));
		
		CPPUNIT_ASSERT(memcmp(Buf, req.GetPacket(), sizeof(Buf)) == 0);
	}
	
	void testSetDataWithExtra()
	{
		CRequest req;
		char x[3] = {1};
		char* data = NULL;
		int len;

		x[0] = '1';
		req.SetDataWithExtra(_T("X"), &x, 1);
		req.GetData((void**)&data, &len);
		CPPUNIT_ASSERT_EQUAL(3, len);
		CPPUNIT_ASSERT_EQUAL('X', data[0]);
		CPPUNIT_ASSERT_EQUAL(0, (int)data[1]);
		CPPUNIT_ASSERT_EQUAL('1', data[2]);
		len = 0;
		data = NULL;
		req.GetBinaryData((void**)&data, &len);
		CPPUNIT_ASSERT_EQUAL(1, len);
		CPPUNIT_ASSERT_EQUAL('1', data[0]);
	}

	void test()
	{
		CPPUNIT_ASSERT_MESSAGE("Important Message", 1);
		CPPUNIT_ASSERT(1);
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CRequestTest);
	CPPUNIT_TEST(testCopyConstructor);
	CPPUNIT_TEST(testOperatorEq);
	CPPUNIT_TEST(testSetData);
	CPPUNIT_TEST(testGetDataAsString);
	
	CPPUNIT_TEST(testParseRequest);

	CPPUNIT_TEST(testParseRequestHeader);

	CPPUNIT_TEST(testCreateRequest);
	CPPUNIT_TEST(testCreateRequest_LowBuffer);
	
	CPPUNIT_TEST(testCreateHeader);
	CPPUNIT_TEST(testCreateHeader_LowBuffer);

	CPPUNIT_TEST(testGetPacket);
	CPPUNIT_TEST(testSetDataWithExtra);
	CPPUNIT_TEST_SUITE_END();
	
};
CPPUNIT_TEST_SUITE_REGISTRATION(CRequestTest);

#endif // _DEBUG
