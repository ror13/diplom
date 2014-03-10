// LinePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LinePropertyPageGeneral.h"
#include "LinePropertySheet.h"
#include <math.h>
#include "Translate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLinePropertyPageGeneral, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPageGeneral property page

CLinePropertyPageGeneral::CLinePropertyPageGeneral() : CPropertyPage(CLinePropertyPageGeneral::IDD)
{
	m_psheet = NULL;
	//{{AFX_DATA_INIT(CLinePropertyPageGeneral)
	m_strText = _T("");
	m_UseRSS = FALSE;
	m_TextStatic = _TRANS(_T("Text:"));
	m_iTextLayout = -1;
	//}}AFX_DATA_INIT

	m_tempURL = _T("http://rss.cnn.com/rss/edition.rss");
}

CLinePropertyPageGeneral::~CLinePropertyPageGeneral()
{
}

void CLinePropertyPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePropertyPageGeneral)
	DDX_Control(pDX, IDC_DIRECTION, m_wndDirection);
	DDX_Control(pDX, IDC_SPEED_EDIT, m_wndSpeedEdit);
	DDX_Control(pDX, IDC_SPEED_DESC, m_wndSpeedDescription);
	DDX_Control(pDX, IDC_SPEED_SLIDER, m_wndSpeed);
	DDX_Text(pDX, IDC_TEXT, m_strText);
	DDX_Check(pDX, IDC_USE_RSS, m_UseRSS);
	DDX_Text(pDX, IDC_TEXT_STATIC, m_TextStatic);
	DDX_CBIndex(pDX, IDC_DIRECTION, m_iTextLayout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinePropertyPageGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CLinePropertyPageGeneral)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_STOP_PREVIEW, OnStopPreview)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_SPEED_EDIT, OnChangeSpeedEdit)
	ON_BN_CLICKED(IDC_USE_RSS, OnUseRss)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CLinePropertyPageGeneral::OnApply() 
{
	return CPropertyPage::OnApply();
}

void CLinePropertyPageGeneral::CalculatePosition(
				double Speed, int* pos,
				int* start, int* end, double* Decode )
{
	const int A = 0;
	const int B = 1000;
	const double flA = 0.5; //0.1;
	const double flB = 500;
	const double logA = log(flA);
	const double logB = log(flB);

	if( start != NULL )
	{
		*start = A;
	}

	if( end != NULL )
	{
		*end = B;
	}

	if( Decode == NULL )
	{
		ASSERT( pos != NULL );
		if( pos != NULL )
		{
			if( Speed < flA )
			{
				Speed = flA;
			}
			if( Speed > flB )
			{
				Speed = flB;
			}

			double logSpeed = log(Speed);
			*pos = int(A+double(B-A)*(logSpeed-logA)/(logB-logA));
		}
	}
	else
	{
		ASSERT( pos != NULL );
		if( pos != NULL )
		{
			double logSpeed = logA + (logB-logA)*double((*pos)-A)/double(B-A);
			//TRACE( _T("log: %9G <= %9G <= %9G\n"), logA, logSpeed, logB );
			*Decode = exp( logSpeed );
		}
	}
}

CString CLinePropertyPageGeneral::CalculateDescription( double Speed )
{
	CString s;

	if( Speed >= 20.0 )
	{
		s.Format( _TRANS(_T("%.0f pixels / second")), Speed );
	}
	else if( Speed >= 1.0 )
	{
		s.Format( _TRANS(_T("%.1f pixels / second")), Speed );
	}
	else
	{
		s.Format( _TRANS(_T("%.2f seconds / pixel (%.2f pixels / second)")),
			1.0/Speed, Speed );
	}

	return s;
}

CString CLinePropertyPageGeneral::CalculateSpeedEdit( double Speed )
{
	CString s;

	if( Speed >= 20.0 )
	{
		s.Format( _T("%.0f"), Speed );
	}
	else if( Speed >= 1.0 )
	{
		s.Format( _T("%.1f"), Speed );
	}
	else
	{
		s.Format( _T("%.2f"), Speed );
	}

	return s;
}


BOOL CLinePropertyPageGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

//	SetModified( TRUE );

	int pos = 0;
	int start = 0;
	int end = 0;
	CalculatePosition( m_ScrollingSpeed, &pos, &start, &end, NULL );

	m_wndSpeed.SetRange( start, end, FALSE );
	m_wndSpeed.SetPos( pos );

	CString desc = CalculateDescription( m_ScrollingSpeed );
	m_wndSpeedDescription.SetWindowText( desc );

	CString speedEdit = CalculateSpeedEdit( m_ScrollingSpeed );
	m_wndSpeedEdit.SetWindowText( speedEdit );

	if(m_UseRSS)
		m_TextStatic = _TRANS(_T("URL:"));
	else
		m_TextStatic = _TRANS(_T("Text:"));

	m_wndDirection.AddString(_T("Left to Right Text"));
	m_wndDirection.AddString(_T("Right to Left Text"));
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CLinePropertyPageGeneral::OnPreview() 
{
	UpdateData();
	m_psheet->OnPreview();
}

void CLinePropertyPageGeneral::OnStopPreview() 
{
	m_psheet->OnStopPreview();
}


void CLinePropertyPageGeneral::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( pScrollBar == (CWnd*) &m_wndSpeed )
	{
		OnChangeSpeedScrolling();
	}

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CLinePropertyPageGeneral::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( pScrollBar == (CWnd*) &m_wndSpeed )
	{
		OnChangeSpeedScrolling();
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CLinePropertyPageGeneral::OnChangeSpeedEdit() 
{
//	TRACE( _T("OnChangeSpeedEdit()\n") );
	CString s;
	m_wndSpeedEdit.GetWindowText( s );
	double x = _tcstod(  s, NULL );

	m_ScrollingSpeed = x;

	int pos = 0;
	CalculatePosition( m_ScrollingSpeed, &pos, NULL, NULL, NULL );
	m_wndSpeed.SetPos( pos );

	CString desc = CalculateDescription( m_ScrollingSpeed );
	m_wndSpeedDescription.SetWindowText( desc );
}


void CLinePropertyPageGeneral::OnChangeSpeedScrolling()
{
	int pos = m_wndSpeed.GetPos();
//	TRACE( _T("OnChangeSpeedScrolling( pos: %d )\n"), pos );

	double x = 0;
	CalculatePosition( 0, &pos, NULL, NULL, &x );

	m_ScrollingSpeed = x;

	CString desc = CalculateDescription( m_ScrollingSpeed );
	m_wndSpeedDescription.SetWindowText( desc );

	CString speedEdit = CalculateSpeedEdit( m_ScrollingSpeed );
	m_wndSpeedEdit.SetWindowText( speedEdit );
}

void CLinePropertyPageGeneral::OnUseRss() 
{
	UpdateData();

	if(m_UseRSS)
	{
		m_TextStatic = _TRANS(_T("URL:"));
		m_tempText = m_strText;
		m_strText = m_tempURL;
	} else
	{
		m_TextStatic = _TRANS(_T("Text:"));
		m_tempURL = m_strText;
		m_strText = m_tempText;
	}

	UpdateData(FALSE);
}
