// CreepingLineWnd.cpp : implementation file
//

#include "stdafx.h"
#include "winsock2.h"
#include "CreepingLineWnd.h"
#include "../Libs/MakeWindowTransparent.h"
//#include "../Libs/Gdi.h"
#include "resource.h"
#include <process.h>
#include "LinePropertySheet.h"
#include "rss.h"

#include "Translate.h"

#include "BitmapEx.h"

#include "CImg.h" 
#include "WebGrab.h"

#include "FileOperation.h"

#include "NetCommanProtocol.h"

using namespace cimg_library; 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define MAKE_TOP_MOST			1000

/////////////////////////////////////////////////////////////////////////////

#define WM_APP_REDRAW		(WM_APP + 0)

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CCreepingLineWnd
/*
CCreepingLineWnd::CCreepingLineWnd()
{
	m_bDragging = FALSE;
	m_HorizontalPos = 0.0;
	m_rTextSize = CRect(0,0,0,0);
	m_bMakeTopMost = FALSE;
	m_hWaitableTimer = NULL;
	m_liLast.QuadPart = 0;
	m_liFreq.QuadPart = 0;
	m_iWorkedMs = 0;

	m_sizeClient = CSize(0,0);
	m_sizeBitmap = CSize(0,0);

	m_AllViewRect.SetRectEmpty();

	m_CurUsedBufNum = -1;
}

CCreepingLineWnd::~CCreepingLineWnd()
{
}

BEGIN_MESSAGE_MAP(CCreepingLineWnd, CWnd)
	//{{AFX_MSG_MAP(CCreepingLineWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_COMMAND(ID_CLOSE_CREEPING_LINE, OnCloseCreepingLine)
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_OPTIONS, OnOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::CreateCreepingLine(CCreepingLineInfo LineInfo, BOOL bMakeTopMost, CRect AllViewRect)
{
	m_line = LineInfo;
	m_bMakeTopMost = bMakeTopMost;
	m_AllViewRect = AllViewRect;
	
	if(m_line.m_bUseRSS)
	{
		m_strText = GetRSStext(m_line.m_strText, &m_strRSSImageLogo);

		if (!m_strRSSImageLogo.IsEmpty())
		{

			CWebGrab WebGrab;
			CString strRSSImageLogoData;

			if(WebGrab.GetFile(m_strRSSImageLogo, strRSSImageLogoData))
			{
// 				CImg<BYTE> img;

				TCHAR* tempfilename = _ttempnam(_tgetenv(_T("TEMP")), _T("rsslogo"));
// 				TCHAR* tempfilename = _ttmpnam(NULL);

				CString strTempFileName;

				strTempFileName.Format(_T("%s.%s"), tempfilename, GetFileExtension(m_strRSSImageLogo));

// 				TRACE(_T("%d %d %d\n"));

				SaveBinFileFromBuf(strTempFileName, (PBYTE)strRSSImageLogoData.GetBuffer(0), strRSSImageLogoData.GetLength());

// 				USES_CONVERSION;
// 				img.load(T2A(strTempFileName));
// 
// 				img.save(("e:/123"));

				DeleteFile(strTempFileName);
			}
		}
	} else
		m_strText = m_line.m_strText;

	TEXTMETRIC TextMetrca;
	{
		CClientDC DC(NULL);
		CFont Font;
		Font.CreateFontIndirect(&m_line.m_font);
		DC.SelectObject(&Font);
		DC.GetTextMetrics(&TextMetrca);
	}
	int font_height = TextMetrca.tmHeight + TextMetrca.tmDescent;

	if(m_AllViewRect.IsRectEmpty())
		::GetWindowRect(::GetDesktopWindow(), &m_AllViewRect);

	if(m_line.m_Position == CCreepingLineInfo::DOCKING_POS_TOP)
	{
		m_line.m_Rect = m_AllViewRect;
		m_line.m_Rect.bottom = m_line.m_Rect.top + font_height;
	} else if(m_line.m_Position == CCreepingLineInfo::DOCKING_POS_BOTTOM)
	{
		m_line.m_Rect = m_AllViewRect;
		m_line.m_Rect.top = m_line.m_Rect.bottom - font_height;
	} else
	{
		m_line.m_Rect.OffsetRect(m_AllViewRect.TopLeft());
	}

	LPCTSTR szClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);
	
	CString strCaption = _TRANS(_T("Line: ")) + m_line.m_strText;

	DWORD dwExtStyle = WS_EX_TOPMOST;

	switch (m_line.m_iTextLayout)
	{
	case CREEPING_LINE_TEXT_LAYOUT_LEFT_TO_RIGHT:
		break;
	case CREEPING_LINE_TEXT_LAYOUT_RIGHT_TO_LEFT:
		dwExtStyle |= WS_EX_LAYOUTRTL;
		break;
	}

	DWORD dwStyle = WS_VISIBLE | WS_POPUP;
	if( m_line.m_bResizing )
		dwStyle |= WS_THICKFRAME;

	BOOL res = CreateEx(
		dwExtStyle, szClass, strCaption, dwStyle,
		m_line.m_Rect.left, m_line.m_Rect.top,
		m_line.m_Rect.Width(), m_line.m_Rect.Height(),
		NULL, NULL, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineWnd message handlers

//CCreepingLineWnd*	g_pCreepingLineWnd = NULL;

int CCreepingLineWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!Initialize())
		return -1;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCreepingLineWnd::Initialize()
{
	m_bDragging = FALSE;
	m_rTextSize = CRect(0,0,0,0);
	m_sizeClient = CSize(0,0);

	if(m_line.m_bTransparent)
	{
		BOOL bTransparentOK =
			MakeWindowTransparent( this->GetSafeHwnd(),
				m_line.m_TransparencyFactor,
				m_line.m_clTransparentColor );
		ASSERT( bTransparentOK );
	} else
		UnMakeWindowTransparent( this->GetSafeHwnd() );

	m_font.DeleteObject();
	m_font.CreateFontIndirect(&m_line.m_font);

	CRect r;
	GetClientRect( &r );
	m_HorizontalPos = 0.0;
	m_CountDrawedSymbols = 0;

	if(m_hWaitableTimer == NULL)
	{
		m_hWaitableTimer = CreateWaitableTimer( NULL, TRUE, NULL );
		if( m_hWaitableTimer == NULL )
		{
			TRACE( _T("CreateWaitableTimer failed! err = %d\n"), GetLastError() );
			return FALSE;	// destroy window
		}
	}

	if(m_Thread.IsStopped())
		m_Thread.Create(this, 0, THREAD_PRIORITY_TIME_CRITICAL);

#ifdef MAKE_TOP_MOST
#define TIMER_TOPMOST	3

	TRACE( _T("CCreepingLineWnd::m_bMakeTopMost = %d\n"), m_bMakeTopMost );
	if( m_bMakeTopMost )
	{
		SetTimer( TIMER_TOPMOST, MAKE_TOP_MOST, NULL );
		OnTimer(TIMER_TOPMOST);
	}
#endif

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnDestroy() 
{
	CWnd::OnDestroy();

	m_Thread.WaitStopThread();

	if(m_hWaitableTimer != NULL)
	{
		CloseHandle( m_hWaitableTimer );
		m_hWaitableTimer = NULL;
	}

	FreeBitmaps();
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnCloseCreepingLine() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////

UINT CCreepingLineWnd::OnThread(int ThreadNum)
{
	const int max_fps = 50;
	double Speed = m_line.m_ScrollingSpeed; 	// pixels / s

	double fps = min(max_fps, Speed);

	if(fps == 0)
	{
		ASSERT( FALSE );
		fps = 1;
	}

	double iTimerTimeOut = 1000.0 / fps;		// ms
	int iSendTimeOut = 300;

	m_iDelta = Speed / fps;

	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart= LONGLONG(-10000.0*iTimerTimeOut);

	TRACE( _T("UpdateThread: fps = %.2f; timeout = %.1f, delta = %.2f\n"),
		(double)fps, (double)iTimerTimeOut, (double)m_iDelta );

	BOOL isSet = SetWaitableTimer( m_hWaitableTimer, &liDueTime, 0, NULL, NULL, FALSE );
	if(isSet == FALSE)
		return 0;

	while(!m_Thread.IsStopped())
	{
		WaitForSingleObject(m_hWaitableTimer, INFINITE);
		
		SetWaitableTimer( m_hWaitableTimer, &liDueTime, 0, NULL, NULL, FALSE );

		DWORD_PTR result = 0;
		LRESULT sendRes = ::SendMessageTimeout( *this, WM_APP_REDRAW, NULL, NULL,
			SMTO_NORMAL, iSendTimeOut, //ms
			&result);

		if(sendRes == 0)
		{
			// error!
			continue;
		}
	}
	CancelWaitableTimer(m_hWaitableTimer);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CCreepingLineWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_APP_REDRAW )
	{
//		TRACE( _T("WM_APP_REDRAW\n") );
		double& delta = m_iDelta;
		if( delta > 0 )
		{
			double OldPos = m_HorizontalPos;
			m_HorizontalPos -= delta; //m_line.m_Speed;
			
			CRect r;
			GetClientRect( &r );
			
			if( m_HorizontalPos <= r.left - r.right - m_rTextSize.Width())
			{
				m_HorizontalPos = 0.0;
				m_CurUsedBufNum = -1;
				
				InvalidateRect( NULL );
			} else
			{
				//				InvalidateRect( NULL );
				//				Draw();
				//				ValidateRect( NULL );
				int dx = (int)m_HorizontalPos - (int)OldPos;
				ScrollWindow(dx, 0, NULL, NULL);
				UpdateWindow();
			}
		}
		
		return 0;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////

HWND g_hWndEnumOwner = NULL;

/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumChildProcAndMakeTopmost(
	HWND hwnd,
	LPARAM lParam
)
{
	CWnd* pWnd = CWnd::FromHandle( hwnd );
	CWnd* pParent = pWnd->GetParent();
	CString s;
	pWnd->GetWindowText( s );
//	TRACE( _T("EnumChildProcAndMakeTopmost: hwnd = 0x%X, parent = 0x%X, Caption = \"%s\"\n"),
//		hwnd, pParent->GetSafeHwnd(), s );

//	pWnd->SetWindowPos( &CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumThreadWndProcAndMakeTopmost(
	HWND hwnd,
	LPARAM lParam
)
{
	CWnd* pWnd = CWnd::FromHandle( hwnd );
	CWnd* pParent = pWnd->GetParent();
	CString s;
	pWnd->GetWindowText( s );
	TCHAR szClass[200] = _T("");
	GetClassName( pWnd->GetSafeHwnd(), szClass, sizeof(szClass)/sizeof(TCHAR) );

//	TRACE( _T("ThreadWindow: hwnd = 0x%X, parent = 0x%X, class = \"%s\", Caption = \"%s\"\n"),
//		hwnd, pParent->GetSafeHwnd(), szClass, s );

	if(
		pParent == NULL
		&& hwnd != g_hWndEnumOwner
		&& s.IsEmpty()
		&& 0==_tcscmp(szClass, _T("#32768"))
		)
	{
		// Find tracking popup menu and change it's Z-order to be visible!!

		TRACE( _T("SetWindowPos( 0x%X )\n"), hwnd );
		::SetWindowPos( pWnd->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnTimer(UINT nIDEvent) 
{
//	TRACE( _T("OnTimer event: %d\n"), nIDEvent );

	switch( nIDEvent ) 
	{
	#ifdef MAKE_TOP_MOST
	case TIMER_TOPMOST:
		if( m_bMakeTopMost )
		{
			//TRACE( _T("SetWindowPos( &CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );\n") )
			SetWindowPos( &CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );

			TRACE( _T("TIMER_TOPMOST\n") );

			// Find tracking popup menu and change it's Z-order to be visible!!

			g_hWndEnumOwner = *this;
//			EnumChildWindows( *this, EnumChildProcAndMakeTopmost, NULL );
			EnumThreadWindows( GetCurrentThreadId(),
				EnumThreadWndProcAndMakeTopmost, NULL );
			g_hWndEnumOwner = NULL;
		}
		break;
	#endif

	default:
		{
			ASSERT( FALSE );
		}
	}

//	CWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::FreeBitmaps()
{
	m_Bitmap.DeleteObject();
	m_CompatibleDc.DeleteDC();

	m_sizeClient = CSize(0,0);
	m_sizeBitmap = CSize(0,0);
}

/////////////////////////////////////////////////////////////////////////////

int CCreepingLineWnd::GetNextBufNum()
{
	int NewBufNum = m_CurUsedBufNum + 1;
	if(NewBufNum >= COUNT_BITMAPS)
		NewBufNum = 0;
	return NewBufNum;
}

void CCreepingLineWnd::Draw()
{
	BOOL res = FALSE;
	BOOL bNeedCreateNextBuf = FALSE;

	CPaintDC dc(this);
	
	for(int i = 0; i < COUNT_BITMAPS; i++)
		m_Bufs[i].SetData(&dc, &m_font, m_line.m_clBackground, m_line.m_clTextColor);

	CRect rectClient;
	GetClientRect( &rectClient );
	CSize sizeClient( rectClient.Width(), rectClient.Height() );

	if(m_CurUsedBufNum == -1)				// need create first buf
	{
		m_CurUsedBufNum = GetNextBufNum();
		int NextBufNum = GetNextBufNum();

		m_Bufs[m_CurUsedBufNum].CreateNewBuf(0,
			sizeClient.cx * 2, sizeClient.cy,
			sizeClient.cx, sizeClient.cx * 2, 
			m_strText,
			(m_line.m_iTextLayout == CREEPING_LINE_TEXT_LAYOUT_RIGHT_TO_LEFT));
		
		m_Bufs[NextBufNum].CreateNewBuf(-sizeClient.cx,
			sizeClient.cx * 2, sizeClient.cy,
			0, sizeClient.cx * 2, 
			m_strText,
			(m_line.m_iTextLayout == CREEPING_LINE_TEXT_LAYOUT_RIGHT_TO_LEFT));

		m_CountDrawedSymbols = 0;

		m_rTextSize = m_Bufs[m_CurUsedBufNum].GetTxtRect(m_strText);
	} else
	{
		int NextBufNum = GetNextBufNum();
		int ImgOffset = m_Bufs[NextBufNum].m_Offset - (int)m_HorizontalPos;
		
		if(ImgOffset >= 0)					// check condition for use next buffer
		{
			m_CurUsedBufNum = NextBufNum;
			bNeedCreateNextBuf = TRUE;
		}
	}

	int save = dc.SaveDC();

	dc.IntersectClipRect(&dc.m_ps.rcPaint);
//	TRACE(_T("rcPaint: %d %d %d %d\n"), dc.m_ps.rcPaint.left, dc.m_ps.rcPaint.right,
//		dc.m_ps.rcPaint.top, dc.m_ps.rcPaint.bottom);

	int ImgOffset = m_Bufs[m_CurUsedBufNum].m_Offset - (int)m_HorizontalPos;

	int SrcOffset = ImgOffset + dc.m_ps.rcPaint.left;

#if _DEBUG && 0
	CBitmapEx img;

	CString strFilename;

	img.Load(m_Bufs[m_CurUsedBufNum].m_Bitmap);

	strFilename.Format(_T("Img_Draw%.8x.bmp"), m_CurUsedBufNum);

	img.Save(strFilename);
#endif

	BOOL bBlt = dc.BitBlt(dc.m_ps.rcPaint.left, dc.m_ps.rcPaint.top,
		dc.m_ps.rcPaint.right - dc.m_ps.rcPaint.left,
		dc.m_ps.rcPaint.bottom - dc.m_ps.rcPaint.top,
		&m_Bufs[m_CurUsedBufNum].m_CompatibleDc, SrcOffset, 0,
		SRCCOPY);
	dc.RestoreDC(save);

	ASSERT( bBlt );

	if(bNeedCreateNextBuf)
	{
		// create next buffer
		int  NextBufNum = GetNextBufNum();
		
		int OldCharsCount = m_Bufs[NextBufNum].FindCountSymbolsForWidth(
			m_strText.Mid(m_CountDrawedSymbols), sizeClient.cx, FALSE);
		
		int PrevBufTxtWidth = m_Bufs[NextBufNum].GetTxtRect(
			m_strText.Mid(m_CountDrawedSymbols, OldCharsCount) ).Width();
		
		int TxtPos = m_CountDrawedSymbols + OldCharsCount;
		
		CString NewText = TxtPos < m_strText.GetLength() ?
			m_strText.Mid(TxtPos) : _T("");
		
		int NewImgOffset = m_Bufs[m_CurUsedBufNum].m_Offset - PrevBufTxtWidth;
		
		m_Bufs[NextBufNum].CreateNewBuf(NewImgOffset,
			sizeClient.cx * 2, sizeClient.cy,
			0, sizeClient.cx * 2, 
			NewText,
			(m_line.m_iTextLayout == CREEPING_LINE_TEXT_LAYOUT_RIGHT_TO_LEFT));
				
		m_CountDrawedSymbols += OldCharsCount;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnPaint() 
{
//	TRACE( _T("CCreepingLineWnd::OnPaint()\n") );
	Draw();
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL bCtrl	= 0 != ( GetKeyState( VK_CONTROL ) & 0x8000 );
	BOOL bAlt	= 0 != ( GetKeyState( VK_MENU ) & 0x8000 );
	BOOL bShift	= 0 != ( GetKeyState( VK_SHIFT ) & 0x8000 );

	if( nChar == VK_ESCAPE )
	{
		DestroyWindow();
	}
	else if( nChar == _T('E') && bCtrl )
	{
		if( m_bMakeTopMost )
		{
			OnOptions();
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	GetCursorPos( &m_ptMouseDragBegin );
	GetWindowRect( &m_rWindowDragBegin );
	m_bDragging = TRUE;	// Set flag to check for key down in mouse move

	CWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		m_bDragging = FALSE;
		ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		POINT	p1 = m_ptMouseDragBegin;
		RECT	r = m_rWindowDragBegin;
		POINT	p2;
		GetCursorPos( &p2 );
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		MoveWindow( r.left + dx, r.top + dy, r.right - r.left, r.bottom - r.top, TRUE );
	}
	CWnd::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////

HBRUSH CCreepingLineWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO Change any attributes of the DC here
//	pDC->SetTextColor( RGB(255,255,255) );
//	pDC->SetBkColor( m_line.m_clBackground );

	// TODO Return a different brush if the default is not desired
	return hbr;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCreepingLineWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnGetMinMaxInfo(MINMAXINFO FAR* pMinMax) 
{
	const int minSize = 25;
	pMinMax->ptMinTrackSize = CPoint( minSize, minSize );
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCreepingLineWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( HTCLIENT == nHitTest )
	{
		HCURSOR hCursor = LoadCursor( NULL, IDC_ARROW );
		SetCursor( hCursor );
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ClientToScreen( &point );

//	CPoint point(0,0);
//	GetCursorPos( &point );

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_CREEPING_LINE_POPUP_MENU));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	if( !m_bMakeTopMost )
	{
		pPopup->DeleteMenu( ID_OPTIONS, MF_BYCOMMAND );
	}

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);

//	CWnd::OnRButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineWnd::OnOptions() 
{
	// HERE we may show creeping line options dialog.
	ASSERT( m_bMakeTopMost );
	if( m_bMakeTopMost )
	{
		CLinePropertySheet propSheet( this );
		propSheet.m_line = m_line;

		BOOL bMakeTopMost = m_bMakeTopMost;
		m_bMakeTopMost = FALSE;

		int res = propSheet.DoModal();

		m_bMakeTopMost = bMakeTopMost;

		if( res == IDOK )
		{
			m_line = propSheet.m_line;
			Initialize();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CCreepingLineMainThread

IMPLEMENT_DYNCREATE(CCreepingLineMainThread, CWinThread)

CCreepingLineMainThread::CCreepingLineMainThread() : CWinThread()
{
	m_LineInfo.InitDefault();
	m_bMakeTopMost = FALSE;
	m_AllViewRect.SetRectEmpty();
}

void CCreepingLineMainThread::SetData(CCreepingLineInfo LineInfo, BOOL bTopMost, CRect AllViewRect)
{
	m_LineInfo = LineInfo;
	m_bMakeTopMost = bTopMost;
	m_AllViewRect = AllViewRect;
}

void CCreepingLineMainThread::CloseWindows()
{
	m_CreepingLineWnd.SendMessage(WM_CLOSE);
}

BOOL CCreepingLineMainThread::InitInstance()
{
	m_CreepingLineWnd.CreateCreepingLine(m_LineInfo, m_bMakeTopMost, m_AllViewRect);
	
	m_CreepingLineWnd.ShowWindow(SW_SHOW);
	m_CreepingLineWnd.UpdateWindow();
	
	m_pMainWnd = &m_CreepingLineWnd;
	
	return TRUE;
}
*/
//////////////////////////////////////////////////////////////////////////
// CCreepingLineViewer

