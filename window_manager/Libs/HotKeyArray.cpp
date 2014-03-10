// HotKeyArray.cpp: implementation of the CHotKeyArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HotKeyArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHotKeyArray::CHotKeyArray()
{

}

CHotKeyArray::~CHotKeyArray()
{

}

BOOL CHotKeyArray::Lock()
{
	return m_CritSection.Lock();
}

BOOL CHotKeyArray::Unlock()
{
	return m_CritSection.Unlock();
}

void CHotKeyArray::Clean()
{
	Lock();
	RemoveAll();
	Unlock();
}

BOOL CHotKeyArray::IsHotKeyExist(CHotKey HotKey)
{
	Lock();
	for(int i = 0; i < GetSize(); i++)
	{
		CHotKey &tempHotKey = ElementAt(i);
		if(tempHotKey == HotKey)
		{
			Unlock();
			return TRUE;
		}
	}
	Unlock();
	return FALSE;
}

BOOL CHotKeyArray::GetHotKey(UINT id, CHotKey &HotKey)
{
	Lock();
	for(int i = 0; i < GetSize(); i++)
	{
		CHotKey &tempHotKey = ElementAt(i);
		if(tempHotKey.m_id == id)
		{
			HotKey = tempHotKey;
			Unlock();
			return TRUE;
		}
	}
	Unlock();
	return FALSE;
}

