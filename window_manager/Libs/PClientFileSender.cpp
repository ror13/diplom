// PClientFileSender.cpp: implementation of the CPClientFileSender class.
//
//////////////////////////////////////////////////////////////////////
#warning "!!!BOOL CPClientFileSender::StopUploadFiles() down work correctly!!!"
#include "stdafx.h"
#include "PClientFileSender.h"

#include "Commands.h"

#include "PresenterGlobals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CPClientFileSender::CPClientFileSender(CLinkPoint *pLink, BOOL UseLoPriority, BOOL bIsInteractiveInfo)
CPClientFileSender::CPClientFileSender(CLinkPoint *pLink, BOOL UseLoPriority, CWnd *InteractiveWnd)
{
	m_pLink = pLink;

	m_Thread.SetName(_T("CPClientFileSender"));
	m_Thread.Create(this, 0);

	m_UseLoPriority = UseLoPriority;
	//m_bIsInteractiveInfo = bIsInteractiveInfo;
	m_InteractiveWnd = InteractiveWnd;

	m_bSending = FALSE;
}

CPClientFileSender::~CPClientFileSender()
{
}


BOOL CPClientFileSender::checkingRequestValide(CRequest &req)
{
	COptionsMap map;
	map.FromString(req.GetDataAsString());
	CCritSectLock lock(&m_UploadMediaMapCritSect);


	std::map<CString, std::pair<CString, LONGLONG> >::iterator i;
	std::map<CString, std::pair<FILE *, LONGLONG> >::iterator iFile;
	
	i = m_UploadMediaMap.find(map.GetOption(KEY_HASH));
	iFile = m_UploadMediaMapFile.find(map.GetOption(KEY_HASH));

	if (i != m_UploadMediaMap.end() || 
		iFile != m_UploadMediaMapFile.end() ||
		map.GetOption(KEY_HASH) == _T("d41d8cd98f00b204e9800998ecf8427e"))
	{
		return TRUE;
	}
	return FALSE;

}
void CPClientFileSender::addFile(const CString& hash, FILE * pTmpFileDiff)
{
	CCritSectLock lock(&m_UploadMediaMapCritSect);
	m_UploadMediaMapFile[hash] = std::pair<FILE *, LONGLONG> (pTmpFileDiff, GetBinFileSizeEx(pTmpFileDiff));
}

void CPClientFileSender::addFile(const CString& hash, const CString& path)
{
	CCritSectLock lock(&m_UploadMediaMapCritSect);
	m_UploadMediaMap[hash] = std::pair<CString, LONGLONG> (path, GetBinFileSizeEx(path));
}


BOOL CPClientFileSender::StopUploadFiles()
{
	m_bSending = FALSE;

	m_GetFileRequestBuf.RemoveAll();
	m_RemFileRequestBuf.RemoveAll();
	
	return TRUE;
}

BOOL CPClientFileSender::AddGetFileRequest(CRequest &req)
{
	m_GetFileRequestBuf.Push(req);
	m_Thread.SetEvent();
	return TRUE;
}

void CPClientFileSender::RemoveGetFileRequest(CRequest &req)
{
	m_RemFileRequestBuf.Push(req);
	m_Thread.SetEvent();
}

void CPClientFileSender::SetStop()
{
	m_Thread.SetStop();
}

BOOL CPClientFileSender::IsWorking()
{
	return m_Thread.IsActive();
}

BOOL CPClientFileSender::IsSkipedFile(CString hash)
{
	BOOL res = FALSE;

	COptionsMap map;
	CRequest rem_req;

	while(m_RemFileRequestBuf.Pop(rem_req))
	{
		map.FromString(rem_req.GetDataAsString());
		CString RemHash = map.GetOption(KEY_HASH);

		// check current sending file for remove
		if(RemHash == hash)
			res = TRUE;

		// check get file request array for remove
		m_GetFileRequestBuf.Lock();
		CRequest *pRec = m_GetFileRequestBuf.GetFirst();
		while(pRec)
		{
			if(pRec->m_iCommand == MESSAGE_TYPE_GET_FILE_BY_FILENAME)
			{
				map.FromString(pRec->GetDataAsString());
				if(map.GetOption(KEY_HASH) == RemHash)
				{
					pRec = m_GetFileRequestBuf.RemoveCurrentAndGetNext();
					continue;
				}
			}
			pRec = m_GetFileRequestBuf.GetNext();
		}
		m_GetFileRequestBuf.Unlock();
	}

	return res;
}

