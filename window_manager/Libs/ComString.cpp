// CCOMString.cpp : implementation file
//
// CCOMString Implementation
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlbase.h>
#include <TCHAR.H>
#include <stdio.h>
#include "COMString.h"

//#pragma comment (lib, "oleaut32.lib")

//////////////////////////////////////////////////////////////////////
//
// Constructors/Destructor
//
//////////////////////////////////////////////////////////////////////

CCOMString::CCOMString()
{
	m_pszString = NULL;
	AllocString(0);
}

CCOMString::CCOMString(CCOMString& str)
{
	m_pszString = NULL;
	int nLen = str.GetLength();
	AllocString(nLen);
	_tcsncpy(m_pszString, (LPCTSTR) str, nLen);
}

CCOMString::CCOMString(LPCTSTR pszString)
{
	m_pszString = NULL;
	int nLen = _tcslen(pszString);
	AllocString(nLen);
	_tcsncpy(m_pszString, pszString, nLen);
}

CCOMString::CCOMString(BSTR bstrString)
{
	USES_CONVERSION;
	LPTSTR pszTemp = OLE2T(bstrString);
	m_pszString = NULL;
	int nLen = _tcslen(pszTemp);
	AllocString(nLen);
	_tcsncpy(m_pszString, pszTemp, nLen);
}

CCOMString::CCOMString(TCHAR ch, int nRepeat) 
{
	m_pszString = NULL;
	if (nRepeat > 0)
	{
		AllocString(nRepeat);
#ifdef _UNICODE
		for (int i=0; i < nRepeat; i++)
			m_pszString[i] = ch;
#else
		memset(m_pszString, ch, nRepeat);
#endif
	}
}

CCOMString::~CCOMString()
{
	free(m_pszString);
}


//////////////////////////////////////////////////////////////////////
//
// Memory Manipulation
//
//////////////////////////////////////////////////////////////////////

void CCOMString::AllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	if (m_pszString != NULL)
		free(m_pszString);
	m_pszString = (TCHAR*) malloc((nLen+1) * sizeof(TCHAR));
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = _T('\0');
}

void CCOMString::ReAllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	m_pszString = (TCHAR*) realloc(m_pszString, (nLen+1) * sizeof(TCHAR));
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = _T('\0');
}

void CCOMString::StringCopy(int nSrcLen, LPCTSTR lpszSrcData)
{
	AllocString(nSrcLen);
	memcpy(m_pszString, lpszSrcData, nSrcLen * sizeof(TCHAR));
	m_pszString[nSrcLen] = _T('\0');
}

void CCOMString::StringCopy(CCOMString &str, int nLen, int nIndex, int nExtra) const
{
	int nNewLen = nLen + nExtra;
	if (nNewLen != 0)
	{
		str.AllocString(nNewLen);
		memcpy(str.GetString(), m_pszString+nIndex, nLen * sizeof(TCHAR));
	}
}

void CCOMString::ConcatCopy(LPCTSTR lpszData)
{
	ATLASSERT(lpszData != NULL);

	// Save the existing string
	int nLen = GetLength();
	TCHAR* pszTemp = (TCHAR*) malloc((nLen+1) * sizeof(TCHAR));
	memcpy(pszTemp, m_pszString, nLen * sizeof(TCHAR));
	pszTemp[nLen] = _T('\0');

	// Calculate the new string length and realloc memory
	int nDataLen = _tcslen(lpszData);
	int nNewLen = nLen + nDataLen;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(TCHAR));
	memcpy(m_pszString+nLen, lpszData, nDataLen * sizeof(TCHAR));

	// Cleanup
	free(pszTemp);
}

void CCOMString::ConcatCopy(TCHAR ch)
{
	// Save the existing string
	int nLen = GetLength();
	TCHAR* pszTemp = (TCHAR*) malloc((nLen+1) * sizeof(TCHAR));
	memcpy(pszTemp, m_pszString, nLen * sizeof(TCHAR));
	pszTemp[nLen] = _T('\0');

	// Calculate the new string length and realloc memory
	int nNewLen = nLen + 1;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(TCHAR));
	m_pszString[nNewLen-1] = ch;

	// Cleanup
	free(pszTemp);
}

