// LinePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LinePropertyPageWindow.h"
#include "LinePropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLinePropertyPageWindow, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPageWindow property page

CLinePropertyPageWindow::CLinePropertyPageWindow() : CPropertyPage(CLinePropertyPageWindow::IDD)
{
	m_psheet = NULL;
	//{{AFX_DATA_INIT(CLinePropertyPageWindow)
	m_TransparentFactor = 0;
	m_bResizingWindow = FALSE;
	m_bTransparent = FALSE;
	//}}AFX_DATA_INIT
}

CLinePropertyPageWindow::~CLinePropertyPageWindow()
{
}

void CLinePropertyPageWindow::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePropertyPageWindow)
	DDX_Control(pDX, IDC_TRANSPARENT_EXAMPLE, m_wndTransparentText);
	DDX_Control(pDX, IDC_TRANSPARENT_FACTOR, m_wndTransparentFactor);
	DDX_Control(pDX, IDC_TRANSPARENT_FACTOR_SLIDER, m_slider);
	DDX_Control(pDX, IDC_TRANSPARENT_COLOR, m_btnTransparentColor);
	DDX_Control(pDX, IDC_BACKGROUND_COLOR, m_btnBackground);
	DDX_Text(pDX, IDC_TRANSPARENT_FACTOR, m_TransparentFactor);
	DDV_MinMaxByte(pDX, m_TransparentFactor, 0, 255);
	DDX_Check(pDX, IDC_RESIZING, m_bResizingWindow);
	DDX_Check(pDX, IDC_TRANSPARENT_WINDOW, m_bTransparent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinePropertyPageWindow, CPropertyPage)
	//{{AFX_MSG_MAP(CLinePropertyPageWindow)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_STOP_PREVIEW, OnStopPreview)
	ON_EN_CHANGE(IDC_TRANSPARENT_FACTOR, OnChangeTransparentFactor)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_TRANSPARENT_WINDOW, OnTransparentWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLinePropertyPageWindow::OnApply() 
{
	return CPropertyPage::OnApply();
}

BOOL CLinePropertyPageWindow::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

//	SetModified( TRUE );
	m_slider.SetRange( 0, 255, TRUE );
	m_slider.SetTicFreq(5);
	m_slider.SetPos( m_TransparentFactor );

	OnTransparentWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLinePropertyPageWindow::OnDestroy() 
{
	CPropertyPage::OnDestroy();

	m_font.DeleteObject();
	m_brush.DeleteObject();
}

void CLinePropertyPageWindow::OnPreview() 
{
	UpdateData();
	m_psheet->OnPreview();
}

void CLinePropertyPageWindow::OnStopPreview() 
{
	m_psheet->OnStopPreview();
}

void CLinePropertyPageWindow::OnChangeTransparentFactor() 
{
	CString s;
	m_wndTransparentFactor.GetWindowText( s );
	int x = _ttoi(s);

	if( x >=0 && x <=255 )
	{
		m_slider.SetPos( x );
		m_wndTransparentText.Invalidate();
	}
}

void CLinePropertyPageWindow::OnSliderChange()
{
	int pos = m_slider.GetPos();
	UpdateData( TRUE );
	m_TransparentFactor = pos;
	UpdateData( FALSE );
	m_wndTransparentText.Invalidate();
}

void CLinePropertyPageWindow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( pScrollBar == (CWnd*)&m_slider )
	{
		OnSliderChange();
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLinePropertyPageWindow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( pScrollBar == (CWnd*)&m_slider )
	{
		OnSliderChange();
	}

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CLinePropertyPageWindow::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

}

BOOL CLinePropertyPageWindow::OnSetActive() 
{
	m_font.DeleteObject();
	m_font.CreateFontIndirect( &m_psheet->m_PageFont.m_font );
	m_wndTransparentText.SetFont( &m_font );
//	m_wndTransparentText.SetWindowText( m_psheet->m_PageGeneral.m_strText );
	OnTransparentWindow();

	return CPropertyPage::OnSetActive();
}

BYTE MakeTransparent( BYTE clBack, BYTE clOrig, BYTE Factor )
{
//	A - back color
//	B - orig color
//	C - transparent factor
//	A <- B, C/255
//	X = A + (B-A)*C/255;
	int A = clBack;
	int B = clOrig;
	int C = Factor;
	int X = A + (B-A)*(255-C)/255;
	return BYTE(X);
}

HBRUSH CLinePropertyPageWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = NULL;
	if( pWnd == &m_wndTransparentText )
	{
		CString s;
		m_wndTransparentFactor.GetWindowText( s );
		BYTE TransparentFactor = _ttoi(s);

		COLORREF clDesiredBack = GetSysColor( COLOR_BTNFACE );
		COLORREF clOrigBack = m_btnBackground.ColorProperty();
		COLORREF clOrigText = m_psheet->m_PageFont.m_clTextColor;

#define TRANS( func ) MakeTransparent( func(clOrigBack), func(clDesiredBack), TransparentFactor )
		COLORREF clTransparentColorBack =
			RGB( TRANS(GetRValue), TRANS(GetGValue), TRANS(GetBValue) );

#undef TRANS
#define TRANS( func ) MakeTransparent( func(clOrigText), func(clDesiredBack), TransparentFactor )
		COLORREF clTransparentColorText =
			RGB( TRANS(GetRValue), TRANS(GetGValue), TRANS(GetBValue) );

		pDC->SetBkColor( clTransparentColorBack );
		pDC->SetTextColor( clTransparentColorText );

		m_brush.DeleteObject();
		m_brush.CreateSolidBrush( clTransparentColorBack );
		hbr = m_brush;
	}
	else
	{
		hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	}

//	ASSERT( hbr != NULL );
	return hbr;
}

void CLinePropertyPageWindow::OnTransparentWindow()
{
	UpdateData( TRUE );

	m_btnTransparentColor.EnableWindow( m_bTransparent );
	m_wndTransparentFactor.EnableWindow( m_bTransparent );
	m_slider.EnableWindow( m_bTransparent );
	m_wndTransparentText.EnableWindow( m_bTransparent );

	if( m_bTransparent )
	{
		m_wndTransparentText.SetWindowText( m_psheet->m_PageGeneral.m_strText );
	}
	else
	{
		m_wndTransparentText.SetWindowText( _T("") );
	}
}
