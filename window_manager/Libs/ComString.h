// CCOMString.h : header file
//
// CCOMString Header
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_)
#define AFX_COMSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <oleauto.h>

class CCOMString  
{
public: 
	CCOMString();
	CCOMString(CCOMString& szString);
	CCOMString(LPCTSTR pszString);
	CCOMString(BSTR bstrString);
	CCOMString(TCHAR ch, int nRepeat);
	//virtual 
	~CCOMString();
	
	// Assignment Operations
	const CCOMString& operator=(CCOMString& strSrc);
	const CCOMString& operator=(LPCTSTR lpsz);
	const CCOMString& operator=(BSTR bstrStr);
	operator LPCTSTR() const	{ return m_pszString; }
	TCHAR*	GetString()			{ return m_pszString; }

	#ifndef NO_SYS_ALLOC_STRING_LEN
	BSTR	AllocSysString();
	#endif
	
	// Concatenation
	const CCOMString& operator+=(CCOMString& strSrc);
	const CCOMString& operator+=(LPCTSTR lpsz);
	const CCOMString& operator+=(BSTR bstrStr);
	const CCOMString& operator+=(TCHAR ch);
	friend CCOMString operator+(CCOMString& strSrc1, CCOMString& strSrc2);
	friend CCOMString operator+(CCOMString& strSrc, LPCTSTR lpsz);
	friend CCOMString operator+(LPCTSTR lpsz, CCOMString& strSrc);
	friend CCOMString operator+(CCOMString& strSrc, BSTR bstrStr);
	friend CCOMString operator+(BSTR bstrStr, CCOMString& strSrc);

	// Accessors for the String as an Array
	int		GetLength() const;
	bool	IsEmpty() const;
	void	Empty();
	TCHAR	GetAt(int nIndex);
	void	SetAt(int nIndex, TCHAR ch);
	TCHAR	operator[] (int nIndex);

	// Conversions
	void	MakeUpper();
	void	MakeLower();
	void	MakeReverse();

	// Trimming
	void	TrimLeft();
	void	TrimLeft( TCHAR chTarget );
	void	TrimLeft( LPCTSTR lpszTargets );
	void	TrimRight();
	void	TrimRight( TCHAR chTarget );
	void	TrimRight( LPCTSTR lpszTargets );

	// Searching
	int		Find(TCHAR ch) const;
	int		Find(TCHAR ch, int nStart) const;
	int		Find(LPCTSTR lpszSub);
	int		Find(LPCTSTR lpszSub, int nStart);
	int		FindOneOf(LPCTSTR lpszCharSet) const;
	int     ReverseFind(TCHAR ch) const;

	// Extraction
	CCOMString Mid(int nFirst) const;
	CCOMString Mid(int nFirst, int nCount) const;
	CCOMString Left(int nCount) const;
	CCOMString Right(int nCount) const;
	CCOMString SpanIncluding(LPCTSTR lpszCharSet) const;
	CCOMString SpanExcluding(LPCTSTR lpszCharSet) const;

	// Comparison
	int Compare(CCOMString& str) const;
	int Compare(LPCTSTR lpsz) const;
	int CompareNoCase(CCOMString& str) const;
	int CompareNoCase(LPCTSTR lpsz) const;
	int Collate(CCOMString& str) const;
	int Collate(LPCTSTR lpsz) const;

	// Formatting
	void Format(LPCTSTR pszFormat, ...);
	void FormatV(LPCTSTR pszFormat, va_list argList);

	// Replacing
	int Replace(TCHAR chOld, TCHAR chNew);
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	
	LPTSTR GetBuffer( int nMinBufLength );
	void ReleaseBuffer( int nNewLength = -1 );
	int Delete( int nIndex, int nCount = 1 );
	int Remove( TCHAR ch );

protected:
	LPTSTR	m_pszString;
	void	StringCopy(CCOMString& str, int nLen, int nIndex, int nExtra) const;
	void	StringCopy(int nSrcLen, LPCTSTR lpszSrcData);
	void	ConcatCopy(LPCTSTR lpszData);
	void	ConcatCopy(TCHAR ch);
	void	ConcatCopy(LPCTSTR lpszData1, LPCTSTR lpszData2);
	void	AllocString(int nLen);
	void	ReAllocString(int nLen);
};	

// Compare operations
bool operator==(const CCOMString& s1, const CCOMString& s2);
bool operator==(const CCOMString& s1, LPCTSTR s2);
bool operator==(LPCTSTR s1, const CCOMString& s2);
bool operator!=(const CCOMString& s1, const CCOMString& s2);
bool operator!=(const CCOMString& s1, LPCTSTR s2);
bool operator!=(LPCTSTR s1, const CCOMString& s2);

// Compare implementations
inline bool operator==(const CCOMString& s1, const CCOMString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const CCOMString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(LPCTSTR s1, const CCOMString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const CCOMString& s1, const CCOMString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const CCOMString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(LPCTSTR s1, const CCOMString& s2)
	{ return s2.Compare(s1) != 0; }

#endif // !defined(AFX_COMSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_)
