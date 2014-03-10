// LinePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LinePropertyPagePosition.h"
#include "LinePropertySheet.h"

#include "CustomResolutionDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLinePropertyPagePosition, CPropertyPage)

enum
{
	RESOLUTION_640_480,
	RESOLUTION_800_600,
	RESOLUTION_1024_768,
	RESOLUTION_1280_1024,
	RESOLUTION_1600_1200,
	RESOLUTIONS_COUNT
};

/////////////////////////////////////////////////////////////////////////////
// CLinePropertyPagePosition property page

CLinePropertyPagePosition::CLinePropertyPagePosition() : CPropertyPage(CLinePropertyPagePosition::IDD)
{
	m_psheet = NULL;
	//{{AFX_DATA_INIT(CLinePropertyPagePosition)
	m_bottom = 0;
	m_left = 0;
	m_right = 0;
	m_top = 0;
	//}}AFX_DATA_INIT

	m_bDragging = FALSE;
	m_Position = CCreepingLineInfo::DOCKING_POS_NONE;
}

CLinePropertyPagePosition::~CLinePropertyPagePosition()
{
}

void CLinePropertyPagePosition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePropertyPagePosition)
	DDX_Control(pDX, IDC_POS, m_Pos);
	DDX_Control(pDX, IDC_RESOLUTION_COMBO, m_wndResolutionCombo);
	DDX_Control(pDX, IDC_MONITOR_FRAME, m_wndMonitorFrame);
	DDX_Control(pDX, IDC_RECT_TOP, m_wndTop);
	DDX_Control(pDX, IDC_RECT_RIGHT, m_wndRight);
	DDX_Control(pDX, IDC_RECT_LEFT, m_wndLeft);
	DDX_Control(pDX, IDC_RECT_BOTTOM, m_wndBottom);
	DDX_Text(pDX, IDC_RECT_BOTTOM, m_bottom);
	DDX_Text(pDX, IDC_RECT_LEFT, m_left);
	DDX_Text(pDX, IDC_RECT_RIGHT, m_right);
	DDX_Text(pDX, IDC_RECT_TOP, m_top);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinePropertyPagePosition, CPropertyPage)
	//{{AFX_MSG_MAP(CLinePropertyPagePosition)
	ON_EN_CHANGE(IDC_RECT_BOTTOM, OnChangeRectBottom)
	ON_EN_CHANGE(IDC_RECT_LEFT, OnChangeRectLeft)
	ON_EN_CHANGE(IDC_RECT_RIGHT, OnChangeRectRight)
	ON_EN_CHANGE(IDC_RECT_TOP, OnChangeRectTop)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_RESOLUTION_COMBO, OnSelchangeResolutionCombo)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_STOP_PREVIEW, OnStopPreview)
	ON_CBN_SELCHANGE(IDC_POS, OnSelchangePos)
	ON_BN_CLICKED(IDC_CUSTOM_RESOLUTIONS, OnCustomResolutions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLinePropertyPagePosition::OnPositionChange()
{
	CString s;

	m_wndLeft.GetWindowText( s );
	m_left = _ttoi( s );

	m_wndTop.GetWindowText( s );
	m_top = _ttoi( s );

	m_wndRight.GetWindowText( s );
	m_right = _ttoi( s );

	m_wndBottom.GetWindowText( s );
	m_bottom = _ttoi( s );

	CRect logic( m_left, m_top, m_right, m_bottom );
	m_wndMonitorFrame.m_rectWindow = logic;
	m_wndMonitorFrame.SendMessage( WM_WINDOW_POSITION_UPDATE_DATA );
}

void CLinePropertyPagePosition::OnChangeRectBottom() 
{
	OnPositionChange();
}

void CLinePropertyPagePosition::OnChangeRectLeft() 
{
	OnPositionChange();
}

void CLinePropertyPagePosition::OnChangeRectRight() 
{
	OnPositionChange();
}

void CLinePropertyPagePosition::OnChangeRectTop() 
{
	OnPositionChange();
}

BOOL CLinePropertyPagePosition::OnApply() 
{
	return CPropertyPage::OnApply();
}

BOOL CLinePropertyPagePosition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

//	DWORD dwStyle = GetClassLong( m_wndMonitorFrame.GetSafeHwnd(), GCL_STYLE );
//	dwStyle &= ~CS_PARENTDC;
//	SetClassLong( m_wndMonitorFrame.GetSafeHwnd(), GCL_STYLE, dwStyle );

	int iString;
	int i = 0;
	int DefaultIndex = 0;

	iString = m_wndResolutionCombo.InsertString( i++, _T("640x480") );
	m_wndResolutionCombo.SetItemData( iString, RESOLUTION_640_480 );
	iString = m_wndResolutionCombo.InsertString( i++, _T("800x600") );
	m_wndResolutionCombo.SetItemData( iString, RESOLUTION_800_600 );
	iString = m_wndResolutionCombo.InsertString( i++, _T("1024x768") );
	m_wndResolutionCombo.SetItemData( iString, RESOLUTION_1024_768 );

	DefaultIndex = iString;

	iString = m_wndResolutionCombo.InsertString( i++, _T("1280x1024") );
	m_wndResolutionCombo.SetItemData( iString, RESOLUTION_1280_1024 );
	iString = m_wndResolutionCombo.InsertString( i++, _T("1600x1200") );
	m_wndResolutionCombo.SetItemData( iString, RESOLUTION_1600_1200 );

	UpdateResolutionsControls();

	m_wndResolutionCombo.SetCurSel( DefaultIndex );
	OnSelchangeResolutionCombo();

//	SetModified( TRUE );
	OnPositionChange();

	m_Pos.SetCurSel(m_Position);
	EnableAbsolutePos(m_Position == CCreepingLineInfo::DOCKING_POS_NONE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////////

LRESULT CLinePropertyPagePosition::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WN_WINDOW_POSITION_CHANGED )
	{
		m_left = m_wndMonitorFrame.m_rectWindow.left;
		m_right = m_wndMonitorFrame.m_rectWindow.right;
		m_top = m_wndMonitorFrame.m_rectWindow.top;
		m_bottom = m_wndMonitorFrame.m_rectWindow.bottom;
		//OnPositionChange();
		UpdateData( FALSE );
		return 0;
	}

	return CPropertyPage::WindowProc(message, wParam, lParam);
}

