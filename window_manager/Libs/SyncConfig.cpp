// SyncConfig.cpp: implementation of the CSyncConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pnetclient.h"
#include "SyncConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSyncConfig::CSyncConfig()
{
	m_SyncClientMode = SYNC_CLIENT_NONE;
}

CSyncConfig::~CSyncConfig()
{

}

CSyncConfig::CSyncConfig(const CSyncConfig &src)
{
	*this = src;
}

CSyncConfig& CSyncConfig::operator =(const CSyncConfig &src)
{
	m_SyncClientMode = src.m_SyncClientMode;
	
	return *this;
}

BOOL CSyncConfig::operator !=(const CSyncConfig &src)
{
	BOOL res = ((*this) == src);
	return !res;
}

BOOL CSyncConfig::operator ==(const CSyncConfig &src)
{
	BOOL res = 
		m_SyncClientMode == src.m_SyncClientMode;
		
	return res;
}

BOOL CSyncConfig::Serialize(SERIALIZE_CONTEXT& context)
{
	SERIALIZE_START
		SERIALIZE_INT( int, m_SyncClientMode,					_T("SyncClientMode"))
	SERIALIZE_END
}

#ifndef COMPILE_DL_PRESENTER
void CSyncConfig::LuaSerialize(CLua &L)
{
	LUA_SERIALIZE(L, _T("m_SyncClientMode"),	m_SyncClientMode);
/*	L.AddKey(_T("FileName"),	m_MediaFileName);
	L.AddKey(_T("MD5Signature"),m_MD5Signature);
	L.AddKey(_T("FileSize"),	m_MediaFileSize);	*/	
}
#endif // COMPILE_DL_PRESENTER

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
BOOL CSyncConfig::PHPSerialize(BOOL GetData, zval *val)
{
	PHP_SERIALIZE_START
		PHP_SERIALIZE_INT(m_SyncClientMode,					_T("SyncClientMode"))
	PHP_SERIALIZE_END
}
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

