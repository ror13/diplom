
#include "StdAfx.h"

#include "LoginManager.h"
#include "Commands.h"
#include "OptionsMap.h"
#include "Translate.h"
#include "ErrorMessageDlg.h"

#include "FileOperation.h"
#include "ServerConnectDlg.h"
#include "PresenterRegistry.h"
#include "LoginInfo.h"
#include "WaitPatchDlg.h"
#include "waitmd5dlg.h"
#include "XDelta.h"
#include "md5.h"

CLoginManager::CLoginManager(UINT nIDTemplate, CWnd* pParentWnd) :
	CResizeDialog(nIDTemplate, pParentWnd), m_FileSender(&m_Link, TRUE, &m_WaitMsgDlg)
{
	m_Link.SetCallback(this);

	m_LinkSendTimeout = 0;
	m_LinkRecvTimeout = 0;
		
	m_bLinkStarted = FALSE;
	m_bErrorDlgShowed = FALSE;
	m_bLoginError = FALSE;

	m_strServerBuildDate = _T("");
	m_ImportMediaFolder = _T("");
}

CLoginManager::~CLoginManager()
{
	std::list<CString>::iterator i;
	for (i = m_TempFiles.begin(); i != m_TempFiles.end(); i++)
	{
		DeleteFile(*i);
	}
	m_LinkTimeoutThread.WaitStopThread();
	m_FileSender.SetStop();
	m_Link.StopLink();
}

	
void CLoginManager::Init()
{	
	BOOL res = TRUE;
	m_bSavePassword = FALSE;
		
#if defined NET_PRESENTER || defined PWALL_PRESENTER
	res = regReadRegString(PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
		m_strRegIpAddress,
		m_strIpAddress.GetBuffer(20), 20);
	m_strIpAddress.ReleaseBuffer();
		
	res &= regReadRegString(PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
		m_strRegUserName,
		m_UserName.GetBuffer(20), 20);
	m_UserName.ReleaseBuffer();
		
	regReadRegDword(PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
		m_strRegSavePassword,
		(PDWORD)&m_bSavePassword );
		
	if (m_bSavePassword)
	{
		CString EncodedPassword;
		res &= regReadRegString(PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
			m_strRegPassword,
			EncodedPassword.GetBuffer(20), 20);
		EncodedPassword.ReleaseBuffer();
		CBase64 Coder;
		Coder.DecodeString(EncodedPassword, m_Password);
	}
		
	if(!res)
	{
		m_strIpAddress = _T("127.0.0.1");
		m_UserName = _T("User");
		m_bSavePassword = FALSE;
	}
#else
	m_strIpAddress = _T("127.0.0.1");
	m_UserName = _T("not_need");
	m_Password = _T("presenter");
	m_bSavePassword = TRUE;
#endif //NET_PRESENTER || defined PWALL_PRESENTER
		
	if(res && m_bSavePassword)
	{
		m_bLinkStarted = m_Link.CreateLink(m_strIpAddress, PRESENTER_SCHEDULESERVER_PORT);
		if(!m_Link.WaitLinkConnect(10000))
		{
			AfxMessageBox(IDS_SERVER_CONNECT_ERROR, MB_OK|MB_ICONSTOP);
		}
	}
		
	m_LinkTimeoutThread.SetName(_T("LinkTimeout"));
	m_LinkTimeoutThread.Create(this);	
}

void CLoginManager::OnProtectionFailed(CRequest &req)
{
	if(req.m_iDataLen == sizeof(m_LicFile))
		m_LicFile = *(LIC_FILE*)req.m_pDataMAX_DATA_SIZE;
	if(!m_bErrorDlgShowed)
	{
		DisplayError( GetSafeHwnd(), 0, DEFAULT_DISPLAY_LENGTH,
			NULL, _T("%s"), GetLoginErrorString(&m_LicFile));
	}
	m_bErrorDlgShowed = TRUE;
}

