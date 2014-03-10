// ReportCameraWorker.cpp: implementation of the CReportCameraWorker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReportCameraWorker.h"

#include "PresenterGlobals.h"

#include "Commands.h"

#include "FileOperation.h"
#include "md5_util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportCameraWorker::CReportCameraWorker()
{
	m_Thread.SetName(_T("CReportCameraWorker"));
	m_Thread.Create(this, 0);

	m_Client = NULL;

	// vlc init
	m_inst = NULL;
	m_mp = NULL;
	m_m = NULL;

	m_bCaptureStarted = FALSE;

	m_hStartCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hStopCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	CreateCaptureFileName();

	m_CaptureWndClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_SAVEBITS);
}

CReportCameraWorker::~CReportCameraWorker()
{
	m_Thread.WaitStopThread();
	CloseHandle(m_hStartCaptureEvent);
	CloseHandle(m_hStopCaptureEvent);

	m_Client = NULL;

	// clear vlc
	StopCapture();

	if (m_m)
	{
		libvlc_media_release (m_m);
		m_m = NULL;
	}

	if (m_mp)
	{
		libvlc_media_player_stop (m_mp);
		
		libvlc_media_player_release (m_mp);
		m_mp = NULL;
	}

	if (m_inst)
	{
		libvlc_release(m_inst);
		m_inst = NULL;
	}
}

void CReportCameraWorker::SetClient(CPClient* Client)
{
	m_Client = Client;
}

void CReportCameraWorker::SetStop()
{
	m_Thread.SetStop();
}

BOOL CReportCameraWorker::IsWorking()
{
	return m_Thread.IsActive();
}

BOOL CReportCameraWorker::SetConfig(CReportCameraConfig& ReportCameraConfig, BOOL bAutoStartCapture)
{
	TRACE(_T("CReportCameraWorker::SetConfig\n"));

	BOOL bCaptureStarted = m_bCaptureStarted;

	if (bCaptureStarted)
	{
		StopCapture();	
	}

	m_ReportCameraConfig = ReportCameraConfig;

	if (bAutoStartCapture)
	{
		StartCapture();	
	}
	
	return TRUE;
}

BOOL CReportCameraWorker::SetStartCapture()
{
	return SetEvent(m_hStartCaptureEvent);
}

BOOL CReportCameraWorker::SetStopCapture()
{
	return SetEvent(m_hStopCaptureEvent);
}

BOOL CReportCameraWorker::SetStopCaptureAndWait()
{
	if (SetStopCapture() == FALSE)
	{
		return FALSE;
	}

	while (m_bCaptureStarted)
	{
		Sleep(0);
	}

	return TRUE;
}

BOOL CReportCameraWorker::StartCapture()
{
	TRACE(_T("CReportCameraWorker::StartCapture\n"));

	CreateCaptureFileName();

	BOOL bRet = FALSE;
	switch(m_ReportCameraConfig.m_Action)
	{
	case REPORT_CAMERA_RECORD_VIDEO:
		bRet = StartCaptureVideo();
		break;
	case REPORT_CAMERA_SHOOT_PHOTO:
		bRet = StartCapturePhoto();
		break;
	case REPORT_CAMERA_NONE:
		bRet = TRUE;
		break;
	}

	return bRet;
}

BOOL CReportCameraWorker::StopCapture()
{
	TRACE(_T("CReportCameraWorker::StopCapture\n"));

	BOOL bRet = FALSE;
	switch(m_ReportCameraConfig.m_Action)
	{
	case REPORT_CAMERA_RECORD_VIDEO:
		bRet = StopCaptureVideo();
		LogCaptureFile();
		break;
	case REPORT_CAMERA_SHOOT_PHOTO:
		bRet = StopCapturePhoto();
		break;
	case REPORT_CAMERA_NONE:
		bRet = TRUE;
		break;
	}

	return bRet;
}

