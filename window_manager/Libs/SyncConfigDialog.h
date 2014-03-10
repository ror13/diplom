#if !defined(AFX_SYNCCONFIGDIALOG_H__9D4EBC95_6172_49A8_917F_63328EC9D7AE__INCLUDED_)
#define AFX_SYNCCONFIGDIALOG_H__9D4EBC95_6172_49A8_917F_63328EC9D7AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyncConfigDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSyncConfigDialog dialog

class CSyncConfigDialog : public CDialog
{
// Construction
public:
	CSyncConfigDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSyncConfigDialog)
	enum { IDD = IDD_SYNC_CONFIG_DIALOG };
	int		m_iSyncMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncConfigDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSyncConfigDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCCONFIGDIALOG_H__9D4EBC95_6172_49A8_917F_63328EC9D7AE__INCLUDED_)
