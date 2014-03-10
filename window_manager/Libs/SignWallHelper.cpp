#include "StdAfx.h"

#include "SignWallHelper.h"
#include "tchar.h"

#include "PresenterRegistry.h"
#include "CommonApi.h"


TCHAR* SignWallSupportedDevices[] = 
{
	// ATI 4670 read from real card
	_T("PCI\\VEN_1002&DEV_9490&SUBSYS_4670"),
	// ATI 5570 read from real card
	_T("PCI\\VEN_1002&DEV_68D9&SUBSYS_5570"),
	// ATI 3870 get from http://pciids.sourceforge.net/pci.ids (http://pci-ids.ucw.cz/read/PC/1002/9501)
	_T("PCI\\VEN_1002&DEV_9501"),
	// VirtualBox Graphics adapter
	_T("PCI\\VEN_80EE&DEV_BEEF"),
	NULL
};

BOOL IsMonitorSupportBySignWall(DWORD iDevNum)
{
	DISPLAY_DEVICE DisplayDevice;
	DisplayDevice.cb = sizeof(DISPLAY_DEVICE);

	BOOL bEnumDisplayDevices = EnumDisplayDevices(
		NULL,					// device name
		iDevNum-1,                // display device
		&DisplayDevice,			// device information
		0						// reserved
		);

	//TRACE(_T("IsMonitorSupportBySignWall: Mon %d"), iDevNum);

	if (bEnumDisplayDevices)
	{
		//TRACE(_T(" DeviceName %s, DeviceString %s, DeviceID %s"), DisplayDevice.DeviceName, DisplayDevice.DeviceString, DisplayDevice.DeviceID);
		for (int i = 0; SignWallSupportedDevices[i] != NULL; i++)
		{
			if (_tcsnicmp(DisplayDevice.DeviceID, SignWallSupportedDevices[i], _tcslen(SignWallSupportedDevices[i])) == 0)
			{
				//TRACE(_T(" Supported\n"));
				return TRUE;
			}
		}
	}
	//TRACE(_T(" Not Supported\n"));

	return FALSE;
}

// if no need to leave host monitor unchanged
BOOL IsWallWithoutHost()
{
	DWORD dwWallWithoutHost = FALSE;

	BOOL res = regReadRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T, REG_WALL_WITHOUT_HOST_VALUE_T, &dwWallWithoutHost);
	if( !res )
	{
		dwWallWithoutHost = FALSE;
	}

	return dwWallWithoutHost;
}

// set if no need to leave host monitor unchanged
BOOL SetWallWithoutHost(BOOL UseWallWithoutHost)
{
	BOOL res = regWriteRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T, REG_WALL_WITHOUT_HOST_VALUE_T, UseWallWithoutHost);
	
	return res;
}

// if need to reset monitors on each client run
BOOL IsAlwaysResetMonitors()
{
	DWORD dwAlwaysResetMonitors = FALSE;

	BOOL res = regReadRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T, REG_WALL_RESET_MONITORS_VALUE_T, &dwAlwaysResetMonitors);
	if( !res )
	{
		dwAlwaysResetMonitors = FALSE;
	}

	return dwAlwaysResetMonitors;
}