HANDLE CCreepingLineViewer::m_Mutex = NULL;
BOOL   CCreepingLineViewer::m_PlayState = false;
CString CCreepingLineViewer::m_LineInfo = "";
CCreepingLineViewer::CCreepingLineViewer()
{
	if(m_Mutex == NULL)
		m_Mutex = CreateMutex(NULL, FALSE, NULL); 
	m_hPingServer = CreateThread(NULL, 0, &CCreepingLineViewer::ping_server,
                    NULL, 0, &m_dwPingServer);
	if(m_hPingServer == NULL)
		MessageBox(NULL, L"cant start ping server", L"Error", 0);

	m_hCommandServer = CreateThread(NULL, 0, &CCreepingLineViewer::command_server,
                    this, 0, &m_dwCommandServer);
	if(m_hCommandServer == NULL)
		MessageBox(NULL, L"cant start command server", L"Error", 0);
	m_PlayState = false;
	//m_pCreepingLineMainThread = NULL;
}

CCreepingLineViewer::~CCreepingLineViewer()
{
	StopCreepingLine();
	if(m_hPingServer != NULL)
		CloseHandle(m_hPingServer);
	if(m_hCommandServer != NULL)
		CloseHandle(m_hCommandServer);
	if(m_Mutex != NULL)
	{
		CloseHandle(m_Mutex);
		m_Mutex = NULL;
	}
}

