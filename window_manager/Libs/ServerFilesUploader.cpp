// ServerFilesUploader.cpp: implementation of the CServerFilesUploader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerFilesUploader.h"

#include "Commands.h"

#include "FileOperation.h"
#include "ScheduleTaskInfo.h"

#include "PClient.h"

#include "TaskInfo.h"
#include "ScheduleInfo.h"
#include "Schedule.h"
#include "SuperSchedule.h"
#include "SiteArray.h"

#include "XDelta.h"
#include <set>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerFilesUploader::CServerFilesUploader(CLinkPoint *pLink)
{
	m_pLink = pLink;

	m_Thread.SetName(_T("CServerFilesUploader"));
	m_Thread.Create(this, 0);

	m_Client = NULL;
}

CServerFilesUploader::~CServerFilesUploader()
{
	m_Client = NULL;
}

void CServerFilesUploader::SetClient(CPClient* Client)
{
	m_Client = Client;
}

BOOL CServerFilesUploader::AddRequest(CRequest &req)
{
	TRACE(_T("CServerFilesUploader::AddRequest\n"));
	m_RequestBuf.Push(req);
	m_Thread.SetEvent();
	return TRUE;
}

void CServerFilesUploader::RemoveRequest(CRequest &req)
{
	CRequest* cur_req;

	cur_req = m_RequestBuf.GetFirst();

	while (cur_req !=NULL)
	{
		if (*cur_req == req)
		{
			cur_req = m_RequestBuf.RemoveCurrentAndGetNext();
		}
		else
		{
			cur_req = m_RequestBuf.GetNext();
		}
	}

	m_Thread.SetEvent();
}

void CServerFilesUploader::SetStop()
{
	m_Thread.SetStop();
}

BOOL CServerFilesUploader::IsWorking()
{
	return m_Thread.IsActive();
}

BOOL CServerFilesUploader::StopUploadFiles()
{
	if (m_Client)
	{
		m_Client->m_bStartedUpload = FALSE;
		m_Client->m_IsUploadingFiles = FALSE;
	}

	CCritSectLock lock(&m_CritSect);

	m_arrMedia.RemoveAll();
	
	return TRUE;
}

