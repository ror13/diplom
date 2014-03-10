// WindowPositionControl.cpp : implementation file
//

#include "stdafx.h"
#include "creepingline.h"
#include "WindowPositionControl.h"

#include "MonResDatas.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowPositionControl

CWindowPositionControl::CWindowPositionControl()
{
	m_rectWindow = CRect( 0, 0, 10, 10 );
	m_sizeScreen = CSize(100, 100);
	m_clBackground = RGB( 50, 150, 50 );

	m_rect = CRect( 0, 0, 10, 10 );
	m_nStyle = 0
		| CMyRectTracker::solidLine
		| CMyRectTracker::resizeOutside
		| CMyRectTracker::resizeInside
		;

	m_nHandleSize = 16;
	m_sizeMin = CSize( 5, 5 );	// Tracker (window) min size
	m_iScreenMinimumSize = 10;	// Virtaul (screen) min size

	m_nTrackingHandle = CCustomRectTracker::hitNothing;

	m_clScreenBackground		= RGB(50,50,150);
	m_clScreenResolutionLines	= RGB(200,200,200);
	m_clScreenResolutionText	= RGB(200,200,200);

	int delta = 15;

	const int MaxX = 1600;
	const int MaxY = 1200;

	//m_arrAligns.Add( ALIGN_INFO( ALIGN_VERT_LINE, 0, delta, MIN_RESOLUTION, MaxY, FALSE ) );
	//m_arrAligns.Add( ALIGN_INFO( ALIGN_HORZ_LINE, 0, delta, MIN_RESOLUTION, MaxX, FALSE ) );

	{
		g_Res.GenerateSmallResolutionsList();

		for (int i = 0; i < g_Res.Size(); i++)
		{
			m_arrAligns.Add( ALIGN_INFO( ALIGN_VERT_LINE, g_Res.GetX(i), delta, MIN_RESOLUTION, g_Res.GetY(i) ) );
			m_arrAligns.Add( ALIGN_INFO( ALIGN_HORZ_LINE, g_Res.GetY(i), delta, MIN_RESOLUTION, g_Res.GetX(i) ) );
		}
	}

// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_VERT_LINE, 1600, delta, MIN_RESOLUTION, 1200 ) );
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_HORZ_LINE, 1200, delta, MIN_RESOLUTION, 1600 ) );
// 
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_VERT_LINE, 1280, delta, MIN_RESOLUTION, 1024 ) );
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_HORZ_LINE, 1024, delta, MIN_RESOLUTION, 1280 ) );
// 
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_VERT_LINE, 1024, delta, MIN_RESOLUTION, 768 ) );
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_HORZ_LINE, 768, delta, MIN_RESOLUTION, 1024 ) );
// 
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_VERT_LINE, 800, delta, MIN_RESOLUTION, 600 ) );
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_HORZ_LINE, 600, delta, MIN_RESOLUTION, 800 ) );
// 
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_VERT_LINE, 640, delta, MIN_RESOLUTION, 480 ) );
// 	m_arrAligns.Add( ALIGN_INFO( ALIGN_HORZ_LINE, 480, delta, MIN_RESOLUTION, 640 ) );
}

CWindowPositionControl::~CWindowPositionControl()
{
}


BEGIN_MESSAGE_MAP(CWindowPositionControl, CStatic)
	//{{AFX_MSG_MAP(CWindowPositionControl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowPositionControl message handlers

int CWindowPositionControl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	ASSERT( FALSE && "This code is unreachable for some reason!!!" );
//	DWORD dwStyle = GetClassLong( this->GetSafeHwnd(), GCL_STYLE );
//	dwStyle &= ~CS_PARENTDC;
//	SetClassLong( this->GetSafeHwnd(), GCL_STYLE, dwStyle );

	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	Update();

	return 0;
}

void CWindowPositionControl::Update()
{
//	TRACE( _T("CWindowPositionControl::Update()\n") );
	m_rect = m_rectWindow;
	VirtualToClient( m_rect );
	InvalidateRect( NULL, FALSE );
}

