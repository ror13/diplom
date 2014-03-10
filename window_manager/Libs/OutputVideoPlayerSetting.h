#ifndef __OutputVideoPlayerSetting_H__
#define __OutputVideoPlayerSetting_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagOutputVideoPlayerSettingNameCmdLine 
{
	TCHAR* Name;
	TCHAR* CommandLineSetting;
} OutputVideoPlayerSettingNameCmdLine;

UINT GetOutputVideoPlayerSettingsCount();
TCHAR* GetOutputVideoPlayerSettingNameByIndex(UINT Index);
TCHAR* GetOutputVideoPlayerSettingCommandLineOptionByIndex(UINT Index);
OutputVideoPlayerSettingNameCmdLine* GetVideoPlayerSetting(UINT Index);

UINT GetOutputVideoPlayerSettingFromRegistry();
TCHAR* GetOutputVideoPlayerSettingCmdLineFromRegistry();
BOOL SetOutputVideoPlayerSettingToRegistry(UINT OutputVideoPlayerSetting);

#endif //__OutputVideoPlayerSetting_H__