void CLinePropertyPagePosition::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	if( bShow )
	{
		m_wndMonitorFrame.m_strText = m_psheet->m_PageGeneral.m_strText;
		m_wndMonitorFrame.m_font = m_psheet->m_PageFont.m_font;
		m_wndMonitorFrame.m_clTextColor = m_psheet->m_PageFont.m_clTextColor;
		m_wndMonitorFrame.m_clBackground = m_psheet->m_PageWindow.m_btnBackground.ColorProperty();

		m_wndMonitorFrame.SendMessage( WM_WINDOW_POSITION_UPDATE_DATA );
	}
}

void CLinePropertyPagePosition::OnSelchangeResolutionCombo() 
{
	int selected = m_wndResolutionCombo.GetCurSel();
	switch( selected )
	{
	case RESOLUTION_640_480:
		m_wndMonitorFrame.m_sizeScreen = CSize( 640, 480 );
		break;
	case RESOLUTION_800_600:
		m_wndMonitorFrame.m_sizeScreen = CSize( 800, 600 );
		break;
	case RESOLUTION_1024_768:
		m_wndMonitorFrame.m_sizeScreen = CSize( 1024, 768 );
		break;
	case RESOLUTION_1280_1024:
		m_wndMonitorFrame.m_sizeScreen = CSize( 1280, 1024 );
		break;
	case RESOLUTION_1600_1200:
		m_wndMonitorFrame.m_sizeScreen = CSize( 1600, 1200 );
		break;
	default:
		m_wndMonitorFrame.m_sizeScreen = CSize( g_CustomRes.GetX(selected-RESOLUTIONS_COUNT), g_CustomRes.GetY(selected-RESOLUTIONS_COUNT) );

//		ASSERT( FALSE && "Unknown resolution!" );
	}

	m_wndMonitorFrame.SendMessage( WM_WINDOW_POSITION_UPDATE_DATA );
}

void CLinePropertyPagePosition::OnPreview() 
{
	UpdateData();
	m_psheet->OnPreview();
}

void CLinePropertyPagePosition::OnStopPreview() 
{
	m_psheet->OnStopPreview();
}

void CLinePropertyPagePosition::OnSelchangePos() 
{
	int ind = m_Pos.GetCurSel();
	if(ind < 0)
		return;

	m_Position = ind;

	EnableAbsolutePos(m_Position == CCreepingLineInfo::DOCKING_POS_NONE);
}

void CLinePropertyPagePosition::EnableAbsolutePos(BOOL bEnable)
{
	GetDlgItem(IDC_RECT_LEFT)->EnableWindow(bEnable);
	GetDlgItem(IDC_RECT_RIGHT)->EnableWindow(bEnable);
	GetDlgItem(IDC_RESOLUTION_COMBO)->EnableWindow(bEnable);
	GetDlgItem(IDC_RECT_TOP)->EnableWindow(bEnable);
	GetDlgItem(IDC_RECT_BOTTOM)->EnableWindow(bEnable);
	GetDlgItem(IDC_MONITOR_FRAME)->EnableWindow(bEnable);
	GetDlgItem(IDC_MONITOR_FRAME)->RedrawWindow();
	GetDlgItem(IDC_CUSTOM_RESOLUTIONS)->EnableWindow(bEnable);
	GetDlgItem(IDC_CUSTOM_RESOLUTIONS)->RedrawWindow();
}

void CLinePropertyPagePosition::OnCustomResolutions() 
{
	if (CustomResolutionsDialogDoModal() == IDOK)
	{
		// update global custom resolutions
		UpdateResolutionsControls();
		
		return;
	}
}

void CLinePropertyPagePosition::UpdateResolutionsControls()
{
	int iString;
	int DefaultIndex = 0;

	while (m_wndResolutionCombo.GetCount() > RESOLUTIONS_COUNT)
	{
		m_wndResolutionCombo.DeleteString(RESOLUTIONS_COUNT);
	}

	g_CustomRes.LoadFromFile();
	for (int i = 0; i < g_CustomRes.Size(); i++)
	{
		CCustomResolution& CustomResolution = g_CustomRes.Get(i);
		
		iString = m_wndResolutionCombo.InsertString( RESOLUTIONS_COUNT+i, g_CustomRes.GetStringFromIndex(i) );
		m_wndResolutionCombo.SetItemData( iString, RESOLUTIONS_COUNT+i );
	}
}
