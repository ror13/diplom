// Tip.cpp : implementation file
//

#include "stdafx.h"
#include "Tip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTip

CTip::CTip()
{
	// Register the window class if it has not already been registered.
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if(!(::GetClassInfo(hInst, _T("TITLE"), &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_SAVEBITS;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0;
		wndcls.hInstance		= hInst;
		wndcls.hIcon			= NULL;
		wndcls.hCursor			= LoadCursor( hInst, IDC_ARROW );
		wndcls.hbrBackground	= (HBRUSH)(COLOR_INFOBK +1);
		wndcls.lpszMenuName		= NULL;
		wndcls.lpszClassName	= _T("TITLE");

		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}
	m_bCreated	= NULL;
	m_Count		= 0;
}

CTip::~CTip()
{
}



BEGIN_MESSAGE_MAP(CTip, CWnd)
	//{{AFX_MSG_MAP(CTip)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTip message handlers

BOOL CTip::Create(CWnd * pParentWnd)
{
	ASSERT_VALID(pParentWnd);

    // Already created?
    if (m_bCreated)
        return TRUE;

	DWORD dwStyle = WS_BORDER | WS_POPUP; 
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
	m_pParentWnd = pParentWnd;

	m_bCreated = CreateEx(dwExStyle, _T("TITLE"), NULL, dwStyle, 
                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		                  NULL, NULL, NULL );

    return m_bCreated;
}

void CTip::Hide()
{
  	if (!::IsWindow(GetSafeHwnd()))
        return;

	ShowWindow( SW_HIDE );
}

CSize CTip::GetTextSize(LPCTSTR lpszTitleText, CDC *pDC)
{
	CString strTitle;
	strTitle += lpszTitleText;
	
	int LinesCount = 1;

	CSize size(0, 0);
	
	int	res = strTitle.Find('\n');
	
	while(res != -1)
	{
		LinesCount++;
		
		CString strdop = strTitle.Left(res);
		CSize line_size = pDC->GetTextExtent(strdop);
		
		if (line_size.cx > size.cx)
			size = line_size;
		
		strTitle = strTitle.Mid(res + 1);
		res = strTitle.Find('\n');
	}
	
	CSize line_size = pDC->GetTextExtent(strTitle);
	if (line_size.cx > size.cx)
		size = line_size;
	
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    size.cx += tm.tmOverhang;
	size.cy *= LinesCount;

	return size;
}

void CTip::Show(CPoint point, LPCTSTR lpszTitleText)
{
    if (!IsWindow(m_hWnd))
        Create(m_pParentWnd);

	ASSERT( ::IsWindow( GetSafeHwnd() ) );

	CClientDC dc(this);

	
	CFont *pOld = dc.SelectObject(m_pParentWnd->GetFont());
	
	CSize size = GetTextSize(lpszTitleText, &dc);
	int offset = dc.GetTextExtent(_T(" ")).cx;

	CRect rectDisplay;
	rectDisplay.left = point.x;
	rectDisplay.right = point.x + size.cx + offset * 2 + 2;
	rectDisplay.top = point.y - (size.cy + 2);
	rectDisplay.bottom = point.y;
	
	// Show the titletip
    SetWindowPos( &wndTop, rectDisplay.left, rectDisplay.top, 
        rectDisplay.Width(), rectDisplay.Height(), 
        SWP_SHOWWINDOW|SWP_NOACTIVATE );
	dc.SetBkMode( TRANSPARENT );

	CRect client_rect;
	GetClientRect(&client_rect);
	FillRect(dc.GetSafeHdc(), &client_rect, (HBRUSH)(COLOR_INFOBK +1));

	CRect text_rect(offset, 0, size.cx+offset, size.cy);
	dc.DrawText(lpszTitleText, &text_rect, DT_LEFT|DT_TOP);

	dc.SelectObject(pOld);
}

