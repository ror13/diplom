#if !defined(AFX_CUSTOMRESOLUTIONDIALOG_H__B19C13E7_70CE_44AA_8434_D5592E7B3C18__INCLUDED_)
#define AFX_CUSTOMRESOLUTIONDIALOG_H__B19C13E7_70CE_44AA_8434_D5592E7B3C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomResolutionDialog.h : header file
//

#include "../CustomResolution/resource.h"

#include "MonCustomRes.h"
#include "CustomResolutionDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CEditCustomResolutionDialog dialog

int CUSTOMRESOLUTION_API CustomResolutionsDialogDoModal();

class CEditCustomResolutionDialog : public CDialog
{
// Construction
public:
	CEditCustomResolutionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditCustomResolutionDialog)
	enum { IDD = IDD_EDIT_CUSTOM_RESOLUTION };
	DWORD	m_dwHeight;
	DWORD	m_dwWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCustomResolutionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditCustomResolutionDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCustomResolutionsDialog dialog

class CCustomResolutionsDialog : public CDialog
{
// Construction
public:
	CCustomResolutionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomResolutionsDialog)
	enum { IDD = IDD_CUSTOM_RESOLUTIONS };
	CButton	m_wndBtCrDel;
	CButton	m_wndBtCrEdit;
	CListBox	m_wndCustomResolutionsList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomResolutionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL UpdateControls();

	CCustomResolutionList m_CustomRes;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomResolutionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEdit();
	virtual void OnOK();
	afx_msg void OnSelchangeCustomResolutionsList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMRESOLUTIONDIALOG_H__B19C13E7_70CE_44AA_8434_D5592E7B3C18__INCLUDED_)
