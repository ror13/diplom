// CreepingLineView.h : interface of the CCreepingLineView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CREEPINGLINEVIEW_H__F2BAEF64_280E_456B_8209_C21C84D34BBE__INCLUDED_)
#define AFX_CREEPINGLINEVIEW_H__F2BAEF64_280E_456B_8209_C21C84D34BBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCreepingLineView : public CView
{
protected: // create from serialization only
	CCreepingLineView();
	DECLARE_DYNCREATE(CCreepingLineView)

// Attributes
public:
	CCreepingLineDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreepingLineView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCreepingLineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCreepingLineView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CreepingLineView.cpp
inline CCreepingLineDoc* CCreepingLineView::GetDocument()
   { return (CCreepingLineDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREEPINGLINEVIEW_H__F2BAEF64_280E_456B_8209_C21C84D34BBE__INCLUDED_)