BOOL CReportCameraWorker::LogCaptureFile(CString FileExt)
{
	CString	strCaptureFileName = GetCaptureFileName(FileExt);
	// if there are capture file save it to log
	if (IsFileExists(strCaptureFileName))
	{
		// save data about file to log
		CString strClientCameraReportFilesDirectory = GetClientCameraReportsFilesDirectory();
		
		CString strDateTime = m_LastStartedCaptureTime.Format(_T("%a %b %d %H:%M:%S %Y"));
		
		CString strCameraLogFileName;
		strCameraLogFileName.Format(_T("%s\\%s.clog"), GetClientCameraReportsFilesDirectory(), m_CaptureFileName.Mid(0, 8));
		TRACE(_T("CAMERA LOG [%s]\n"), strCameraLogFileName);
		
		CStdioFile LogFile;
		
		if (LogFile.Open(strCameraLogFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText))
		{
			CString strOutLogLine;
			
			//Wed Sep 28 15:52:22 2011,20110928155222.ogg,1029380,da1381f68be6f165d11358847d68a5f0
			strOutLogLine.Format(_T("%s,%s,%I64u,%s\n"), strDateTime, GetFileName(strCaptureFileName), GetBinFileSizeEx(strCaptureFileName), GetMD5Signature(strCaptureFileName));
			
			LogFile.SeekToEnd();
			LogFile.WriteString(strOutLogLine);
			LogFile.Close();

			return TRUE;
		}
	}

	return FALSE;
}

CString CReportCameraWorker::CreateCaptureFileName()
{
	CTime CurrentTime = m_LastStartedCaptureTime = CTime::GetCurrentTime();
	
	//Wed Sep 21 20:41:29 2011,6,C:\aptv2\AD_POOH2.SWF
	//CString strDateTime = StartTime.Format(_T("%c"));
	CString strDateTime = CurrentTime.Format(_T("%a %b %d %H:%M:%S %Y"));
	
	m_CaptureFileName = CurrentTime.Format(_T("%Y%m%d%H%M%S"));
	
	return m_CaptureFileName;
}

CString CReportCameraWorker::GetCaptureFileName(CString FileExt, BOOL bAutoCreate)
{
	if (bAutoCreate)
	{
		CreateCaptureFileName();
	}

	CString strCaptureFileName;
	
	strCaptureFileName.Format(_T("%s\\%s.%s"), GetClientCameraReportsFilesDirectory(), m_CaptureFileName, FileExt);
	
	return strCaptureFileName;
}