BOOL CLoginManager::HandleLogin(CRequest &req)
{
	if (m_bLoginSuccess)
		return TRUE;
	
	switch(req.m_iCommand)
	{
	case MESSAGE_TYPE_LOGIN_SUCCESS:
		{
			COptionsMap reqMap;
			reqMap.FromString(req.GetDataAsString());
			if(!reqMap.IsOptionExists(KEY_SUPPORTED_MESSAGES))
			{
				m_bLoginSuccess = FALSE;
				if(!m_bErrorDlgShowed)
				{
					DisplayError( GetSafeHwnd(), 0, DEFAULT_DISPLAY_LENGTH,
						NULL, _TRANS(_T("Error: Server is too old")));
				}
				m_bErrorDlgShowed = TRUE;
				m_bLoginError = TRUE;
			}
			else
			{
				m_bLoginSuccess = TRUE;
				m_bErrorDlgShowed = FALSE;
				m_bLoginError = FALSE;
				
				if(reqMap.IsOptionExists(KEY_TIMESTAMP))
				{
					CString DateTime = reqMap.GetOption(KEY_TIMESTAMP);
					CString revision = reqMap.GetOption(KEY_REVISION);
					int length = DateTime.GetLength() - 9;
					DateTime = DateTime.Mid(0, length);
					m_strServerBuildDate.Format(_TRANS(_T(" %s rev: %s, msg %d")), DateTime, revision, MESSAGE_TYPE_COUNT);
				}
				else
					m_strServerBuildDate = _TRANS(_T("Not definite"));
				
				if(reqMap.IsOptionExists(KEY_TRIAL_DAYS_LEFT))
				{
					CString strMessage;
					strMessage.Format(_TRANS(_T("Server works in evaluation mode! %d days left.\nPlease, register. Sorry for any inconviniences.")), 
							reqMap.GetOptionInt(KEY_TRIAL_DAYS_LEFT));
					DisplayError( GetSafeHwnd(), 0, DEFAULT_DISPLAY_LENGTH,
						NULL, strMessage);
				}
			}
		}
		break;
		
	case MESSAGE_TYPE_LOGIN_ERROR:
		m_bLoginSuccess = FALSE;
		if(!m_bErrorDlgShowed)
		{
			DisplayError( GetSafeHwnd(), 0, DEFAULT_DISPLAY_LENGTH,
				NULL, _TRANS(_T("Error connect to server: %s")), req.GetErrorDescrition());
		}
		m_bErrorDlgShowed = TRUE;
		m_bLoginError = TRUE;
		break;
	}
	return FALSE;
}


void CLoginManager::OnLoginServer()
{
	CServerConnectDlg dlg;
	BOOL res = FALSE;
	
	dlg.m_ServerAddr = m_strIpAddress;
	dlg.m_UserName = m_UserName;
	dlg.m_Password = m_Password;
	dlg.m_bSavePassword = m_bSavePassword;
	
	if(dlg.DoModal() == IDOK)
	{
		m_StatusBarCtrl.SetText(_T("Connecting..."), 0, 0);
		ClearAllDataBeforeLogin();
		m_bLoginSuccess = FALSE;
		m_bLoginError = FALSE;
		m_bErrorDlgShowed = FALSE;
		
		m_strIpAddress = dlg.m_ServerAddr;
		m_UserName = dlg.m_UserName;
		m_Password = dlg.m_Password;
		m_bSavePassword = dlg.m_bSavePassword;
		
		m_bLinkStarted = m_Link.CreateLink(m_strIpAddress, PRESENTER_SCHEDULESERVER_PORT);
		res = m_Link.WaitLinkConnect(10000);
		if(!res)
		{
			AfxMessageBox(IDS_SERVER_CONNECT_ERROR, MB_OK|MB_ICONSTOP);
		}
		else
		{
			regWriteRegString( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
				m_strRegIpAddress,
				m_strIpAddress );
			regWriteRegString( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
				m_strRegUserName,
				m_UserName );
			if (m_bSavePassword)
			{
				CString EncodedPassword;
				CBase64 Coder;
				Coder.EncodeString(m_Password, EncodedPassword);
				regWriteRegString( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
					m_strRegPassword,
					EncodedPassword );
			}
			regWriteRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T,
				m_strRegSavePassword,
				m_bSavePassword );
		}
	}
	
	TRACE(_T("m_bLinkStarted = %d, bRes = %d\n"), m_bLinkStarted, res);
}

