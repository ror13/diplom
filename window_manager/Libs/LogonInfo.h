#if !defined(AFX_LOGONINFO_H__80490F98_055B_4DC7_9813_A0A571405C22__INCLUDED_)
#define AFX_LOGONINFO_H__80490F98_055B_4DC7_9813_A0A571405C22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogonInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////

CString	GetMessagesAgentExeName( int iStationXP );
DWORD	GetDisableServiceState( VOID );
BOOL	SetDisableServiceState( DWORD dwDisableService );

/////////////////////////////////////////////////////////////////////////////
// CLogonInfo window

class CLogonInfo
{
public:
	CLogonInfo();
	virtual ~CLogonInfo();

	BOOL	m_bAutoLogon;
	CString	m_strDomain;
	CString	m_strLogin;
	CString	m_strPassword;
	CString	m_strInitialCommand;

	BOOL Load( int iStationXP );
	BOOL Save( int iStationXP ) const;
};

/////////////////////////////////////////////////////////////////////////////

class CLastLogonInfo
{
public:
	CLastLogonInfo();
	virtual ~CLastLogonInfo();

	CString	m_strDomain;
	CString	m_strLogin;

	BOOL Load( int iStationXP );
	BOOL Save( int iStationXP ) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGONINFO_H__80490F98_055B_4DC7_9813_A0A571405C22__INCLUDED_)
