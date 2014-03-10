// LogonInfo.cpp : implementation file
//

#include "stdafx.h"
#include "LogonInfo.h"

#include "../Inc/CommonApi.h"
#include "../Inc/Applica.h"

//{{AXPDEL
// STATION_HOST is normally defined in StationInfo.h (Presenter)
//}}AXPDEL
#ifndef STATION_HOST
#define STATION_HOST 0
#else
#if STATION_HOST != 0
#error "STATION_HOST is incorrect!!!"
#endif
#endif

/////////////////////////////////////////////////////////////////////////////

CString GetStationInfoKey( int iStationXP )
{
//{{AXPDEL
//	const CStaticRuntimeInfo& srti = g_info.GetStaticInfo();
//	ASSERT( srti.m_product == PRESENTER_XP );
//}}AXPDEL

	TCHAR szApplicaRegistry[200] = _T("");

	BOOL res = regReadRegString( REG_HKLM, HKLM_INFO_REG_KEY_T, REG_BASE_REG_VALUE_T,
		szApplicaRegistry, SIZEOF_ARRAY(szApplicaRegistry) );

	if( !res )
	{
		return _T("");
	}

	if( iStationXP == STATION_HOST )
	{
		return _T("");
	}

	CString stationkey;
	stationkey.Format( APPLICA_STATION_INFORMATION_FMT,
		szApplicaRegistry, iStationXP-1 );

	return stationkey;
}

/////////////////////////////////////////////////////////////////////////////

CString	GetMessagesAgentExeName( int iStationXP )
{
	DWORD dwSize;
	BOOL res;

//{{AXPDEL
//	const CStaticRuntimeInfo& srti = g_info.GetStaticInfo();
//	ASSERT( srti.m_product == PRESENTER_XP || srti.m_product == PRESENTER_IP );
//}}AXPDEL

	TCHAR szValue[MAX_PATH] = _T("");

	if( iStationXP == STATION_HOST )
	{
		return _T("");
	}

	TCHAR szApplicaRegistry[200] = _T("");

	res = regReadRegString( REG_HKLM, HKLM_INFO_REG_KEY_T, REG_BASE_REG_VALUE_T,
		szApplicaRegistry, SIZEOF_ARRAY(szApplicaRegistry) );

	if( !res )
	{
		return _T("");
	}

	szValue[0] = _T('\0');
	dwSize = SIZEOF_ARRAY(szValue);
	res = regReadRegString( REG_HKLM, szApplicaRegistry,
		_T("AgentExe"), szValue, dwSize );

	return szValue;
}

/////////////////////////////////////////////////////////////////////////////

DWORD	GetDisableServiceState( VOID )
{
	DWORD dwDisableService = FALSE;

	BOOL res = regReadRegDword( REG_HKLM, HKLM_INFO_REG_KEY_T, REG_DISABLE_SERVICE_VALUE_T,
		&dwDisableService);

	if( !res )
	{
		return 0;
	}

	return dwDisableService;
}

/////////////////////////////////////////////////////////////////////////////

