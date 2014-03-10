#include  "stdafx.h"
#include "OutputVideoSetting.h"
#include "tchar.h"

#include "PresenterRegistry.h"
#include "CommonApi.h"

OutputVideoSettingNameCmdLine OutputVideoSettings[] = 
{
	{ _T("Default"),		_T("direct3d") },
	{ _T("direct3d"),		_T("direct3d") },
	{ _T("OpenGL"),			_T("glwin32") },
	{ _T("directx"),		_T("directx") },
	{ _T("direct2d"),		_T("direct2d") },
};

UINT GetOutputVideoSettingsCount()
{
	return sizeof(OutputVideoSettings)/sizeof(OutputVideoSettings[0]);
}

//TCHAR* GetCommandLineOptionByName(TCHAR* )
TCHAR* GetOutputVideoSettingNameByIndex(UINT Index)
{
	if ( Index > GetOutputVideoSettingsCount())
	{
		return NULL;
	}
	
	return OutputVideoSettings[Index].Name;
}
TCHAR* GetOutputVideoSettingCommandLineOptionByIndex(UINT Index)
{
	if ( Index > GetOutputVideoSettingsCount())
	{
		return NULL;
	}

	return OutputVideoSettings[Index].CommandLineSetting;
}
OutputVideoSettingNameCmdLine* GetVideoSetting(UINT Index)
{
	if ( Index > GetOutputVideoSettingsCount())
	{
		return NULL;
	}
	
	return &OutputVideoSettings[Index];
}

// if no need to leave host monitor unchanged
UINT GetOutputVideoSettingFromRegistry()
{
	DWORD dwOutputVideoSetting = 1;
	
	BOOL res = regReadRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T, REG_OUTPUT_VIDEO_SETTING_VALUE_T, &dwOutputVideoSetting);
	if( !res )
	{
		dwOutputVideoSetting = 1;
	}
	
	return dwOutputVideoSetting;
}
TCHAR* GetOutputVideoSettingCmdLineFromRegistry()
{
	return GetOutputVideoSettingCommandLineOptionByIndex( GetOutputVideoSettingFromRegistry() );
}
// set if no need to leave host monitor unchanged
BOOL SetOutputVideoSettingToRegistry(UINT OutputVideoSetting)
{
	BOOL res = regWriteRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T, REG_OUTPUT_VIDEO_SETTING_VALUE_T, OutputVideoSetting);
	
	return res;
}

