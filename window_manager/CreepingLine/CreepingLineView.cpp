// CreepingLineView.cpp : implementation of the CCreepingLineView class
//

#include "stdafx.h"
#include "CreepingLine.h"

#include "CreepingLineDoc.h"
#include "CreepingLineView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineView

IMPLEMENT_DYNCREATE(CCreepingLineView, CView)

BEGIN_MESSAGE_MAP(CCreepingLineView, CView)
	//{{AFX_MSG_MAP(CCreepingLineView)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineView construction/destruction

CCreepingLineView::CCreepingLineView()
{
}

CCreepingLineView::~CCreepingLineView()
{
}

BOOL CCreepingLineView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineView drawing

void CCreepingLineView::OnDraw(CDC* pRealDC)
{
	CCreepingLineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect r;
	GetClientRect( &r );
	CCreepingLineInfo& m_line = pDoc->m_line;

	CHDCBuffer buffer;
	CDC* pDC = buffer.GetBufferDC( pRealDC,
		r.Width(), r.Height(),  m_line.m_clBackground );

	CFont font;
	font.CreateFontIndirect( &m_line.m_font );
	CRect& m_rTextSize = pDoc->m_rTextSize;
	double m_HorizontalPos = 0.0;

	pDC->SaveDC();

	CFont* pOldFont = pDC->SelectObject( &font );

	CBrush brush( m_line.m_clBackground );
	pDC->FillRect( &r, &brush );

	pDC->SetBkMode( TRANSPARENT );
	pDC->SetTextColor( m_line.m_clTextColor );

	m_rTextSize = r;
	pDC->DrawText( m_line.m_strText, &m_rTextSize, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT );

	r.left += (int)m_HorizontalPos;
	pDC->DrawText( m_line.m_strText, &r, DT_SINGLELINE | DT_VCENTER );

	pDC->SelectObject( pOldFont );

	pDC->RestoreDC( -1 );
	font.DeleteObject();

	buffer.RestoreBufferDC();
}

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineView printing

BOOL CCreepingLineView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCreepingLineView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CCreepingLineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineView diagnostics

#ifdef _DEBUG
void CCreepingLineView::AssertValid() const
{
	CView::AssertValid();
}

void CCreepingLineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCreepingLineDoc* CCreepingLineView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCreepingLineDoc)));
	return (CCreepingLineDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineView message handlers

BOOL CCreepingLineView::OnEraseBkgnd(CDC* pDC) 
{
	CCreepingLineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDC->SetBkColor( pDoc->m_line.m_clBackground );
	return TRUE;

//	return CView::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineView::OnRButtonUp(UINT nFlags, CPoint point) 
{
//	CView::OnRButtonUp(nFlags, point);

	ClientToScreen( &point );

//	CPoint point(0,0);
//	GetCursorPos( &point );
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_MENU));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}

/////////////////////////////////////////////////////////////////////////////

void CCreepingLineView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CCreepingLineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->OnLineOptions();

	CView::OnLButtonDblClk(nFlags, point);
}