BOOL	SetDisableServiceState( DWORD dwDisableService )
{
	BOOL res = regWriteRegDword( REG_HKLM, HKLM_INFO_REG_KEY_T, REG_DISABLE_SERVICE_VALUE_T,
		dwDisableService);

	if( !res )
	{
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLogonInfo

CLogonInfo::CLogonInfo()
{
	m_bAutoLogon = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

CLogonInfo::~CLogonInfo()
{
}

/////////////////////////////////////////////////////////////////////////////

BOOL CLogonInfo::Load( int iStationXP )
{
	BYTE b;
	DWORD dwSize;
	BOOL res;

	m_bAutoLogon = FALSE;
	m_strDomain.Empty();
	m_strLogin.Empty();
	m_strPassword.Empty();
	m_strInitialCommand.Empty();

	TCHAR szValue[MAX_PATH] = _T("");

	if( iStationXP == STATION_HOST )
	{
		return FALSE;
	}

	CString stationkey = GetStationInfoKey( iStationXP );
	if( stationkey.IsEmpty() )
	{
		return FALSE;
	}

	b = 0;
	dwSize = 1;
	res = regReadRegBin( REG_HKLM, stationkey,
		APPLICA_STATION_AUTOLOGON_VALUE, &b, &dwSize );
	if( res )
	{
		m_bAutoLogon = b;
	}

	szValue[0] = _T('\0');
	dwSize = SIZEOF_ARRAY(szValue);
	res = regReadRegString( REG_HKLM, stationkey,
		APPLICA_STATION_DOMAIN_VALUE, szValue, dwSize );
	m_strDomain = szValue;

	szValue[0] = _T('\0');
	dwSize = SIZEOF_ARRAY(szValue);
	res = regReadRegString( REG_HKLM, stationkey,
		APPLICA_STATION_LOGIN_VALUE, szValue, dwSize );
	m_strLogin = szValue;

	szValue[0] = _T('\0');
	dwSize = SIZEOF_ARRAY(szValue);
	res = regReadRegString( REG_HKLM, stationkey,
		APPLICA_STATION_PASSWORD_VALUE, szValue, dwSize );
	m_strPassword = szValue;

	szValue[0] = _T('\0');
	dwSize = SIZEOF_ARRAY(szValue);
	res = regReadRegString( REG_HKLM, stationkey,
		APPLICA_STATION_COMMAND_VALUE, szValue, dwSize );
	m_strInitialCommand = szValue;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CLogonInfo::Save( int iStationXP ) const
{
	BYTE b;
	BOOL res;

	if( iStationXP == STATION_HOST )
	{
		return FALSE;
	}

	CString stationkey = GetStationInfoKey( iStationXP );
	if( stationkey.IsEmpty() )
	{
		return FALSE;
	}

	b = (BYTE) m_bAutoLogon;
	res = regWriteRegBin( REG_HKLM, stationkey,
		APPLICA_STATION_AUTOLOGON_VALUE, &b, 1 );

	res = regWriteRegString( REG_HKLM, stationkey,
		APPLICA_STATION_DOMAIN_VALUE, m_strDomain );

	res = regWriteRegString( REG_HKLM, stationkey,
		APPLICA_STATION_LOGIN_VALUE, m_strLogin );

	res = regWriteRegString( REG_HKLM, stationkey,
		APPLICA_STATION_PASSWORD_VALUE, m_strPassword );

	res = regWriteRegString( REG_HKLM, stationkey,
		APPLICA_STATION_COMMAND_VALUE, m_strInitialCommand );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLastLogonInfo

CLastLogonInfo::CLastLogonInfo()
{
}

/////////////////////////////////////////////////////////////////////////////

CLastLogonInfo::~CLastLogonInfo()
{
}

/////////////////////////////////////////////////////////////////////////////

BOOL CLastLogonInfo::Load( int iStationXP )
{
	DWORD dwSize;
	BOOL res;

	m_strDomain.Empty();
	m_strLogin.Empty();

	TCHAR szValue[MAX_PATH] = _T("");

	if( iStationXP == STATION_HOST )
	{
		return FALSE;
	}

	CString stationkey = GetStationInfoKey( iStationXP );
	if( stationkey.IsEmpty() )
	{
		return FALSE;
	}

	szValue[0] = _T('\0');
	dwSize = SIZEOF_ARRAY(szValue);
	res = regReadRegString( REG_HKLM, stationkey,
		APPLICA_STATION_LAST_DOMAIN_VALUE, szValue, dwSize );
	m_strDomain = szValue;

	szValue[0] = _T('\0');
	dwSize = SIZEOF_ARRAY(szValue);
	res = regReadRegString( REG_HKLM, stationkey,
		APPLICA_STATION_LAST_LOGIN_VALUE, szValue, dwSize );
	m_strLogin = szValue;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CLastLogonInfo::Save( int iStationXP ) const
{
	BOOL res;

	if( iStationXP == STATION_HOST )
	{
		return FALSE;
	}

	CString stationkey = GetStationInfoKey( iStationXP );
	if( stationkey.IsEmpty() )
	{
		return FALSE;
	}

	res = regWriteRegString( REG_HKLM, stationkey,
		APPLICA_STATION_LAST_DOMAIN_VALUE, m_strDomain );

	res = regWriteRegString( REG_HKLM, stationkey,
		APPLICA_STATION_LAST_LOGIN_VALUE, m_strLogin );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
