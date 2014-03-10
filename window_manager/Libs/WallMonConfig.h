// WallMonConfig.h: interface for the CWallMonConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WALLMONCONFIG_H__E4DE294A_FBBA_4619_9BDD_DEB7E5DC31D0__INCLUDED_)
#define AFX_WALLMONCONFIG_H__E4DE294A_FBBA_4619_9BDD_DEB7E5DC31D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "OptionsMap.h"

class MON_MODE : public COptionsMapStruct
{
public:
	DWORD		dmBitsPerPel;
	DWORD		dmPelsWidth;
	DWORD		dmPelsHeight;
	DWORD		dmDisplayFlags;
	DWORD		dmDisplayFrequency;
	
	MON_MODE();
	
	BOOL GetMonMode(LPCTSTR lpszDeviceName, DWORD iModeNum);

	BOOL operator ==(const MON_MODE &mode);
	BOOL operator !=(const MON_MODE &mode);

	// COptionsMapStruct
	virtual CString Serialize();
	virtual BOOL Unserialize(CString strStruct);
};

typedef std::vector<MON_MODE> MON_MODE_ARRAY;

class MON_CONF
{
public:
	TCHAR		m_DeviceName[32];
	TCHAR		m_DeviceString[128];

	int			m_MonNum;
	CRect		m_MonRect;
	BOOL		m_bAttached;
	BOOL		m_bPrimary;

	int			m_MonPosCol;
	int			m_MonPosRow;

	MON_MODE	m_CurrentMode;
	
	MON_MODE_ARRAY	m_Modes;
	
	MON_CONF();

	BOOL operator ==(MON_CONF &conf);
	BOOL operator !=(MON_CONF &conf);
};

typedef std::vector<MON_CONF> MON_CONF_ARRAY;

class CMultiMonConfig
{
public:
	CMultiMonConfig();

	BOOL GetSystemConfig(BOOL bReset = FALSE);
	
	BOOL ChangePrimaryMonitor(CString NewPrimaryMonitor);
	BOOL SetSystemConfig();
	BOOL TestSystemConfig();
	
	int GetMonitorCount();
	MON_CONF* GetMonitorByName(CString MonName);
	MON_CONF* GetMonitorByNumber(int MonNum);
	MON_CONF* GetPrimaryMonitor();
	
	void CalculateMonRects();

	CRect GetWallScreenRect();
	
	CString GetErrorMessageString(int Result);

	BOOL operator ==(CMultiMonConfig &conf);
	BOOL operator !=(CMultiMonConfig &conf);
	
	MON_CONF_ARRAY	m_Mons;

	int		m_bAttachPrimaryMonitor;
	int		m_iPrimaryMonitor;

protected:
	CString GetDisplayDeviceStateFlagsAsString(DWORD flags);
};

class CWallMonConfig : public CMultiMonConfig
{
public:
	CWallMonConfig();

	BOOL GetSystemMonitorConfig();

	BOOL GetSystemConfig(BOOL bReset = FALSE);

	void CalculateMonRects();

	BOOL SetSystemConfig();
	
	BOOL LoadLastConfigFromRegistry();
	BOOL SaveConfigToRegistry();

	int GetCols();
	int GetRows();

	BOOL GetCell(int MonNum, int &Col, int &Row);
	
	int		m_DisplayResolutionX;
	int		m_DisplayResolutionY;
	int		m_RefreshRate;
	int		m_ColorBits;
	int		m_WallCols;
	int		m_WallRows;
};

#endif // !defined(AFX_WALLMONCONFIG_H__E4DE294A_FBBA_4619_9BDD_DEB7E5DC31D0__INCLUDED_)
