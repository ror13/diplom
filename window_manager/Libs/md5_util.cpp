// md5_util.cpp
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//#include "ErrorMessageDlg.h"
#include "md5.h"
#include "fileoperation.h"

#include "md5_util.h"

#include <map>


struct FileDescription
{
	FILETIME time;
	CString md5;

	FileDescription()
	{
		time.dwLowDateTime = 0;
		time.dwHighDateTime = 0;
		md5 = _T("");
	}
};

CString GetMD5SignatureCache(LPCTSTR szFileName, CMD5ProgressDlg *pProgressDlg)
{
	static CMutex mutex;
	static std::map<CString, FileDescription> cache;

	FILETIME mTime;
	FileDescription desc;

	mutex.Lock();
	if (cache.find(szFileName) != cache.end())
	{
		desc = cache[szFileName];
	}
	mutex.Unlock();

	if (!GetFileModificationTime(szFileName, &mTime))
		return GetMD5Signature(szFileName, pProgressDlg);;

	if (mTime.dwLowDateTime == desc.time.dwLowDateTime &&
		mTime.dwHighDateTime == desc.time.dwHighDateTime)
		return desc.md5;

	desc.md5 = GetMD5Signature(szFileName, pProgressDlg);
	desc.time = mTime;

	mutex.Lock();
	cache[szFileName] = desc;
	mutex.Unlock();
	return desc.md5;
}

CString GetMD5Signature(FILE *fl, CMD5ProgressDlg *pProgressDlg)
{
	CString sig;
	BYTE signature[16];
	DWORD readed;
	
	unsigned char buffer[16384];
	
	MD5Context md5c;

	fpos_t pos = 0;
	fpos_t fsize = 0;
	fgetpos(fl, &pos);
	fseek(fl, 0, SEEK_END);
	fgetpos(fl, &fsize);
	rewind(fl);

	MD5Init(&md5c);
	
	if(pProgressDlg)
		pProgressDlg->ShowProgress(fsize);
	
	BOOL bCancel = FALSE;
	do
	{
		readed = fread(buffer, 1, sizeof(buffer), fl);

		if(readed)
		{
			if(pProgressDlg)
				pProgressDlg->UpdateProgress(readed);
			MD5Update(&md5c, buffer, readed);
		}
		
		if(pProgressDlg)
			bCancel = pProgressDlg->IsCancel();
	} while(readed > 0 && !bCancel);
	fsetpos(fl, &pos);

	if(pProgressDlg)
		pProgressDlg->HideProgress();
	
	MD5Final(signature, &md5c);
	
	if(bCancel)
		return _T("");
	
	CString tmp;
	for(int i = 0; i < 16; i++)
	{
		tmp.Format(_T("%02x"), signature[i]);
		sig += tmp;
	}
	return sig;
}

CString GetMD5Signature(LPCTSTR szFileName, CMD5ProgressDlg *pProgressDlg)
{
	CString sig;
	BYTE signature[16];
	DWORD readed;
	BOOL res;
	
	unsigned char buffer[16384];
	
	MD5Context md5c;
	
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
	{
//		DisplayError(NULL, 0, DEFAULT_DISPLAY_FOREVER,
//			NULL, _T("Can't open file %s"), szFileName);
		return _T("");
	}
	
	LARGE_INTEGER FileSize;
	if(!GetFileSizeEx(hFile, &FileSize))
	{
		CloseHandle(hFile);
		return _T("");
	}
	
	MD5Init(&md5c);
	
	if(pProgressDlg)
		pProgressDlg->ShowProgress(FileSize.QuadPart);
	
	BOOL bCancel = FALSE;
	do
	{
		readed = 0;
		res = ReadFile(hFile, buffer, sizeof(buffer), &readed, NULL);
		if(res && readed)
		{
			if(pProgressDlg)
				pProgressDlg->UpdateProgress(readed);
			MD5Update(&md5c, buffer, readed);
		}
		
		if(pProgressDlg)
			bCancel = pProgressDlg->IsCancel();
	} while(readed > 0 && !bCancel);

	if(pProgressDlg)
		pProgressDlg->HideProgress();
	
	MD5Final(signature, &md5c);
	
	CloseHandle(hFile);
	
	if(bCancel)
		return _T("");
	
	CString tmp;
	for(int i = 0; i < 16; i++)
	{
		tmp.Format(_T("%02x"), signature[i]);
		sig += tmp;
	}
	
	return sig;
}

CString GetMD5SignatureFromString(LPCTSTR stzString)
{
	CString sig;
	BYTE signature[16];
	
	MD5Context md5c;
	MD5Init(&md5c);
	USES_CONVERSION;
#ifdef UNICODE
	LPSTR szString = W2A(stzString);
#else
	LPSTR szString = (LPSTR)stzString;
#endif
	MD5Update(&md5c, (LPBYTE)szString, strlen(szString));
	MD5Final(signature, &md5c);
	
	CString tmp;
	for(int i = 0; i < 16; i++)
	{
		tmp.Format(_T("%02x"), signature[i]);
		sig += tmp;
	}
	
	return sig;
}

