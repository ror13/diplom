// WallMonConfig.cpp: implementation of the CWallMonConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallMonConfig.h"

#include "MonConf.h"

#include "Product.h"
#include "CommonApi.h"

#include "SignWallHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// #ifdef _DEBUG
// #define CHANGEDISPLAYSETTINGSEX(x1, x2, x3, x4, x5) (DISP_CHANGE_SUCCESSFUL)
// #define CHANGEDISPLAYSETTINGS(x1, x2)				(DISP_CHANGE_SUCCESSFUL)
// #else
// #define CHANGEDISPLAYSETTINGSEX(x1, x2, x3, x4, x5) ChangeDisplaySettingsEx(x1, x2, x3, x4, x5)
// #define CHANGEDISPLAYSETTINGS(x1, x2)				ChangeDisplaySettings(x1, x2)
// #endif
#define CHANGEDISPLAYSETTINGSEX(x1, x2, x3, x4, x5) ChangeDisplaySettingsEx(x1, x2, x3, x4, x5)
#define CHANGEDISPLAYSETTINGS(x1, x2)				ChangeDisplaySettings(x1, x2)

//////////////////////////////////////////////////////////////////////////
// MON_MODE

MON_MODE::MON_MODE()
{
	dmBitsPerPel = 0;
	dmPelsWidth = 0;
	dmPelsHeight = 0;
	dmDisplayFlags = 0;
	dmDisplayFrequency = 0;
}

BOOL MON_MODE::GetMonMode(LPCTSTR lpszDeviceName, DWORD iModeNum)
{
	DEVMODE dm = {0};
	dm.dmSize = sizeof(dm);
	BOOL res = EnumDisplaySettingsEx(lpszDeviceName, iModeNum, &dm, EDS_RAWMODE);
	if(res)
	{
		dmBitsPerPel = dm.dmBitsPerPel;
		dmPelsWidth = dm.dmPelsWidth;
		dmPelsHeight = dm.dmPelsHeight;
		dmDisplayFlags = dm.dmDisplayFlags;
		dmDisplayFrequency = dm.dmDisplayFrequency;
	}
	return res;
}

BOOL MON_MODE::operator ==(const MON_MODE &mode)
{
	if(dmBitsPerPel != mode.dmBitsPerPel)
	{
		TRACE(_T("MON_MODE::== dmBitsPerPel != mode.dmBitsPerPel\n"));
		return FALSE;
	}
	if(dmPelsWidth != mode.dmPelsWidth)
	{
		TRACE(_T("MON_MODE::== dmPelsWidth != mode.dmPelsWidth\n"));
		return FALSE;
	}
	if(dmPelsHeight != mode.dmPelsHeight)
	{
		TRACE(_T("MON_MODE::== dmPelsHeight != mode.dmPelsHeight\n"));
		return FALSE;
	}
	if(dmDisplayFlags != mode.dmDisplayFlags)
	{
		TRACE(_T("MON_MODE::== dmDisplayFlags != mode.dmDisplayFlags\n"));
		return FALSE;
	}
	if(dmDisplayFrequency != mode.dmDisplayFrequency)
	{
		TRACE(_T("MON_MODE::== dmDisplayFrequency != mode.dmDisplayFrequency\n"));
		return FALSE;
	}
	return TRUE;
}

BOOL MON_MODE::operator !=(const MON_MODE &mode)
{
	return !(*this == mode);
}

CString MON_MODE::Serialize()
{
	CString str;
	str.Format(_T("BitsPerPel:%d; PelsWidth:%d; PelsHeight:%d; DisplayFlags:%d; DisplayFrequency:%d"),
		dmBitsPerPel, dmPelsWidth, dmPelsHeight, dmDisplayFlags, dmDisplayFrequency);
	return str;
}

