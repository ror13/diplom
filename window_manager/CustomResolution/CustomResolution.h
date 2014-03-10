// CustomResolution.h : main header file for the CUSTOMRESOLUTION DLL
//

#if !defined(AFX_CUSTOMRESOLUTION_H__0B2D3D75_EDF5_4936_8CE7_3D370E09142B__INCLUDED_)
#define AFX_CUSTOMRESOLUTION_H__0B2D3D75_EDF5_4936_8CE7_3D370E09142B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCustomResolutionApp
// See CustomResolution.cpp for the implementation of this class
//

class CCustomResolutionApp : public CWinApp
{
public:
	CCustomResolutionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomResolutionApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCustomResolutionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMRESOLUTION_H__0B2D3D75_EDF5_4936_8CE7_3D370E09142B__INCLUDED_)