FILE * CPClientFileSender::createFileToSend(const CString & strHash, const CString& strFilename)
{
	CString hash;

	CString strPath;
	CRequest reqSend;
	FILE *ret;
	CCritSectLock lock(&m_UploadMediaMapCritSect);
	std::map<CString, std::pair<CString, LONGLONG> >::iterator i;
	std::map<CString, std::pair<FILE *, LONGLONG> >::iterator iFile;

	i = m_UploadMediaMap.find(strHash);
	iFile = m_UploadMediaMapFile.find(strHash);

	if (i != m_UploadMediaMap.end())
	{
		ret = _tfopen((*i).second.first, _T("rb"));
		return ret;
	}

	if (iFile != m_UploadMediaMapFile.end())
	{
		ret = (*iFile).second.first;
		return ret;
	}
	return _tfopen(strFilename, _T("rb"));
}

void CPClientFileSender::closeFile(FILE * pFile, const CString & strHash)
{
	CString hash;
	COptionsMap map;
	CString strPath;
	CRequest reqSend;
	std::map<CString, std::pair<CString, LONGLONG> >::iterator i;
	std::map<CString, std::pair<FILE *, LONGLONG> >::iterator iFile;
	CCritSectLock lock(&m_UploadMediaMapCritSect);
	
	i = m_UploadMediaMap.find(strHash);
	iFile = m_UploadMediaMapFile.find(strHash);
	
	if (i != m_UploadMediaMap.end())
	{
		m_UploadMediaMap.erase(i);
	}

	if (iFile != m_UploadMediaMapFile.end())
	{
		m_UploadMediaMapFile.erase(iFile);
	}
	fclose(pFile);
}


