// md5_util.h
//////////////////////////////////////////////////////////////////////////

#ifndef _MD5_UTIL_H_
#define _MD5_UTIL_H_

#include <stdio.h>

class CMD5ProgressDlg
{
public:
	virtual void ShowProgress(LONGLONG MaxVal) = 0;
	virtual void UpdateProgress(DWORD DifVal) = 0;
	virtual void HideProgress() = 0;

	virtual BOOL IsCancel() = 0;
};


CString GetMD5SignatureCache(LPCTSTR szFileName, CMD5ProgressDlg *pProgressDlg);
CString GetMD5Signature(FILE *fl, CMD5ProgressDlg *pProgressDlg);
CString GetMD5Signature(LPCTSTR szFileName, CMD5ProgressDlg *pProgressDlg = NULL);

CString GetMD5SignatureFromString(LPCTSTR stzString);

#endif //_MD5_UTIL_H_
