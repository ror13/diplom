// ReportCameraSizeWatchDog.cpp: implementation of the CReportCameraSizeWatchDog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReportCameraSizeWatchDog.h"

#include "Commands.h"

#include "FileOperation.h"

#include "TaskMedia.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportCameraSizeWatchDog::CReportCameraSizeWatchDog()
{
	m_Thread.SetName(_T("CReportCameraSizeWatchDog"));
	m_Thread.Create(this, 0);

	m_Client = NULL;

	m_bWatchDogStarted = FALSE;

	m_hStartWatchDogEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hStopWatchDogEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hRemoveFilesToSizeLimitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hDirectoryChangeNotification = NULL;
//	m_hDirectoryChangeNotification = FindFirstChangeNotification(GetClientCameraReportsFilesDirectory(), TRUE, FILE_NOTIFY_CHANGE_SIZE);
// 	if (m_hDirectoryChangeNotification == INVALID_HANDLE_VALUE)
// 	{
// 		m_hDirectoryChangeNotification = NULL;
// 	}
}

CReportCameraSizeWatchDog::~CReportCameraSizeWatchDog()
{
	m_Thread.WaitStopThread();
	CloseHandle(m_hStartWatchDogEvent);
	CloseHandle(m_hStopWatchDogEvent);
	CloseHandle(m_hRemoveFilesToSizeLimitEvent);

	if (m_hDirectoryChangeNotification)
	{
		FindCloseChangeNotification(m_hDirectoryChangeNotification);
	}

	m_Client = NULL;

	StopWatchDog();
}

void CReportCameraSizeWatchDog::SetClient(CPClient* Client)
{
	m_Client = Client;
}

void CReportCameraSizeWatchDog::SetStop()
{
	m_Thread.SetStop();
}

BOOL CReportCameraSizeWatchDog::IsWorking()
{
	return m_Thread.IsActive();
}

BOOL CReportCameraSizeWatchDog::SetConfig(CReportCameraConfig& ReportCameraConfig, BOOL bAutoStartWatchDog)
{
	TRACE(_T("CReportCameraSizeWatchDog::SetConfig\n"));

	BOOL bWatchDogStarted = m_bWatchDogStarted;

	if (bWatchDogStarted)
	{
		StopWatchDog();	
	}

	m_ReportCameraConfig = ReportCameraConfig;

	if (bAutoStartWatchDog)
	{
		StartWatchDog();	
	}
	
	return TRUE;
}

BOOL CReportCameraSizeWatchDog::SetStartWatchDog()
{
	return SetEvent(m_hStartWatchDogEvent);
}

BOOL CReportCameraSizeWatchDog::SetStopWatchDog()
{
	return SetEvent(m_hStopWatchDogEvent);
}


BOOL CReportCameraSizeWatchDog::SetRemoveFilesToSizeLimit()
{
	return SetEvent(m_hRemoveFilesToSizeLimitEvent);
}

BOOL CReportCameraSizeWatchDog::StartWatchDog()
{
	TRACE(_T("CReportCameraSizeWatchDog::StartWatchDog\n"));

	BOOL bRet = FALSE;
	switch(m_ReportCameraConfig.m_Action)
	{
	case REPORT_CAMERA_RECORD_VIDEO:
		break;
	case REPORT_CAMERA_SHOOT_PHOTO:
		break;
	}

	m_bWatchDogStarted = TRUE;

	return bRet;
}

BOOL CReportCameraSizeWatchDog::StopWatchDog()
{
	TRACE(_T("CReportCameraSizeWatchDog::StopWatchDog\n"));

	m_bWatchDogStarted = FALSE;

	BOOL bRet = FALSE;
	switch(m_ReportCameraConfig.m_Action)
	{
	case REPORT_CAMERA_RECORD_VIDEO:
		break;
	case REPORT_CAMERA_SHOOT_PHOTO:
		break;
	}

	return bRet;
}

BOOL CReportCameraSizeWatchDog::RestartWatchDog()
{
	StopWatchDog();
	return StartWatchDog();
}


bool CompareMediaFileName(const CTaskMedia& first, const CTaskMedia& second)
{
	CString strFirst = first.m_MediaFileName;
	CString strSecond = second.m_MediaFileName;

	strFirst.MakeLower();
	strSecond.MakeLower();

	return strFirst < strSecond;
}

