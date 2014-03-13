// CreepingLine.h : main header file for the CREEPINGLINE application
//

#if !defined(AFX_CREEPINGLINE_H__DE67A373_64E7_4AB6_A159_69B4535A5FC5__INCLUDED_)
#define AFX_CREEPINGLINE_H__DE67A373_64E7_4AB6_A159_69B4535A5FC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "CreepingLineWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineApp:
// See CreepingLine.cpp for the implementation of this class
//

class CCreepingLineApp : public CWinApp
{
public:
	CCreepingLineApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreepingLineApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	BOOL				m_bRunCreepingLine;
	//CCreepingLineViewer	m_CrippingLineViewer;

// Implementation
	//{{AFX_MSG(CCreepingLineApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREEPINGLINE_H__DE67A373_64E7_4AB6_A159_69B4535A5FC5__INCLUDED_)
