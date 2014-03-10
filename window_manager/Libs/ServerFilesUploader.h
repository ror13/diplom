// ServerFilesUploader.h: interface for the CServerFilesUploader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERFILESUPLOADER_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
#define AFX_SERVERFILESUPLOADER_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Request.h"
#include "RequestListBuf.h"

#include "LinkPoint.h"
#include "CritSect.h"
#include "Thread.h"
#include "OptionsMap.h"

#include "TaskInfo.h"
#include "ScheduleInfo.h"

class CPClient;

class CServerFilesUploader : public CThreadClient
{
public:
	CServerFilesUploader(CLinkPoint *pLink);
	virtual ~CServerFilesUploader();

	BOOL AddRequest(CRequest &req);
	void RemoveRequest(CRequest &req);
	
	void SetStop();
	BOOL IsWorking();

	void SetClient(CPClient* Client);

	BOOL StopUploadFiles();
	
protected:
	CCritSect		m_CritSect;
	CLinkPoint		*m_pLink;

	CRequestListBuf	m_RequestBuf;	

	CThread			m_Thread;

	CPClient*		m_Client;
	
	// CThreadClient
	virtual UINT OnThread(int ThreadNum);

	BOOL GetMedia();
	BOOL CheckAndPrepareGetTaskMedia(CTaskInfo &task);
	BOOL CheckAndPrepareGetScheduleMedia(CScheduleInfo *pscheduleinfo);
	BOOL CheckAndPrepareGetUpdatePlayerFirmwareMedia(const CString& spec);
	BOOL CheckAndPrepareSitesMedia(const CString& ini);

	CTaskMediaArray	m_arrMedia;

};

#endif // !defined(AFX_SERVERFILESUPLOADER_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
