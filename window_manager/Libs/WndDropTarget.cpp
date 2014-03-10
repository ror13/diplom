// WndDropTarget.cpp: implementation of the CWndDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "presenter.h"
#include "WndDropTarget.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndDropTarget::CWndDropTarget()
{
	m_pClient = NULL;
}

CWndDropTarget::~CWndDropTarget()
{

}

void CWndDropTarget::SetClient(CWndDropTargetClient* pWnd)
{
	m_pClient = pWnd;
}

BOOL CWndDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
									DROPEFFECT dropEffect, CPoint point)
{
	return m_pClient->OnDrop(pDataObject, dropEffect, point);
}

DROPEFFECT CWndDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
									DWORD dwKeyState, CPoint point)
{
	return m_pClient->OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT CWndDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
									DWORD dwKeyState, CPoint point)
{
	return m_pClient->OnDragOver(pDataObject, dwKeyState, point);
}

void CWndDropTarget::OnDragLeave(CWnd* pWnd)
{
	m_pClient->OnDragLeave();
}