BOOL CReportCameraSizeWatchDog::RemoveFilesToSizeLimit()
{
	TRACE(_T("CReportCameraSizeWatchDog::RemoveFilesToSizeLimitEventOnCommandGetCameraReportList CFileFind\n"));

	__int64 TotalFileSize = 0;
	CTaskMediaArray CameraReportFileList;

	CString strClientCameraReportFilesDirectory = GetClientCameraReportsFilesDirectory();
 	CFileFind finder;
 	BOOL bWorking = finder.FindFile(strClientCameraReportFilesDirectory + _T("\\") + _T("*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString filename	= finder.GetFileName();
		CString filePath	= finder.GetFilePath();
		CString fileext		= GetFileExtension(filename);

		if (!finder.IsDirectory() && 
			(fileext.CompareNoCase(_T(".avi")) == 0 || fileext.CompareNoCase(_T(".jpg")) == 0)
			)
		{
			//TRACE(_T("%s (%s)\n"), filename, filePath);
			
			CTaskMedia media;
			media.m_MediaFileName = filename;
			media.m_MediaFileSize = finder.GetLength64();

			TotalFileSize += finder.GetLength64();
				
			CameraReportFileList.Add(media);
		}
	}

	//long OneGb = 1024L*1024L*1024L;
	long OneGb = 1024L*1024L*1024L;
	__int64 SizeLimit = m_ReportCameraConfig.m_iReportsSizeLimit*OneGb;
	if (m_ReportCameraConfig.m_iReportsSizeLimit < (TotalFileSize / OneGb))
	{
		// remove 
		std::sort(CameraReportFileList.GetData(), CameraReportFileList.GetData() + CameraReportFileList.GetSize(), CompareMediaFileName);

		for (int i = 0; i < CameraReportFileList.GetSize(); i++)
		{
			CTaskMedia& media = CameraReportFileList[i];
			//TRACE(_T("%s, %lu\n"), media.m_MediaFileName, media.m_MediaFileSize);

			if (TotalFileSize > SizeLimit)
			{
				//TRACE(_T("remove %s\n"), media.m_MediaFileName);

				CString strFileToRemoveWOExt = GetClientCameraReportsFilesDirectory() + GetFileNameOnly(media.m_MediaFileName);
				// remove capture report file
				DeleteFile(strFileToRemoveWOExt + _T(".avi"));
				DeleteFile(strFileToRemoveWOExt + _T(".jpg"));
				// remove log file
				DeleteFile(strFileToRemoveWOExt + _T(".clog"));
				// remove compressed log file
				DeleteFile(strFileToRemoveWOExt + _T(".gz"));

				TotalFileSize -= media.m_MediaFileSize;
			}
			else
				break;
		}
	}

	return TRUE;
}

UINT CReportCameraSizeWatchDog::OnThread(int ThreadNum)
{
	CRequest req;
	CRequest reqSend;
	COptionsMap map;

	CString strData;
	CTaskInfo task;

	CString folder;
	CString strFilename;

	ULONG	OnThreadTimes = 0;
	
	while(!m_Thread.IsStopped())
	{
		//TRACE(_T("CReportCameraSizeWatchDog::OnThread\n"));

// 		HANDLE hEvents[] = {m_hStartWatchDogEvent, m_hStopWatchDogEvent, m_hDirectoryChangeNotification};
// 		DWORD res = WaitForMultipleObjects(3, hEvents, FALSE, 1000);
		HANDLE hEvents[] = {m_hStartWatchDogEvent, m_hStopWatchDogEvent, m_hRemoveFilesToSizeLimitEvent};
		DWORD res = WaitForMultipleObjects(3, hEvents, FALSE, 1000);

		switch (res)
		{
			// start capture
		case WAIT_OBJECT_0:
			StartWatchDog();
			break;
			// stop capture
		case WAIT_OBJECT_0 + 1:
			StopWatchDog();
			break;
			// RemoveFilesToSizeLimitEvent (manual remove)
		case WAIT_OBJECT_0 + 2:
			RemoveFilesToSizeLimit();
			break;
		case WAIT_TIMEOUT:
			break;
		}

		if(m_Thread.WaitEvent(1000) == CThread::WAIT_STATUS_ON_EVENT)
		{
			TRACE(_T("CReportCameraSizeWatchDog::OnThread Wake\n"));
		}
		if(m_Thread.WaitEvent(1000) == -1)
			break;
		//TRACE(_T("CReportCameraSizeWatchDog::OnThread2\n"));

		OnThreadTimes++;
		if (m_bWatchDogStarted)
		{
			//if (m_Thread.WaitEvent(1000) == CThread::WAIT_STATUS_TIMEOUT)
			if(OnThreadTimes > 6)
			{
				TRACE(_T("Start RemoveFilesToSizeLimit by timeout\n"));
				RemoveFilesToSizeLimit();

				OnThreadTimes = 0;
			}
		}

	}

	TRACE(_T("CReportCameraSizeWatchDog::OnThread end\n"));

	return 0;
}
