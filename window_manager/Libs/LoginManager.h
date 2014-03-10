#pragma once

#include "Request.h"
#include "NetLink.h"
#include "ResizeDialog.h"
#include "Thread.h"
#include "WaitMsgDlg.h"
#include "SessionInfo.h"
#include "PCLientFileSender.h"

#include "md5.h"

#include "ISendMsg.h"

#include <map>
#include <list>

class CLoginManager : public CResizeDialog, public CLinkPointCallback, public CThreadClient, public ISendMsg
{
	BOOL CheckIPAddress(SOCKADDR_IN server_addr, char* ip);
	
	std::list<CString> m_TempFiles;

public:
	CStatusBarCtrl			m_StatusBarCtrl;

	CString					m_strRegIpAddress;
	CString					m_strRegUserName;
	CString					m_strRegPassword;
	CString					m_strRegSavePassword;

	CONNECT_TYPE			m_ConnectType;

	CString					m_strIpAddress;
	CString					m_UserName;
	CString					m_Password;
	BOOL					m_bSavePassword;

	BOOL					m_bLoginSuccess;
	BOOL					m_bErrorDlgShowed;
	
	int						m_LinkSendTimeout;
	int						m_LinkRecvTimeout;

	CNetLink				m_Link;
	BOOL					m_bLinkStarted;
	CThread					m_LinkTimeoutThread;
	CWaitMsgDlg				m_WaitMsgDlg;

	BOOL					m_bLoginError;
	CString					m_strServerBuildDate;

	CLoginManager(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CLoginManager();

	virtual void ClearAllDataBeforeLogin(void) = 0;
	virtual void OnLoginServer(void);
	virtual void OnLinkDisconnect();
	virtual void OnLinkConnect();

	void Init();
	void OnProtectionFailed(CRequest &req);
	virtual void OnMessageGetFileByFilename(CRequest &req);

	virtual BOOL SendMsg(CRequest &reqOut);
	BOOL SendAndWaitMsgDlg(CRequest &reqSend, CRequest &reqRecv, int Timeout = 1000, int *WaitMsgs = NULL);
	BOOL SendAndWaitMsgDlg(CRequest &reqSend, int Timeout = 1000, SendAndWaitFunc SendAndWaitFunct = NULL, LPVOID data = NULL);

	BOOL HandleLogin(CRequest &req);

	void AddUploadMedia(const CString& hash, const CString& path);
	void AddUploadMedia(const CString& hash, FILE * pTmpFileDiff);
	virtual BOOL OnGetFileFromServer(DWORD stationId, const CString& FileMD5, LONGLONG FileSize, const CString& TargetPath, CString OriginalFileName = _T(""));
	virtual BOOL OnRecvFileSection(CRequest &req);

	virtual BOOL IsRunOnServer();

	virtual BOOL UpdateTaskMedia(CTaskMedia& media, const CString& path);

protected:
	CString m_ImportMediaFolder;
	LIC_FILE				m_LicFile;
	CPClientFileSender		m_FileSender;

	CCritSect				m_BeginFileDownloadCritSect;
	CCritSect				m_OnRecvFileSectionCritSect;

	CString					m_strDownloadFileMD5;
	LARGE_INTEGER			m_DownloadFileSize;
	HANDLE					m_hDownloadFile;
	MD5Context				m_DownloadFileMD5C;


};

