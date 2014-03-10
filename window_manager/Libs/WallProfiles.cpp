// WallProfiles.cpp: implementation of the CWallProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallProfiles.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CWallProfile
CWallProfile::CWallProfile()
{
	m_WallProfileName = _T("WallProfile");

	m_WallMonitorResolution.dmPelsWidth = 1024;
	m_WallMonitorResolution.dmPelsHeight = 768;
	m_WallMonitorResolution.dmBitsPerPel = 32;
	m_WallMonitorResolution.dmDisplayFrequency = 60;
}

CWallProfile::CWallProfile(CWallProfile &src)
{
	*this = src;
}

CWallProfile& CWallProfile::operator =(const CWallProfile &src)
{
	m_WallProfileName		=	src.m_WallProfileName;
	m_WallConfig			=	src.m_WallConfig;
	m_WallMonitorResolution	=	src.m_WallMonitorResolution;

	return *this;
}

BOOL CWallProfile::operator ==(const CWallProfile &src)
{
	return 		m_WallProfileName		==	src.m_WallProfileName &&
				m_WallConfig			==	src.m_WallConfig &&
				m_WallMonitorResolution	==	src.m_WallMonitorResolution;
}

BOOL CWallProfile::operator !=(const CWallProfile &src)
{
	return !((*this) == src);
}

BOOL CWallProfile::Serialize( SERIALIZE_CONTEXT& context )
{
	SERIALIZE_START
	
	SERIALIZE_STRING(	m_WallProfileName,			_T("WallProfileName"))

	SERIALIZE_STRUCT(	m_WallConfig,				_T("WallConfig"))
	SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmPelsWidth,			_T("dmPelsWidth"))
	SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmPelsHeight,		_T("dmPelsHeight"))
	SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmDisplayFrequency,	_T("dmDisplayFrequency"))
	SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmBitsPerPel,		_T("dmBitsPerPel"))
	
	SERIALIZE_END
}

#ifndef COMPILE_DL_PRESENTER
void CWallProfile::LuaSerialize(CLua &L)
{
	L.AddKey(_T("WallProfileName"),m_WallProfileName);
	L.AddKey(_T("WallConfig"),	&m_WallConfig);
	L.AddKey(_T("dmPelsWidth"), m_WallMonitorResolution.dmPelsWidth);
	L.AddKey(_T("dmPelsHeight"), m_WallMonitorResolution.dmPelsHeight);
	L.AddKey(_T("dmDisplayFrequency"), m_WallMonitorResolution.dmDisplayFrequency);
	L.AddKey(_T("dmBitsPerPel"), m_WallMonitorResolution.dmBitsPerPel);
}
#endif // COMPILE_DL_PRESENTER

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
BOOL CWallProfile::PHPSerialize(BOOL GetData, zval *val)
{
	PHP_SERIALIZE_START

	PHP_SERIALIZE_STRING(	m_WallProfilesName,			_T("WallProfilesName"))

	PHP_SERIALIZE_STRUCT(	m_WallConfig,				_T("WallConfig"))
	PHP_SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmPelsWidth,			_T("dmPelsWidth"))
	PHP_SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmPelsHeight,		_T("dmPelsHeight"))
	PHP_SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmDisplayFrequency,	_T("dmDisplayFrequency"))
	PHP_SERIALIZE_INT(	DWORD, m_WallMonitorResolution.dmBitsPerPel,		_T("dmBitsPerPel"))

	PHP_SERIALIZE_END
}
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

//////////////////////////////////////////////////////////////////////////
// CWallProfileArray

CWallProfileArray::CWallProfileArray()
{
}

CWallProfileArray& CWallProfileArray::operator =(const CWallProfileArray &src)
{
	RemoveAll();
	Copy(src);

	return *this;
}

BOOL CWallProfileArray::operator ==(const CWallProfileArray &src)
{
	if(GetSize() != src.GetSize())
		return FALSE;
	for(int i = 0; i < GetSize(); i++)
	{
		if(ElementAt(i) != src[i])
			return FALSE;
	}
	return TRUE;
}

BOOL CWallProfileArray::operator !=(const CWallProfileArray &src)
{
	return !((*this) == src);
}

int CWallProfileArray::GetCurrentWallProfileIndex(CWallProfile &WallProfile)
{
	// find current profile
	for (int i = 0; i < GetSize(); i++)
	{
		if (GetAt(i)== WallProfile)
		{
			return i;
		}
	}
	
	// if not found we always return 0
	return 0;
}

int CWallProfileArray::GetCurrentWallProfileIndex(CString strIni)
{
	CWallProfile WallProfile;
	WallProfile.SetValues(strIni);
	
	return GetCurrentWallProfileIndex(WallProfile);
}

BOOL CWallProfileArray::Serialize(SERIALIZE_CONTEXT& context)
{
	SERIALIZE_START
	SERIALIZE_ARRAY(CWallProfile, *this,			_T("arrWallProfile") )
	SERIALIZE_END
}

#ifndef COMPILE_DL_PRESENTER
void CWallProfileArray::LuaSerialize(CLua &L)
{
	L.AddKey("Count", GetSize());
	for(int i = 0; i < GetSize(); i++)
		L.AddKey(i + 1, &GetAt(i));
}
#endif // COMPILE_DL_PRESENTER

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
BOOL CWallProfileArray::PHPSerialize(BOOL GetData, zval *val)
{
	ASSERT(FALSE);
	return TRUE;
}
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

