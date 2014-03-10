#include "StdAfx.h"

#include "CommonApi.h"
#include "FileOperation.h"

#include "ShellHelper.h"

// -i - install our shell
BOOL PRShellInstall()
{
	// Check if our shell installed simple return
	// else backup current shell
	// install our

	CString strKey2Current;
	BOOL bRet;

	bRet = regReadRegString(HKEY_LOCAL_MACHINE, SHELL_KEY2_KEY_NAME, 
		strKey2Current.GetBuffer(1024), 1024);
	strKey2Current.ReleaseBuffer();

	if (!bRet)
	{
		TRACE(_T("PRShellInstall: Can't read current shell values\n"));
		return FALSE;
	}

	//if already exists
	if (strKey2Current.CompareNoCase(GetExeFileName()) == 0)
	{
		TRACE(_T("PRShellInstall: Already installed\n"));

		return TRUE;
	}


	// backup
	bRet = regWriteRegString(HKEY_LOCAL_MACHINE, SHELL_KEY2_BACKUP_KEY_NAME, 
		strKey2Current);

	// install
	bRet = regWriteRegString(HKEY_LOCAL_MACHINE, SHELL_KEY1_KEY_NAME, 
		SHELL_KEY1_DEFAULT_VALUE);
	bRet = regWriteRegString(HKEY_LOCAL_MACHINE, SHELL_KEY2_KEY_NAME, 
		GetExeFileName());

	return TRUE;
}

// -r - remove our shell
BOOL PRShellRemove()
{
	// Check if our shell not installed simple return
	// else restore from backup current shell
	// remove backup
	CString strKey2Current;
	BOOL bRet;
	
	bRet = regReadRegString(HKEY_LOCAL_MACHINE, SHELL_KEY2_KEY_NAME, 
		strKey2Current.GetBuffer(1024), 1024);
	strKey2Current.ReleaseBuffer();
	
	if (!bRet)
	{
		TRACE(_T("PRShellInstall: Can't read current shell values\n"));
		return FALSE;
	}
	
	//if not exists
	if (strKey2Current.CompareNoCase(GetExeFileName()) != 0)
	{
		TRACE(_T("PRShellInstall: Not installed\n"));
		
		return TRUE;
	}
	
	
	// restore from backup
	CString strKey2Backup;
	bRet = regReadRegString(HKEY_LOCAL_MACHINE, SHELL_KEY2_BACKUP_KEY_NAME, 
		strKey2Backup.GetBuffer(1024), 1024);
	strKey2Backup.ReleaseBuffer();
	bRet = regWriteRegString(HKEY_LOCAL_MACHINE, SHELL_KEY2_KEY_NAME, 
		strKey2Backup);
	
	// remove backup
	regDeleteRegKey(HKEY_LOCAL_MACHINE, SHELL_KEY2_BACKUP_KEY_NAME);
	
	return TRUE;
}

// -d - create windows default values
BOOL PRShellInstallWindowsDefaultShell()
{
	// Install windows default shell values
	// remove backup
	// install
	BOOL bRet;

	bRet = regWriteRegString(HKEY_LOCAL_MACHINE, SHELL_KEY1_KEY_NAME, 
		SHELL_KEY1_DEFAULT_VALUE);
	bRet = regWriteRegString(HKEY_LOCAL_MACHINE, SHELL_KEY2_KEY_NAME, 
		SHELL_KEY2_DEFAULT_VALUE);

	// remove backup
	regDeleteRegKey(HKEY_LOCAL_MACHINE, SHELL_KEY2_BACKUP_KEY_NAME);
	
	return TRUE;
}