UINT CPClientFileSender::OnThread(int ThreadNum)
{
	CRequest req;
	CRequest reqSend;
	COptionsMap map;
	
	while(!m_Thread.IsStopped())
	{
		if(m_Thread.WaitEvent() == CThread::WAIT_STATUS_ON_EVENT)
		{
			while(m_GetFileRequestBuf.Pop(req) && !m_Thread.IsStopped())
			{
				if(req.m_iCommand == MESSAGE_TYPE_GET_FILE_BY_FILENAME)
				{
					map.FromString(req.GetDataAsString());

					CString strFilename = map.GetOption(KEY_FILE);
					CString strOffset = map.GetOption(KEY_OFFSET);
					CString strHash = map.GetOption(KEY_HASH);

					TRACE(_T("CPClientFileSender::OnThread try to send %s\n"), strFilename);

					if(IsSkipedFile(strHash))
					{
						TRACE(_T("CPClientFileSender::OnThread IsSkipedFile==TRUE\n"));
						continue;
					}
					
					if (strHash == _T("d41d8cd98f00b204e9800998ecf8427e"))
					{
						map.SetOption(KEY_OFFSET, 0);
						map.SetOption(KEY_SIZE, 0);
						map.SetOption(KEY_FILE, _T("d41d8cd98f00b204e9800998ecf8427e"));
						map.SetOption(KEY_HASH, _T("d41d8cd98f00b204e9800998ecf8427e"));

						CRequest reqTmp;
						reqTmp.m_iCommand = MESSAGE_TYPE_FILE;
						reqTmp.m_dwStationId = req.m_dwStationId;
						reqTmp.SetData(map.ToString());
						m_pLink->SendMsg(reqTmp);
						continue;
					}

					LARGE_INTEGER Offset;
					Offset.QuadPart = _ttoi64(strOffset);
					//StrToInt64Ex(strOffset, STIF_DEFAULT, &Offset.QuadPart);

					reqSend.m_dwStationId = req.m_dwStationId;
					FILE * hFile = createFileToSend(strHash, strFilename);

					if( hFile == NULL )
					{
						TRACE(_T("CPClientFileSender::OnThread CreateFile failed\n"));
						if (strFilename.GetLength())
							map.SetOption(KEY_FILE, strFilename);
						map.SetOption(KEY_HASH, strHash);
						map.SetOption(KEY_OFFSET, Offset.QuadPart);
						LPVOID lpMsgBuf;
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
							FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), 0, (LPTSTR) &lpMsgBuf, 0, NULL);
						
						map.SetOption(KEY_ERROR, (LPCTSTR)lpMsgBuf);
						LocalFree( lpMsgBuf );
						reqSend.m_iCommand = MESSAGE_TYPE_FILE_ERROR;
						reqSend.SetData(map.ToString());
						m_pLink->SendMsg(reqSend);
						continue;
					}

					LARGE_INTEGER iDataLen, FullDataLen;
					iDataLen.QuadPart = GetBinFileSizeEx( hFile);
					FullDataLen = iDataLen;

					if(Offset.QuadPart > -1L && Offset.QuadPart < iDataLen.QuadPart)
					{
						fpos_t pos = Offset.QuadPart;
						fsetpos(hFile, &pos);
						iDataLen.QuadPart -= Offset.QuadPart;
					} else if(Offset.QuadPart > -1L && Offset.QuadPart >= iDataLen.QuadPart)
					{
						if (strFilename.GetLength())
							map.SetOption(KEY_FILE, strFilename);
						else
							map.SetOption(KEY_FILE, strHash);
						map.SetOption(KEY_HASH, strHash);
						map.SetOption(KEY_OFFSET, Offset.QuadPart);
						map.SetOption(KEY_ERROR, _T("Bad file offset"));
						
						reqSend.m_iCommand = MESSAGE_TYPE_FILE_ERROR;
						reqSend.SetData(map.ToString());
						m_pLink->SendMsg(reqSend);
						closeFile(hFile, strHash);
						continue;
					}

					reqSend.m_iCommand = MESSAGE_TYPE_FILE;
					
#define MAX_FILE_DATA_SECTION	16384L

					if(iDataLen.QuadPart > MAX_FILE_DATA_SECTION)
					{
						m_bSending = TRUE;

						while(iDataLen.QuadPart > 0 && !m_Thread.IsStopped() &&
							!IsSkipedFile(strHash) && m_bSending)
						{
							DWORD size = (DWORD)min(MAX_FILE_DATA_SECTION, iDataLen.QuadPart);
							
							if (strFilename.GetLength())
								map.SetOption(KEY_FILE, strFilename);
							else
								map.SetOption(KEY_FILE, strHash);
							map.SetOption(KEY_HASH, strHash);
							map.SetOption(KEY_OFFSET, Offset.QuadPart);
							
							if(ReadFileBuf(hFile, size, map, reqSend))
							{

								if (m_InteractiveWnd)
								{
									//TRACE(_T("::PostMessage(HWND_BROADCAST, WM_FILE_INFO, %d, %d)\n"), Offset.LowPart, Offset.HighPart);
									//::PostMessage(HWND_BROADCAST, WM_FILE_INFO, 1, 2);
									PDataFileSendInfo pFileSendInfo = new DataFileSendInfo;

									pFileSendInfo->FileSize = FullDataLen;
									pFileSendInfo->SendedBytes = Offset;
									pFileSendInfo->FileCount = m_GetFileRequestBuf.GetCount() + 1;
									::PostMessage(m_InteractiveWnd->GetSafeHwnd(), WM_FILE_INFO, (WPARAM)pFileSendInfo, NULL);
								}
								
								if (m_UseLoPriority)
								{
									if(!m_pLink->SendMsgLoPriority(reqSend))
										break;
								}
								else
								{
									if(!m_pLink->SendMsg(reqSend))
										break;
								}
							} else
								break;
							
							iDataLen.QuadPart -= size;
							Offset.QuadPart += size;
						}

						m_bSending = FALSE;
					} else
					{
						if (strFilename.GetLength())
							map.SetOption(KEY_FILE, strFilename);
						else
							map.SetOption(KEY_FILE, strHash);
						map.SetOption(KEY_HASH, strHash);
						map.SetOption(KEY_OFFSET, Offset.QuadPart);

						if(ReadFileBuf(hFile, iDataLen.LowPart, map, reqSend))
						{
							m_pLink->SendMsg(reqSend);
						}
					}
					closeFile(hFile, strHash);
				}
				else
				{
					TRACE(_T("CPClientFileSender::OnThread req.m_iCommand != MESSAGE_TYPE_GET_FILE_BY_FILENAME\n"));
				}
			}
		}
	}
	return 0;
}