void CWindowPositionControl::OnDestroy() 
{
	CStatic::OnDestroy();

}

BOOL CWindowPositionControl::OnEraseBkgnd(CDC* pDC) 
{

	return CStatic::OnEraseBkgnd(pDC);
}

HBRUSH CWindowPositionControl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CWindowPositionControl::OnPaint() 
{
//	CStatic::OnPaint();
	if(!IsWindowEnabled())
	{
		CStatic::OnPaint();
		return;
	}

	CRect rectClient;
	GetClientRect( &rectClient );

	CPaintDC dcReal(this); // device context for painting
	CHDCBuffer buffer;
	CDC* pDC = buffer.GetBufferDC( &dcReal,
		rectClient.Width(), rectClient.Height(), m_clScreenBackground );
	CDC& dc = *pDC;

	// Set window dc special mapping mode:
	dc.SetMapMode( MM_ANISOTROPIC );
	dc.SetWindowOrg( 0, 0 );
	dc.SetWindowExt( m_sizeScreen );
	dc.SetViewportOrg( 0, 0 );
	dc.SetViewportExt( rectClient.Size() );

	{
		// Draw align lines:

		CPen pen( PS_DOT, 1, m_clScreenResolutionLines );
		CPen* pOldPen = dc.SelectObject( &pen );

		for( int i=0; i<m_arrAligns.GetSize(); i++ )
		{
			ALIGN_INFO info = m_arrAligns.GetAt(i);
			if( !info.bDraw )
			{
				continue;
			}

			if( info.align == ALIGN_VERT_LINE )
			{
				dc.MoveTo( CPoint( info.position, info.from ) );
				dc.LineTo( CPoint( info.position, info.to ) );
			}
			else if( info.align == ALIGN_HORZ_LINE )
			{
				dc.MoveTo( CPoint( info.from, info.position ) );
				dc.LineTo( CPoint( info.to, info.position ) );
			}
			else
			{
				ASSERT( FALSE && "Unknown align type!" );
			}
		}

		dc.SelectObject( pOldPen );
		pen.DeleteObject();
	}

	{
		// Print resolution sizes:
		LPCTSTR szFont = _T("MS Sans Serif");
		int FontSize = 18;
		int FontWeight = FW_BOLD; // FW_NORMAL

		CDC* pDC = &dc; 
		int nHeight = -MulDiv(FontSize, GetDeviceCaps(*pDC, LOGPIXELSY), 72);

		CFont font;
		font.CreateFont( nHeight, 0,
			0, 0, FontWeight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, szFont );

		CFont* pOldFont = pDC->SelectObject( &font );

		pDC->SetBkMode( TRANSPARENT );
		pDC->SetTextColor( m_clScreenResolutionText );

		CRect r;

// 		r = CRect( CPoint(0,0), CSize(6400,4096) );
// 		pDC->DrawText( _T("6400x4096"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 		
// 		r = CRect( CPoint(0,0), CSize(3200,2048) );
// 		pDC->DrawText( _T("3200x2048"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 
// 		r = CRect( CPoint(0,0), CSize(2560,2048) );
// 		pDC->DrawText( _T("2560x2048"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 		
// 		r = CRect( CPoint(0,0), CSize(2880,1800) );
// 		pDC->DrawText( _T("2880x1800"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 		
// 		r = CRect( CPoint(0,0), CSize(2560,1600) );
// 		pDC->DrawText( _T("2560x1600"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 		
// 		r = CRect( CPoint(0,0), CSize(1920,1200) );
// 		pDC->DrawText( _T("1920x1200"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 		
// 		r = CRect( CPoint(0,0), CSize(1600,1200) );
// 		pDC->DrawText( _T("1600x1200"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 		
// 		r = CRect( CPoint(0,0), CSize(1280,1024) );
// 		pDC->DrawText( _T("1280x1024"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 
// 		r = CRect( CPoint(0,0), CSize(1024,768) );
// 		pDC->DrawText( _T("1024x768"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 
// 		r = CRect( CPoint(0,0), CSize(800,600) );
// 		pDC->DrawText( _T("800x600"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
// 
// 		r = CRect( CPoint(0,0), CSize(640,480) );
// 		pDC->DrawText( _T("640x480"), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );

		{
			for (int i = 0; i < g_Res.Size(); i++)
			{
				r = CRect( CPoint(0,0), CSize(g_Res.GetX(i),g_Res.GetY(i)) );
				pDC->DrawText( g_Res.GetStringFromIndex(i), &r, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT );
			}
		}
		
		pDC->SelectObject( pOldFont );

		font.DeleteObject();
	}

	{
		// Draw creeping line window:
		CBrush brush( m_clBackground );
		dc.FillRect( m_rectWindow, &brush );
		brush.DeleteObject();
	}

	{
		// Print creeping line text:
		CFont font;
		font.CreateFontIndirect( &m_font );
		CDC* pDC = &dc;

		CFont* pOldFont = pDC->SelectObject( &font );

		pDC->SetBkMode( TRANSPARENT );
		pDC->SetTextColor( m_clTextColor );

		pDC->DrawText( m_strText, &m_rectWindow, DT_SINGLELINE | DT_VCENTER );

		pDC->SelectObject( pOldFont );

		font.DeleteObject();
	}

	dc.SetWindowExt( rectClient.Size() );	// Restore WindowExt before BitBlt inside RestoreBufferDC()
	buffer.RestoreBufferDC();

	// Do not call CStatic::OnPaint() for painting messages
}

