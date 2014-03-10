// WndDropTarget.h: interface for the CWndDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDDROPTARGET_H__83D820D6_E37A_4331_9660_45F24336600A__INCLUDED_)
#define AFX_WNDDROPTARGET_H__83D820D6_E37A_4331_9660_45F24336600A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndDropTargetClient
{
public:
	virtual BOOL OnDrop(COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point) = 0;
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
		DWORD grfKeyState, CPoint point) = 0;
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
		DWORD grfKeyState, CPoint point) = 0;
	virtual void OnDragLeave() = 0;
};

class CWndDropTarget : public COleDropTarget  
{
public:
	CWndDropTarget();
	virtual ~CWndDropTarget();

	void SetClient(CWndDropTargetClient* pWnd);
	
protected:
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);

	CWndDropTargetClient	*m_pClient;
};

#endif // !defined(AFX_WNDDROPTARGET_H__83D820D6_E37A_4331_9660_45F24336600A__INCLUDED_)
