#include  "stdafx.h"
#include "OutputVideoPlayerSetting.h"
#include "tchar.h"

#include "PresenterRegistry.h"
#include "CommonApi.h"

#include "PresenterGlobals.h"

OutputVideoPlayerSettingNameCmdLine OutputVideoPlayerSettings[] = 
{
	{ _T("Default"),	PRVLC_EXE_FILE_NAME },
	{ _T("mpc-hc"),		MPC_EXE_FILE_NAME },
};

UINT GetOutputVideoPlayerSettingsCount()
{
	return sizeof(OutputVideoPlayerSettings)/sizeof(OutputVideoPlayerSettings[0]);
}

//TCHAR* GetCommandLineOptionByName(TCHAR* )
TCHAR* GetOutputVideoPlayerSettingNameByIndex(UINT Index)
{
	if ( Index > GetOutputVideoPlayerSettingsCount())
	{
		return NULL;
	}
	
	return OutputVideoPlayerSettings[Index].Name;
}
TCHAR* GetOutputVideoPlayerSettingCommandLineOptionByIndex(UINT Index)
{
	if ( Index > GetOutputVideoPlayerSettingsCount())
	{
		return NULL;
	}

	return OutputVideoPlayerSettings[Index].CommandLineSetting;
}
OutputVideoPlayerSettingNameCmdLine* GetVideoPlayerSetting(UINT Index)
{
	if ( Index > GetOutputVideoPlayerSettingsCount())
	{
		return NULL;
	}
	
	return &OutputVideoPlayerSettings[Index];
}

// if no need to leave host monitor unchanged
UINT GetOutputVideoPlayerSettingFromRegistry()
{
	DWORD dwOutputVideoPlayerSetting = 0;
	
	BOOL res = regReadRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T, REG_OUTPUT_VIDEO_PLAYER_SETTING_VALUE_T, &dwOutputVideoPlayerSetting);
	if( !res )
	{
		dwOutputVideoPlayerSetting = 0;
	}
	
	return dwOutputVideoPlayerSetting;
}
TCHAR* GetOutputVideoPlayerSettingCmdLineFromRegistry()
{
	return GetOutputVideoPlayerSettingCommandLineOptionByIndex( GetOutputVideoPlayerSettingFromRegistry() );
}
// set if no need to leave host monitor unchanged
BOOL SetOutputVideoPlayerSettingToRegistry(UINT OutputVideoPlayerSetting)
{
	BOOL res = regWriteRegDword( PRESENTER_REG_ROOT, PRESENTER_REG_KEY_T, REG_OUTPUT_VIDEO_PLAYER_SETTING_VALUE_T, OutputVideoPlayerSetting);
	
	return res;
}

