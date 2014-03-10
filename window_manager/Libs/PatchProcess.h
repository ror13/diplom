// PatchProcess.h
//////////////////////////////////////////////////////////////////////////

#ifndef _PATCHPROCESS_H_
#define _PATCHPROCESS_H_

struct PATCHDATA
{
	ULONG Addr;
	BYTE OldData;
	BYTE NewData;
};
typedef const PATCHDATA* LPCPATCHDATA;


BOOL PatchProcess(HANDLE hProcess, LPCPATCHDATA pData);

#endif //_PATCHPROCESS_H_
