// LinePropertyPage.h : header file
//

#ifndef __LINEPROPERTYPAGE_POSITION_H__
#define __LINEPROPERTYPAGE_POSITION_H__

/////////////////////////////////////////////////////////////////////////////

class CLinePropertySheet;
class CLinePropertyPagePosition;

/////////////////////////////////////////////////////////////////////////////

#include "WindowPositionControl.h"

/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPagePosition dialog

class CLinePropertyPagePosition : public CPropertyPage
{
	DECLARE_DYNCREATE(CLinePropertyPagePosition)

// Construction
public:
	void EnableAbsolutePos(BOOL bEnable);
	CLinePropertyPagePosition();
	~CLinePropertyPagePosition();

// Dialog Data
	//{{AFX_DATA(CLinePropertyPagePosition)
	enum { IDD = IDD_PROPPAGE_POSITION };
	CComboBox	m_Pos;
	CComboBox	m_wndResolutionCombo;
	CWindowPositionControl	m_wndMonitorFrame;
	CEdit	m_wndTop;
	CEdit	m_wndRight;
	CEdit	m_wndLeft;
	CEdit	m_wndBottom;
	int		m_bottom;
	int		m_left;
	int		m_right;
	int		m_top;
	//}}AFX_DATA

	void OnPositionChange();

	BOOL	m_bDragging;
	CPoint	m_ptMouseDragBegin;
	CRect	m_rWindowDragBegin2;
	int		m_Position;

	CLinePropertySheet*	m_psheet;

	void UpdateResolutionsControls();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLinePropertyPagePosition)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLinePropertyPagePosition)
	afx_msg void OnChangeRectBottom();
	afx_msg void OnChangeRectLeft();
	afx_msg void OnChangeRectRight();
	afx_msg void OnChangeRectTop();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeResolutionCombo();
	afx_msg void OnPreview();
	afx_msg void OnStopPreview();
	afx_msg void OnSelchangePos();
	afx_msg void OnCustomResolutions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif // __LINEPROPERTYPAGE_POSITION_H__
