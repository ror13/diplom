/////////////////////////////////////////////////////////////
// Tip.h : header file                                 //
//                                                         //
// This class was created by Victor Shunkin 25.04.2002     //
//                                                         //
// Provide simple tooltip functional                       //
/////////////////////////////////////////////////////////////
#if !defined(AFX_TIP_H__A620369D_5020_4CE9_941E_0E4D3E506A84__INCLUDED_)
#define AFX_TIP_H__A620369D_5020_4CE9_941E_0E4D3E506A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CTip window

class CTip : public CWnd
{
// Construction
public:
	CTip();

// Attributes
protected:
	CWnd  *m_pParentWnd;
	CRect  m_rectTitle;
    BOOL   m_bCreated;
	

// Operations
public:
	void Hide();
	void Show(CPoint point, LPCTSTR lpszTitleText);
	void SetParentWnd(CWnd* pParentWnd)  { m_pParentWnd = pParentWnd; }

	int	m_Count;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTip)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTip();
	virtual BOOL Create( CWnd *pParentWnd);

	CSize GetTextSize(LPCTSTR lpszTitleText, CDC *pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CTip)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIP_H__A620369D_5020_4CE9_941E_0E4D3E506A84__INCLUDED_)
