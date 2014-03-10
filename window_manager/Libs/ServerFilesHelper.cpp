#include "stdafx.h"
#include "ServerFilesHelper.h"

CTaskMediaArray GetServerMediaFiles(CTaskArray tasks, CScheduleArray schedules)
{
	CTaskMediaArray arrMedias;
	int k;

	tasks.Lock();
	int TaskCount = tasks.GetTaskCount();
	for(int i = 0; i < TaskCount; i++)
	{
		CTaskInfo task;
		tasks.GetTask(i, task);
		
		CTaskMediaArray arrTaskMedia;
		arrTaskMedia = task.GetAllMedia();
		for(int j = 0; j < arrTaskMedia.GetSize(); j++)
		{
			for(k = 0; k < arrMedias.GetSize(); k++)
				if(arrMedias[k] == arrTaskMedia[j])
					break;
			if (k == arrMedias.GetSize())
				arrMedias.Add(arrTaskMedia[j]);
		}
	}
	tasks.Unlock();

	schedules.Lock();
	int ScheduleCount = schedules.GetSize();
	for(i = 0; i < ScheduleCount; i++)
	{
		CScheduleInfo *pSchedule = schedules[i];
		CTaskMediaArray arrScheduleMedia;

		arrScheduleMedia = pSchedule->GetAllMedia();
		for(int j = 0; j < arrScheduleMedia.GetSize(); j++)
		{
			for(k = 0; k < arrMedias.GetSize(); k++)
				if(arrMedias[k] == arrScheduleMedia[j])
					break;
			if (k == arrMedias.GetSize())
				arrMedias.Add(arrScheduleMedia[j]);
		}
	}
	schedules.Unlock();

	return arrMedias;
}