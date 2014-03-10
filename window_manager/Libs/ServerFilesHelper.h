#include "stdafx.h"

#pragma once

#include "../Presenter/Presenter.h"
#include "TaskInfo.h"
#include "ScheduleInfo.h"

struct MediaFileInfo
{
	CString MD5;
	__int64 FileSize;
	CString FileName;
};

CTaskMediaArray GetServerMediaFiles(CTaskArray tasks, CScheduleArray schedules);
