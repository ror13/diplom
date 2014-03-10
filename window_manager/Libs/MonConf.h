// MonConf.h

#ifndef _MONCONF_H_
#define _MONCONF_H_

#include <list>

#include "IntArray.h"

#define MAX_MON_COUNT 256

class CMonConfClient
{
public:
	virtual void OnMonitor(	HMONITOR hMonitor,	// handle to display monitor
							HDC hdcMonitor,		// handle to monitor DC
							LPRECT lprcMonitor,	// monitor intersection rectangle
							MONITORINFOEX *pInfo) = 0;
};

class CMonConf
{
public:
	struct MON_ITEM
	{
		MONITORINFOEX	Info;
		HMONITOR		hMonitor;
		int				MonNumber;
	};

	CMonConf();
	
	void SetClient(CMonConfClient *pClient){m_pClient = pClient;}

	BOOL RefreshInfo(	HDC hdc = NULL,				// handle to display DC
						LPCRECT lprcClip = NULL		// clipping rectangle
						);

	RECT GetViewRecktangle(LPCTSTR MonAssock);

	RECT GetMonitorRect(int MonitorNum);
	
	BOOL CheckMonConfig(int cols, int rows, CString MonitorsAssock);

	int			m_CountMonInfo;
	MON_ITEM	m_MonInfoArray[MAX_MON_COUNT];

protected:
	CMonConfClient	*m_pClient;

	static int CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor,
		LPRECT lprcMonitor, LPARAM dwData);
};


#endif // _MONCONF_H_
