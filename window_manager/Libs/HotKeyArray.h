// HotKeyArray.h: interface for the CHotKeyArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOTKEYARRAY_H__9D4DDEAE_BF44_4F25_8C2A_7836DD21B274__INCLUDED_)
#define AFX_HOTKEYARRAY_H__9D4DDEAE_BF44_4F25_8C2A_7836DD21B274__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HotKey.h"

class CHotKeyArray : public CArray<CHotKey, CHotKey&>
{
public:
	CHotKeyArray();
	virtual ~CHotKeyArray();

	BOOL IsHotKeyExist(CHotKey HotKey);
	BOOL GetHotKey(UINT id, CHotKey &HotKey);

	BOOL Lock();
	BOOL Unlock();
	void Clean();
	
protected:
	CCriticalSection	m_CritSection;

};

#endif // !defined(AFX_HOTKEYARRAY_H__9D4DDEAE_BF44_4F25_8C2A_7836DD21B274__INCLUDED_)
