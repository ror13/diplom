////////////////////////////////////////////////////////////////////////
//  (MFC independent)
//  class CNotifyIconEx 
//  Version 1.07 (Sept 21 2001)
//
////////////////////////////////////////////////////////////////////////
#ifndef IV_NOTIFYICONEX_class_H
#define IV_NOTIFYICONEX_class_H

#include "notifyicon.h"

class CNotifyIconExClient
{
public:
	virtual BOOL OnAdjustNIContextMenu(UINT uID, HMENU& hMenu, UINT& uFlags, int& nSubMenu);
	virtual void OnLButtonDown(UINT uID);
	virtual void OnLButtonUp(UINT uID);
	virtual void OnLButtonDblclk(UINT uID);
	virtual void OnRButtonDown(UINT uID);
	virtual void OnRButtonDblclk(UINT uID);
	virtual void OnMouseMove(UINT uID);
};

class CNotifyIconEx : public CNotifyIcon  
{
public:
	CNotifyIconEx(HWND hWnd, UINT nIDRes, UINT uRegisteredMessage = 0,
		HINSTANCE hInst = NULL, LPCTSTR   szTip = NULL);

	CNotifyIconEx(HWND hWnd, UINT nID, HINSTANCE hInst, LPCTSTR szIconRes,
		LPCTSTR szTip = NULL, LPCTSTR szMenuRes = NULL, UINT uRegisteredMessage = 0);

	CNotifyIconEx(HWND hWnd, UINT nID, HINSTANCE hInst, UINT uIconRes,
		LPCTSTR szTip = NULL, UINT uMenuRes = 0, UINT uRegisteredMessage = 0);

	void SetClient(CNotifyIconExClient	*pClient);

	virtual void Dispatch(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	MOUSE_HANDLER    OnLButtonDown;
	MOUSE_HANDLER    OnLButtonUp;
	MOUSE_HANDLER    OnLButtonDblclk;
	MOUSE_HANDLER    OnRButtonDown;
	MOUSE_HANDLER    OnRButtonDblclk;
	MOUSE_HANDLER    OnMouseMove;

protected:
	CNotifyIconExClient	*m_pClient;

	void OnNIContextMenu(HWND hWnd, UINT uID);
};

inline void NICProcessWndProc( CNotifyIconEx* pnix, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(pnix!=NULL)
		pnix->Dispatch(hWnd, msg, wParam, lParam);
}

#endif 
