#if !defined(AFX_COMBOBOX1_H__1D059AC2_BB00_4AE4_8493_4E2BB3E3D0E6__INCLUDED_)
#define AFX_COMBOBOX1_H__1D059AC2_BB00_4AE4_8493_4E2BB3E3D0E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// combobox1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboBox1 window

class CComboBox1 : public CComboBox
{
// Construction
public:
	CComboBox1();

// Attributes
public:

// Operations
public:
	int AddString(LPCTSTR lpszString, unsigned long Data);
	int GetCurSelData();
	int SetCurSelData(unsigned long Data);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBox1)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBox1();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBox1)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOBOX1_H__1D059AC2_BB00_4AE4_8493_4E2BB3E3D0E6__INCLUDED_)