BOOL CServerFilesUploader::GetMedia()
{
	CCritSectLock lock(&m_CritSect);

	int i;
	TRACE(_T("CServerFilesUploader::GetMedia\n"));

	int iMediaCount;
	int iMediaUploaded = 0;

	std::set<CTaskMedia> mediaset;
	for (i = 0; i< m_arrMedia.GetSize(); i++)
	{
		mediaset.insert(m_arrMedia[i]);
	}

	iMediaCount = mediaset.size();

	for (std::set<CTaskMedia>::iterator it = mediaset.begin(); it != mediaset.end(); it++)
	{
		CString sLocalFilePath = GetServerLocalFilePath((*it).m_MediaFileName, (*it).m_MD5Signature);
		if (!IsFileExists(sLocalFilePath))
		{
			CString hash;
			CString fname;
			LONGLONG size;
			CString sLocalPrevMediaName;

			int len = (*it).m_patches.GetSize();
			if (len)
			{
				hash = (*it).m_patches[len-1].m_MD5Signature;
				size = (*it).m_patches[len-1].m_Size;
				fname = _T("");
				if (!GetMediaFileByHash(GetServerFilesDirectory(), (*it).m_patches[len-1].m_strSource, sLocalPrevMediaName))
				{
					TRACE(_T("CServerFilesUploader::GetMedia Unable to find source media"));
					return FALSE;
				}
			}
			else
			{
				hash = (*it).m_MD5Signature;
				fname = (*it).m_MediaFileName;
				size = (*it).m_MediaFileSize;
			}
			CString sLocalDownloadFilePath = GetServerLocalFilePath(fname, hash);

			// if file not exist we start downloading
			while (!IsFileExists(sLocalDownloadFilePath) && m_Client && m_Client->m_IsUploadingFiles)
			{
				if (m_Client)
				{
					m_Client->FileUpload(fname, hash, size);
				}
				while (m_Client && m_Client->m_bStartedUpload != 0)
				{
					if (m_Thread.IsStopped())
					{
						TRACE(_T("CServerFilesUploader::GetMedia m_Thread.IsStopped()\n"));
						if (m_Client)
						{
							m_Client->m_bStartedUpload = 0;
							m_Client->m_IsUploadingFiles = 0;
						}
						m_arrMedia.RemoveAll();
						return FALSE;
					}
					Sleep(50);
					// Check download media content timeout
					if(m_Client && m_Client->m_bStartedUpload && --m_Client->m_bStartedUpload == 0)
					{
						TRACE(_T("CServerFilesUploader::GetMedia m_bStartedUpload = 0\n"));

						if (m_Client)
						{
							CloseHandle(m_Client->m_hUploadFile);
							m_Client->m_hUploadFile = INVALID_HANDLE_VALUE;
						}
						return FALSE;
					}
				}
			}

			if (len)
			{
				if (!XDeltaPatch(sLocalPrevMediaName, sLocalDownloadFilePath, sLocalFilePath + _T("patching")))
				{
					TRACE(_T("CServerFilesUploader::GetMedia: XDeltaPatch failed len=%d"), len);
					TRACE(_T("CServerFilesUploader::GetMedia: sLocalPrevMediaName=%s"), sLocalPrevMediaName);
					TRACE(_T("CServerFilesUploader::GetMedia: sLocalDownloadFilePath=%s"), sLocalDownloadFilePath);
					TRACE(_T("CServerFilesUploader::GetMedia: sLocalFilePath=%s"), sLocalFilePath);
				}
				else
					MoveFile(sLocalFilePath + _T("patching"), sLocalFilePath);
			}
		}
		if (IsFileExists(sLocalFilePath))
		{
			iMediaUploaded ++;
		}
	}

	if (m_Client)
	{
		m_Client->m_bStartedUpload = 0;
		m_Client->m_IsUploadingFiles = 0;
	}
	m_arrMedia.RemoveAll();

	if (iMediaCount != iMediaUploaded)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CServerFilesUploader::CheckAndPrepareGetTaskMedia(CTaskInfo &task)
{
	// is need files
	if((task.m_TaskType == TASK_TYPE_BACKGROUND_TASK ||
		task.m_TaskType == TASK_TYPE_POWERPOINT ||
		task.m_TaskType == TASK_TYPE_FLASH ||
		task.m_TaskType == TASK_TYPE_VIDEO ||
		task.m_TaskType == TASK_TYPE_PDF ||
		task.m_TaskType == TASK_TYPE_IMAGE ||
		task.m_TaskType == TASK_TYPE_UPDATE ||
		task.m_TaskType == TASK_TYPE_EXCEL) &&
		!task.m_bPlayList)
	{
		if (!task.m_MainMedia.m_MD5Signature.IsEmpty())
		{
			m_arrMedia.Add(task.m_MainMedia);
		}
		else
		{
			CTaskMedia media;
			media.m_MediaFileName = task.m_strProgramPath;
			media.m_MediaFileSize = task.m_DocumentSize;
			media.m_MD5Signature = task.m_MD5Signature;
			m_arrMedia.Add(media);
		}
	}

	// is there are creeping line
	for (int i = 0; i < task.m_DefaultScheduleTaskInfo.m_arrBackgroundTaskMedia.GetSize(); i++)
	{
		m_arrMedia.Add(task.m_DefaultScheduleTaskInfo.m_arrBackgroundTaskMedia[i]);
	}

	// is there are additional media files
	for (i = 0; i < task.m_arrMedia.GetSize(); i++)
	{
		m_arrMedia.Add(task.m_arrMedia[i]);
	}

	// is there are playlist media files
	for (i = 0; i < task.m_arrPlayListMedia.GetSize(); i++)
	{
		m_arrMedia.Add(task.m_arrPlayListMedia[i]);
	}

	return TRUE;
}

BOOL CServerFilesUploader::CheckAndPrepareGetScheduleMedia(CScheduleInfo *pscheduleinfo)
{
	int i, j, v;
	CSchedule schedule;
	CSuperSchedule superschedule;

	//
	switch (pscheduleinfo->m_ObjectType)
	{
	case SCHEDULE_OBJECT_TYPE_SIMPLE:
		schedule = *(CSchedule*)pscheduleinfo;
		if (schedule.m_bUseTemplate)
		{
			CTaskMedia media;
			media.m_MediaFileName = schedule.m_Template.m_strTemplatePath;
			media.m_MediaFileSize = schedule.m_Template.m_TemplateSize;
			media.m_MD5Signature = schedule.m_Template.m_MD5Signature;
			
			m_arrMedia.Add(media);
		}

		// is there are creeping line
		for (i = 0; i < schedule.m_arrScheduleTaskInfo.GetSize(); i++)
		{
			CScheduleItem &ScheduleTaskInfo = schedule.m_arrScheduleTaskInfo[i];
			for (j = 0; j < ScheduleTaskInfo.m_ScheduleTaskInfo.m_arrBackgroundTaskMedia.GetSize(); j++)
			{
				m_arrMedia.Add(ScheduleTaskInfo.m_ScheduleTaskInfo.m_arrBackgroundTaskMedia[j]);
			}
		}
		break;

	case SCHEDULE_OBJECT_TYPE_WALL:
		superschedule = *(CSuperSchedule*)pscheduleinfo;

		for(i = 0; i < superschedule.GetCount(); i++)
		{
			CScheduleGroup &group = superschedule[i];
			if (group.m_bUseTemplate && group.m_Template.m_TemplateSize > 0)
			{
				CTaskMedia media;
				media.m_MediaFileName = GetClientTemplatesFilesDirectory() + group.m_Template.m_strTemplatePath;
				media.m_MediaFileSize = group.m_Template.m_TemplateSize;
				media.m_MD5Signature = group.m_Template.m_MD5Signature;
				
				m_arrMedia.Add(media);
			}
			
			for(v = 0; v < group.m_ViewTasks.GetSize(); v++)
			{
				if(!CheckAndPrepareGetTaskMedia(group.m_ViewTasks[v].m_Task))
					return FALSE;
			}
		}
		break;
	}

	return TRUE;
}

BOOL CServerFilesUploader::CheckAndPrepareGetUpdatePlayerFirmwareMedia(const CString& spec)
{
	CTaskMedia media;
	COptionsMap map;

	map.FromString(spec);
	if (!map.IsOptionExists(KEY_HASH))
		return FALSE;
	if (!map.IsOptionExists(KEY_SIZE))
		return FALSE;

	media.m_MD5Signature = map.GetOption(KEY_HASH);
	media.m_MediaFileSize = map.GetOptionLONGLONG(KEY_SIZE);
	media.m_MediaFileName = media.m_MD5Signature + _T(".img");
	
	m_Client->m_pServiceData->m_UpdatesMutex.Lock();
	m_Client->m_pServiceData->m_Updates.insert(media);
	m_Client->m_pServiceData->m_UpdatesMutex.Unlock();

	m_arrMedia.Add(media);
	return TRUE;
}

BOOL CServerFilesUploader::CheckAndPrepareSitesMedia(const CString& ini)
{
	std::set<CTaskMedia> site_media;
	std::set<CTaskMedia>::iterator it;

	CSiteArray sites;
	CString iniCopy = ini;
	sites.FromString(iniCopy);

	sites.GetMedia(site_media);
	for (it = site_media.begin(); it != site_media.end(); it++)
	{
		m_arrMedia.Add(*it);
	}
	return TRUE;
}

UINT CServerFilesUploader::OnThread(int ThreadNum)
{
	CRequest req;
	CRequest reqSend;
	COptionsMap map;

	CString strData;
	CTaskInfo task;

	CScheduleInfo *pschedule;

	CString folder;
	CString strFilename;
	
	while(!m_Thread.IsStopped())
	{
		if(m_Thread.WaitEvent() == CThread::WAIT_STATUS_ON_EVENT)
		{
			TRACE(_T("CServerFilesUploader::OnThread Wake\n"));
			while(m_RequestBuf.Pop(req) && !m_Thread.IsStopped())
			{
				TRACE(_T("CServerFilesUploader::OnThread PopElement\n"));
				switch (req.m_iCommand)
				{
				case MESSAGE_TYPE_SET_SERVER_SITES:
					TRACE(_T("CServerFilesUploader::OnThread MESSAGE_TYPE_SET_SERVER_SITES\n"));
					if (CheckAndPrepareSitesMedia(req.GetDataAsString()))
					{
						if (GetMedia() && m_Client)
						{
							m_Client->OnMessageSetServerSites(req);
						}
					}
					break;
				case MESSAGE_TYPE_SET_TASK_BY_FILENAME:
					TRACE(_T("CServerFilesUploader::OnThread MESSAGE_TYPE_SET_TASK_BY_FILENAME\n"));
					strData = req.GetDataAsString();
					task.SetValues(strData);
					if (!task.UpdateDynamicMedia())
					{
						m_Client->SendErrorReport(MESSAGE_TYPE_SET_TASK_ERROR, 0, 
							_T("Can't save dynamic task, not all media available at server"));
					} 
					else 
					{
						if (CheckAndPrepareGetTaskMedia(task))
						{
							if (GetMedia() && m_Client)
							{
								m_Client->OnMessageSetTaskByFilename(req);
							}
						}
					}
					break;
				case MESSAGE_TYPE_SET_SCHEDULE_BY_FILENAME:
					TRACE(_T("CServerFilesUploader::OnThread MESSAGE_TYPE_SET_SCHEDULE_BY_FILENAME\n"));
					strData = req.GetDataAsString();
					folder = GetSchedulesIniFilesDirectory();
					strFilename = folder + GetLine(strData);

					pschedule = CScheduleInfo::CreateFromString(strData, strFilename);
					if (!pschedule->UpdateDynamicMedia())
					{
						m_Client->SendErrorReport(MESSAGE_TYPE_SET_SCHEDULE_ERROR, 0, 
							_T("Can't save dynamic task in schedule, not all media available at server"));
					}
					else
					{
						if (CheckAndPrepareGetScheduleMedia(pschedule))
						{
							if (GetMedia() && m_Client)
							{
								m_Client->OnMessageSetScheduleByFilename(req);
							}
						}
					}
					delete pschedule;
					break;
				case MESSAGE_TYPE_NEW_TASK:
					TRACE(_T("CServerFilesUploader::OnThread MESSAGE_TYPE_NEW_TASK\n"));
					strData = req.GetDataAsString();
					task.SetValues(strData);
					if (!task.UpdateDynamicMedia())
					{
						m_Client->SendErrorReport(MESSAGE_TYPE_NEW_TASK_ERROR, 0, 
							_T("Can't save dynamic task, not all media available at server"));
					}
					else
					{
						if (CheckAndPrepareGetTaskMedia(task))
						{
							if (GetMedia() && m_Client)
							{
								m_Client->OnMessageNewTask(req);
							}
						}
					}
					break;
				case MESSAGE_TYPE_NEW_SCHEDULE:
					TRACE(_T("CServerFilesUploader::OnThread MESSAGE_TYPE_NEW_SCHEDULE\n"));
					strData = req.GetDataAsString();
					folder = GetSchedulesIniFilesDirectory();
					strFilename = folder + GetLine(strData);
					
					pschedule = CScheduleInfo::CreateFromString(strData, strFilename);
					if (!pschedule->UpdateDynamicMedia())
					{
						m_Client->SendErrorReport(MESSAGE_TYPE_NEW_SCHEDULE_ERROR, 0, 
							_T("Can't save dynamic task in schedule, not all media available at server"));
					}
					else
					{
						if (CheckAndPrepareGetScheduleMedia(pschedule))
						{
							if (GetMedia() && m_Client)
							{
								m_Client->OnMessageNewSchedule(req);
							}
						}
					}
					break;
				case MESSAGE_TYPE_UPDATE_PLAYER_FIRMWARE:
					TRACE(_T("CServerFilesUploader::OnThread MESSAGE_TYPE_UPDATE_PLAYER_FIRMWARE\n"));
					strData  = req.GetDataAsString();
					if (CheckAndPrepareGetUpdatePlayerFirmwareMedia(strData))
					{
						if (GetMedia() && m_Client)
						{
							m_Client->OnMessageTypeUpdatePlayerFirmware(req);
						}
					}
					break;
				default:
					TRACE(_T("CServerFilesUploader::OnThread Unexpected message\n"));
				}
			}
			if (m_Client)
			{
				m_Client->m_IsUploadingFiles = FALSE;
			}
		}
	}
	return 0;
}