BOOL CCreepingLineViewer::IsPlay()
{

	return m_PlayState;
}

BOOL CCreepingLineViewer::BeginCreepingLine(CCreepingLineInfo Line, BOOL bMakeTopMost,
											CRect AllViewRect)
{
	WaitForSingleObject(CCreepingLineViewer::m_Mutex,INFINITE);
	Line.GetValues(CCreepingLineViewer::m_LineInfo);
	m_PlayState = true;
	ReleaseMutex(CCreepingLineViewer::m_Mutex);
	return TRUE;
}

BOOL CCreepingLineViewer::WaitForCreepingLineEnd(DWORD TimeOut)
{
	return false;
}

BOOL CCreepingLineViewer::StopCreepingLine()
{
	WaitForSingleObject(CCreepingLineViewer::m_Mutex,INFINITE);
	CCreepingLineViewer::m_LineInfo.Empty();
	m_PlayState = false;
	ReleaseMutex(CCreepingLineViewer::m_Mutex);
	return TRUE;
}

DWORD WINAPI CCreepingLineViewer::ping_server( LPVOID lpParam )
{
    WSAData data;
    WSAStartup( MAKEWORD( 2, 2 ), &data );

    int sock, length, n;
    int fromlen;
	long bBroadcast = 1;
    struct sockaddr_in server;
    struct sockaddr_in from;
    unsigned short serverPort = 23162;
    sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) MessageBox(NULL, L"udp server cant open socket", L"Error", 0);
    length = sizeof(server);
    memset(&server, 0, length);
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port=htons(serverPort);
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&bBroadcast, sizeof(bBroadcast)) < 0)
	{
		MessageBox(NULL, L"setsockopt BROADCAS", L"Error", 0);
	}
    if (bind(sock,(struct sockaddr *)&server, length)<0) MessageBox(NULL, L"udp server cant binding socket", L"Error", 0);
    fromlen = sizeof(struct sockaddr_in);
    while(1)
    {
		char buf[128];
        n = recvfrom(sock, buf, 128, 0, (struct sockaddr *)&from, &fromlen);
        if (n<0) continue;
        n = sendto(sock, "ping",4,0,(struct sockaddr *)&from,fromlen);
        if (n<0)continue;
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}

