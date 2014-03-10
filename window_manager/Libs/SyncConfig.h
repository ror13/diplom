// SyncConfig.h: interface for the CSyncConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCCONFIG_H__321ED8CC_5F67_42AD_B206_5335F089403E__INCLUDED_)
#define AFX_SYNCCONFIG_H__321ED8CC_5F67_42AD_B206_5335F089403E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "product.h"

#include "SerializeStruct.h"

enum E_SYNC_CLIENT_MODES
{
	SYNC_CLIENT_NONE,
	SYNC_CLIENT_MASTER,
	SYNC_CLIENT_SLAVE,
		
	SYNC_CLIENT_MODES_COUNT
};

#define SYNC_CLIENT_ERROR_NAME	_T('E')
#define SYNC_CLIENT_NONE_NAME	_T('N')
#define SYNC_CLIENT_MASTER_NAME	_T('M')
#define SYNC_CLIENT_SLAVE_NAME	_T('S')
		
class CSyncConfig : public CSerializeStruct  
{
public:
	int	m_SyncClientMode;

	CSyncConfig();
	CSyncConfig(const CSyncConfig& src);

	virtual ~CSyncConfig();

	CSyncConfig& operator =(const CSyncConfig &src);
	
	BOOL operator !=(const CSyncConfig &src);
	BOOL operator ==(const CSyncConfig &src);
	
	virtual BOOL Serialize( SERIALIZE_CONTEXT& context );
#ifndef COMPILE_DL_PRESENTER
	virtual void LuaSerialize(CLua &L);
#endif // !COMPILE_DL_PRESENTER
#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
	virtual BOOL PHPSerialize(BOOL GetData, zval *val);
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER
};

#endif // !defined(AFX_SYNCCONFIG_H__321ED8CC_5F67_42AD_B206_5335F089403E__INCLUDED_)
