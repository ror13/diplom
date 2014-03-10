#ifndef __UTILS_H__
#define __UTILS_H__

#define Kb	(1024L)
#define Mb	(1024L*Kb)
#define Gb	(1024L*Mb)
#define Tb	((LONGLONG)1024*Gb)

#define MD5_STRING_LEN	32

CString GetSizeAsShortSize(LONGLONG Size);
char* GetAnsiString(const CString &s);

#endif //__UTILS_H__