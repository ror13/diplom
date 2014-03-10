// CreepingLineDoc.h : interface of the CCreepingLineDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CREEPINGLINEDOC_H__B284D606_01C3_4BD0_84C8_CABCABD8E47D__INCLUDED_)
#define AFX_CREEPINGLINEDOC_H__B284D606_01C3_4BD0_84C8_CABCABD8E47D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CreepingLineInfo.h"
#include "CreepingLineWnd.h"

class CCreepingLineDoc : public CDocument
{
protected: // create from serialization only
	CCreepingLineDoc();
	DECLARE_DYNCREATE(CCreepingLineDoc)

// Attributes
public:

	CCreepingLineInfo	m_line;
	CCreepingLineInfo	m_line_orig;

	CRect				m_rTextSize;

	CCreepingLineViewer	m_CrippingLinePreview;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreepingLineDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CCreepingLineDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void OnLineOptions()
	{
		OnOptions();
	}

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCreepingLineDoc)
	afx_msg void OnOptions();
	afx_msg void OnRunCreepingLine();
	afx_msg void OnStopCreepingLine();
	afx_msg void OnUpdateRunCreepingLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStopCreepingLine(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREEPINGLINEDOC_H__B284D606_01C3_4BD0_84C8_CABCABD8E47D__INCLUDED_)