void CWindowPositionControl::DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo,
		CDC* pDC, CWnd* pWnd)
{
}

int CWindowPositionControl::Align( int pos, int net, int delta ) // returns 0 if too far else returns distance to the net
{
	if( abs(pos - net) <= delta )
	{
		return net - pos;
	}
	return 0;
}

inline bool inside( int pos, int a, int b )
{
	return a <= pos && pos <= b;
}

BOOL CWindowPositionControl::Align( CRect& rect, int adjust,
								   const ALIGN_INFO& info, BOOL& bAlignedAny )
{
	CRect rectOrig = rect;
	BOOL bAlignedAnyOrig = bAlignedAny;

	BOOL bAligned = FALSE;

	if( info.align == ALIGN_VERT_LINE )
	{
		if( inside( rect.top, info.from, info.to ) ||
			inside( rect.bottom, info.from, info.to ) )
		{
			int d = Align( rect.left, info.position, info.delta );
			if( d == 0 )
			{
				d = Align( rect.right, info.position, info.delta );
			}

			if( d != 0 )
			{
				BOOL bChanged = TRUE;
				switch( adjust & (~ADJUST_TOP) & (~ADJUST_BOTTOM) )
				{
				case 0:
					bChanged = FALSE;
					break;
				case ADJUST_MOVE:
					rect += CPoint( d, 0 );
					break;
				case ADJUST_LEFT:
					rect.left += d;
					break;
				case ADJUST_RIGHT:
					rect.right += d;
					break;
				default:
					ASSERT( FALSE );
				}

				if( bChanged )
				{
					bAligned = TRUE;
					bAlignedAny = TRUE;
				}
			}
		}
	}
	else if( info.align == ALIGN_HORZ_LINE )
	{
		if( inside( rect.left, info.from, info.to ) ||
			inside( rect.right, info.from, info.to ) )
		{
			int d = Align( rect.top, info.position, info.delta );
			if( d == 0 )
			{
				d = Align( rect.bottom, info.position, info.delta );
			}

			if( d != 0 )
			{
				BOOL bChanged = TRUE;
				switch( adjust & (~ADJUST_LEFT) & (~ADJUST_RIGHT) )
				{
				case 0:
					bChanged = FALSE;
					break;
				case ADJUST_MOVE:
					rect += CPoint( 0, d );
					break;
				case ADJUST_TOP:
					rect.top += d;
					break;
				case ADJUST_BOTTOM:
					rect.bottom += d;
					break;
				default:
					ASSERT( FALSE );
				}

				if( bChanged )
				{
					bAligned = TRUE;
					bAlignedAny = TRUE;
				}
			}
		}
	}
	else
	{
		ASSERT( FALSE && "Unknown align!" );
	}

	if(
		(rect.Width() < m_iScreenMinimumSize && rectOrig.Width() >= m_iScreenMinimumSize )
		||
		(rect.Height() < m_iScreenMinimumSize && rectOrig.Height() >= m_iScreenMinimumSize )
		)
	{
		rect = rectOrig;
		bAlignedAny = bAlignedAnyOrig;
		bAligned = FALSE;
	}

	return bAligned;
}