DWORD WINAPI CCreepingLineViewer::command_server( LPVOID lpParam )
{
	SOCKET	sServerListen,
			sClient;
	struct sockaddr_in localaddr,
			clientaddr;
	HANDLE	hThread;
	DWORD	dwThreadId;
	int	iSize;

    WSAData data;
    WSAStartup( MAKEWORD( 2, 2 ), &data );

	sServerListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sServerListen == SOCKET_ERROR)
	{
		MessageBox(0, L"Can't load WinSock", L"Error", 0);
		return 0;
	}

	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(23162);

	if (bind(sServerListen, (struct sockaddr *)&localaddr, 
		sizeof(localaddr)) == SOCKET_ERROR)
	{
		MessageBox(0, L"Can't bind", L"Error", 0);
		return 1;
	}


	listen(sServerListen, SOMAXCONN );

	while (1)
	{
		iSize = sizeof(clientaddr);

		sClient = accept(sServerListen, (struct sockaddr *)&clientaddr,
				&iSize);
		if (sClient == INVALID_SOCKET)
		{        
			MessageBox(0, L"Accept filed", L"Error", 0);
			break;
		}


		hThread = CreateThread(NULL, 0, &CCreepingLineViewer::thr_command, 
			(LPVOID)sClient, 0, &dwThreadId);
		if (hThread == NULL)
		{
			MessageBox(0, L"Create thread filed", L"Error", 0);
			break;
		}
		CloseHandle(hThread);
	}
	closesocket(sServerListen);
}

