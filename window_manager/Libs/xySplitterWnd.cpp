// xySplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "xySplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CxSplitterWnd

CxSplitterWnd::CxSplitterWnd() 
{
}

CxSplitterWnd::~CxSplitterWnd()
{
}

bool CxSplitterWnd::BindWithControl(CWnd *parent, DWORD ctrlId)
{
	m_pParent = parent;
	m_minLeft = m_minRight = 10;

	if (!SubclassWindow(m_pParent->GetDlgItem(ctrlId)->GetSafeHwnd()))
		return FALSE;
	// Make sure to get mouse message from the dialog window
	DWORD style = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, style | SS_NOTIFY);

	return TRUE;
}

bool CxSplitterWnd::CreateInWnd(CWnd *parent, DWORD ctrlId, LPRECT rc)
{
	m_pParent = parent;
	if (!Create(_T("STATIC"), _T("Hi"), WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_EX_CLIENTEDGE | SS_GRAYRECT,
			*rc, parent, ctrlId))
		return FALSE;

	return TRUE;
}

void CxSplitterWnd::Unbind(void)
{
	if (!::IsWindow(m_hWnd))
		return;
	DetachAllCtrls();
	UnsubclassWindow();
}

void CxSplitterWnd::SetMinWidth(int left, int right)
{
	m_minLeft = left;
	m_minRight = right;
}

/////////////////////////////////////////////////////////////////////////////////////

bool CxSplitterWnd::AttachAsLeftCtrl(int ctrlid, 
									 bool bResize/* = true*/)
{
	return AttachAsLeftCtrl(GetParent()->GetDlgItem(ctrlid), bResize);
}

bool CxSplitterWnd::AttachAsLeftCtrl(CWnd *pWnd, 
									 bool bResize/* = true*/)
{
	CRect rcCtrl;
	CRect rcSplitter;
	pWnd->GetWindowRect(rcCtrl);
	GetWindowRect(rcSplitter);
	return AttachAsLeftCtrl(pWnd, rcSplitter.left - rcCtrl.right, bResize);
}

bool CxSplitterWnd::AttachAsLeftCtrl(int ctrlid, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	return AttachAsLeftCtrl(GetParent()->GetDlgItem(ctrlid), nMargin, bResize);
}