BOOL CReportCameraWorker::StartCaptureVideo()
{
	CString strOutputFileName;
	strOutputFileName = GetCaptureFileName();

	if (m_ReportCameraConfig.m_CaptureDevicePath.GetLength() < 1)
	{
		TRACE(_T("CReportCameraWorker::StartCaptureVideo no capture device selected\n"));

		return FALSE;
	}

//////////////////////////////////////////////////////////////////////////
	USES_CONVERSION;

	CString options;
	
	options = _T("--play-and-exit --no-osd --intf dummy --dummy-quiet --no-video-deco --aspect-ratio auto");
	options += _T(" --no-fullscreen");
//	options += _T(" --vout-filter \"\"");
	
	////options += _T(" --width 640 --height 480");
	

	CString video_input;
	//video_input.Format(_T("dshow:// :dshow-vdev=\"%s\""), m_ReportCameraConfig.m_CaptureDevicePath);
	video_input.Format(_T("--dshow-vdev=%s"), m_ReportCameraConfig.m_CaptureDevicePath);
	
	int iOutputFormat = 0/*m_wndOutputFormat.GetCurSel()*/;
	CString transcode;
	CString transcode_out_params;
	CString one_param;
	profile video_profile = video_profile_value_list[iOutputFormat];
	
	transcode_out_params.Format(_T("vcodec=%s,scale=%d"),
		A2T(video_profile.video_codec),
		video_profile.scale);
	if(video_profile.video_bitrate > 0)
	{
		one_param.Format(_T(",vb=%d"), video_profile.video_bitrate);
		transcode_out_params += one_param;
	}
	
	one_param.Format(_T(",acodec=%s,ab=%d,channels=%d,samplerate=%d"),
		A2T(video_profile.audio_codec),
		video_profile.audio_bitrate,
		video_profile.channels,
		video_profile.samplerate
		);
	transcode_out_params += one_param;
	
	transcode.Format(_T("transcode{%s}:standard{access=file,mux=%s,dst=\"%s\"}"), 
		transcode_out_params,
		A2T(video_profile.Container),
		strOutputFileName );
	
	CString command_line;
	command_line.Format(_T("%s %s :dshow-fps=5 --sout=#duplicate{dst=display,dst={%s}"), options, video_input, transcode);
	
	CString sout_arg;
	// dont show on screen/windows/etc
	//sout_arg.Format(_T("--sout=#duplicate{dst=display,dst={%s}"), transcode);
	sout_arg.Format(_T("--sout=#duplicate{dst={%s}"), transcode);
	
	const char * const vlc_args[] = {
		"-I", "dummy", /* Don't use any interface */
		"--ignore-config", /* Don't use VLC's config */
//		"--plugin-path=E:\\TEMP\\VLC-1.0.5\\plugins\\",
		T2A(video_input.GetBuffer(0)),
		T2A(sout_arg.GetBuffer(0))
	};
	
#if 1
	for (int i = 0; i < sizeof(vlc_args) / sizeof(vlc_args[0]); i++)
	{
		TRACE( _T("%d - %s\n"), i, A2T(vlc_args[i]));
	}
#endif
	
	/* init vlc modules, should be done only once */
	m_inst = libvlc_new (sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	
	if (m_inst == NULL)
	{
		return FALSE;
	}
	
	/* Create a new item */
	//m_m = libvlc_media_new (m_inst, "E:/Media/2/1.avi", &m_ex);
	//m_m = libvlc_media_new_path (m_inst, T2A(video_input));
	m_m = libvlc_media_new_path (m_inst, "dshow://");
	
	if (m_m == NULL)
	{
		libvlc_release(m_inst);
		m_inst = NULL;
		return FALSE;
	}
	
	/* Create a media player playing environment */
	m_mp = libvlc_media_player_new_from_media (m_m);
	
	if (m_mp == NULL)
    {
		libvlc_media_release(m_m);
		m_m = NULL;
		
		libvlc_release(m_inst);
		m_inst = NULL;
		return FALSE;
	}
	
	/* No need to keep the media now */
	libvlc_media_release (m_m);
	m_m = NULL;
	
//!!!	libvlc_media_player_set_hwnd(m_mp, m_wndCaptureWindow.GetSafeHwnd());
	
	/* play the media_player */
	int iplay = libvlc_media_player_play (m_mp);
	
	if (iplay == -1)
    {
		libvlc_media_player_release (m_mp);
		m_mp = NULL;
		
		libvlc_release(m_inst);
		m_inst = NULL;
		return FALSE;
	}
	
	m_bCaptureStarted = TRUE;

	TRACE(_T("CReportCameraWorker::StartCaptureVideo m_bCaptureStarted = TRUE\n"));
	
	return TRUE;
}

BOOL CReportCameraWorker::StopCaptureVideo()
{
	TRACE(_T("m_bCaptureStarted = FALSE\n"));
	
	if (m_inst == NULL)
	{
		m_bCaptureStarted = FALSE;
		
		return TRUE;
	}
	
	libvlc_media_player_stop (m_mp);
	
	/* Free the media_player */
	libvlc_media_player_release (m_mp);
	m_mp = NULL;
	
	libvlc_release (m_inst);
	m_inst = NULL;
	
	m_bCaptureStarted = FALSE;
	
	return TRUE;
}

BOOL CReportCameraWorker::StartCapturePhoto()
{
	CString strOutputFileName;
	strOutputFileName = GetCaptureFileName(_T("jpg"));

	if (m_ReportCameraConfig.m_CaptureDevicePath.GetLength() < 1)
	{
		TRACE(_T("CReportCameraWorker::StartCaptureVideo no capture device selected\n"));

		return FALSE;
	}

//////////////////////////////////////////////////////////////////////////
	USES_CONVERSION;

	CString options;
	
	options = _T("--play-and-exit --no-osd --intf dummy --dummy-quiet --no-video-deco --aspect-ratio auto");
	options += _T(" --no-fullscreen");
//	options += _T(" --vout-filter \"\"");
	
	////options += _T(" --width 640 --height 480");
	

	CString video_input;
	video_input.Format(_T("dshow:// :dshow-vdev=\"%s\""), m_ReportCameraConfig.m_CaptureDevicePath);
	
	int iOutputFormat = 0/*m_wndOutputFormat.GetCurSel()*/;
	CString transcode;
	CString transcode_out_params;
	CString one_param;
	profile video_profile = video_profile_value_list[iOutputFormat];
	
	transcode_out_params.Format(_T("vcodec=%s,scale=%d"),
		A2T(video_profile.video_codec),
		video_profile.scale);
	if(video_profile.video_bitrate > 0)
	{
		one_param.Format(_T(",vb=%d"), video_profile.video_bitrate);
		transcode_out_params += one_param;
	}
	
	one_param.Format(_T(",acodec=%s,ab=%d,channels=%d,samplerate=%d"),
		A2T(video_profile.audio_codec),
		video_profile.audio_bitrate,
		video_profile.channels,
		video_profile.samplerate
		);
	transcode_out_params += one_param;
	
	transcode.Format(_T("transcode{%s}:standard{access=file,mux=%s,dst=\"%s\"}"), 
		transcode_out_params,
		A2T(video_profile.Container),
		strOutputFileName );
	
	CString command_line;
	//command_line.Format(_T("%s %s :dshow-fps=5 --sout=#duplicate{dst=display,dst={%s}"), options, video_input, transcode);
	command_line.Format(_T("%s %s :dshow-fps=5 --sout=#duplicate{dst=display,dst={%s}"), options, video_input, transcode);
	
	CString sout_arg;
	// dont show on screen/windows/etc
	//sout_arg.Format(_T("--sout=#duplicate{dst=display,dst={%s}"), transcode);
	//sout_arg.Format(_T("--sout=#duplicate{dst={%s}"), transcode);
	sout_arg.Format(_T("--sout=#duplicate{dst=display"), transcode);
	
	const char * const vlc_args[] = {
		"-I", "dummy", /* Don't use any interface */
		"--ignore-config", /* Don't use VLC's config */
//		"--plugin-path=E:\\TEMP\\VLC-1.0.5\\plugins\\",
		T2A(sout_arg.GetBuffer(0))
	};
	
#if 1
	for (int i = 0; i < sizeof(vlc_args) / sizeof(vlc_args[0]); i++)
	{
		TRACE( _T("%d - %s\n"), i, A2T(vlc_args[i]));
	}
#endif
	
	/* init vlc modules, should be done only once */
	m_inst = libvlc_new (sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	
	if (m_inst == NULL)
	{
		return FALSE;
	}
	
	/* Create a new item */
	//m_m = libvlc_media_new (m_inst, "E:/Media/2/1.avi", &m_ex);
	m_m = libvlc_media_new_path (m_inst, T2A(video_input));
	
	if (m_m == NULL)
	{
		libvlc_release(m_inst);
		m_inst = NULL;
		return FALSE;
	}
	
	/* Create a media player playing environment */
	m_mp = libvlc_media_player_new_from_media (m_m);
	
	if (m_mp == NULL)
    {
		libvlc_media_release(m_m);
		m_m = NULL;
		
		libvlc_release(m_inst);
		m_inst = NULL;
		return FALSE;
	}
	
	/* No need to keep the media now */
	libvlc_media_release (m_m);
	m_m = NULL;

	//m_wndCaptureWindow.CreateEx(0, )
		CRect rect(0, 0, 800, 600);
		CWnd *pWnd = new CWnd;
		pWnd->CreateEx(/*WS_EX_TOPMOST | */WS_EX_TOOLWINDOW, m_CaptureWndClass, _T("CaptureWindow"), WS_POPUP | WS_CHILD,
		rect, NULL, 0);
		pWnd->ShowWindow(SW_HIDE);
	
	//libvlc_media_player_set_hwnd(m_mp, m_wndCaptureWindow.GetSafeHwnd());
	libvlc_media_player_set_hwnd(m_mp, pWnd->GetSafeHwnd());
	
	/* play the media_player */
	int iplay = libvlc_media_player_play (m_mp);
	
	if (iplay == -1)
    {
		libvlc_media_player_release (m_mp);
		m_mp = NULL;
		
		libvlc_release(m_inst);
		m_inst = NULL;
		return FALSE;
	}
	
	m_bCaptureStarted = TRUE;

	TRACE(_T("CReportCameraWorker::StartCapturePhoto m_bCaptureStarted = TRUE\n"));
	
	return TRUE;
}

BOOL CReportCameraWorker::StopCapturePhoto()
{
	TRACE(_T("m_bCaptureStarted = FALSE\n"));
	
	if (m_inst == NULL)
	{
		m_bCaptureStarted = FALSE;
		
		return TRUE;
	}
	
	libvlc_media_player_stop (m_mp);
	
	/* Free the media_player */
	libvlc_media_player_release (m_mp);
	m_mp = NULL;
	
	libvlc_release (m_inst);
	m_inst = NULL;
	
	m_bCaptureStarted = FALSE;
	
	return TRUE;
}

BOOL CReportCameraWorker::MakeCapturePhoto()
{
	TRACE(_T("CReportCameraWorker::MakeCapturePhoto\n"));

	if (m_inst == NULL)
	{
		return FALSE;
	}

	USES_CONVERSION;
	CString strOutputFileName;
	strOutputFileName = GetCaptureFileName(_T("jpg"), TRUE);

	/* retrieve width of video */
	int width = libvlc_video_get_width (m_mp);
	
	/* retrieve height of video */
	int height = libvlc_video_get_height (m_mp);

	int iRet = 
	libvlc_video_take_snapshot(m_mp, 
		0, 
		T2A(strOutputFileName),
		width,
		height);

	TRACE(_T("CReportCameraWorker::MakeCapturePhoto iRet=%d\n"), iRet);
	
	if (iRet == 0)
	{
		LogCaptureFile(_T("jpg"));

		return TRUE;
	}

	return FALSE;
}

BOOL CReportCameraWorker::RestartCapture()
{
	StopCapture();
	return StartCapture();
}

UINT CReportCameraWorker::OnThread(int ThreadNum)
{
	CRequest req;
	CRequest reqSend;
	COptionsMap map;

	CString strData;
	CTaskInfo task;

	CString folder;
	CString strFilename;
	
	while(!m_Thread.IsStopped())
	{
		//TRACE(_T("CReportCameraWorker::OnThread\n"));

		HANDLE hEvents[] = {m_hStartCaptureEvent, m_hStopCaptureEvent};
		DWORD res = WaitForMultipleObjects(2, hEvents, FALSE, 1000);

		switch (res)
		{
			// start capture
		case WAIT_OBJECT_0:
			StartCapture();
			break;
			// stop capture
		case WAIT_OBJECT_0 + 1:
			StopCapture();
			break;
		case WAIT_OBJECT_0 + 2:
			break;
		case WAIT_TIMEOUT:
			break;
		}

		if(m_Thread.WaitEvent(1000) == CThread::WAIT_STATUS_ON_EVENT)
		{
			TRACE(_T("CReportCameraWorker::OnThread Wake\n"));
		}
		if(m_Thread.WaitEvent(1000) == -1)
			break;
		//TRACE(_T("CReportCameraWorker::OnThread2\n"));

		if (m_bCaptureStarted)
		{
			CTime CurrentTime = CTime::GetCurrentTime();
			
			CTimeSpan ts = CurrentTime - m_LastStartedCaptureTime;

			//TRACE(_T("%ld, %ld\n"), ts.GetTotalMinutes(), ts.GetTotalSeconds());
			
			switch (m_ReportCameraConfig.m_Action)
			{
			case REPORT_CAMERA_RECORD_VIDEO:
				if (ts.GetTotalMinutes() >= REPORT_CAMERA_VIDEO_DURATION)
				//if (ts.GetTotalSeconds() > 10)
				{
					RestartCapture();
				}
				break;
			case REPORT_CAMERA_SHOOT_PHOTO:
				if (ts.GetTotalMinutes() >= m_ReportCameraConfig.m_iShootPhotoEvery)
				//if (ts.GetTotalSeconds() > 10)
				{
					MakeCapturePhoto();
				}
				break;
			}

		}

	}

	TRACE(_T("CReportCameraWorker::OnThread end\n"));

	return 0;
}
