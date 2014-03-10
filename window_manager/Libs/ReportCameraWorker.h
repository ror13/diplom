// ReportCameraWorker.h: interface for the CReportCameraWorker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ReportCameraWorker_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
#define AFX_ReportCameraWorker_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_

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

#include "video_capture.h"

#include "vlc/vlc.h"

#include "vlc/vlc_additions.h"

class CPClient;

class CReportCameraWorker : public CThreadClient
{
public:
	CReportCameraWorker();
	virtual ~CReportCameraWorker();

	void SetStop();
	BOOL IsWorking();

	BOOL SetConfig(CReportCameraConfig& ReportCameraConfig, BOOL bAutoStartCapture = TRUE);

// for vlc control
	BOOL	SetStartCapture();
	BOOL	SetStopCapture();
	BOOL	SetStopCaptureAndWait();

	BOOL	IsCaptureStarted() { return m_bCaptureStarted; };

protected:
	CCritSect		m_CritSect;

	CThread			m_Thread;

	CPClient*		m_Client;

	CReportCameraConfig m_ReportCameraConfig;
	
	// CThreadClient
	virtual UINT OnThread(int ThreadNum);

	void SetClient(CPClient* Client);

// for vlc control
	HANDLE			m_hStopCaptureEvent;
	HANDLE			m_hStartCaptureEvent;

	// common 
	CString GetCaptureFileName(CString FileExt = _T("avi"), BOOL bAutoCreate = FALSE);
	CString CreateCaptureFileName();

	BOOL	StartCaptureVideo();
	BOOL	StopCaptureVideo();

	BOOL	LogCaptureFile(CString FileExt = _T("avi"));


	BOOL	StartCapturePhoto();
	BOOL	StopCapturePhoto();
	BOOL	MakeCapturePhoto();
	
	BOOL	StartCapture();
	BOOL	StopCapture();

	BOOL	RestartCapture();

	volatile BOOL	m_bCaptureStarted;
	CTime	m_LastStartedCaptureTime;
	CString	m_CaptureFileName;

// VLC usage
	libvlc_instance_t * m_inst;
	libvlc_media_player_t *m_mp;
	libvlc_media_t *m_m;

	CString	m_CaptureWndClass;
	CWnd	m_wndCaptureWindow;
};

#endif // !defined(AFX_ReportCameraWorker_H__C9783A2E_4739_478C_9369_6547E3B5CB7E__INCLUDED_)
