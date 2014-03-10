// HotKey.cpp: implementation of the CHotKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SerializeStruct.h"
#include "HotKey.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHotKey::CHotKey()
{
	m_fsModifiers = 0;
	m_vk = 0;
	m_id = 0;
}

CHotKey::~CHotKey()
{

}

bool CHotKey::operator ==(const CHotKey &info)
{
	bool res = true;

	res &= (m_vk == info.m_vk);
	res &= (m_fsModifiers == info.m_fsModifiers);

	return res;
}

BOOL CHotKey::RegisterHotKey(
						 HWND hWnd
						 )
{
	UINT uiWinfsModifiers = 0;

	if (m_fsModifiers & HOTKEYF_ALT)
	{
		uiWinfsModifiers |= MOD_ALT;
	}
	if (m_fsModifiers & HOTKEYF_CONTROL)
	{
		uiWinfsModifiers |= MOD_CONTROL;
	}
	if (m_fsModifiers & HOTKEYF_SHIFT)
	{
		uiWinfsModifiers |= MOD_SHIFT;
	}
	if (m_fsModifiers & HOTKEYF_EXT)
	{
		uiWinfsModifiers |= MOD_WIN;
	}

	return ::RegisterHotKey(hWnd, m_id, uiWinfsModifiers, m_vk);
}

BOOL CHotKey::RegisterHotKey(
						 HWND hWnd,
						 int id,
						 int vk,
						 int fsModifiers
						 )
{
	m_id = id;
	m_vk = vk;
	m_fsModifiers = fsModifiers;

	return RegisterHotKey(hWnd);
}

BOOL CHotKey::Serialize( SERIALIZE_CONTEXT& context )
{
	SERIALIZE_START
		
	SERIALIZE_INT( UINT,	m_fsModifiers,	_T("fsModifiers") )
	SERIALIZE_INT( UINT,	m_vk,			_T("vk") )
	
	SERIALIZE_END
}

#ifndef COMPILE_DL_PRESENTER
void CHotKey::LuaSerialize(CLua &L)
{
//	LUA_SERIALIZE(L, _T("fsModifiers"),	m_fsModifiers);
//	LUA_SERIALIZE(L, _T("vk"),			m_vk);
}
#endif // COMPILE_DL_PRESENTER

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
BOOL CHotKey::PHPSerialize(BOOL GetData, zval *val)
{
	PHP_SERIALIZE_START
		
	PHP_SERIALIZE_INT( UINT,	m_fsModifiers,	_T("fsModifiers") )
	PHP_SERIALIZE_INT( UINT,	m_vk,			_T("vk") )
	
	PHP_SERIALIZE_END
}
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

