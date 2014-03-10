// WallProfiles.h: interface for the CWallProfiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WallProfiles_H__9D4DDEAE_BF44_4F25_8C2A_7836DD21B274__INCLUDED_)
#define AFX_WallProfiles_H__9D4DDEAE_BF44_4F25_8C2A_7836DD21B274__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerializeStruct.h"
#include "VirtualWallConfig.h"
#include "WallMonConfig.h"

class CWallProfile : public CSerializeStruct
{
public:
	CWallProfile();
	CWallProfile(CWallProfile &src);
	CWallProfile& operator =(const CWallProfile &src);
	BOOL operator ==(const CWallProfile &src);
	BOOL operator !=(const CWallProfile &src);

	CString				m_WallProfileName;
	CVirtualWallConfig	m_WallConfig;
	MON_MODE			m_WallMonitorResolution;
	
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

//////////////////////////////////////////////////////////////////////////

class CWallProfileArray : public CArray<CWallProfile, CWallProfile&>, public CSerializeStruct
{
public:
	CWallProfileArray();
	CWallProfileArray& operator =(const CWallProfileArray &src);
	BOOL operator ==(const CWallProfileArray &src);
	BOOL operator !=(const CWallProfileArray &src);
	
	int GetCurrentWallProfileIndex(CWallProfile &WallProfile);
	int GetCurrentWallProfileIndex(CString strIni);

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


#endif // !defined(AFX_WallProfiles_H__9D4DDEAE_BF44_4F25_8C2A_7836DD21B274__INCLUDED_)
