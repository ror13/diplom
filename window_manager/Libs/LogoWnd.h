#if !defined(AFX_LOGOWND_H__6A561396_4879_48DF_A0C9_A100AD893050__INCLUDED_)
#define AFX_LOGOWND_H__6A561396_4879_48DF_A0C9_A100AD893050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogoWnd.h : header file
//

#define DEFAULT_LOGO_WND_NAME	_T("PresenterLogoWindow")

/////////////////////////////////////////////////////////////////////////////
// CLogoWnd window

class CLogoWnd : public CWnd
{
// Construction
public:
	CLogoWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogoWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogoWnd();

	BOOL EnableLogo(BOOL bEnable);
	// Generated message map functions
protected:
	CBitmap m_Logo;
	BOOL m_bLogoEnabled;

	//{{AFX_MSG(CLogoWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGOWND_H__6A561396_4879_48DF_A0C9_A100AD893050__INCLUDED_)
