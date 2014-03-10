// MonConf.cpp

#include "StdAfx.h"

#include "MonConf.h"

CMonConf::CMonConf()
{
	m_CountMonInfo = 0;
	memset(m_MonInfoArray, 0, sizeof(m_MonInfoArray));
	m_pClient = NULL;
}

int CALLBACK CMonConf::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor,
		LPRECT lprcMonitor, LPARAM dwData)
{
	CMonConf *pThis = (CMonConf*)dwData;
	
	pThis->m_MonInfoArray[pThis->m_CountMonInfo].hMonitor = hMonitor;
	pThis->m_MonInfoArray[pThis->m_CountMonInfo].Info.cbSize = sizeof(MONITORINFOEX);
	
	int res = GetMonitorInfo(hMonitor,
		(LPMONITORINFO)&(pThis->m_MonInfoArray[pThis->m_CountMonInfo].Info));

	DISPLAY_DEVICE DisplayDevice;
	memset(&DisplayDevice, 0, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);
	if (EnumDisplayDevices(pThis->m_MonInfoArray[pThis->m_CountMonInfo].Info.szDevice, 0, 
		&DisplayDevice, 0))
	{
		if((DisplayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0)
		{
			return TRUE;
		}
	}
	else
	{
		return TRUE;
	}

	_stscanf(pThis->m_MonInfoArray[pThis->m_CountMonInfo].Info.szDevice,
		_T("\\\\.\\DISPLAY%d"), &pThis->m_MonInfoArray[pThis->m_CountMonInfo].MonNumber);

	if(pThis->m_pClient)
		pThis->m_pClient->OnMonitor(hMonitor, hdcMonitor, lprcMonitor, &(pThis->m_MonInfoArray[pThis->m_CountMonInfo].Info));
	
	if(res)
		pThis->m_CountMonInfo++;

	if(pThis->m_CountMonInfo >= MAX_MON_COUNT)
		res = FALSE;	// Stop the enumeration
	
	return res;
}

BOOL CMonConf::RefreshInfo(HDC hdc, LPCRECT lprcClip)
{
	m_CountMonInfo = 0;
	memset(m_MonInfoArray, 0, sizeof(m_MonInfoArray));
	
	BOOL bGetMonInfo = EnumDisplayMonitors(hdc, lprcClip,
		MonitorEnumProc, (LPARAM)this);
	
	return bGetMonInfo;
}

RECT CMonConf::GetViewRecktangle(LPCTSTR MonAssock)
{
	BOOL bInit = FALSE;
	CRect rect(0,0,0,0);
	
	INT_ARRAY arr;
	StrToIntArray(arr, MonAssock);
	
	for(int i = 0; i < m_CountMonInfo; i++)
	{
		if(arr.size())
		{
			if(IsIntPresent(arr, m_MonInfoArray[i].MonNumber))
			{
				if(bInit)
					rect.UnionRect(&rect, &m_MonInfoArray[i].Info.rcMonitor);
				else
				{
					rect = m_MonInfoArray[i].Info.rcMonitor;
					bInit = TRUE;
				}
			}
		} else
		{
			if(m_MonInfoArray[i].Info.dwFlags & MONITORINFOF_PRIMARY)
			{
				rect = m_MonInfoArray[i].Info.rcMonitor;
				break;
			}
		}
	}
	
	return rect;
}

RECT CMonConf::GetMonitorRect(int MonitorNum)
{
	CRect rect(0,0,0,0);
	
	for(int i = 0; i < m_CountMonInfo; i++)
	{
		if(m_MonInfoArray[i].MonNumber == MonitorNum)
		{
			rect = m_MonInfoArray[i].Info.rcMonitor;
			break;
		}
	}
	
	return rect;
}

BOOL CMonConf::CheckMonConfig(int cols, int rows, CString MonitorsAssock)
{
	INT_ARRAY arr;
	StrToIntArray(arr, MonitorsAssock);

	if(arr.size() != cols * rows)
		return FALSE;

	CRect wall_rect = GetViewRecktangle(MonitorsAssock);

	CPoint MonOffset(0, 0);

	INT_ARRAY::iterator i = arr.begin();

	for(int y = 0; y < rows; y++)
	{
		MonOffset.x = 0;

		CRect mon_rect;

		for(int x = 0; x < cols; x++)
		{
			mon_rect = GetMonitorRect(*i++);

			if(mon_rect.IsRectEmpty())
				return FALSE;

			CPoint top_left = wall_rect.TopLeft() + MonOffset;
			CSize mon_size = mon_rect.Size();

			CRect ideal_mon_rect(top_left, mon_size);

			if(mon_rect != ideal_mon_rect)
				return FALSE;

			MonOffset.x += mon_rect.Width();
		}

		MonOffset.y += mon_rect.Height();
	}

	return TRUE;
}
