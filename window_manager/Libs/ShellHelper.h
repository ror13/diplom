#ifndef __ShellHelper_H__
#define __ShellHelper_H__

/*
Registry Is the Key

Microsoft® Windows® XP has a feature that can provide the solution through the registry. The registry for each user account and administrator account can be set up to start a user-specific shell. There are three keys that must be set up. The first two are generic for all users. This article refers to the keys as "Key1," "Key2," and "Key3" for simplicity.

Key1 is a string value. When Windows XP starts, Key1 is called and the default Windows shell is started. However, if the default value is changed to USR:Software\Microsoft\Windows NT\CurrentVersion\Winlogon, Windows looks in the HKEY_Current_User key to start a specific shell for the user logging on. If the specific user shell is not found, Key2 is called and a default shell is started.

    Key: HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\IniFileMapping\system.ini\boot\Shell

    Type: REG_SZ

    Value: SYS:Microsoft\Windows NT\CurrentVersion\Winlogon

Key2 provides a default shell if the user shell application cannot be found. When you select a shell component for a Microsoft Windows XP Embedded configuration, Key 2 is set up to the shell application as the default shell.

    Key: HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Winlogon\Shell

    Type: REG_SZ

    Value: Explorer.exe (or this can be a different default application)

Key3 sets up a shell for the current user or logged-on user. Thus, the only way to change a particular user's shell is to log on to the user account and create this registry entry.

    Key: HKEY_Current_User\Software\Microsoft\Windows NT\CurrentVersion\Winlogon\Shell

    Type: REG_SZ

    Value: c:\windows\system32\account shell.exe, where account shell.exe is the name of the application

*/

// HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\IniFileMapping\system.ini\boot\Shell
#define SHELL_KEY1_KEY_NAME			_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\IniFileMapping\\system.ini\\boot\\Shell")
#define SHELL_KEY1_DEFAULT_VALUE	_T("SYS:Microsoft\\Windows NT\\CurrentVersion\\Winlogon")
#define SHELL_KEY1_USER_VALUE		_T("USR:Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon")

// HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Winlogon\Shell
#define SHELL_KEY2_KEY_NAME			_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Shell")
#define SHELL_KEY2_DEFAULT_VALUE	_T("Explorer.exe")

#define SHELL_KEY2_BACKUP_KEY_NAME	_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\PRShell_BackUp")

// HKEY_Current_User\Software\Microsoft\Windows NT\CurrentVersion\Winlogon\Shell
#define SHELL_KEY3_KEY_NAME			_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Shell")
#define SHELL_KEY3_DEFAULT_VALUE	_T("Explorer.exe")

#define SHELL_KEY3_BACKUP_KEY_NAME	_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\PRShell_BackUp")

// -i - install our shell
BOOL PRShellInstall();
// -r - remove our shell
BOOL PRShellRemove();
// -d - create windows default values
BOOL PRShellInstallWindowsDefaultShell();


#endif // __ShellHelper_H__