BOOL CPClientFileSender::ReadFileBuf(FILE * hFile, DWORD DataSize, COptionsMap &Map, CRequest &Req)
{
	Req.SetData(Map.ToString());

	/*if(Req.m_iDataLen + DataSize > sizeof(Req.m_pData))
		return FALSE;*/
	if(Req.m_iDataLen + DataSize > MAX_DATA_SIZE)
		return FALSE;

	Req.m_pDataMAX_DATA_SIZE = (PBYTE)realloc(Req.m_pDataMAX_DATA_SIZE, Req.m_iDataLen + DataSize);
	
	DWORD read_bytes = fread(Req.m_pDataMAX_DATA_SIZE + Req.m_iDataLen, 1, DataSize, hFile);
	if(!read_bytes)
		return FALSE;
	if(read_bytes != DataSize)
		return FALSE;

	Req.m_iDataLen += read_bytes;
	
	return TRUE;
}

BOOL CPClientFileSender::ReadFileBuf(HANDLE hFile, DWORD DataSize, COptionsMap &Map, CRequest &Req)
{
	Req.SetData(Map.ToString());

	/*if(Req.m_iDataLen + DataSize > sizeof(Req.m_pData))
		return FALSE;*/
	if(Req.m_iDataLen + DataSize > MAX_DATA_SIZE)
		return FALSE;

	Req.m_pDataMAX_DATA_SIZE = (PBYTE)realloc(Req.m_pDataMAX_DATA_SIZE, Req.m_iDataLen + DataSize);
	
	DWORD read_bytes = 0;
	if(!ReadFile(hFile, Req.m_pDataMAX_DATA_SIZE + Req.m_iDataLen, DataSize, &read_bytes, NULL))
		return FALSE;
	if(read_bytes != DataSize)
		return FALSE;

	Req.m_iDataLen += read_bytes;
	
	return TRUE;
}


#ifdef _DEBUG

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class CFileSenderTest: public CppUnit::TestFixture
{
private:
public:
	void setUp()
	{
	}
	
	void testReadFileBuf()
	{
		CPClientFileSender FileSender(NULL);
		
		CString strFilename = _T(__FILE__);
		COptionsMap map;
		map.SetOption(KEY_FILE, strFilename);
		map.SetOption(KEY_OFFSET, 0);
		
		HANDLE hFile = CreateFile(strFilename, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		
		CRequest req;
		CPPUNIT_ASSERT(FileSender.ReadFileBuf(hFile, 100, map, req));
		
		CloseHandle(hFile);
		
		CPPUNIT_ASSERT_EQUAL(map.ToString().GetLength()+100+1, req.m_iDataLen);
	}
	
	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CFileSenderTest);
	CPPUNIT_TEST(testReadFileBuf);
	CPPUNIT_TEST_SUITE_END();
	
};
CPPUNIT_TEST_SUITE_REGISTRATION(CFileSenderTest);

#endif // _DEBUG