void CLoginManager::OnLinkConnect()
{
	CLoginInfo	LoginInfo;

	if(m_bLoginError)
		return;
	
	LoginInfo.m_SupportedProtocols.Add(CURRENT_PROTOCOL_VERSION);
	LoginInfo.m_SupportedMessages = MESSAGE_TYPE_COUNT;
	
#if APPLICA_PRODUCT == PRODUCT_PRESENTER
	TCHAR szUser[200]  =_T("");
	ULONG uBufSize = SIZEOF_ARRAY(szUser);
	GetUserName( szUser, &uBufSize );
	
	LoginInfo.m_UserName = szUser;
	LoginInfo.m_Password = "";
#else
	LoginInfo.m_UserName = m_UserName;
	LoginInfo.m_Password = m_Password;
	
	DWORD BufSize = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(LoginInfo.m_StationName.GetBuffer(BufSize), &BufSize);
	LoginInfo.m_StationName.ReleaseBuffer();
#endif // APPLICA_PRODUCT == PRODUCT_PRESENTER
	
	CString strSend;
	LoginInfo.GetValues(strSend);
	
	CRequest reqSend;
	reqSend.m_iCommand = MESSAGE_TYPE_LOGIN_INFO;
	reqSend.m_iConnectType = m_ConnectType;
	reqSend.SetData(strSend);
	
#if APPLICA_PRODUCT == PRODUCT_PRESENTER
	reqSend.m_dwStationId = GetCurrentStationId();
#endif // APPLICA_PRODUCT == PRODUCT_PRESENTER
	
	SendMsg(reqSend);
}

BOOL CLoginManager::SendAndWaitMsgDlg(CRequest &reqSend, int Timeout, SendAndWaitFunc SendAndWaitFunct, LPVOID data)
{
	m_LinkSendTimeout = SEND_TIMEOUT;
	
	return m_WaitMsgDlg.SendAndWaitMsg(&m_Link, reqSend, Timeout, SendAndWaitFunct, data);
}

BOOL CLoginManager::SendAndWaitMsgDlg(CRequest &reqSend, CRequest &reqRecv, int Timeout, int *WaitMsgs)
{
	m_LinkSendTimeout = SEND_TIMEOUT;
	
	return m_WaitMsgDlg.SendAndWaitMsg(&m_Link, reqSend, reqRecv, Timeout, WaitMsgs);
}

BOOL CLoginManager::SendMsg(CRequest &reqOut)
{
	m_LinkSendTimeout = SEND_TIMEOUT;
	
	return m_Link.SendMsg(reqOut);
}

void CLoginManager::OnLinkDisconnect()
{
	TRACE(_T("CLoginManager::OnLinkDisconnect\n"));
	m_WaitMsgDlg.OnLinkDisconnect();
	m_LinkSendTimeout = 0;
	m_LinkRecvTimeout = 0;
	memset(&m_LicFile, 0, sizeof(m_LicFile));
	
	if(IsWindow(GetSafeHwnd()))
	{
		ClearAllDataBeforeLogin();
	}
}


void CLoginManager::AddUploadMedia(const CString& hash, const CString& path)
{
	m_FileSender.addFile(hash, path);
}

void CLoginManager::AddUploadMedia(const CString& hash, FILE * pTmpFileDiff)
{
	m_FileSender.addFile(hash, pTmpFileDiff);

}



void CLoginManager::OnMessageGetFileByFilename(CRequest &req)
{
	COptionsMap map;

	if(m_FileSender.checkingRequestValide(req))
	{
		m_FileSender.AddGetFileRequest(req);
		return;
	}
	map.SetOption(KEY_ERROR, _T("File not found"));
	CRequest reqSend;
	reqSend.m_iCommand = MESSAGE_TYPE_FILE_ERROR;
	reqSend.SetData(map.ToString());
	SendMsg(reqSend);
	return;
}

static BOOL OnRecvFileSectionLa(LPVOID data, CRequest & req)
{
	CLoginManager *dlg = (CLoginManager *)data;	
	return dlg->OnRecvFileSection(req);
}