void CWindowPositionControl::AdjustRect(int nHandle, LPRECT lpRect)
{
//	CCustomRectTracker::SetCursor( this, HTCLIENT );
	CCustomRectTracker::AdjustRect( nHandle, lpRect );
}

UINT CWindowPositionControl::GetHandleMask() const
{
//	UINT res = CMyRectTracker::GetHandleMask();
//	return res;
	return 0xFF;
}

void CWindowPositionControl::OnChangedRect(const CRect& rectOld)
{
//	TRACE( _T("OnChangedRect: %d, %d - %d, %d\n"), 
//		m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );

	m_rectWindow = m_rect;
	ClientToVirtual( m_rectWindow );

	AdjustChangedRect( rectOld );

	InvalidateRect( NULL, FALSE );
	GetParent()->SendMessage( WN_WINDOW_POSITION_CHANGED );
}

void CWindowPositionControl::AdjustChangedRect(const CRect& rectOld)
{
	SHORT vkShift = GetKeyState( VK_SHIFT );
	BOOL bShift = vkShift&0x8000;
	BOOL bAllowEverything = bShift;

	BOOL bAlignedAny = FALSE;
	BOOL bAligned;

	int	adjust;

	switch( m_nTrackingHandle )
	{
		case CCustomRectTracker::hitNothing:
			ASSERT( FALSE );
			return;
			break;
		case CCustomRectTracker::hitTopLeft:
			adjust = ADJUST_LEFT | ADJUST_TOP;
			break;
		case CCustomRectTracker::hitTopRight:
			adjust = ADJUST_RIGHT | ADJUST_TOP;
			break;
		case CCustomRectTracker::hitBottomRight:
			adjust = ADJUST_RIGHT | ADJUST_BOTTOM;
			break;
		case CCustomRectTracker::hitBottomLeft:
			adjust = ADJUST_LEFT | ADJUST_BOTTOM;
			break;
		case CCustomRectTracker::hitTop:
			adjust = ADJUST_TOP;
			break;
		case CCustomRectTracker::hitRight:
			adjust = ADJUST_RIGHT;
			break;
		case CCustomRectTracker::hitBottom:
			adjust = ADJUST_BOTTOM;
			break;
		case CCustomRectTracker::hitLeft:
			adjust = ADJUST_LEFT;
			break;
		case CCustomRectTracker::hitMiddle:
			adjust = ADJUST_MOVE;
			break;
		default:
			ASSERT( FALSE );
			return;
	}

//	TRACE( _T("Align: %d\n"), adjust );

//*
	CRect rectScreen = CRect( CPoint(0,0), m_sizeScreen );

	// Don't let to move window outside of the screen on the X-axe
	if( !bAllowEverything )
	{
		if( rectOld.Width() < rectScreen.Width() )
		{
			int delta = 0;
			if( m_rectWindow.right > rectScreen.right )
			{
				delta = m_rectWindow.right - rectScreen.right;
			}
			if( m_rectWindow.left < rectScreen.left )
			{
				delta = m_rectWindow.left - rectScreen.left;
			}

			if( delta != 0 )
			{
				switch( adjust & (~ADJUST_TOP) & (~ADJUST_BOTTOM) )
				{
				case ADJUST_MOVE:
					m_rectWindow -= CPoint( delta, 0 );
					break;
				case ADJUST_LEFT:
					m_rectWindow.left -= delta;
					break;
				case ADJUST_RIGHT:
					m_rectWindow.right -= delta;
					break;
				default:
					ASSERT( FALSE );
				}
				bAlignedAny = TRUE;
			}
		}
	}

	// Don't let to move window outside of the screen on the Y-axe
	if( !bAllowEverything )
	{
		if( rectOld.Height() <= rectScreen.Height() )
		{
			int delta = 0;
			if( m_rectWindow.bottom > rectScreen.bottom )
			{
				delta = m_rectWindow.bottom - rectScreen.bottom;
			}
			if( m_rectWindow.top < rectScreen.top )
			{
				delta = m_rectWindow.top - rectScreen.top;
			}

			if( delta != 0 )
			{
				switch( adjust & (~ADJUST_LEFT) & (~ADJUST_RIGHT) )
				{
				case ADJUST_MOVE:
					m_rectWindow -= CPoint( 0, delta );
					break;
				case ADJUST_TOP:
					m_rectWindow.top -= delta;
					break;
				case ADJUST_BOTTOM:
					m_rectWindow.bottom -= delta;
					break;
				default:
					ASSERT( FALSE );
				}
				bAlignedAny = TRUE;
			}
		}
	}
//*/

	if( !bAllowEverything )
	{
		for( int i=0; i<m_arrAligns.GetSize(); i++ )
		{
			ALIGN_INFO info = m_arrAligns.GetAt(i);
			bAligned = Align( m_rectWindow, adjust, info, bAlignedAny );
		}
	}

	if( bAlignedAny )
	{
//		TRACE( _T("Aligned! - recalculating m_rect\n") );
		m_rect = m_rectWindow;
		VirtualToClient( m_rect );
	}
}

