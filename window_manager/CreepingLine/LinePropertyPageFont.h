// LinePropertyPage.h : header file
//

#ifndef __LINEPROPERTYPAGE_FONT_H__
#define __LINEPROPERTYPAGE_FONT_H__

/////////////////////////////////////////////////////////////////////////////

class CLinePropertySheet;

/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPageFont dialog

class CLinePropertyPageFont : public CPropertyPage
{
	DECLARE_DYNCREATE(CLinePropertyPageFont)

// Construction
public:
	CLinePropertyPageFont();
	~CLinePropertyPageFont();

// Dialog Data
	//{{AFX_DATA(CLinePropertyPageFont)
	enum { IDD = IDD_PROPPAGE_FONT };
	CStatic	m_wndFontExampleFrame;
	CString	m_strFontDesc;
	//}}AFX_DATA

//	CString		m_strExampleText;
	LOGFONT		m_font;
	COLORREF	m_clTextColor;
	LONG		m_clTextSize;		// in pt

	void UpdateFontDescription();

	CLinePropertySheet*	m_psheet;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLinePropertyPageFont)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLinePropertyPageFont)
	afx_msg void OnChooseFont();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg void OnStopPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



#endif // __LINEPROPERTYPAGE_FONT_H__
