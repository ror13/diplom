#if !defined(AFX_WINDOWPOSITIONCONTROL_H__7140C75A_C0CE_4385_86C9_E999EF71B474__INCLUDED_)
#define AFX_WINDOWPOSITIONCONTROL_H__7140C75A_C0CE_4385_86C9_E999EF71B474__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowPositionControl.h : header file
//

#include "MyTracker.h"

// Messages:
#define WM_WINDOW_POSITION_UPDATE_DATA	(WM_APP+1)

// Parent notifications:
#define WN_WINDOW_POSITION_CHANGED		(WM_USER+1)

/////////////////////////////////////////////////////////////////////////////
// CWindowPositionControl window

//typedef CMyRectTracker CCustomRectTracker;
typedef CRectTracker CCustomRectTracker;

class CWindowPositionControl : public CStatic, public CCustomRectTracker
{
// Construction
public:
	CWindowPositionControl();

// Attributes
public:
	CRect		m_rectWindow;
	CSize		m_sizeScreen;

	CString		m_strText;
	LOGFONT		m_font;
	COLORREF	m_clTextColor;
	COLORREF	m_clBackground;

	// Internal:
	
	COLORREF	m_clScreenBackground;
	COLORREF	m_clScreenResolutionLines;
	COLORREF	m_clScreenResolutionText;

	int			m_iScreenMinimumSize;

	void Update();

	void ClientToVirtual( CPoint& pt );
	void VirtualToClient( CPoint& pt );

	void ClientToVirtual( CRect& r )
	{
		CPoint TopLeft = r.TopLeft();
		CPoint BottomRight = r.BottomRight();
		ClientToVirtual( TopLeft );
		ClientToVirtual( BottomRight );
		r = CRect( TopLeft, BottomRight );
	}

	void VirtualToClient( CRect& r )
	{
		CPoint TopLeft = r.TopLeft();
		CPoint BottomRight = r.BottomRight();
		VirtualToClient( TopLeft );
		VirtualToClient( BottomRight );
		r = CRect( TopLeft, BottomRight );
	}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowPositionControl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo,
		CDC* pDC, CWnd* pWnd);
	virtual void OnChangedRect(const CRect& rectOld);
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
	virtual UINT GetHandleMask() const;

// Implementation
public:
	virtual ~CWindowPositionControl();

	// Generated message map functions
protected:

public:
	enum ALIGN_TYPE
	{
		ALIGN_VERT_LINE,
		ALIGN_HORZ_LINE
	};

	enum
	{
		MIN_RESOLUTION = 0,
		MAX_RESOLUTION = 10000,
	};

protected:

	struct ALIGN_INFO
	{
		ALIGN_TYPE	align;
		int			position;
		int			delta;

		BOOL		bDraw;
		int			from;
		int			to;

		ALIGN_INFO()
		{
			align = ALIGN_VERT_LINE;
			position = 0;
			delta = 0;
			bDraw = TRUE;
			from = MIN_RESOLUTION;
			to = MAX_RESOLUTION;
		}

		ALIGN_INFO( ALIGN_TYPE align2, int position2, int delta2,
			int from2 = MIN_RESOLUTION, int to2 = MIN_RESOLUTION,
			BOOL bDraw2 = TRUE )
		{
			align = align2;
			position = position2;
			delta = delta2;
			from = from2;
			to = to2;
			bDraw = bDraw2;
		}
	};

	CArray<ALIGN_INFO, ALIGN_INFO&>	m_arrAligns;

	enum
	{
		ADJUST_MOVE		=  1,
		ADJUST_LEFT		=  2,
		ADJUST_RIGHT	=  4,
		ADJUST_TOP		=  8,
		ADJUST_BOTTOM	= 16,
	};

	static int Align( int pos, int net, int delta ); // returns 0 if too far else returns distance to the net
	BOOL Align( CRect& rect, int adjust, const ALIGN_INFO& info, BOOL& bAlignedAny );

	int		m_nTrackingHandle;

	void AdjustChangedRect(const CRect& rectOld);

	//{{AFX_MSG(CWindowPositionControl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWPOSITIONCONTROL_H__7140C75A_C0CE_4385_86C9_E999EF71B474__INCLUDED_)