void CCOMString::ConcatCopy(LPCTSTR lpszData1, LPCTSTR lpszData2)
{
	ATLASSERT(lpszData1 != NULL);
	ATLASSERT(lpszData2 != NULL);
	int nLen1 = _tcslen(lpszData1);
	int nLen2 = _tcslen(lpszData2);
	int nLen = nLen1 + nLen2;
	AllocString(nLen);
	memcpy(m_pszString, lpszData1, nLen1 * sizeof(TCHAR));
	memcpy(m_pszString+nLen1, lpszData2, nLen2 * sizeof(TCHAR)); 
}

#ifndef NO_SYS_ALLOC_STRING_LEN

BSTR CCOMString::AllocSysString()
{
#ifdef _UNICODE
	BSTR bstr = ::SysAllocStringLen(m_pszString, GetLength());
	if (bstr == NULL)
	{
		ATLASSERT(0);
		return NULL;
	}
#else
	int nLen = MultiByteToWideChar(CP_ACP, 0, m_pszString, GetLength(), NULL, NULL);
	BSTR bstr = ::SysAllocStringLen(NULL, nLen);
	if (bstr == NULL)
	{
		ATLASSERT(0);
		return NULL;
	}
	MultiByteToWideChar(CP_ACP, 0, m_pszString, GetLength(), bstr, nLen);
#endif

	return bstr;
}

#endif

//////////////////////////////////////////////////////////////////////
//
// Accessors for the String as an Array
//
//////////////////////////////////////////////////////////////////////

void CCOMString::Empty()
{
	if (_tcslen(m_pszString) > 0)
		m_pszString[0] = _T('\0');
}

