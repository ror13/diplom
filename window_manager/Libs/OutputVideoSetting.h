#ifndef __OutputVideoSetting_H__
#define __OutputVideoSetting_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagOutputVideoSettingNameCmdLine 
{
	TCHAR* Name;
	TCHAR* CommandLineSetting;
} OutputVideoSettingNameCmdLine;

UINT GetOutputVideoSettingsCount();
TCHAR* GetOutputVideoSettingNameByIndex(UINT Index);
TCHAR* GetOutputVideoSettingCommandLineOptionByIndex(UINT Index);
OutputVideoSettingNameCmdLine* GetVideoSetting(UINT Index);

UINT GetOutputVideoSettingFromRegistry();
TCHAR* GetOutputVideoSettingCmdLineFromRegistry();
BOOL SetOutputVideoSettingToRegistry(UINT OutputVideoSetting);

#endif //__OutputVideoSetting_H__