void CWindowPositionControl::ClientToVirtual( CPoint& pt )
{
	CRect rectClient;
	GetClientRect( &rectClient );

	pt.x = (pt.x * m_sizeScreen.cx + rectClient.Width()/2) / rectClient.Width();
	pt.y = (pt.y * m_sizeScreen.cy + rectClient.Height()/2) / rectClient.Height();
}

void CWindowPositionControl::VirtualToClient( CPoint& pt )
{
	CRect rectClient;
	GetClientRect( &rectClient );

	pt.x = (pt.x * rectClient.Width() + m_sizeScreen.cx/2) / m_sizeScreen.cx;
	pt.y = (pt.y * rectClient.Height() + m_sizeScreen.cy/2) / m_sizeScreen.cy;
}

void CWindowPositionControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	int hit = HitTestHandles(point);
	int hit = HitTest(point);
	if( hit != CMyRectTracker::hitNothing )
	{
		CCustomRectTracker::SetCursor( this, HTCLIENT );

		m_nTrackingHandle = hit;
		Track( this, point );
		m_nTrackingHandle = CCustomRectTracker::hitNothing;
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CWindowPositionControl::OnLButtonUp(UINT nFlags, CPoint point) 
{

	CStatic::OnLButtonUp(nFlags, point);
}

void CWindowPositionControl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CCustomRectTracker::SetCursor( this, HTCLIENT );

	CStatic::OnMouseMove(nFlags, point);
}

LRESULT CWindowPositionControl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_WINDOW_POSITION_UPDATE_DATA )
	{
		Update();
		return 0;
	}

	return CStatic::WindowProc(message, wParam, lParam);
}


BOOL CWindowPositionControl::PreCreateWindow(CREATESTRUCT& cs) 
{

	return CStatic::PreCreateWindow(cs);
}
