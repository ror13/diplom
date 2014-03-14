#if !defined(AFX_CREEPING_LINE_WND_H__1F0A3176_3465_4037_9874_49ab64298F4C__INCLUDED_)
#define AFX_CREEPING_LINE_WND_H__1F0A3176_3465_4037_9874_49ab64298F4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreepingLineWnd.h : header file
//

#include "CreepingLineInfo.h"
#include "Thread.h"

//////////////////////////////////////////////////////////////////////////
//
/*
#define COUNT_BITMAPS	2

class CCreepingLineWnd : public CWnd, public CThreadClient
{
public:
	// Construction
	CCreepingLineWnd();
	virtual ~CCreepingLineWnd();

	// Operations
	void SetAllView();
	void CreateCreepingLine(CCreepingLineInfo LineInfo, BOOL bMakeTopMost = FALSE, CRect AllViewRect = CRect(0,0,0,0));

// Implementation
protected:
	// Attributes
	CCreepingLineInfo	m_line;

	CString			m_strText;
	CString			m_strRSSImageLogo;
	
	double			m_HorizontalPos;
	CFont			m_font;
	
	BOOL			m_bDragging;
	CPoint			m_ptMouseDragBegin;
	CRect			m_rWindowDragBegin;
	
	CRect			m_rTextSize;
	
	BOOL			m_bMakeTopMost;
	
	// for NEW_BITMAP_DRAW:
	CSize			m_sizeClient;
	CSize			m_sizeBitmap;
	
	CDC				m_CompatibleDc;
	CBitmap			m_Bitmap;
	
	CRect			m_AllViewRect;

	HANDLE			m_hWaitableTimer;
	CThread			m_Thread;
//	volatile BOOL	m_bClosing;
	LARGE_INTEGER	m_liFreq;
	LARGE_INTEGER	m_liLast;
	int				m_iWorkedMs;
	double			m_iDelta;

	CBitmapBuf		m_Bufs[COUNT_BITMAPS];
	int				m_CurUsedBufNum;
	int				m_CountDrawedSymbols;
	int				m_SymbolOverWidth;
	int GetNextBufNum();

	// CThreadClient
	virtual UINT OnThread(int ThreadNum);

	void FreeBitmaps();
	
	BOOL Initialize();

	void Draw();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreepingLineWnd)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:

	//{{AFX_MSG(CCreepingLineWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnCloseCreepingLine();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
// CCreepingLineMainThread

class CCreepingLineMainThread : public CWinThread
{
public:
	DECLARE_DYNCREATE(CCreepingLineMainThread);

	void SetData(CCreepingLineInfo LineInfo, BOOL bTopMost, CRect AllViewRect);
	void CloseWindows();

protected:
	CCreepingLineMainThread();

	virtual BOOL InitInstance();

	CCreepingLineWnd	m_CreepingLineWnd;
	
	CCreepingLineInfo	m_LineInfo;
	BOOL				m_bMakeTopMost;
	CRect				m_AllViewRect;
};
*/
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CCreepingLineViewer

class CCreepingLineViewer
{
public:
	CCreepingLineViewer();
	~CCreepingLineViewer();

	BOOL IsPlay();

	BOOL BeginCreepingLine(CCreepingLineInfo Line, BOOL bMakeTopMost = FALSE,
		CRect AllViewRect = CRect(0,0,0,0));

	BOOL WaitForCreepingLineEnd(DWORD TimeOut = INFINITE);

	BOOL StopCreepingLine();

protected:
	static DWORD WINAPI ping_server( LPVOID lpParam );
	static DWORD WINAPI command_server( LPVOID lpParam );
	static DWORD WINAPI thr_command( LPVOID lpParam );
	//CCreepingLineMainThread	*m_pCreepingLineMainThread;
private:
	static HANDLE  m_Mutex;
	static BOOL	m_PlayState;
	static CString m_LineInfo;
	HANDLE  m_hPingServer, m_hCommandServer; 
    DWORD   m_dwPingServer, m_dwCommandServer;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREEPING_LINE_WND_H__1F0A3176_3465_4037_9874_49ab64298F4C__INCLUDED_)
