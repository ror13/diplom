////////////////////////////////////////////////////////////////////////
//  (MFC independent)
//  class CNotifyIconEx implementation 
//  Version 1.07 (Sept 21 2001)
//
////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _UNICODE
    #ifndef UNICODE
        #define UNICODE
    #endif
#else
    #ifdef UNICODE
        #define _UNICODE
    #endif 
#endif

#include <windows.h>
#include <tchar.h>
#include "notifyiconex.h"

//////////////////////////////////////////////////////////////////////////
// CNotifyIconExClient

BOOL CNotifyIconExClient::OnAdjustNIContextMenu(UINT uID, HMENU& hMenu, UINT& uFlags, int& nSubMenu)
{
	return TRUE;
}
void CNotifyIconExClient::OnLButtonDown(UINT uID)
{
}
void CNotifyIconExClient::OnLButtonUp(UINT uID)
{
}
void CNotifyIconExClient::OnLButtonDblclk(UINT uID)
{
}
void CNotifyIconExClient::OnRButtonDown(UINT uID)
{
}
void CNotifyIconExClient::OnRButtonDblclk(UINT uID)
{
}
void CNotifyIconExClient::OnMouseMove(UINT uID)
{
}

//////////////////////////////////////////////////////////////////////////
// CNotifyIconEx

CNotifyIconEx::CNotifyIconEx(HWND hWnd, UINT nIDRes, UINT uRegisteredMessage,
							 HINSTANCE hInst, LPCTSTR szTip)
: CNotifyIcon(hWnd, nIDRes, uRegisteredMessage, hInst, szTip)
{
	OnLButtonDown   = NULL;
	OnLButtonUp     = NULL;
	OnLButtonDblclk = NULL;
	OnRButtonDown   = NULL;
	OnRButtonDblclk = NULL;
	OnMouseMove     = NULL;
	m_pClient		= NULL;
};

CNotifyIconEx::CNotifyIconEx(HWND hWnd, UINT nID, HINSTANCE hInst,
			  LPCTSTR szIconRes, LPCTSTR szTip, LPCTSTR szMenuRes,
			  UINT uRegisteredMessage)
: CNotifyIcon(hWnd, nID, hInst, szIconRes, szTip, szMenuRes, uRegisteredMessage) 
{
	OnLButtonDown   = NULL;
	OnLButtonUp     = NULL;
	OnLButtonDblclk = NULL;
	OnRButtonDown   = NULL;
	OnRButtonDblclk = NULL;
	OnMouseMove     = NULL;
	m_pClient		= NULL;
};

CNotifyIconEx::CNotifyIconEx(HWND hWnd, UINT nID, HINSTANCE hInst,
			  UINT uIconRes, LPCTSTR szTip, UINT uMenuRes,
			  UINT uRegisteredMessage)
: CNotifyIcon(hWnd, nID, hInst, uIconRes, szTip, uMenuRes, uRegisteredMessage) 
{
	OnLButtonDown   = NULL;
	OnLButtonUp     = NULL;
	OnLButtonDblclk = NULL;
	OnRButtonDown   = NULL;
	OnRButtonDblclk = NULL;
	OnMouseMove     = NULL;
	m_pClient		= NULL;
};

void CNotifyIconEx::SetClient(CNotifyIconExClient	*pClient)
{
	m_pClient = pClient;
}

void CNotifyIconEx::Dispatch(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static const UINT uTaskbarCreatedMessageId = 
        ::RegisterWindowMessage(_T("TaskbarCreated"));

    if(uTaskbarCreatedMessageId == msg)
    {
        if(m_bAdded)
            Restore();
        return;
    }

    if(m_hWnd != hWnd || m_uRegisteredMessage != msg || m_uID != wParam)
        return;
	
    switch(lParam)
    {
    case WM_MOUSEMOVE:
		if(m_pClient)
			m_pClient->OnMouseMove(wParam);
		else if(OnMouseMove)
			OnMouseMove(hWnd, wParam);
        break;
    case WM_LBUTTONDOWN:
		if(m_pClient)
			m_pClient->OnLButtonDown(wParam);
		else if(OnLButtonDown)
			OnLButtonDown(hWnd, wParam);
		break;
    case WM_LBUTTONUP:
		if(m_pClient)
			m_pClient->OnLButtonUp(wParam);
		else if(OnLButtonUp)
			OnLButtonUp(hWnd, wParam);
		break;
    case WM_LBUTTONDBLCLK:
		if(m_pClient)
			m_pClient->OnLButtonDblclk(wParam);
		else if(OnLButtonDblclk)
			OnLButtonDblclk(hWnd, wParam);
		break;
    case WM_RBUTTONDOWN:
		if(m_pClient)
			m_pClient->OnRButtonDown(wParam);
		else if(OnRButtonDown)
			OnRButtonDown(hWnd, wParam);
		break;
	case WM_RBUTTONUP:
		if(OnRButtonUp)
			OnRButtonUp(hWnd, wParam);
		else
			OnNIContextMenu(hWnd, wParam);
		break;
    case WM_RBUTTONDBLCLK:
		if(m_pClient)
			m_pClient->OnRButtonDblclk(wParam);
		else if(OnRButtonDblclk)
			OnRButtonDblclk(hWnd, wParam);
		break;
    }
}

void CNotifyIconEx::OnNIContextMenu(HWND hWnd, UINT uID)
{
    int nSubMenu = 0;
    if(!m_lpszMenu)
        return;
    
	HINSTANCE hInst = m_hInst;
    if(!hInst)
        hInst = (HINSTANCE) ::GetModuleHandle(NULL);

    POINT p;
    GetCursorPos(&p);
    
	HMENU hPopup = ::LoadMenu(hInst, m_lpszMenu);
    if(!hPopup)
        return;
    
	UINT  uFlags = TPM_BOTTOMALIGN | TPM_CENTERALIGN;
	
    if(m_pClient)
	{
        if(!m_pClient->OnAdjustNIContextMenu(uID, hPopup, uFlags, nSubMenu))
            return;
	} else if(AdjustNIContextMenu)
	{
        if(!AdjustNIContextMenu(hWnd, uID, hPopup, uFlags, nSubMenu))
            return;
	}
		
	HMENU hSub = ::GetSubMenu(hPopup, nSubMenu);
	if(hSub) 
	{
		SetForegroundWindow(hWnd);
		TrackPopupMenuEx(hSub, uFlags, p.x, p.y, hWnd, NULL);
		PostMessage(hWnd, WM_NULL, 0, 0);
	}
	::DestroyMenu(hPopup);
}