BOOL CLoginManager::OnGetFileFromServer(DWORD stationId, const CString& FileMD5, LONGLONG FileSize, const CString& TargetPath, CString OriginalFileName)
{
	TRACE(_T("OnGetFileFromServer"));
	CCritSectLock lockOnRecvFileSection(&m_OnRecvFileSectionCritSect);
	CCritSectLock lock(&m_BeginFileDownloadCritSect);

	BYTE tmp[16];
	CString digest;

	m_strDownloadFileMD5 = FileMD5;
	m_DownloadFileSize.QuadPart = FileSize;

	m_hDownloadFile = CreateFile(TargetPath + _T(".download"),
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, 0, NULL);

	if(m_hDownloadFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// init MD5 sum
	MD5Init(&m_DownloadFileMD5C);

	// begin media file download
	COptionsMap map;
	map.SetOption(KEY_HASH, FileMD5);
	if (OriginalFileName != _T(""))
	{
		map.SetOption(KEY_ORIGINAL_FILE, OriginalFileName);
	}

	CRequest req;
	req.m_iCommand = MESSAGE_TYPE_GET_FILE_BY_FILENAME;
	req.m_dwStationId = stationId;
	req.SetData(map.ToString());
	
	BOOL res = SendAndWaitMsgDlg(req, 15000, &OnRecvFileSectionLa, this);

	MD5Final(tmp, &m_DownloadFileMD5C);
	digest.Format(_T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
		tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7], tmp[8], tmp[9],
		tmp[10], tmp[11], tmp[12], tmp[13], tmp[14], tmp[15]);

	CloseHandle(m_hDownloadFile);

	if(digest == m_strDownloadFileMD5)
	{
		MoveFileEx(TargetPath + _T(".download"),
			TargetPath, MOVEFILE_REPLACE_EXISTING);
	} 
	else
	{
		DeleteFile(TargetPath + _T(".download"));
		return FALSE;
	}

	return TRUE;
}

