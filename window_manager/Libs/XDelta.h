
#pragma once

#include "md5_util.h"

BOOL XDeltaDiffOld(const CString& strFrom, const CString& strTo, const CString& strDiff);
BOOL XDeltaPatchOld(const CString& strFrom, const CString& strPatch, const CString& strDest);

BOOL XDeltaDiff(const CString& strFrom, const CString& strTo, const CString& strDiff, CMD5ProgressDlg *pProgressDlg = NULL);
BOOL XDeltaDiff(const CString& strFrom, const CString& strTo, FILE* diff, CMD5ProgressDlg *pProgressDlg);
BOOL XDeltaPatch(const CString& strFrom, const CString& strPatch, const CString& strDest);

BOOL XDeltaGetSrcMd5(const CString& patch, CString& ret);
BOOL XDeltaGetDstMd5(const CString& patch, CString& ret);
BOOL XDeltaGetSrcSize(const CString& patch, LONGLONG& ret);
BOOL XDeltaGetDstSize(const CString& patch, LONGLONG& ret);

BOOL XDeltaGetSrcMd5(FILE * patch, CString& ret);
BOOL XDeltaGetDstMd5(FILE * patch, CString& ret);
BOOL XDeltaGetSrcSize(FILE * patch, LONGLONG& ret);
BOOL XDeltaGetDstSize(FILE * patch, LONGLONG& ret);