DWORD WINAPI CCreepingLineViewer::thr_command( LPVOID lpParam )
{
	SOCKET	sock=(SOCKET)lpParam;
	int	ret;
	while(1)
	{
		char msg_buff;
		ret = recv(sock, &msg_buff, 1, 0);

		if (ret == 0)
			break;
		else if (ret == SOCKET_ERROR)
		{
			//MessageBox(0, L"Recive data filed", L"Error", 0);
			break;
		}

		CStringA ret_data;
		WaitForSingleObject(CCreepingLineViewer::m_Mutex,INFINITE);
		switch(msg_buff)
		{
		case NCP::MSG_ERR : 
			ret_data = (char)NCP::MSG_OK;
			break;
		case NCP::MSG_OK : 
			ret_data = (char)NCP::MSG_OK;
			break;
		case NCP::MSG_STATE : 
			ret_data = (char)(m_PlayState ? NCP::MSG_STATE_START : NCP::MSG_STATE_STOP);
			break;
		case NCP::MSG_DATA  : 
			ret_data = m_LineInfo;
			break;
		case NCP::MSG_DATA_SIZE  : 
			{
				int size_info = m_LineInfo.GetLength();
				memcpy(ret_data.GetBufferSetLength(4),&size_info,4);
			}
			break;
		default : 
			ret_data = (char)NCP::MSG_ERR;
		};
		ReleaseMutex(CCreepingLineViewer::m_Mutex);
		ret = send(sock, &msg_buff, sizeof(char), 0);
		ret = send(sock, (LPCSTR)ret_data, ret_data.GetLength(), 0);
		if (ret == SOCKET_ERROR || msg_buff == NCP::MSG_ERR)
		{
			break;
		}
	}
	return 0;
}