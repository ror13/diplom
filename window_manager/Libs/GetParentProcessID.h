// GetParentProcessId.h

#ifndef _GETPARENTPROCESSID_H_
#define _GETPARENTPROCESSID_H_

#include <Tlhelp32.h>

DWORD GetParentProcessId(DWORD ProcessID)
{
	HANDLE hSnapshot= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	BOOL bFind = Process32First(hSnapshot, &pe);
	while(bFind)
	{
		if(pe.th32ProcessID == ProcessID)
		{
			CloseHandle(hSnapshot);
			return pe.th32ParentProcessID;
		}
		
		bFind = Process32Next(hSnapshot, &pe);
	}

	CloseHandle(hSnapshot);

	return 0;
}

#endif //_GETPARENTPROCESSID_H_