TCHAR CCOMString::GetAt(int nIndex)
{
	int nLen = _tcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

TCHAR CCOMString::operator[] (int nIndex)
{
	int nLen = _tcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

void CCOMString::SetAt(int nIndex, TCHAR ch)
{
	int nLen = _tcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	m_pszString[nIndex] = ch;	
}

int CCOMString::GetLength() const
{ 
	if( m_pszString == NULL )
		return 0;
	return _tcslen(m_pszString); 
}

bool CCOMString::IsEmpty() const
{ 
	return (GetLength() > 0) ? false : true; 
}


//////////////////////////////////////////////////////////////////////
//
// Conversions
//
//////////////////////////////////////////////////////////////////////

void CCOMString::MakeUpper()
{
	_tcsupr(m_pszString);
}

void CCOMString::MakeLower()
{
	_tcslwr(m_pszString);
}

void CCOMString::MakeReverse()
{
	_tcsrev(m_pszString);
}

//////////////////////////////////////////////////////////////////////
//
// Trimming
//
//////////////////////////////////////////////////////////////////////


void CCOMString::TrimLeft()
{
	LPTSTR lpsz = m_pszString;

	while (_istspace(*lpsz))
		lpsz = _tcsinc(lpsz);

	if (lpsz != m_pszString)
	{
		memmove(m_pszString, lpsz, (_tcslen(lpsz)+1) * sizeof(TCHAR));
	}
}

void CCOMString::TrimRight()
{
	LPTSTR lpsz = m_pszString;
	LPTSTR lpszLast = NULL;

	while (*lpsz != _T('\0'))
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
		*lpszLast = _T('\0');
}

void CCOMString::TrimLeft( TCHAR chTarget )
{
	TCHAR szTargets[2] = { chTarget, _T('\0') };
	TrimLeft( szTargets );
}

void CCOMString::TrimLeft( LPCTSTR lpszTargets )
{
	LPTSTR lpsz = m_pszString;

	while ( _tcschr(lpszTargets, *lpsz) != NULL )
		lpsz = _tcsinc(lpsz);

	if (lpsz != m_pszString)
	{
		memmove(m_pszString, lpsz, (_tcslen(lpsz)+1) * sizeof(TCHAR));
	}
}

void CCOMString::TrimRight( TCHAR chTarget )
{
	TCHAR szTargets[2] = { chTarget, _T('\0') };
	TrimRight( szTargets );
}

void CCOMString::TrimRight( LPCTSTR lpszTargets )
{
	LPTSTR lpsz = m_pszString;
	LPTSTR lpszLast = NULL;

	while (*lpsz != _T('\0'))
	{
		if ( _tcschr(lpszTargets, *lpsz) != NULL )
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
		*lpszLast = _T('\0');
}

//////////////////////////////////////////////////////////////////////
//
// Searching
//
//////////////////////////////////////////////////////////////////////

int CCOMString::Find(TCHAR ch) const
{
	return Find(ch, 0);
}

int CCOMString::Find(TCHAR ch, int nStart) const
{
	if (nStart >= GetLength())
		return -1;
	LPTSTR lpsz = _tcschr(m_pszString + nStart, (_TUCHAR) ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CCOMString::Find(LPCTSTR lpszSub)
{
	return Find(lpszSub, 0);
}

int CCOMString::Find(LPCTSTR lpszSub, int nStart)
{
	ATLASSERT(_tcslen(lpszSub) > 0);

	if (nStart > GetLength())
		return -1;

	LPTSTR lpsz = _tcsstr(m_pszString + nStart, lpszSub);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CCOMString::FindOneOf(LPCTSTR lpszCharSet) const
{
	LPTSTR lpsz = _tcspbrk(m_pszString, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CCOMString::ReverseFind(TCHAR ch) const
{
	LPTSTR lpsz = _tcsrchr(m_pszString, (_TUCHAR) ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

//////////////////////////////////////////////////////////////////////
//
// Assignment Operations
//
//////////////////////////////////////////////////////////////////////

const CCOMString& CCOMString::operator=(CCOMString& strSrc)
{
	if (m_pszString != strSrc.GetString())
		StringCopy(strSrc.GetLength(), strSrc.GetString());
	return *this;
}

const CCOMString& CCOMString::operator=(LPCTSTR lpsz)
{
	ATLASSERT(lpsz != NULL);
	StringCopy(_tcslen(lpsz), lpsz);
	return *this;
}

const CCOMString& CCOMString::operator=(BSTR bstrStr)
{
	USES_CONVERSION;
	int nLen = _tcslen(OLE2T(bstrStr));
	StringCopy(nLen, OLE2T(bstrStr));
	return *this;
}


//////////////////////////////////////////////////////////////////////
//
// Concatenation
//
//////////////////////////////////////////////////////////////////////

CCOMString operator+(CCOMString& strSrc1, CCOMString& strSrc2)
{
	CCOMString s;
	s.ConcatCopy((LPCTSTR) strSrc1, (LPCTSTR) strSrc2);
	return s;
}

CCOMString operator+(CCOMString& strSrc, LPCTSTR lpsz)
{
	CCOMString s;
	s.ConcatCopy((LPCTSTR) strSrc, lpsz);
	return s;
}

CCOMString operator+(LPCTSTR lpsz, CCOMString& strSrc)
{
	CCOMString s;
	s.ConcatCopy(lpsz, (LPCTSTR) strSrc);
	return s;
}

CCOMString operator+(CCOMString& strSrc, BSTR bstrStr)
{
	USES_CONVERSION;
	CCOMString s;
	s.ConcatCopy((LPCTSTR) strSrc, OLE2T(bstrStr));
	return s;
}

CCOMString operator+(BSTR bstrStr, CCOMString& strSrc)
{
	USES_CONVERSION;
	CCOMString s;
	s.ConcatCopy(OLE2T(bstrStr), (LPCTSTR) strSrc);
	return s;
}

const CCOMString& CCOMString::operator+=(CCOMString& strSrc)
{
	ConcatCopy((LPCTSTR) strSrc);
	return *this;
}

const CCOMString& CCOMString::operator+=(LPCTSTR lpsz)
{
	ConcatCopy(lpsz);
	return *this;
}

const CCOMString& CCOMString::operator+=(BSTR bstrStr)
{
	USES_CONVERSION;
	ConcatCopy(OLE2T(bstrStr));
	return *this;
}

const CCOMString& CCOMString::operator+=(TCHAR ch)
{
	ConcatCopy(ch);
	return *this;
}


//////////////////////////////////////////////////////////////////////
//
// Extraction
//
//////////////////////////////////////////////////////////////////////

CCOMString CCOMString::Mid(int nFirst) const
{
	return Mid(nFirst, GetLength() - nFirst);	
}

CCOMString CCOMString::Mid(int nFirst, int nCount) const
{
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;

	if (nFirst + nCount > GetLength())
		nCount = GetLength() - nFirst;
	if (nFirst > GetLength())
		nCount = 0;

	ATLASSERT(nFirst >= 0);
	ATLASSERT(nFirst + nCount <= GetLength());

	if (nFirst == 0 && nFirst + nCount == GetLength())
		return *this;

	CCOMString newStr;
	StringCopy(newStr, nCount, nFirst, 0);
	return newStr;
}

CCOMString CCOMString::Left(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetLength())
		return *this;

	CCOMString newStr;
	StringCopy(newStr, nCount, 0, 0);
	return newStr;
}

CCOMString CCOMString::Right(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetLength())
		return *this;

	CCOMString newStr;
	StringCopy(newStr, nCount, GetLength() - nCount, 0);
	return newStr;
}

CCOMString CCOMString::SpanIncluding(LPCTSTR lpszCharSet) const
{
	ATLASSERT(lpszCharSet != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, -1) == FALSE);
#else
	ATLASSERT(::IsBadStringPtrA(lpszCharSet, -1) == FALSE);
#endif
	return Left(_tcsspn(m_pszString, lpszCharSet));
}

CCOMString CCOMString::SpanExcluding(LPCTSTR lpszCharSet) const
{
	ATLASSERT(lpszCharSet != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, -1) == FALSE);
#else
	ATLASSERT(::IsBadStringPtrA(lpszCharSet, -1) == FALSE);
#endif
	return Left(_tcscspn(m_pszString, lpszCharSet));
}


//////////////////////////////////////////////////////////////////////
//
// Comparison
//
//////////////////////////////////////////////////////////////////////
	
int CCOMString::Compare(CCOMString& str) const
{
	ATLASSERT((LPCTSTR) str != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW((LPCTSTR) str, -1) == FALSE);
#else
	ATLASSERT(::IsBadStringPtrA((LPCTSTR) str, -1) == FALSE);
#endif
	return _tcscmp(m_pszString, (LPCTSTR) str);	
}

int CCOMString::Compare(LPCTSTR lpsz) const
{
	ATLASSERT(lpsz != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == FALSE);
#else
	ATLASSERT(::IsBadStringPtrA(lpsz, -1) == FALSE);
#endif
	return _tcscmp(m_pszString, lpsz);	
}

int CCOMString::CompareNoCase(CCOMString& str) const
{
	ATLASSERT((LPCTSTR) str != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW((LPCTSTR) str, -1) == FALSE);
#else
	ATLASSERT(::IsBadStringPtrA((LPCTSTR) str, -1) == FALSE);
#endif
	return _tcsicmp(m_pszString, (LPCTSTR) str);	
}

int CCOMString::CompareNoCase(LPCTSTR lpsz) const
{
	ATLASSERT(lpsz != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == FALSE);
#else
	ATLASSERT(::IsBadStringPtrA(lpsz, -1) == FALSE);
#endif
	return _tcsicmp(m_pszString, lpsz);	
}

int CCOMString::Collate(LPCTSTR lpsz) const
{
	ATLASSERT(lpsz != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == FALSE);
#else
	ATLASSERT(::IsBadStringPtrA(lpsz, -1) == FALSE);
#endif
	return _tcscoll(m_pszString, lpsz);	
}

int CCOMString::Collate(CCOMString &str) const
{
	ATLASSERT((LPCTSTR) str != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW((LPCTSTR) str, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA((LPCTSTR) str, -1) == 0);
#endif
	return _tcscoll(m_pszString, (LPCTSTR) str);	
}


//////////////////////////////////////////////////////////////////////
//
// Formatting
//
//////////////////////////////////////////////////////////////////////
	
void CCOMString::Format(LPCTSTR pszFormat, ...)
{
	va_list vl;
	va_start(vl, pszFormat);

	TCHAR* pszTemp = NULL;
	int nBufferSize = 0;
	int nRetVal = -1;

	do {
		nBufferSize += 100;
		delete [] pszTemp;
		pszTemp = new TCHAR[nBufferSize];
		ATLASSERT( pszTemp != NULL );
		if( pszTemp == NULL )
			return;
		//memset(pszTemp, 0, nBufferSize);
		nRetVal = _vsntprintf(pszTemp, nBufferSize - 1, pszFormat, vl);
		if( nRetVal == nBufferSize - 1 )
			pszTemp[nBufferSize - 1] = _T('\0');
	} while (nRetVal < 0);

	int nNewLen = _tcslen(pszTemp);
	AllocString(nNewLen);
	_tcscpy(m_pszString, pszTemp);
	delete [] pszTemp;

	va_end(vl);
}

void CCOMString::FormatV(LPCTSTR pszFormat, va_list argList)
{
	TCHAR* pszTemp = NULL;
	int nBufferSize = 0;
	int nRetVal = -1;

	do {
		nBufferSize += 100;
		delete [] pszTemp;
		pszTemp = new TCHAR[nBufferSize];
		ATLASSERT( pszTemp != NULL );
		if( pszTemp == NULL )
			return;
		//memset(pszTemp, 0, nBufferSize);
		nRetVal = _vsntprintf(pszTemp, nBufferSize - 1, pszFormat, argList);
		if( nRetVal == nBufferSize - 1 )
			pszTemp[nBufferSize - 1] = _T('\0');
	} while (nRetVal < 0);

	int nNewLen = _tcslen(pszTemp);
	AllocString(nNewLen);
	_tcscpy(m_pszString, pszTemp);
	delete [] pszTemp;
}

//////////////////////////////////////////////////////////////////////
//
// Replacing
//
//////////////////////////////////////////////////////////////////////

int CCOMString::Replace(TCHAR chOld, TCHAR chNew)
{
	int nCount = 0;

	if (chOld != chNew)
	{
		LPTSTR psz = m_pszString;
		LPTSTR pszEnd = psz + GetLength();
		while(psz < pszEnd)
		{
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = _tcsinc(psz);
		}
	}

	return nCount;
}

int CCOMString::Replace(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	int nSourceLen = _tcslen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplaceLen = _tcslen(lpszNew);

	int nCount = 0;
	LPTSTR lpszStart = m_pszString;
	LPTSTR lpszEnd = lpszStart + GetLength();
	LPTSTR lpszTarget;

	// Check to see if any changes need to be made
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
		{
			lpszStart = lpszTarget + nSourceLen;
			nCount++;
		}
		lpszStart += _tcslen(lpszStart) + 1;
	}

	// Do the actual work
	if (nCount > 0)
	{
		int nOldLen = GetLength();
		int nNewLen = nOldLen + (nReplaceLen - nSourceLen) * nCount;
		if (GetLength() < nNewLen)
		{
			CCOMString szTemp = m_pszString;
			ReAllocString(nNewLen);
			memcpy(m_pszString, (LPCTSTR) szTemp, nOldLen * sizeof(TCHAR));
		}

		lpszStart = m_pszString;
		lpszEnd = lpszStart + GetLength();

		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLen - (lpszTarget - m_pszString + nSourceLen);
				memmove(lpszTarget + nReplaceLen, lpszTarget + nSourceLen, nBalance * sizeof(TCHAR));
				memcpy(lpszTarget, lpszNew, nReplaceLen * sizeof(TCHAR));
				lpszStart = lpszTarget + nReplaceLen;
				lpszStart[nBalance] = _T('\0');
				nOldLen += (nReplaceLen - nSourceLen);
			}
			lpszStart += _tcslen(lpszStart) + 1;
		}
		ATLASSERT(m_pszString[GetLength()] == _T('\0'));
	}
	return nCount;
}

LPTSTR CCOMString::GetBuffer( int nMinBufLength )
{
	ReAllocString( nMinBufLength );
	return m_pszString;
}

void CCOMString::ReleaseBuffer( int nNewLength )
{
	if( nNewLength == -1 )	
		nNewLength = _tcslen(m_pszString) + 1;
	ReAllocString( nNewLength );
}

int CCOMString::Delete( int nIndex, int nCount )
{
	int nLength = GetLength();
	ATLASSERT( nCount >= 0 );
	if( nCount == 0 )
		return nLength;

	ATLASSERT( nIndex < nLength );
	if( nIndex + nCount > nLength )
	{
		nCount = nLength - nIndex;
	}

	memmove(
		&m_pszString[nIndex],
		&m_pszString[nIndex+nCount],
		(nLength+1-nIndex-nCount) * sizeof(TCHAR));

	return nLength - nCount;
}

int CCOMString::Remove( TCHAR ch )
{
/*
	TCHAR find[2] = _T(" ");
	TCHAR paste[1] = _T("");
	*find = ch;
	int res = Replace(find, paste);
	return res;
*/
	int nFound = 0;

	LPTSTR p1 = m_pszString;
	LPTSTR p2 = m_pszString;

	if( m_pszString == NULL )
		return 0;

	if( ch == _T('\0') )
		return 0;

	while( TRUE )
	{
		TCHAR ch2 = *p2;
		if( ch2 == ch )
		{
			p2 = _tcsinc(p2);
		}
		else
		{
			//if( p1 != p2 ) // optional string: maybe faster or may be slower...
				*p1 = ch2;
			p1 = _tcsinc(p1);
			p2 = _tcsinc(p2);
		}

		if( ch2 == _T('\0') )
			break;
	};

	return nFound;
}

