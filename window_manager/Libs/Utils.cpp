#include "stdafx.h"
#include "Utils.h"

CString GetSizeAsShortSize(LONGLONG Size)
{
	CString str;
	LONGLONG Divider;
	CString strPostfix;

	if (Size > Tb)
	{
		Divider = Tb;
		strPostfix = _T("Tb");
	} 
	else
	if (Size > Gb)
	{
		Divider = Gb;
		strPostfix = _T("Gb");
	} 
	else
	if (Size > Mb)
	{
		Divider = Mb;
		strPostfix = _T("Mb");
	} 
	else
	if (Size > Kb)
	{
		Divider = Kb;
		strPostfix = _T("Kb");
	} 
	else
	{
		Divider = 1;
		strPostfix = _T("");
	} 

	str.Format(_T("%.1f %s"), (double)Size / Divider, strPostfix);

	return str;
}

char* GetAnsiString(const CString &s) 
{ 
	int nSize = s.GetLength(); 
	char *pAnsiString = new char[nSize+1]; 
#ifdef UNICODE
	wcstombs(pAnsiString, s, nSize+1); 
#else
	strcpy(pAnsiString, s);
#endif
	return pAnsiString; 
} 

