// LinePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LinePropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinePropertySheet

IMPLEMENT_DYNAMIC(CLinePropertySheet, CPropertySheet)

CLinePropertySheet::CLinePropertySheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	m_PageGeneral.m_psheet = this;
	m_PageFont.m_psheet = this;
	m_PageWindow.m_psheet = this;
	m_PagePosition.m_psheet = this;

	AddPage(&m_PageGeneral);
	AddPage(&m_PageFont);
	AddPage(&m_PageWindow);
	AddPage(&m_PagePosition);
}

CLinePropertySheet::~CLinePropertySheet()
{
}


BEGIN_MESSAGE_MAP(CLinePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CLinePropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLinePropertySheet message handlers

int CLinePropertySheet::DoModal() 
{
	Load();

	int res = CPropertySheet::DoModal();

	Save();

	OnStopPreview();

	return res;
}

/////////////////////////////////////////////////////////////////////////////

void CLinePropertySheet::Load()
{
#define LINK( orig, temp )	temp = orig;

	// General:
	LINK( m_line.m_bUseRSS, m_PageGeneral.m_UseRSS )
	LINK( m_line.m_strText, m_PageGeneral.m_strText )
//	LINK( m_line.m_Timer , m_PageGeneral.m_iTimer )
//	LINK( m_line.m_Speed , m_PageGeneral.m_iSpeed )
	LINK( m_line.m_ScrollingSpeed , m_PageGeneral.m_ScrollingSpeed )

	// Font:
	LINK( m_line.m_font, m_PageFont.m_font )
	LINK( m_line.m_clTextColor, m_PageFont.m_clTextColor )
//	m_PageFont.m_strExampleText = m_line.m_strText;
	LINK( m_line.m_clTextSize, m_PageFont.m_clTextSize )

	// Window:
	LINK( m_line.m_clBackground, m_PageWindow.m_btnBackground.ColorProperty() )
	LINK( m_line.m_bResizing, m_PageWindow.m_bResizingWindow )
	LINK( m_line.m_bTransparent, m_PageWindow.m_bTransparent )
	LINK( m_line.m_clTransparentColor, m_PageWindow.m_btnTransparentColor.ColorProperty() )
	LINK( m_line.m_TransparencyFactor, m_PageWindow.m_TransparentFactor )

	LINK( m_line.m_Rect.left, m_PagePosition.m_left )
	LINK( m_line.m_Rect.top, m_PagePosition.m_top )
	LINK( m_line.m_Rect.right, m_PagePosition.m_right )
	LINK( m_line.m_Rect.bottom, m_PagePosition.m_bottom )

	LINK( m_line.m_Position, m_PagePosition.m_Position )

	// Direction
	LINK( m_line.m_iTextLayout, m_PageGeneral.m_iTextLayout)
}

/////////////////////////////////////////////////////////////////////////////

void CLinePropertySheet::Save()
{
#undef  LINK
#define LINK( orig, temp )	orig = temp;

	// General:
	LINK( m_line.m_bUseRSS, m_PageGeneral.m_UseRSS )
	LINK( m_line.m_strText, m_PageGeneral.m_strText )
//	LINK( m_line.m_Timer , m_PageGeneral.m_iTimer )
//	LINK( m_line.m_Speed , m_PageGeneral.m_iSpeed )
	LINK( m_line.m_ScrollingSpeed , m_PageGeneral.m_ScrollingSpeed )

	// Font:
	LINK( m_line.m_font, m_PageFont.m_font )
	LINK( m_line.m_clTextColor, m_PageFont.m_clTextColor )
//	m_PageFont.m_strExampleText = m_line.m_strText;
	LINK( m_line.m_clTextSize, m_PageFont.m_clTextSize )

	// Window:
	LINK( m_line.m_clBackground, m_PageWindow.m_btnBackground.ColorProperty() )
	LINK( m_line.m_bResizing , m_PageWindow.m_bResizingWindow )
	LINK( m_line.m_bTransparent, m_PageWindow.m_bTransparent )
	LINK( m_line.m_clTransparentColor, m_PageWindow.m_btnTransparentColor.ColorProperty() )
	LINK( m_line.m_TransparencyFactor, m_PageWindow.m_TransparentFactor )

	LINK( m_line.m_Rect.left, m_PagePosition.m_left )
	LINK( m_line.m_Rect.top, m_PagePosition.m_top )
	LINK( m_line.m_Rect.right, m_PagePosition.m_right )
	LINK( m_line.m_Rect.bottom, m_PagePosition.m_bottom )

	LINK( m_line.m_Position, m_PagePosition.m_Position )

	// Direction
	LINK( m_line.m_iTextLayout, m_PageGeneral.m_iTextLayout)
}

/////////////////////////////////////////////////////////////////////////////

void CLinePropertySheet::OnPreview()
{

}

/////////////////////////////////////////////////////////////////////////////

void CLinePropertySheet::OnStopPreview()
{

}

/////////////////////////////////////////////////////////////////////////////
