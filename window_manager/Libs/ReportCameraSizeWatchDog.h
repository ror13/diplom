// ReportCameraSizeWatchDog.h: interface for the CReportCameraSizeWatchDog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ReportCameraSizeWatchDog_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
#define AFX_ReportCameraSizeWatchDog_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_

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

#include "ReportCameraConfig.h"

class CPClient;

class CReportCameraSizeWatchDog : public CThreadClient
{
public:
	CReportCameraSizeWatchDog();
	virtual ~CReportCameraSizeWatchDog();

	void SetStop();
	BOOL IsWorking();

	BOOL SetConfig(CReportCameraConfig& ReportCameraConfig, BOOL bAutoStartWatchDog = TRUE);

// for vlc control
	BOOL	SetStartWatchDog();
	BOOL	SetStopWatchDog();

	BOOL	SetRemoveFilesToSizeLimit();

	BOOL	RemoveFilesToSizeLimit();

protected:
	CCritSect		m_CritSect;

	CThread			m_Thread;

	CPClient*		m_Client;

	CReportCameraConfig m_ReportCameraConfig;
	
	// CThreadClient
	virtual UINT OnThread(int ThreadNum);

	void SetClient(CPClient* Client);

// for WatchDog control
	HANDLE			m_hStopWatchDogEvent;
	HANDLE			m_hStartWatchDogEvent;

	HANDLE			m_hDirectoryChangeNotification;

	HANDLE			m_hRemoveFilesToSizeLimitEvent;

	// common 

	BOOL	StartWatchDog();
	BOOL	StopWatchDog();

	BOOL	RestartWatchDog();

	BOOL	m_bWatchDogStarted;
};

#endif // !defined(AFX_ReportCameraSizeWatchDog_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
