// HotKey.h: interface for the CHotKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOTKEY_H__ADA48262_C751_46A8_BA9E_6C2C3D620211__INCLUDED_)
#define AFX_HOTKEY_H__ADA48262_C751_46A8_BA9E_6C2C3D620211__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerializeStruct.h"

class CHotKey: public CSerializeStruct
{
public:
	CHotKey();
	virtual ~CHotKey();

public:
	UINT	m_fsModifiers;
	UINT	m_vk;
	UINT	m_id;

	bool operator ==(const CHotKey &info);
	BOOL RegisterHotKey(
		HWND hWnd
		);

	BOOL RegisterHotKey(
		HWND hWnd,
		int id,
		int vk,
		int fsModifiers
		);
	

protected:
	virtual BOOL Serialize( SERIALIZE_CONTEXT& context );
#ifndef COMPILE_DL_PRESENTER
	virtual void LuaSerialize(CLua &L);
#endif // COMPILE_DL_PRESENTER
#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
	virtual BOOL PHPSerialize(BOOL GetData, zval *val);
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER
};

#endif // !defined(AFX_HOTKEY_H__ADA48262_C751_46A8_BA9E_6C2C3D620211__INCLUDED_)