bool CxSplitterWnd::AttachAsLeftCtrl(CWnd *pWnd, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	ControlInfo info(pWnd);
	info.nMargin = nMargin;
	info.bResize = bResize;
	m_leftCtrls.Add(info);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////

bool CxSplitterWnd::AttachAsRightCtrl(int ctrlid, 
									 bool bResize/* = true*/)
{
	return AttachAsRightCtrl(GetParent()->GetDlgItem(ctrlid), bResize);
}

bool CxSplitterWnd::AttachAsRightCtrl(CWnd *pWnd, 
									 bool bResize/* = true*/)
{
	CRect rcCtrl;
	CRect rcSplitter;
	pWnd->GetWindowRect(rcCtrl);
	GetWindowRect(rcSplitter);
	return AttachAsRightCtrl(pWnd, rcCtrl.left - rcSplitter.right, bResize);
}

bool CxSplitterWnd::AttachAsRightCtrl(int ctrlid, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	return AttachAsRightCtrl(GetParent()->GetDlgItem(ctrlid), nMargin, bResize);
}

bool CxSplitterWnd::AttachAsRightCtrl(CWnd *pWnd, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	ControlInfo info(pWnd);
	info.nMargin = nMargin;
	info.bResize = bResize;
	m_rightCtrls.Add(info);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////

bool CxSplitterWnd::DetachAllCtrls(void)
{
	m_leftCtrls.RemoveAll();
	m_rightCtrls.RemoveAll();
	m_dwaRelationID.RemoveAll(); 
	return TRUE;
}

bool CxSplitterWnd::AttachToParent(DWORD ctrlId,DWORD ctrlParentID) 
{ 
	m_dwaRelationID.Add(ctrlParentID); 
	m_dwaRelationID.Add(ctrlId); 

	CRect rcCtrl; 
	CWnd* pCtrl; 
	pCtrl = m_pParent->GetDlgItem(ctrlId); 
	pCtrl->GetWindowRect(&rcCtrl); 
	m_pParent->ScreenToClient(&rcCtrl); 

	m_dwaRelationID.Add(rcCtrl.Width()); 

	CRect rcSplitter; 
	GetWindowRect(&rcSplitter); 
	m_pParent->ScreenToClient(&rcSplitter); 

	DWORD dwCtrlDistance; 
	dwCtrlDistance = rcCtrl.left - rcSplitter.right; 
	m_dwaRelationID.Add(dwCtrlDistance); 

	return TRUE; 
} 

void CxSplitterWnd::RecalcLayout(void) 
{ 
	CRect rcSplitter, rcCtrl; 

	GetWindowRect(&rcSplitter); 
	m_pParent->ScreenToClient(&rcSplitter); 

	ControlInfo cInfo;

	for (int i=0; i<m_leftCtrls.GetSize(); i++) 
	{ 
		cInfo = m_leftCtrls.GetAt(i); 
		cInfo.pWnd->GetWindowRect(&rcCtrl); 
		m_pParent->ScreenToClient(&rcCtrl); 
		if (cInfo.bResize)
		{
			rcCtrl.right = rcSplitter.left - cInfo.nMargin;
		}
		else	
		{
			int nWidth = rcCtrl.Width();
			rcCtrl.right = rcSplitter.left - cInfo.nMargin;
			rcCtrl.left = rcCtrl.right - nWidth;
		}
		cInfo.pWnd->MoveWindow(&rcCtrl, FALSE); 

		for (int j=0; j<m_dwaRelationID.GetSize(); j+=4) 
		{ 
			int nParentId = m_dwaRelationID.GetAt(j); 
			if (nParentId == cInfo.nID) 
			{ 
				int nChildId = m_dwaRelationID.GetAt(j+1); 
				CWnd *pCtrl = m_pParent->GetDlgItem(nChildId); 
				pCtrl->GetWindowRect(&rcCtrl); 
				m_pParent->ScreenToClient(&rcCtrl); 
				if (rcSplitter.left > rcCtrl.right)
				{ 
					if (rcSplitter.left < (LONG)(rcCtrl.left + m_dwaRelationID.GetAt(j+2))) 
					{ 
						rcCtrl.right = rcSplitter.left;// - m_nMargin; 
					}
					else 
						rcCtrl.right = rcCtrl.left + m_dwaRelationID.GetAt(j+2); 
				}
				else if (rcSplitter.left < rcCtrl.right) 
				{ 
					if (rcCtrl.left < rcSplitter.left) 
					{ 
						rcCtrl.right = rcSplitter.left;// - m_nMargin; 
					}
					else 
					{ 
						rcCtrl.right = rcCtrl.left; 
					} 
				} 
				pCtrl->MoveWindow(&rcCtrl, FALSE); 
			} 
		} 
	} 

	for (i=0; i<m_rightCtrls.GetSize(); i++) 
	{ 
		cInfo = m_rightCtrls.GetAt(i); 
		cInfo.pWnd->GetWindowRect(&rcCtrl); 
		m_pParent->ScreenToClient(&rcCtrl); 
		if (cInfo.bResize)
		{
			rcCtrl.left = rcSplitter.right + cInfo.nMargin;
		}
		else	
		{
			int nWidth = rcCtrl.Width();
			rcCtrl.left = rcSplitter.right + cInfo.nMargin;
			rcCtrl.right = rcCtrl.left + nWidth;
		}
		cInfo.pWnd->MoveWindow(&rcCtrl, FALSE); 

		RECT rcFrame = rcCtrl; 

		// ii?e?aiee (?aiue i?aaue) Ctrl aie??i ai?oaaaou i?aai?i e?ay ??oiaie?a
		CWnd* pCtrlWithMaxRight = NULL;
		int nMaxRight = -1;

		for (int j=0; j<m_dwaRelationID.GetSize(); j+=4) 
		{ 
			int nParentId = m_dwaRelationID.GetAt(j); 
			if (nParentId == cInfo.nID) 
			{ 
				int nChildId = m_dwaRelationID.GetAt(j+1); 
				CWnd *pCtrl = m_pParent->GetDlgItem(nChildId); 
				pCtrl->GetWindowRect(&rcCtrl); 
				m_pParent->ScreenToClient(&rcCtrl); 
				rcCtrl.left = rcSplitter.right + m_dwaRelationID.GetAt(j+3); 
				rcCtrl.right = rcCtrl.left + m_dwaRelationID.GetAt(j+2); 

				if (rcCtrl.right >= nMaxRight)
				{
					nMaxRight = rcCtrl.right;
					pCtrlWithMaxRight = pCtrl;
				}

				if (rcCtrl.right > rcFrame.right) 
				{ 
					if (rcCtrl.left < rcFrame.right) 
					{ 
						rcCtrl.right = rcFrame.right;// - m_nMargin; 
					}
					else 
					{ 
						rcCtrl.right = rcCtrl.left ; 
					} 
				} 
				pCtrl->MoveWindow(&rcCtrl, FALSE); 
			} 
		} 

		if (nMaxRight < rcFrame.right && pCtrlWithMaxRight)
		{
			// ii?e?aiee (?aiue i?aaue) Ctrl aie??i ai?oaaaou i?aai?i e?ay ??oiaie?a
			// => i?aao? ??aieoo iooyi?i ai eiioa
//			nChildId=m_dwaRelationID.GetAt(jWithMaxRight+1);
//			pCtrl = m_pParent->GetDlgItem(nChildId); 
			pCtrlWithMaxRight->GetWindowRect(&rcCtrl); 
			m_pParent->ScreenToClient(&rcCtrl); 
			rcCtrl.right = rcFrame.right;// - m_nMargin; 
			pCtrlWithMaxRight->MoveWindow(&rcCtrl, FALSE); 
		}
	} 
	m_pParent->Invalidate(); 
} 

bool CxSplitterWnd::GetMouseClipRect(LPRECT rectClip, CPoint pointInSplitter)
{
	CRect rectSplitter, rectTarget, rectParent, rectCtrl;
	ControlInfo cInfo;

	GetWindowRect(&rectSplitter);
	m_pParent->GetClientRect(&rectParent);
	m_pParent->ClientToScreen(&rectParent);

	rectTarget = rectSplitter;
	rectTarget.left = rectParent.left + m_minLeft;
	for (int i=0; i<m_leftCtrls.GetSize(); i++) 
	{
		cInfo = m_leftCtrls.GetAt(i);
		if (cInfo.bResize) //need at least one resize control.
		{
			cInfo.pWnd->GetWindowRect(&rectCtrl);
			if (rectTarget.left < rectCtrl.left + m_minLeft) 
			{
				rectTarget.left = rectCtrl.left + m_minLeft;
			}
		}
	}

	rectTarget.right = rectParent.right - m_minRight;
	for (i=0; i<m_rightCtrls.GetSize(); i++) 
	{
		cInfo = m_rightCtrls.GetAt(i);
		if (cInfo.bResize) //need at least one resize control.
		{
			cInfo.pWnd->GetWindowRect(&rectCtrl);
			if (rectTarget.right > rectCtrl.right - m_minRight) 
			{
				rectTarget.right = rectCtrl.right - m_minRight;
			}
		}
	}

	if (rectTarget.left >= rectTarget.right) 
	{
		TRACE(_T("No room to drag the x-splitter bar"));
		return FALSE;
	}

	rectClip->left = rectTarget.left + pointInSplitter.x;
	rectClip->right = rectTarget.right - (rectSplitter.Width() - pointInSplitter.x) + 1;
	rectClip->top = rectSplitter.top;
	rectClip->bottom = rectSplitter.bottom;

	return TRUE;
}

BEGIN_MESSAGE_MAP(CxSplitterWnd, CWnd)
	//{{AFX_MSG_MAP(CxSplitterWnd)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CxSplitterWnd message handlers

bool CxSplitterWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	// TODO: Add your message handler code here and/or call default
	HCURSOR hc = AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR);
	::SetCursor(hc);
	return TRUE;

	//return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CxSplitterWnd::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	// don't handle if capture already set
	if (::GetCapture() != NULL) return;

	// don't handle if no room to drag
	RECT rectMouseClip;
	if (!GetMouseClipRect(&rectMouseClip, point)) return;
	::ClipCursor(&rectMouseClip);

	// set capture to the window which received this message
	SetCapture();
	ASSERT(this == CWnd::GetCapture());

	// get DC for drawing
	CDC* pDrawDC;
	pDrawDC = m_pParent->GetDC();
	ASSERT_VALID(pDrawDC);

	int 	curX, curY;
	int 	xDiff, yDiff;
	CRect	rectSplitter, rectCur, rectOld;
	CSize	sizeBar;

	GetWindowRect(rectSplitter);
	sizeBar = CSize(rectSplitter.Width(), rectSplitter.Height());

	m_pParent->ScreenToClient(rectSplitter);
	pDrawDC->DrawDragRect(&rectSplitter, sizeBar, NULL, sizeBar);
	rectOld = rectCur = rectSplitter;
	xDiff = yDiff = 0;

	// get messages until capture lost or cancelled/accepted
	for (;;) {
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != this)
			break;

		switch (msg.message) {
		// handle movement/accept messages
		case WM_MOUSEMOVE:
			// handle resize cases (and part of move)
			curX = (int)(short)LOWORD(msg.lParam);
			curY = (int)(short)HIWORD(msg.lParam);

			xDiff = curX - point.x;
			yDiff = curY - point.y;

			rectCur = rectSplitter;
			rectCur.left += xDiff;
			rectCur.right += xDiff;
			pDrawDC->DrawDragRect(&rectCur, sizeBar, &rectOld, sizeBar);
			rectOld = rectCur;

			break;

		// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
			goto ExitLoop;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	pDrawDC->DrawDragRect(&rectCur, sizeBar, NULL, sizeBar);

	m_pParent->ReleaseDC(pDrawDC);
	ReleaseCapture();
	::ClipCursor(NULL);

	if (xDiff == 0) return;

	// move the splitter bar & re-position the attached Ctrls if necessary
	MoveWindow(rectCur, FALSE);
	RecalcLayout();

	m_pParent->SendMessage(WM_SPLITTER_MOVED, xDiff, GetDlgCtrlID());

	//CWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CySplitterWnd

CySplitterWnd::CySplitterWnd() 
{
}

CySplitterWnd::~CySplitterWnd()
{
}

bool CySplitterWnd::BindWithControl(CWnd *parent, DWORD ctrlId)
{
	m_pParent = parent;

	if (!SubclassWindow(m_pParent->GetDlgItem(ctrlId)->GetSafeHwnd()))
		return FALSE;
	
	// Make sure to get mouse message from the dialog window
	DWORD style = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, style | SS_NOTIFY);

	return TRUE;
}

bool CySplitterWnd::CreateInWnd(CWnd *parent, DWORD ctrlId, LPRECT rc)
{
	m_pParent = parent;

	if (!Create(_T("STATIC"), _T("Hi"), WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_EX_CLIENTEDGE | SS_GRAYRECT,
			*rc, parent, ctrlId))
		return FALSE;

	return TRUE;
}

void CySplitterWnd::Unbind(void)
{
	if (!::IsWindow(m_hWnd))
		return;
	DetachAllCtrls();
	UnsubclassWindow();
}

void CySplitterWnd::SetMinHeight(int above, int below)
{
	m_minAbove = above;
	m_minBelow = below;
}

/////////////////////////////////////////////////////////////////////////////////////

bool CySplitterWnd::AttachAsAboveCtrl(int ctrlid, 
									 bool bResize/* = true*/)
{
	return AttachAsAboveCtrl(GetParent()->GetDlgItem(ctrlid), bResize);
}

bool CySplitterWnd::AttachAsAboveCtrl(CWnd *pWnd, 
									 bool bResize/* = true*/)
{
	CRect rcCtrl;
	CRect rcSplitter;
	pWnd->GetWindowRect(rcCtrl);
	GetWindowRect(rcSplitter);
	return AttachAsAboveCtrl(pWnd, rcSplitter.top - rcCtrl.bottom, bResize);
}

bool CySplitterWnd::AttachAsAboveCtrl(int ctrlid, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	return AttachAsAboveCtrl(GetParent()->GetDlgItem(ctrlid), nMargin, bResize);
}

bool CySplitterWnd::AttachAsAboveCtrl(CWnd *pWnd, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	ControlInfo info(pWnd);
	info.nMargin = nMargin;
	info.bResize = bResize;
	m_aboveCtrls.Add(info);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////

bool CySplitterWnd::AttachAsBelowCtrl(int ctrlid, 
									 bool bResize/* = true*/)
{
	return AttachAsBelowCtrl(GetParent()->GetDlgItem(ctrlid), bResize);
}

bool CySplitterWnd::AttachAsBelowCtrl(CWnd *pWnd, 
									 bool bResize/* = true*/)
{
	CRect rcCtrl;
	CRect rcSplitter;
	pWnd->GetWindowRect(rcCtrl);
	GetWindowRect(rcSplitter);
	return AttachAsBelowCtrl(pWnd, rcCtrl.top - rcSplitter.bottom, bResize);
}

bool CySplitterWnd::AttachAsBelowCtrl(int ctrlid, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	return AttachAsBelowCtrl(GetParent()->GetDlgItem(ctrlid), nMargin, bResize);
}

bool CySplitterWnd::AttachAsBelowCtrl(CWnd *pWnd, 
									 int nMargin, 
									 bool bResize/* = true*/)
{
	ControlInfo info(pWnd);
	info.nMargin = nMargin;
	info.bResize = bResize;
	m_belowCtrls.Add(info);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////

bool CySplitterWnd::DetachAllCtrls(void)
{
	m_aboveCtrls.RemoveAll();
	m_belowCtrls.RemoveAll();
	m_dwaRelationID.RemoveAll(); 
	return TRUE;
}

bool CySplitterWnd::AttachToParent(DWORD ctrlId,DWORD ctrlParentID) 
{ 
	m_dwaRelationID.Add(ctrlParentID); 
	m_dwaRelationID.Add(ctrlId); 

	CRect rcCtrl; 
	CWnd* pCtrl; 
	pCtrl = m_pParent->GetDlgItem(ctrlId); 
	pCtrl->GetWindowRect(&rcCtrl); 
	m_pParent->ScreenToClient(&rcCtrl); 

	m_dwaRelationID.Add(rcCtrl.Height()); 

	CRect rcSplitter; 
	GetWindowRect(&rcSplitter); 
	m_pParent->ScreenToClient(&rcSplitter); 
	DWORD dwCtrlDistance; 
	dwCtrlDistance = rcCtrl.top - rcSplitter.bottom; 
	m_dwaRelationID.Add(dwCtrlDistance); 

	return TRUE; 
} 

void CySplitterWnd::RecalcLayout(void) 
{ 
	CRect rcSplitter, rcCtrl; 

	GetWindowRect(&rcSplitter); 
	m_pParent->ScreenToClient(&rcSplitter); 

	ControlInfo cInfo;

	for (int i=0; i<m_aboveCtrls.GetSize(); i++) 
	{ 
		cInfo = m_aboveCtrls.GetAt(i); 
		cInfo.pWnd->GetWindowRect(&rcCtrl); 
		m_pParent->ScreenToClient(&rcCtrl); 
		if (cInfo.bResize)
		{
			rcCtrl.bottom = rcSplitter.top - cInfo.nMargin; 
		}
		else	
		{
			int nWidth = rcCtrl.Width();
			rcCtrl.right = rcSplitter.left - cInfo.nMargin;
			rcCtrl.left = rcCtrl.right - nWidth;
		}
		cInfo.pWnd->MoveWindow(&rcCtrl, FALSE); 

		for (int j=0; j<m_dwaRelationID.GetSize(); j+=4) 
		{ 
			int nParentId = m_dwaRelationID.GetAt(j); 
			if (nParentId == cInfo.nID) 
			{ 
				int nChildId = m_dwaRelationID.GetAt(j+1); 
				CWnd *pCtrl = m_pParent->GetDlgItem(nChildId); 
				pCtrl->GetWindowRect(&rcCtrl); 
				m_pParent->ScreenToClient(&rcCtrl); 
				if (rcSplitter.top > rcCtrl.bottom)
				{ 
					if (rcSplitter.top < (LONG)(rcCtrl.top + m_dwaRelationID.GetAt(j+2))) 
					{ 
						rcCtrl.bottom = rcSplitter.top;// - m_nMargin; 
					}
					else 
						rcCtrl.bottom = rcCtrl.top + m_dwaRelationID.GetAt(j+2); 
				}
				else if (rcSplitter.top < rcCtrl.bottom) 
				{ 
					if (rcCtrl.top < rcSplitter.top) 
					{ 
						rcCtrl.bottom = rcSplitter.top;// - m_nMargin; 
					}
					else 
					{ 
						rcCtrl.bottom = rcCtrl.top; 
					} 
				} 
				pCtrl->MoveWindow(&rcCtrl, FALSE); 
			} 
		} 
	} 

	for (i=0; i<m_belowCtrls.GetSize(); i++) 
	{ 
		cInfo = m_belowCtrls.GetAt(i); 
		cInfo.pWnd->GetWindowRect(&rcCtrl); 
		m_pParent->ScreenToClient(&rcCtrl); 
		if (cInfo.bResize)
		{
			rcCtrl.top = rcSplitter.bottom + cInfo.nMargin;
		}
		else	
		{
			int nWidth = rcCtrl.Width();
			rcCtrl.top = rcSplitter.bottom + cInfo.nMargin;
			rcCtrl.bottom = rcCtrl.top + nWidth;
		}
		cInfo.pWnd->MoveWindow(&rcCtrl, FALSE); 

		RECT rcFrame = rcCtrl; 

		// ii?e?aiee (?aiue ieceee) Ctrl aie??i ai?oaaaou ie?i??i e?ay ??oiaie?a
		CWnd* pCtrlWithMaxBottom = NULL;
		int nMaxBottom = -1;

		for (int j=0; j<m_dwaRelationID.GetSize(); j+=4) 
		{ 
			int nParentId = m_dwaRelationID.GetAt(j); 
			if (nParentId == cInfo.nID) 
			{ 
				int nChildId = m_dwaRelationID.GetAt(j+1); 
				CWnd *pCtrl = m_pParent->GetDlgItem(nChildId); 
				pCtrl->GetWindowRect(&rcCtrl); 
				m_pParent->ScreenToClient(&rcCtrl); 
				rcCtrl.top = rcSplitter.bottom + m_dwaRelationID.GetAt(j+3); 
				rcCtrl.bottom = rcCtrl.top + m_dwaRelationID.GetAt(j+2); 

				if (rcCtrl.bottom > nMaxBottom)
				{
					nMaxBottom = rcCtrl.bottom;
					pCtrlWithMaxBottom = pCtrl;
				}

				if (rcCtrl.bottom > rcFrame.bottom) 
				{ 
					if (rcCtrl.top < rcFrame.bottom) 
					{ 
						rcCtrl.bottom = rcFrame.bottom;// - m_nMargin; 
					}
					else 
					{ 
						rcCtrl.bottom = rcCtrl.top ; 
					} 
				} 
				pCtrl->MoveWindow(&rcCtrl, FALSE); 
			} 
		}

		if (nMaxBottom < rcFrame.bottom && pCtrlWithMaxBottom)
		{
			// ii?e?aiee (?aiue ieceee) Ctrl aie??i ai?oaaaou ie?i??i e?ay ??oiaie?a
			// => ie?i?? ??aieoo iooyi?i ai eiioa
//			nChildId=m_dwaRelationID.GetAt(jWithMaxBottom+1);
//			pCtrl = m_pParent->GetDlgItem(nChildId); 
			pCtrlWithMaxBottom->GetWindowRect(&rcCtrl); 
			m_pParent->ScreenToClient(&rcCtrl); 
			rcCtrl.bottom = rcFrame.bottom;// - m_nMargin; 
			pCtrlWithMaxBottom->MoveWindow(&rcCtrl, FALSE); 
		}
	} 
	m_pParent->Invalidate(); 
} 

bool CySplitterWnd::GetMouseClipRect(LPRECT rectClip, CPoint pointInSplitter)
{
	CRect rectSplitter, rectTarget, rectParent, rectCtrl;
	ControlInfo cInfo;

	GetWindowRect(&rectSplitter);
	m_pParent->GetClientRect(&rectParent);
	m_pParent->ClientToScreen(&rectParent);

	rectTarget = rectSplitter;
	rectTarget.top = rectParent.top + m_minAbove;
	for (int i=0; i<m_aboveCtrls.GetSize(); i++) 
	{
		cInfo = m_aboveCtrls.GetAt(i);
		if (cInfo.bResize) //need at least one resize control.
		{
			cInfo.pWnd->GetWindowRect(&rectCtrl);
			if (rectTarget.top < rectCtrl.top + m_minAbove) 
			{
				rectTarget.top = rectCtrl.top + m_minAbove;
			}
		}
	}
	rectTarget.bottom = rectParent.bottom - m_minBelow;
	for (i=0; i<m_belowCtrls.GetSize(); i++) 
	{
		cInfo = m_belowCtrls.GetAt(i);
		if (cInfo.bResize) //need at least one resize control.
		{
			cInfo.pWnd->GetWindowRect(&rectCtrl);
			if (rectTarget.bottom > rectCtrl.bottom - m_minBelow) 
			{
				rectTarget.bottom = rectCtrl.bottom - m_minBelow;
			}
		}
	}

	if (rectTarget.top >= rectTarget.bottom) 
	{
		TRACE(_T("No room to drag the y-splitter bar"));
		return FALSE;
	}

	rectClip->left = rectSplitter.left;
	rectClip->right = rectSplitter.right;
	rectClip->top = rectTarget.top + pointInSplitter.y;
	rectClip->bottom = rectTarget.bottom - (rectSplitter.Height() - pointInSplitter.y) + 1;

	return TRUE;
}


BEGIN_MESSAGE_MAP(CySplitterWnd, CWnd)
	//{{AFX_MSG_MAP(CySplitterWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CySplitterWnd message handlers

bool CySplitterWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	// TODO: Add your message handler code here and/or call default
	::SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR));
	return TRUE;
	
	//return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CySplitterWnd::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	// don't handle if capture already set
	if (::GetCapture() != NULL) return;

	// don't handle if no room to drag
	RECT rectMouseClip;
	if (!GetMouseClipRect(&rectMouseClip, point)) return;
	::ClipCursor(&rectMouseClip);

	// set capture to the window which received this message
	SetCapture();
	ASSERT(this == CWnd::GetCapture());

	// get DC for drawing
	CDC* pDrawDC;
	pDrawDC = m_pParent->GetDC();
	ASSERT_VALID(pDrawDC);

	int 	curX, curY;
	int 	xDiff, yDiff;
	CRect	rectSplitter, rectCur, rectOld;
	CSize	sizeBar;

	GetWindowRect(rectSplitter);
	sizeBar = CSize(rectSplitter.Width(), rectSplitter.Height());

	m_pParent->ScreenToClient(rectSplitter);
	pDrawDC->DrawDragRect(&rectSplitter, sizeBar, NULL, sizeBar);
	rectOld = rectCur = rectSplitter;
	xDiff = yDiff = 0;

	// get messages until capture lost or cancelled/accepted
	for (;;) {
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != this)
			break;

		switch (msg.message) {
		// handle movement/accept messages
		case WM_MOUSEMOVE:
			// handle resize cases (and part of move)
			curX = (int)(short)LOWORD(msg.lParam);
			curY = (int)(short)HIWORD(msg.lParam);

			xDiff = curX - point.x;
			yDiff = curY - point.y;

			rectCur = rectSplitter;
			rectCur.top += yDiff;
			rectCur.bottom += yDiff;
			pDrawDC->DrawDragRect(&rectCur, sizeBar, &rectOld, sizeBar);
			rectOld = rectCur;

			break;

		// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
			goto ExitLoop;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	pDrawDC->DrawDragRect(&rectCur, sizeBar, NULL, sizeBar);

	m_pParent->ReleaseDC(pDrawDC);
	ReleaseCapture();
	::ClipCursor(NULL);

	if (yDiff == 0) return;

	// move the splitter bar & re-position the attached Ctrls if necessary
	MoveWindow(rectCur, FALSE);
	RecalcLayout();

	m_pParent->SendMessage(WM_SPLITTER_MOVED, yDiff, GetDlgCtrlID());

	//CWnd::OnLButtonDown(nFlags, point);
}
