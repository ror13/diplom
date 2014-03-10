// LinePropertyPage.h : header file
//

#ifndef __LINEPROPERTYPAGE_WINDOW_H__
#define __LINEPROPERTYPAGE_WINDOW_H__

/////////////////////////////////////////////////////////////////////////////

#include "..\Libs\ColorBox.h"
class CLinePropertySheet;

/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPageWindow dialog

class CLinePropertyPageWindow : public CPropertyPage
{
	DECLARE_DYNCREATE(CLinePropertyPageWindow)

// Construction
public:
	CLinePropertyPageWindow();
	~CLinePropertyPageWindow();

// Dialog Data
	//{{AFX_DATA(CLinePropertyPageWindow)
	enum { IDD = IDD_PROPPAGE_WINDOW };
	CEdit	m_wndTransparentText;
	CEdit	m_wndTransparentFactor;
	CSliderCtrl	m_slider;
	CColorBox	m_btnTransparentColor;
	CColorBox	m_btnBackground;
	BYTE	m_TransparentFactor;
	BOOL	m_bResizingWindow;
	BOOL	m_bTransparent;
	//}}AFX_DATA

	CLinePropertySheet*	m_psheet;

	void OnSliderChange();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLinePropertyPageWindow)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFont	m_font;
	CBrush	m_brush;

	// Generated message map functions
	//{{AFX_MSG(CLinePropertyPageWindow)
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg void OnStopPreview();
	afx_msg void OnChangeTransparentFactor();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTransparentWindow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif // __LINEPROPERTYPAGE_WINDOW_H__
