// PatchProcess.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PatchProcess.h"

void PrinfErrorMessage(CString strName)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	CString strCaption;
	strCaption.Format(_T("Error %s"), strName);
	//MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, strCaption, MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}

BOOL PatchProcess(HANDLE hProcess, LPCPATCHDATA pData)
{
	int i = 0;
	BYTE OldVal;
	while(pData[i].Addr)
	{
		if(!ReadProcessMemory(hProcess, (LPVOID)(pData[i].Addr), (PVOID)&OldVal, sizeof(BYTE), NULL))
		{
// 			CString strCaption;
// 			strCaption.Format(_T("ReadProcessMemory %d, %p, %.2X, %.2X"), i, (LPVOID)(pData[i].Addr), pData[i].OldData, pData[i].NewData);
// 			PrinfErrorMessage(strCaption);
			return FALSE;
		}
		
		if(OldVal != pData[i].OldData)
		{
// 			MessageBox(NULL, _T("Data error"), _T("Error"), MB_OK);
			return FALSE;
		}
		
		if(!WriteProcessMemory(hProcess, (LPVOID)(pData[i].Addr), (PVOID)&(pData[i].NewData), sizeof(BYTE), NULL))
		{
// 			CString strCaption;
// 			strCaption.Format(_T("WriteProcessMemory %d, %p, %.2X, %.2X"), i, (LPVOID)(pData[i].Addr), pData[i].OldData, pData[i].NewData);
// 			PrinfErrorMessage(strCaption);
			return FALSE;
		}

		i++;
	}
	return TRUE;
}
