// IdentifyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "IdentifyWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIdentifyWnd

CIdentifyWnd::CIdentifyWnd(int Num)
{
	m_Num = Num;
}

CIdentifyWnd::~CIdentifyWnd()
{
}


BEGIN_MESSAGE_MAP(CIdentifyWnd, CWnd)
	//{{AFX_MSG_MAP(CIdentifyWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIdentifyWnd message handlers

void CIdentifyWnd::OnDraw(class CDC *)
{
}

void CIdentifyWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CString str;
	str.Format(_T("%d"), m_Num);
	
	int s = dc.SaveDC();
	
	dc.SetBkMode(TRANSPARENT);
	
	
	CFont font1;
	CFont font2;
	
	CRect Monitor;
	GetClientRect(&Monitor);
	int h = Monitor.bottom - Monitor.top;

	dc.FillSolidRect(&Monitor, 0);
	
//	font1.CreateFont(h - h/11, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0,0,0,
//		0,_T("Arial"));
	font2.CreateFont(h - h/10, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0,0,0,
		0,_T("Arial "));
	
//	dc.SelectObject(&font1);
	
//	dc.DrawText(str, -1, &Monitor, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
	dc.SelectObject(&font2);
	dc.SetTextColor(RGB(255,255,255));
	dc.DrawText(str, -1, &Monitor, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
	dc.RestoreDC(s);
}

BOOL CIdentifyWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (cs.lpszClass == NULL)
	{
		cs.lpszClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_SAVEBITS);
	}
	return TRUE;
}
