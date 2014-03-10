#if !defined(AFX_XYSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_)
#define AFX_XYSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// xySplitterWnd.h : header file
//
#include <afxtempl.h>

#define WM_SPLITTER_MOVED   (WM_USER+1000)

////////////////////////////////////////////////////////////////////////////
// CxSplitterWnd window
struct ControlInfo
{
	ControlInfo(CWnd* _pWnd = NULL) :
		pWnd(_pWnd),
		nMargin(0),
		bResize(TRUE)
	{
		if (pWnd)
			nID = pWnd->GetDlgCtrlID();
		else
			nID = NULL;
	}
	int		nID;
	CWnd*	pWnd;
	int		nMargin;
	bool	bResize;
};
typedef CArray<ControlInfo, ControlInfo&> _TypeControlInfoArray;

class  CxSplitterWnd : public CWnd
{
// Construction
public:
	CxSplitterWnd();

// Attributes
public:

protected:
    CWnd        *m_pParent;

	_TypeControlInfoArray m_leftCtrls, m_rightCtrls;

    int         m_minLeft, m_minRight;

	CDWordArray m_dwaRelationID; 

// Operations
public:
	bool BindWithControl(CWnd *parent, DWORD ctrlId);
	bool CreateInWnd(CWnd *parent, DWORD ctrlId, LPRECT rc);
	void Unbind(void);

	void SetMinWidth(int left, int right);

	bool AttachAsLeftCtrl(int ctrlid, 
						  bool bResize = true);
	bool AttachAsLeftCtrl(CWnd *pWnd, 
						   bool bResize = true);
	bool AttachAsLeftCtrl(int ctrlid, 
						   int nMargin, 
						   bool bResize = true);
	bool AttachAsLeftCtrl(CWnd *pWnd, 
						  int nMargin, 
							bool bResize = true);
	bool AttachAsRightCtrl(int ctrlid, 
						  bool bResize = true);
	bool AttachAsRightCtrl(CWnd *pWnd, 
						   bool bResize = true);
	bool AttachAsRightCtrl(int ctrlid, 
						   int nMargin, 
						   bool bResize = true);
	bool AttachAsRightCtrl(CWnd *pWnd, 
						  int nMargin, 
							bool bResize = true);

	bool DetachAllCtrls(void);
	void RecalcLayout(void);

	bool AttachToParent(DWORD ctrlId,DWORD ctrlParentID); 

protected:
	bool GetMouseClipRect(LPRECT rectClip, CPoint pointInSplitter);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSplitterWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CxSplitterWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CxSplitterWnd)
	afx_msg bool OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CySplitterWnd window

class  CySplitterWnd : public CWnd
{
// Construction
public:
	CySplitterWnd();

// Attributes
public:

protected:
    CWnd        *m_pParent;

	_TypeControlInfoArray m_aboveCtrls, m_belowCtrls;
    int         m_minAbove, m_minBelow;

	CDWordArray m_dwaRelationID; 

// Operations
public:
	bool BindWithControl(CWnd *parent, DWORD ctrlId);
	bool CreateInWnd(CWnd *parent, DWORD ctrlId, LPRECT rc);
	void Unbind(void);

	void SetMinHeight(int above, int below);

	bool AttachAsAboveCtrl(int ctrlid, 
						  bool bResize = true);
	bool AttachAsAboveCtrl(CWnd *pWnd, 
						   bool bResize = true);
	bool AttachAsAboveCtrl(int ctrlid, 
						   int nMargin, 
						   bool bResize = true);
	bool AttachAsAboveCtrl(CWnd *pWnd, 
						  int nMargin, 
							bool bResize = true);
	bool AttachAsBelowCtrl(int ctrlid, 
						  bool bResize = true);
	bool AttachAsBelowCtrl(CWnd *pWnd, 
						   bool bResize = true);
	bool AttachAsBelowCtrl(int ctrlid, 
						   int nMargin, 
						   bool bResize = true);
	bool AttachAsBelowCtrl(CWnd *pWnd, 
						  int nMargin, 
							bool bResize = true);

	bool DetachAllCtrls(void);
	void RecalcLayout(void);

	bool AttachToParent(DWORD ctrlId,DWORD ctrlParentID); 

protected:
	bool GetMouseClipRect(LPRECT rectClip, CPoint pointInSplitter);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CySplitterWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CySplitterWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CySplitterWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg bool OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}

// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_XYSPLITTERWND_H__8E3E5264_02A9_11D2_BF99_000021000B7C__INCLUDED_)
