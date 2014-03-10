// LinePropertyPage.h : header file
//

#ifndef __LINEPROPERTYPAGE_GENERAL_H__
#define __LINEPROPERTYPAGE_GENERAL_H__

/////////////////////////////////////////////////////////////////////////////

class CLinePropertySheet;

/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPageGeneral dialog

class CLinePropertyPageGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CLinePropertyPageGeneral)

// Construction
public:
	CLinePropertyPageGeneral();
	~CLinePropertyPageGeneral();

// Dialog Data
	//{{AFX_DATA(CLinePropertyPageGeneral)
	enum { IDD = IDD_PROPPAGE_GENERAL };
	CComboBox	m_wndDirection;
	CEdit	m_wndSpeedEdit;
	CStatic	m_wndSpeedDescription;
	CSliderCtrl	m_wndSpeed;
	CString	m_strText;
	BOOL	m_UseRSS;
	CString	m_TextStatic;
	int		m_iTextLayout;
	//}}AFX_DATA

	CLinePropertySheet*	m_psheet;

	double		m_ScrollingSpeed;	// pixels per second

	CString m_tempText;
	CString m_tempURL;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLinePropertyPageGeneral)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void OnChangeSpeedScrolling();
	void CalculatePosition( double Speed, int* pos,
		int* start, int* end, double* Decode );
	CString CalculateDescription( double Speed );
	CString CalculateSpeedEdit( double Speed );

	// Generated message map functions
	//{{AFX_MSG(CLinePropertyPageGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg void OnStopPreview();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeSpeedEdit();
	afx_msg void OnUseRss();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif // __LINEPROPERTYPAGE_GENERAL_H__