BOOL MON_MODE::Unserialize(CString strStruct)
{
	COptionsMap map;
	FromStringToMap(strStruct, map);

	dmBitsPerPel = _ttoi(map.GetOption(_T("BitsPerPel")));
	dmPelsWidth = _ttoi(map.GetOption(_T("PelsWidth")));
	dmPelsHeight = _ttoi(map.GetOption(_T("PelsHeight")));
	dmDisplayFlags = _ttoi(map.GetOption(_T("DisplayFlags")));
	dmDisplayFrequency = _ttoi(map.GetOption(_T("DisplayFrequency")));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// MON_CONF

MON_CONF::MON_CONF()
{
	memset(m_DeviceName, 0, sizeof(m_DeviceName));
	memset(m_DeviceString, 0, sizeof(m_DeviceString));
	m_MonNum = 0;
	memset(&m_MonRect, 0, sizeof(m_MonRect));
	m_bAttached = FALSE;
	m_bPrimary = FALSE;

	m_MonPosCol = -1;
	m_MonPosRow = -1;
}

BOOL MON_CONF::operator ==(MON_CONF &conf)
{
	if(lstrcmp(m_DeviceName, conf.m_DeviceName) != 0)
	{
		TRACE(_T("MON_CONF::== m_DeviceName != conf.m_DeviceName\n"));
		return FALSE;
	}
	if(lstrcmp(m_DeviceString, conf.m_DeviceString) != 0)
	{
		TRACE(_T("MON_CONF::== m_DeviceString != conf.m_DeviceString\n"));
		return FALSE;
	}
	if(m_MonNum != conf.m_MonNum)
	{
		TRACE(_T("MON_CONF::== m_MonNum != conf.m_MonNum\n"));
		return FALSE;
	}
	if(m_bAttached != conf.m_bAttached)
	{
		TRACE(_T("MON_CONF::== m_bAttached != conf.m_bAttached\n"));
		return FALSE;
	}

	if(m_bAttached &&
		(m_MonRect != conf.m_MonRect))
	{
		TRACE(_T("MON_CONF::== m_MonRect != conf.m_MonRect\n"));
		return FALSE;
	}
	if(m_bAttached &&
		(m_bPrimary != conf.m_bPrimary))
	{
		TRACE(_T("MON_CONF::== m_bPrimary != conf.m_bPrimary\n"));
		return FALSE;
	}
	if(m_bAttached &&
		(m_CurrentMode != conf.m_CurrentMode))
	{
		TRACE(_T("MON_CONF::== m_CurrentMode != conf.m_CurrentMode\n"));
		return FALSE;
	}

	return TRUE;
}

BOOL MON_CONF::operator !=(MON_CONF &conf)
{
	return !(*this == conf);
}

//////////////////////////////////////////////////////////////////////////
// CMultiMonConfig

CMultiMonConfig::CMultiMonConfig()
{
	m_bAttachPrimaryMonitor = FALSE;
	m_iPrimaryMonitor = 1;
}

BOOL CMultiMonConfig::GetSystemConfig(BOOL bReset)
{
	m_Mons.clear();
	
	CMonConf screen_conf;
	screen_conf.RefreshInfo();
	
	// enum all display devices
	DISPLAY_DEVICE DisplayDevice;
	memset(&DisplayDevice, 0, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);
	
	DWORD d = 0;
	while(EnumDisplayDevices(NULL, d++, &DisplayDevice, 0))
	{
		if((DisplayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) == 0)
		{
			MON_CONF mon;
			memset(&mon, 0, sizeof(mon));
			_tcscpy(mon.m_DeviceName, DisplayDevice.DeviceName);
			_tcscpy(mon.m_DeviceString, DisplayDevice.DeviceString);
			_stscanf(DisplayDevice.DeviceName, _T("\\\\.\\DISPLAY%d"), &mon.m_MonNum);
			
			mon.m_bAttached =
				(DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
			mon.m_bPrimary =
				(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) == DISPLAY_DEVICE_PRIMARY_DEVICE;
			
			if (bReset)
			{
				mon.m_MonPosCol = -1;
				mon.m_MonPosRow = -1;
				mon.m_bAttached = FALSE;
			}

			if (mon.m_bPrimary)
			{
				mon.m_bAttached = TRUE;
			}

			TRACE(_T("CMultiMonConfig::GetSystemConfig Display device %d: %s %s %s\n"), mon.m_MonNum,
				mon.m_DeviceName, mon.m_DeviceString,
				(LPCTSTR)GetDisplayDeviceStateFlagsAsString(DisplayDevice.StateFlags));
			
			DWORD m = 0;
			MON_MODE mode;
			while(mode.GetMonMode(mon.m_DeviceName, m++))
			{
				mon.m_Modes.push_back(mode);
			}
			
			mon.m_CurrentMode.GetMonMode(mon.m_DeviceName, ENUM_CURRENT_SETTINGS);
			
			if(mon.m_bAttached)
			{
				for(int i = 0; i < screen_conf.m_CountMonInfo; i++)
				{
					if(_tcscmp(screen_conf.m_MonInfoArray[i].Info.szDevice,
						mon.m_DeviceName) == 0)
					{
						mon.m_MonRect = screen_conf.m_MonInfoArray[i].Info.rcMonitor;
						break;
					}
				}
			}
			
			m_Mons.push_back(mon);
		}
	}
	
	return TRUE;
}

BOOL CMultiMonConfig::ChangePrimaryMonitor(CString NewPrimaryMonitor)
{
	// off all monitor except primary
	{
		MON_CONF_ARRAY::iterator i;
		for(i = m_Mons.begin(); i != m_Mons.end(); i++)
		{
			const MON_CONF &mon = *i;
			
			if (mon.m_DeviceName != NewPrimaryMonitor)
			{
				// off all monitors except new primary
				DEVMODE dm;
				ZeroMemory(&dm, sizeof(dm));
				dm.dmSize = sizeof(dm);
// 				dm.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_POSITION|
// 					DM_DISPLAYFREQUENCY;
				dm.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_POSITION;
				
				LONG res = CHANGEDISPLAYSETTINGSEX(mon.m_DeviceName, &dm, NULL,
					CDS_NORESET|CDS_UPDATEREGISTRY, NULL);
				if(res != DISP_CHANGE_SUCCESSFUL)
				{
					CString Msg;
					Msg.Format(_T("Monitor%d: %s"), mon.m_MonNum, GetErrorMessageString(res));
					AfxMessageBox(Msg, MB_OK|MB_ICONSTOP);
					// 				return FALSE;
					continue;
				}
			}
		}
	}
	

	{
		MON_CONF mon = *GetMonitorByName(NewPrimaryMonitor);

		m_bAttachPrimaryMonitor = TRUE;
		m_iPrimaryMonitor = mon.m_MonNum;
		
		DEVMODE dm;
		ZeroMemory(&dm, sizeof(dm));
		dm.dmSize = sizeof(dm);
		
		dm.dmPelsWidth = 1024;
		dm.dmPelsHeight = 768;
		dm.dmBitsPerPel = 32;
		dm.dmDisplayFrequency = 60;
		
		dm.dmPosition.x = 0;
		dm.dmPosition.y = 0;
		
		// 		dm.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY|
		// 			DM_POSITION|DM_DISPLAYFLAGS;
		dm.dmFields = DM_POSITION|DM_DISPLAYFLAGS;
		
		TRACE(_T("Set primary monitor%d: ResX:%d ResY:%d PosX:%d PosY:%d Bits:%d Frequency:%d"),
			mon.m_MonNum,
			dm.dmPelsWidth, dm.dmPelsHeight, dm.dmPosition.x, dm.dmPosition.y,
			dm.dmBitsPerPel, dm.dmDisplayFrequency);
		
		LONG res = CHANGEDISPLAYSETTINGSEX(mon.m_DeviceName, &dm, NULL,
			// 			CDS_RESET|CDS_UPDATEREGISTRY|CDS_SET_PRIMARY, NULL);
			CDS_NORESET|CDS_UPDATEREGISTRY|CDS_SET_PRIMARY, NULL);
		
		TRACE(_T(" Res=%d\n"), res);
		
		if(res != DISP_CHANGE_SUCCESSFUL)
		{
			CString Msg;
			Msg.Format(_T("Monitor%d: %s"), mon.m_MonNum, GetErrorMessageString(res));
			AfxMessageBox(Msg, MB_OK|MB_ICONSTOP);
			return FALSE;
		}
	}
	// apply
	CHANGEDISPLAYSETTINGS(NULL, 0);

	return TRUE;
}

BOOL CMultiMonConfig::SetSystemConfig()
{
	// attach primary monitor
// 	{
// 		MON_CONF *pPrimaryMon = GetPrimaryMonitor();
// 		if(pPrimaryMon)
// 		{
// 			DEVMODE dm;
// 			ZeroMemory(&dm, sizeof(dm));
// 			dm.dmSize = sizeof(dm);
// 			
// 			dm.dmPelsWidth = pPrimaryMon->m_CurrentMode.dmPelsWidth;
// 			dm.dmPelsHeight = pPrimaryMon->m_CurrentMode.dmPelsHeight;
// 			
// 			dm.dmBitsPerPel = pPrimaryMon->m_CurrentMode.dmBitsPerPel;
// 			
// 			dm.dmDisplayFrequency = pPrimaryMon->m_CurrentMode.dmDisplayFrequency;
// 			
// 			dm.dmPosition.x = 0;
// 			dm.dmPosition.y = 0;
// 			
// 			dm.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY|
// 				DM_POSITION;
// 
// 			TRACE(_T("Set primary monitor%d: ResX:%d ResY:%d PosX:%d PosY:%d Bits:%d Frequency:%d"),
// 				pPrimaryMon->m_MonNum,
// 				dm.dmPelsWidth, dm.dmPelsHeight, dm.dmPosition.x, dm.dmPosition.y,
// 				dm.dmBitsPerPel, dm.dmDisplayFrequency);
// 			
// 			LONG res = CHANGEDISPLAYSETTINGSEX(pPrimaryMon->m_DeviceName, &dm, NULL,
// 				CDS_NORESET|CDS_SET_PRIMARY|CDS_UPDATEREGISTRY, NULL);
// 
// 			TRACE(_T(" Res=%d\n"), res);
// 
// 			if(res != DISP_CHANGE_SUCCESSFUL)
// 			{
// 				CString Msg;
// 				Msg.Format(_T("Monitor%d: %s"), pPrimaryMon->m_MonNum, GetErrorMessageString(res));
// 				AfxMessageBox(Msg, MB_OK|MB_ICONSTOP);
// 				return FALSE;
// 			}
// 		}
// 	}

	MON_CONF_ARRAY::iterator i;
	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		const MON_CONF &mon = *i;
		if(mon.m_bAttached && (!mon.m_bPrimary || IsWallWithoutHost()))
		{
			DEVMODE dm;
			ZeroMemory(&dm, sizeof(dm));
			dm.dmSize = sizeof(dm);
			
			dm.dmPelsWidth = mon.m_CurrentMode.dmPelsWidth;
			dm.dmPelsHeight = mon.m_CurrentMode.dmPelsHeight;
			dm.dmBitsPerPel = mon.m_CurrentMode.dmBitsPerPel;
			dm.dmDisplayFrequency = mon.m_CurrentMode.dmDisplayFrequency;

			dm.dmPosition.x = mon.m_MonRect.left;
			dm.dmPosition.y = mon.m_MonRect.top;
			
			dm.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY|
				DM_POSITION|DM_DISPLAYFLAGS;
// 			dm.dmFields =
// 				DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY|DM_POSITION;
			
			TRACE(_T("Set config secondary monitor%d: ResX:%d ResY:%d PosX:%d PosY:%d Bits:%d Frequency:%d (%d/%d)"),
				mon.m_MonNum,
				dm.dmPelsWidth, dm.dmPelsHeight, dm.dmPosition.x, dm.dmPosition.y,
				dm.dmBitsPerPel, dm.dmDisplayFrequency,
				mon.m_bPrimary, mon.m_bAttached);

			LONG res = CHANGEDISPLAYSETTINGSEX(mon.m_DeviceName, &dm, NULL,
				(mon.m_bPrimary?CDS_SET_PRIMARY:0)|CDS_NORESET|CDS_UPDATEREGISTRY, NULL);

			TRACE(_T(" Res=%d\n"), res);

			if(res != DISP_CHANGE_SUCCESSFUL)
			{
				CString Msg;
				Msg.Format(_T("SetConfig Monitor%d: %s"), mon.m_MonNum, GetErrorMessageString(res));
				AfxMessageBox(Msg, MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		} else if(!mon.m_bAttached)
		{
			DEVMODE dm;
			ZeroMemory(&dm, sizeof(dm));
			dm.dmSize = sizeof(dm);
//			dm.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_POSITION|
//		 		DM_DISPLAYFREQUENCY;
// 			dm.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_POSITION|
// 		 		DM_DISPLAYFREQUENCY|DM_DISPLAYFLAGS;
			dm.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_POSITION;
			
			LONG res = CHANGEDISPLAYSETTINGSEX(mon.m_DeviceName, &dm, NULL,
				CDS_NORESET|CDS_UPDATEREGISTRY, NULL);
			ChangeDisplaySettingsEx(NULL,NULL,NULL,0,NULL);

			if(res != DISP_CHANGE_SUCCESSFUL)
			{
				CString Msg;
				Msg.Format(_T("SetConfig (not attached) Monitor%d: %s. Continue?"), mon.m_MonNum, GetErrorMessageString(res));
// 				if(AfxMessageBox(Msg, MB_OKCANCEL|MB_ICONSTOP) == IDCANCEL)
// 				{
// 					return FALSE;
// 				}
			}
		}
	}

	CHANGEDISPLAYSETTINGS(NULL, 0);
	
	return TRUE;
}

BOOL CMultiMonConfig::TestSystemConfig()
{
	DEVMODE dm;

	MON_CONF_ARRAY::iterator i;
	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		MON_CONF &mon = *i;
		LONG res;
		if(mon.m_bAttached && (!mon.m_bPrimary || IsWallWithoutHost()))
		{
			ZeroMemory(&dm, sizeof(dm));
			dm.dmSize = sizeof(dm);

			dm.dmPelsWidth = mon.m_CurrentMode.dmPelsWidth;
			dm.dmPelsHeight = mon.m_CurrentMode.dmPelsHeight;
			dm.dmBitsPerPel = mon.m_CurrentMode.dmBitsPerPel;
			dm.dmDisplayFrequency = mon.m_CurrentMode.dmDisplayFrequency;

			dm.dmPosition.x = mon.m_MonRect.left;
			dm.dmPosition.y = mon.m_MonRect.top;

			dm.dmFields =
				DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY|DM_POSITION;

			res = CHANGEDISPLAYSETTINGSEX(mon.m_DeviceName, &dm, NULL,
				(mon.m_bPrimary?CDS_SET_PRIMARY:0)|CDS_TEST, NULL);
			if(res != DISP_CHANGE_SUCCESSFUL)
			{
				CString Msg;
				Msg.Format(_T("TestConfig Monitor%d: %s"), mon.m_MonNum, GetErrorMessageString(res));
				AfxMessageBox(Msg, MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		} else if(!mon.m_bAttached)
		{
			ZeroMemory(&dm, sizeof(dm));
			dm.dmSize = sizeof(dm);
			dm.dmFields =
				DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY|DM_POSITION;
//				/*DM_PELSWIDTH|DM_PELSHEIGHT|*/DM_POSITION;

			res = CHANGEDISPLAYSETTINGSEX(mon.m_DeviceName, &dm, NULL, CDS_TEST, NULL);

			if(res != DISP_CHANGE_SUCCESSFUL)
			{
				CString Msg;
				Msg.Format(_T("TestConfig (not attached) Monitor%d: %s"), mon.m_MonNum, GetErrorMessageString(res));
// 				AfxMessageBox(Msg, MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}

// 		if(res != DISP_CHANGE_SUCCESSFUL)
// 		{
// 			CString Msg;
// 			Msg.Format(_T("Monitor%d: %s"), mon.m_MonNum, GetErrorMessageString(res));
// 			AfxMessageBox(Msg, MB_OK|MB_ICONSTOP);
// 			return FALSE;
// 		}
	}

	return TRUE;
}

void CMultiMonConfig::CalculateMonRects()
{
	int PosX = 0;
	int PosY = 0;
	
	MON_CONF *pPrimaryMon = GetPrimaryMonitor();
	if(pPrimaryMon)
		PosX += pPrimaryMon->m_CurrentMode.dmPelsWidth;

	MON_CONF_ARRAY::iterator i;
	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		MON_CONF &mon = *i;
		if(mon.m_bAttached && (!mon.m_bPrimary || IsWallWithoutHost()))
		{
			mon.m_MonRect.left = PosX;
			mon.m_MonRect.top = PosY;

			mon.m_MonRect.right = PosX + mon.m_CurrentMode.dmPelsWidth;
			mon.m_MonRect.bottom = PosY + mon.m_CurrentMode.dmPelsHeight;

			PosX += mon.m_CurrentMode.dmPelsWidth;
		}
	}
}

CRect CMultiMonConfig::GetWallScreenRect()
{
	CRect R;
	R.SetRectEmpty();
	
	BOOL b_R_is_Inited = FALSE;
	
	MON_CONF_ARRAY::iterator i = m_Mons.begin();
	
	while(i != m_Mons.end())
	{
		MON_CONF &mon = *i;
		if(mon.m_bAttached && (!mon.m_bPrimary || IsWallWithoutHost()))
		{
			if(b_R_is_Inited == FALSE)
			{
				R = mon.m_MonRect;
				b_R_is_Inited = TRUE;
			} else
			{
				RECT tmp = R;
				R.UnionRect(&tmp, &mon.m_MonRect);
			}
		}
		i++;
	}
	
	return R;
}

int CMultiMonConfig::GetMonitorCount()
{
	int Count = 0;
	
	MON_CONF_ARRAY::iterator i = m_Mons.begin();
	while(i != m_Mons.end())
	{
		MON_CONF &mon = *i;
		if(!mon.m_bPrimary || IsWallWithoutHost())
			Count++;
		i++;
	}
	
	return Count;
}

MON_CONF* CMultiMonConfig::GetMonitorByName(CString MonName)
{
	MON_CONF_ARRAY::iterator i = m_Mons.begin();
	while(i != m_Mons.end())
	{
		MON_CONF &mon = *i;
		if(mon.m_DeviceName == MonName)
			return &mon;
		i++;
	}
	return NULL;
}

MON_CONF* CMultiMonConfig::GetMonitorByNumber(int MonNum)
{
	MON_CONF_ARRAY::iterator i = m_Mons.begin();
	while(i != m_Mons.end())
	{
		MON_CONF &mon = *i;
		if(mon.m_MonNum == MonNum)
			return &mon;
		i++;
	}
	return NULL;
}

MON_CONF* CMultiMonConfig::GetPrimaryMonitor()
{
	MON_CONF_ARRAY::iterator i;
	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		MON_CONF &mon = *i;
		if(mon.m_bAttached && mon.m_bPrimary)
			return &mon;
	}
	return NULL;
}

CString CMultiMonConfig::GetErrorMessageString(int Result)
{
	CString Msg;
	switch(Result)
	{
	case DISP_CHANGE_RESTART:
		Msg = _T("The computer must be restarted for the graphics mode to work.");
		break;
	case DISP_CHANGE_FAILED:
		Msg = _T("The display driver failed the specified graphics mode.");
		break;
	case DISP_CHANGE_BADMODE:
		Msg = _T("The graphics mode is not supported.");
		break;
	case DISP_CHANGE_NOTUPDATED:
		Msg = _T("Windows NT/2000/XP: Unable to write settings to the registry.");
		break;
	case DISP_CHANGE_BADFLAGS:
		Msg = _T("An invalid set of flags was passed in.");
		break;
	case DISP_CHANGE_BADPARAM:
		Msg = _T("An invalid parameter was passed in. This can include an invalid flag or combination of flags.");
		break;
	case DISP_CHANGE_BADDUALVIEW:
		Msg = _T("Windows XP: The settings change was unsuccessful because the system is DualView capable.");
		break;
	}
	return Msg;
}

CString CMultiMonConfig::GetDisplayDeviceStateFlagsAsString(DWORD flags)
{
	CString str;
	CString tmp;
	if(flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		tmp = "ATTACHED_TO_DESKTOP";
	else
		tmp.Empty();
	
	if(!str.IsEmpty() && !tmp.IsEmpty())
		str += ", " + tmp;
	else
		str += tmp;
	
	if(flags & DISPLAY_DEVICE_MIRRORING_DRIVER)
		tmp = "MIRRORING_DRIVER";
	else
		tmp.Empty();
	
	if(!str.IsEmpty() && !tmp.IsEmpty())
		str += ", " + tmp;
	else
		str += tmp;
	
	if(flags & DISPLAY_DEVICE_MODESPRUNED)
		tmp = "MODESPRUNED";
	else
		tmp.Empty();
	
	if(!str.IsEmpty() && !tmp.IsEmpty())
		str += ", " + tmp;
	else
		str += tmp;
	
	if(flags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		tmp = "PRIMARY_DEVICE";
	else
		tmp.Empty();
	
	if(!str.IsEmpty() && !tmp.IsEmpty())
		str += ", " + tmp;
	else
		str += tmp;
	
	if(flags & DISPLAY_DEVICE_REMOVABLE)
		tmp = "REMOVABLE";
	else
		tmp.Empty();
	
	if(!str.IsEmpty() && !tmp.IsEmpty())
		str += ", " + tmp;
	else
		str += tmp;
	
	if(flags & DISPLAY_DEVICE_VGA_COMPATIBLE)
		tmp = "VGA_COMPATIBLE";
	else
		tmp.Empty();
	
	if(!str.IsEmpty() && !tmp.IsEmpty())
		str += ", " + tmp;
	else
		str += tmp;
	
	return str;
}

// compare only monitors from wall
BOOL CMultiMonConfig::operator ==(CMultiMonConfig &conf)
{
	MON_CONF_ARRAY::iterator i;

	if (GetMonitorCount() != conf.GetMonitorCount())
	{
		return FALSE;
	}

	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		MON_CONF &mon = *i;
		
		if(mon.m_MonPosCol == -1 || mon.m_MonPosRow == -1)
			continue;

		MON_CONF *pmon = conf.GetMonitorByNumber(mon.m_MonNum);
		if(pmon == NULL)
			return FALSE;
		
		if(mon != *pmon)
			return FALSE;
	}

	return TRUE;
}

BOOL CMultiMonConfig::operator !=(CMultiMonConfig &conf)
{
	return !(*this == conf);
}

//////////////////////////////////////////////////////////////////////////
// CWallMonConfig

CWallMonConfig::CWallMonConfig()
{
	m_DisplayResolutionX = 0;
	m_DisplayResolutionY = 0;
	
	m_RefreshRate = 0;
	
	m_ColorBits = 0;

	m_WallCols = 0;
	m_WallRows = 0;
}

BOOL CWallMonConfig::GetSystemConfig(BOOL bReset)
{
	DEVMODE dm = {0};
	dm.dmSize = sizeof(dm);

	// get default values
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	m_DisplayResolutionX = dm.dmPelsWidth;
	m_DisplayResolutionY = dm.dmPelsHeight;
	m_ColorBits = dm.dmBitsPerPel;
	m_RefreshRate = dm.dmDisplayFrequency;

	
	return CMultiMonConfig::GetSystemConfig(bReset);
}

BOOL CWallMonConfig::GetSystemMonitorConfig()
{
	return CMultiMonConfig::GetSystemConfig(TRUE);
}

BOOL CWallMonConfig::SetSystemConfig()
{
	SaveConfigToRegistry();

	CMultiMonConfig::SetSystemConfig();

	return TRUE;
}

void CWallMonConfig::CalculateMonRects()
{
	MON_CONF_ARRAY::iterator i;
	
	int col = 0;
	int row = 0;
	
	// find max col and row
	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		MON_CONF &mon = *i;
		if(mon.m_bAttached && (!mon.m_bPrimary || IsWallWithoutHost()))
		{
			if(mon.m_MonPosCol > col)
				col = mon.m_MonPosCol;
			if(mon.m_MonPosRow > row)
				row = mon.m_MonPosRow;
		}
	}
	
	// calculate wall offset (all wall displays plaice in negative area)
	POINT offset;
	offset.x = (col+1) * m_DisplayResolutionX;
	offset.y = (row+1) * m_DisplayResolutionY;

	BOOL bIsWallWithoutHost;
	bIsWallWithoutHost = IsWallWithoutHost();
	
	// convert monitors position (col, row) to monitors rect in virtual desctop
	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		MON_CONF &mon = *i;

		if (!bIsWallWithoutHost)
		{
			if(mon.m_bAttached && (!mon.m_bPrimary || IsWallWithoutHost()))
			{
				mon.m_MonRect.left = mon.m_MonPosCol * m_DisplayResolutionX;
				mon.m_MonRect.top = mon.m_MonPosRow * m_DisplayResolutionY;
				mon.m_MonRect.right = mon.m_MonRect.left + m_DisplayResolutionX;
				mon.m_MonRect.bottom = mon.m_MonRect.top + m_DisplayResolutionY;
				OffsetRect(&mon.m_MonRect, -offset.x, -offset.y);
			}
		} 
		else
		{
			if(mon.m_bAttached)
			{
				mon.m_MonRect.left = mon.m_MonPosCol * m_DisplayResolutionX;
				mon.m_MonRect.top = mon.m_MonPosRow * m_DisplayResolutionY;
				mon.m_MonRect.right = mon.m_MonRect.left + m_DisplayResolutionX;
				mon.m_MonRect.bottom = mon.m_MonRect.top + m_DisplayResolutionY;
				OffsetRect(&mon.m_MonRect, 0, 0);
			}
		}
	}
}

BOOL CWallMonConfig::LoadLastConfigFromRegistry()
{
	COptionsMap map;
	BOOL res = map.LoadFromRegistry(PRODUCT_ROOT, PRODUCT_KEY _T("\\MonitorConfig"));
	if(!res)
		return FALSE;

	m_DisplayResolutionX = _ttoi(map.GetOption(_T("DisplayResolutionX")));
	m_DisplayResolutionY = _ttoi(map.GetOption(_T("DisplayResolutionY")));
	m_RefreshRate = _ttoi(map.GetOption(_T("RefreshRate")));
	m_ColorBits = _ttoi(map.GetOption(_T("ColorBits")));
	m_WallCols = _ttoi(map.GetOption(_T("WallCols")));
	m_WallRows = _ttoi(map.GetOption(_T("WallRows")));
	
	int MonitorCount = _ttoi(map.GetOption(_T("MonitorCount")));

	for(int i = 0; i < MonitorCount; i++)
	{
		MON_CONF mon;

		mon.m_MonNum = _ttoi(map.GetOption(_T("MonNum"), i));
		lstrcpy(mon.m_DeviceName, map.GetOption(_T("DeviceName"), i));
		lstrcpy(mon.m_DeviceString, map.GetOption(_T("DeviceString"), i));
		mon.m_bAttached = _ttoi(map.GetOption(_T("Attached"), i));
		mon.m_bPrimary = _ttoi(map.GetOption(_T("Primary"), i));
		mon.m_MonPosCol = _ttoi(map.GetOption(_T("MonPosCol"), i));
		mon.m_MonPosRow = _ttoi(map.GetOption(_T("MonPosRow"), i));
		CRectMapStruct(&mon.m_MonRect).Unserialize(map.GetOption(_T("MonRect"), i));
		mon.m_CurrentMode.Unserialize(map.GetOption(_T("CurrentMode"), i));

		if (mon.m_bPrimary)
		{
			mon.m_bAttached = TRUE;
		}
		
		if((!mon.m_bPrimary || IsWallWithoutHost()) && (mon.m_MonPosCol == -1 || mon.m_MonPosRow == -1))
			mon.m_bAttached = FALSE;

		m_Mons.push_back(mon);
	}

	if (!IsWallWithoutHost())
	{
		m_bAttachPrimaryMonitor = _ttoi(map.GetOption(_T("AttachPrimaryMonitor")));
		m_iPrimaryMonitor = _ttoi(map.GetOption(_T("PrimaryMonitor")));
	}

	return TRUE;
}

BOOL CWallMonConfig::SaveConfigToRegistry()
{
	COptionsMap map;
	map.SetOption(_T("DisplayResolutionX"), m_DisplayResolutionX);
	map.SetOption(_T("DisplayResolutionY"), m_DisplayResolutionY);
	map.SetOption(_T("RefreshRate"), m_RefreshRate);
	map.SetOption(_T("ColorBits"), m_ColorBits);
	map.SetOption(_T("WallCols"), m_WallCols);
	map.SetOption(_T("WallRows"), m_WallRows);

	int Index = 0;

	MON_CONF_ARRAY::iterator i;
	for(i = m_Mons.begin(); i != m_Mons.end(); i++)
	{
		MON_CONF &mon = *i;
		
		map.SetOption(_T("MonNum"), Index, mon.m_MonNum);
		map.SetOption(_T("DeviceName"), Index, mon.m_DeviceName);
		map.SetOption(_T("DeviceString"), Index, mon.m_DeviceString);
		map.SetOption(_T("Attached"), Index, (mon.m_bPrimary)?FALSE:mon.m_bAttached);
		map.SetOption(_T("Primary"), Index, mon.m_bPrimary);
		map.SetOption(_T("MonPosCol"), Index, mon.m_MonPosCol);
		map.SetOption(_T("MonPosRow"), Index, mon.m_MonPosRow);
		map.SetOption(_T("MonRect"), Index, &CRectMapStruct(&mon.m_MonRect));
		map.SetOption(_T("CurrentMode"), Index, &mon.m_CurrentMode);
		Index++;
	}

	map.SetOption(_T("MonitorCount"), Index);

	map.SetOption(_T("AttachPrimaryMonitor"), m_bAttachPrimaryMonitor);
	map.SetOption(_T("PrimaryMonitor"), m_iPrimaryMonitor);
	
	return map.SaveToRegistry(PRODUCT_ROOT, PRODUCT_KEY _T("\\MonitorConfig"));
}

int CWallMonConfig::GetCols()
{
	return max(m_WallCols, 1);
}

int CWallMonConfig::GetRows()
{
	return max(m_WallRows, 1);
}

BOOL CWallMonConfig::GetCell(int MonNum, int &Col, int &Row)
{
	MON_CONF_ARRAY::iterator i = m_Mons.begin();
	
	while(i != m_Mons.end())
	{
		MON_CONF &mon = *i;
		if(mon.m_bAttached && mon.m_MonNum == MonNum &&
			mon.m_MonPosCol != -1 && mon.m_MonPosRow != -1)
		{
			Col = mon.m_MonPosCol;
			Row = mon.m_MonPosRow;
			return TRUE;
		}
		i++;
	}
	return FALSE;
}
