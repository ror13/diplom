#if !defined(AFX_IDENTIFYWND_H__67C39796_9FCB_4037_89E4_26013FF15DB0__INCLUDED_)
#define AFX_IDENTIFYWND_H__67C39796_9FCB_4037_89E4_26013FF15DB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IdentifyWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIdentifyWnd window

class CIdentifyWnd : public CWnd
{
// Construction
public:
	CIdentifyWnd(int Num);

// Attributes
public:
	int m_Num;

// Operations
public:
	void OnDraw(class CDC *);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdentifyWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIdentifyWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIdentifyWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDENTIFYWND_H__67C39796_9FCB_4037_89E4_26013FF15DB0__INCLUDED_)