BOOL CLoginManager::OnRecvFileSection(CRequest &req)
{
	TRACE(_T("OnRecvFileSection"));

	if (req.m_iCommand == MESSAGE_TYPE_FILE_ERROR)
		return TRUE;
	if(req.m_iCommand != MESSAGE_TYPE_FILE)
		return FALSE;

	COptionsMap map;
	map.FromString(req.GetDataAsString());

	CString FileMD5 = map.GetOption(KEY_HASH);
	LARGE_INTEGER Offset;
	Offset.QuadPart = _ttoi64(map.GetOption(KEY_OFFSET));

	if(m_strDownloadFileMD5 == FileMD5)
	{
		DWORD header_size = strlen((char*)req.m_pDataMAX_DATA_SIZE) + 1;
		DWORD size = req.m_iDataLen - header_size;
		LPBYTE buf = req.m_pDataMAX_DATA_SIZE + header_size;

		DWORD writen = 0;
		if (!WriteFile(m_hDownloadFile, buf, size, &writen, NULL))
			MessageBox(_TRANS(_T("Write Failed!")), MB_OK);

		if(writen != size)
		{
			return TRUE;
		}

		MD5Update(&m_DownloadFileMD5C, buf, size);
		if(m_DownloadFileSize.QuadPart == Offset.QuadPart + size) 
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CLoginManager::CheckIPAddress(SOCKADDR_IN server_addr, char* ip)
{
	BOOL bIPEqu = FALSE;
	//--------------------------------
	// Declare and initialize variables.
	// 	char* ip = "127.0.0.1";
	// 	char* port = "27015";
	//char* ip = "";
	char* port = "9999";
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	struct addrinfo *ai;
	int retVal;
	
	TRACE(_T("CheckIPAddress: server %d.%d.%d.%d\n"), 
		server_addr.sin_addr.s_net, server_addr.sin_addr.s_host, 
		server_addr.sin_addr.s_lh, server_addr.sin_addr.s_impno);
	
	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	//aiHints.ai_flags = AI_PASSIVE;
	
	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the aiList variable will hold a linked list
	// of addrinfo structures containing response
	// information about the host
	if ((retVal = getaddrinfo(ip, port, &aiHints, &aiList)) == 0) 
		//if ((retVal = getaddrinfo(NULL, NULL, &aiHints, &aiList)) == 0) 
	{
		ai = aiList;
		
		while (ai != NULL)
		{
			PSOCKADDR_IN paddr = (PSOCKADDR_IN)ai->ai_addr;
			TRACE(_T("CheckIPAddress: IP %d.%d.%d.%d\n"), 
				paddr->sin_addr.s_net, paddr->sin_addr.s_host, 
				paddr->sin_addr.s_lh, paddr->sin_addr.s_impno);
			if (paddr->sin_addr.s_addr == server_addr.sin_addr.s_addr)
			{
				bIPEqu = TRUE;
				TRACE(_T("CheckIPAddress: equal\n")); 
				//MessageBox(_T("Run on server"));
			}
			ai = ai->ai_next;
		}
		
		freeaddrinfo(aiList);
	}
	
	
	return bIPEqu;
}

BOOL CLoginManager::IsRunOnServer()
{
	SOCKADDR_IN server_addr = m_Link.GetAddr();
	BOOL bIsRunOnServer = FALSE;
	
	if(CheckIPAddress(server_addr, "127.0.0.1") || CheckIPAddress(server_addr, ""))
	{
		bIsRunOnServer = TRUE;
	}	
	return bIsRunOnServer;
}

static int staticCreatePatch(CLoginManager* LoginManager, const CString& oldHash, LONGLONG oldSize, const CString& newPath, CString& ret)
{
	CString folder;
	folder = GetFileFolder(newPath);
	CreateDirectory(folder, NULL);
	CWaitPatchDlg WaitDlg;
	BOOL bRunOnServer = FALSE;
					
	CString oldPath = GetTempDirectory() + oldHash;
	CString diffPath = GetTempDirectory() + oldHash + _T(".vcdiff");

	if (LoginManager->IsRunOnServer())
	{
		bRunOnServer = TRUE;
		if (!GetMediaFileByHash(GetServerFilesDirectory(), oldHash, oldPath))
		{
			return -1;
		}
	}
	else
	{
		if (!LoginManager->OnGetFileFromServer(0, oldHash, oldSize, oldPath))
		{
			return -2;
		}
	}

	if (!XDeltaDiff(oldPath, newPath, diffPath, &WaitDlg))
	{
		if (!bRunOnServer)
			DeleteFile(oldPath);
		if (WaitDlg.IsCancel())
			return -3;
		return -4;
	}
	if (oldSize <= GetBinFileSizeEx(diffPath))
	{
		// patch make no sence
		if (!bRunOnServer)
			DeleteFile(oldPath);
		DeleteFile(diffPath);
		return -5;
	}
	if (!bRunOnServer)
		DeleteFile(oldPath);
	ret = diffPath;
	return TRUE;
}

int CLoginManager::UpdateTaskMedia(CTaskMedia& media, const CString& path)
{
	int ret;
	CString diffPath;
	CTaskMediaPatchItem patchItem;
	CWaitMD5Dlg WaitDlg;

	ret = staticCreatePatch(this, media.m_MD5Signature, media.m_MediaFileSize, path, diffPath);
	if (ret < 0)
		return ret;

	m_TempFiles.push_back(diffPath);

	CString patchSrc;
	LONGLONG patchSrcSize;
	CString patchDst;
	LONGLONG patchDstSize;

	if (!(XDeltaGetSrcMd5(diffPath, patchSrc) && 
	    XDeltaGetSrcSize(diffPath, patchSrcSize) &&
		XDeltaGetDstMd5(diffPath, patchDst) &&
		XDeltaGetDstSize(diffPath, patchDstSize)))
	{
					return -1;
	}

	patchItem.m_strSource = patchSrc;
	patchItem.m_strTarget = patchDst;
	patchItem.m_SourceSize = patchSrcSize;
	patchItem.m_TargetSize = patchDstSize;
	patchItem.m_MD5Signature = GetMD5Signature(diffPath, &WaitDlg);
	patchItem.m_Size = GetBinFileSizeEx(diffPath);
		
				
	media.m_patches.Add(patchItem);

	media.m_MD5Signature = patchDst;
	media.m_MediaFileSize = patchDstSize;
	media.CleanOldPatches();

	AddUploadMedia(patchItem.m_MD5Signature, diffPath);

	return 1;
}
