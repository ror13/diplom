// dragsourcestatic.cpp : implementation file
//

#include "stdafx.h"
#include "..\Presenter\presenter.h"
#include "dragsourcestatic.h"

#include "Icon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragSourceStatic

CDragSourceStatic::CDragSourceStatic()
{
}

CDragSourceStatic::~CDragSourceStatic()
{
}


BEGIN_MESSAGE_MAP(CDragSourceStatic, CStatic)
	//{{AFX_MSG_MAP(CDragSourceStatic)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDragSourceStatic::SetData(UINT ClipFormat, int Param)
{
	m_ClipFormat = ClipFormat;
	m_Param = Param;
}

/////////////////////////////////////////////////////////////////////////////
// CDragSourceStatic message handlers

void CDragSourceStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_DataSrc.Empty();

	HICON hIcon = GetIcon();

	int BufSize = 0;
	SaveIconToMem(hIcon, NULL, BufSize);

	int IconOffset = sizeof(POINT) + sizeof(int);
	BufSize += IconOffset;

 	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, BufSize);
	LPBYTE pBuf = (LPBYTE)GlobalLock(hMem);

	LPPOINT pPoint = (LPPOINT)pBuf;
	*pPoint = point;

	int *pType = (int*)(pBuf + sizeof(POINT));
	*pType = m_Param;

	SaveIconToMem(hIcon, pBuf + IconOffset, BufSize);

	GlobalUnlock(hMem);

	STGMEDIUM	Data;
	Data.tymed = TYMED_HGLOBAL;
	Data.hGlobal = hMem;
	Data.pUnkForRelease = NULL;

	m_DataSrc.CacheData(m_ClipFormat, &Data);

	DROPEFFECT de = m_DataSrc.DoDragDrop();
	if(de == DROPEFFECT_NONE)
		GlobalFree(hMem);
	
	CStatic::OnLButtonDown(nFlags, point);
}
