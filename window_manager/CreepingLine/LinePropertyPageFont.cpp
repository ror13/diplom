// LinePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LinePropertyPageFont.h"
#include "LinePropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLinePropertyPageFont, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPageFont property page

CLinePropertyPageFont::CLinePropertyPageFont() : CPropertyPage(CLinePropertyPageFont::IDD)
{
	m_psheet = NULL;
	//{{AFX_DATA_INIT(CLinePropertyPageFont)
	m_strFontDesc = _T("");
	//}}AFX_DATA_INIT

/*	
	m_strExampleText = 
		_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n")
		_T("abcdefghijklmnopqrstuvwxyz\n")
		_T("0123456789 !@#$%^&*()_+=-[]{};:'\"<>,./?\\|")
		;
*/
}

CLinePropertyPageFont::~CLinePropertyPageFont()
{
}

void CLinePropertyPageFont::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePropertyPageFont)
	DDX_Control(pDX, IDC_FONT_EXAMPLE_FRAME, m_wndFontExampleFrame);
	DDX_Text(pDX, IDC_FONT_DESC, m_strFontDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinePropertyPageFont, CPropertyPage)
	//{{AFX_MSG_MAP(CLinePropertyPageFont)
	ON_BN_CLICKED(IDC_CHOOSE_FONT, OnChooseFont)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_STOP_PREVIEW, OnStopPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CLinePropertyPageFont::UpdateFontDescription()
{
//	UpdateData( TRUE ); // not necessary

	m_strFontDesc.Format( _T("%s, %d"),
		m_font.lfFaceName, m_clTextSize );

	if( m_font.lfWeight >= FW_BOLD )
	{
		m_strFontDesc += _T(", Bold");
	}
	if( m_font.lfItalic )
	{
		m_strFontDesc += _T(", Italic");
	}
	if( m_font.lfUnderline )
	{
		m_strFontDesc += _T(", Underline");
	}
	if( m_font.lfStrikeOut )
	{
		m_strFontDesc += _T(", StrikeOut");
	}

	UpdateData( FALSE );
}

void CLinePropertyPageFont::OnChooseFont() 
{
	CFontDialog dlg(&m_font);
	dlg.m_cf.rgbColors = m_clTextColor;

	//dlg.m_cf.Flags |= CF_FIXEDPITCHONLY;
	//dlg.m_cf.Flags &= ~CF_EFFECTS;		// remove the Effects controls
	//dlg.m_cf.Flags |= CF_NOSCRIPTSEL;		// disable the script combo

	int res = dlg.DoModal();

	if( res == IDOK)
	{
		dlg.GetCurrentFont(&m_font);
		m_clTextColor = dlg.GetColor();

		int FontSize = m_font.lfHeight;
		CDC* pDC = GetWindowDC();
		m_clTextSize = -MulDiv(FontSize, 72, GetDeviceCaps(*pDC, LOGPIXELSY) );
		ReleaseDC( pDC );
		pDC = NULL;

		UpdateFontDescription();
		InvalidateRect( NULL );
	}
}

void CLinePropertyPageFont::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	ASSERT( m_psheet != NULL );

//	CString s = m_strExampleText;
//	if( m_psheet != NULL )
//	{
//		s = m_psheet->m_PageGeneral.m_strText;
//	}
	CString s = m_psheet->m_PageGeneral.m_strText;

	CRect r;
	m_wndFontExampleFrame.GetWindowRect( &r );
	ScreenToClient( &r );
	CDC* pDC = &dc;

	CFont font;
	font.CreateFontIndirect( &m_font );

	pDC->SaveDC();

	CFont* pOldFont = pDC->SelectObject( &font );

//	pDC->SetBkMode( TRANSPARENT );
	pDC->SetTextColor( m_clTextColor );
	pDC->SetBkColor( m_psheet->m_PageWindow.m_btnBackground.ColorProperty() );

	pDC->DrawText( s, &r, 0 ); //DT_SINGLELINE | DT_VCENTER ); //| DT_CENTER );

	pDC->SelectObject( pOldFont );

	pDC->RestoreDC( -1 );
	font.DeleteObject();

	// Do not call CPropertyPage::OnPaint() for painting messages
}

BOOL CLinePropertyPageFont::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

//	SetModified( TRUE );
	UpdateFontDescription();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLinePropertyPageFont::OnApply() 
{
	return CPropertyPage::OnApply();
}


void CLinePropertyPageFont::OnPreview() 
{
	UpdateData();
	m_psheet->OnPreview();
}

void CLinePropertyPageFont::OnStopPreview() 
{
	m_psheet->OnStopPreview();
}
