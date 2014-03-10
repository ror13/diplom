// LogoWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LogoWnd.h"

#include "Resource.h"

#include "SignWallHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogoWnd

CLogoWnd::CLogoWnd()
{
	m_bLogoEnabled = TRUE;
#ifndef _DEBUG
	if (IsWallWithoutHost())
	{
		ShowCursor(FALSE);
	}
#endif
}

CLogoWnd::~CLogoWnd()
{
#ifndef _DEBUG
	if (IsWallWithoutHost())
	{
		ShowCursor(TRUE);
	}
#endif
}


BEGIN_MESSAGE_MAP(CLogoWnd, CWnd)
	//{{AFX_MSG_MAP(CLogoWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLogoWnd message handlers

int CLogoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	// Create name of logo.bmp file located in program directory
	CString szPath, szDrive, szDir, szLogoFile;
	
	// get file path information of this executable
	GetModuleFileName(NULL,szPath.GetBuffer(_MAX_PATH),_MAX_PATH);
	szPath.ReleaseBuffer();
	
	// split file path into drive, directory, file name, extension (incl. leading period)
	_tsplitpath(szPath, szDrive.GetBuffer(_MAX_DRIVE), szDir.GetBuffer(_MAX_DIR),
		NULL, NULL);
	szDrive.ReleaseBuffer();
	szDir.ReleaseBuffer();
	
	// name of logo bmp file stored in program directory
	szLogoFile = szDrive + szDir + _T("Logo") + _T(".BMP");
	
	// load logo
	HBITMAP bmp = (HBITMAP) ::LoadImage(NULL, szLogoFile, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	TRACE(_T("CLogoWnd: LoadImage %s %s\n"), bmp?_T("success"):_T("failed"), szLogoFile);
	// if load failed or attach failed set black logo to screen
	if (!bmp || !m_Logo.Attach(bmp))
	{
		TRACE(_T("CLogoWnd: LoadImage failed.\n"));
	}

	return 0;
}

void CLogoWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect; 
	GetClientRect(rect); 
	
	int SavedDC = dc.SaveDC();

	if (SavedDC == 0)
	{
		return;
	}
	
	dc.FillSolidRect(&rect, RGB(0, 0, 0));
	dc.SetBkMode(TRANSPARENT); 
	
	if((HBITMAP)m_Logo && m_bLogoEnabled)
	{
		// Get the size of the bitmap
		BITMAP bmpInfo;
		m_Logo.GetBitmap(&bmpInfo);
		
		// Create an in-memory DC compatible with the
		// display DC we're using to paint
		CDC dcMemory;
		BOOL bRet = dcMemory.CreateCompatibleDC(&dc);

		if (bRet)
		{
			CDC dcScaled;
			bRet = dcScaled.CreateCompatibleDC(&dc);

			if (bRet)
			{
				CBitmap bmScaled;
				bRet = bmScaled.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()); 

				if (bRet)
				{
					PVOID pObj = dcScaled.SelectObject(bmScaled);

					if (pObj)
					{
						// Select the bitmap into the in-memory DC
						CBitmap* pOldBitmap = dcMemory.SelectObject(&m_Logo);

						if (pOldBitmap)
						{
							// Copy the bits from the in-memory DC into the on-
							// screen DC to actually do the painting. Use the centerpoint
							// we computed for the target offset.
							dc.SetStretchBltMode(COLORONCOLOR);
							TRACE(_T("rect.Width %d, rect.Height %d bmpInfo.bmWidth %d, bmpInfo.bmHeight %d\n"), 
								rect.Width(), rect.Height(), bmpInfo.bmWidth, bmpInfo.bmHeight);
							dcScaled.StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory,
								0, 0, bmpInfo.bmWidth-1, bmpInfo.bmHeight-1, SRCCOPY);
							dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcScaled,
								0, 0, SRCCOPY);
							
							dcMemory.SelectObject(pOldBitmap);
						}
					}
				}
			}
		}
	}
	
	dc.RestoreDC(SavedDC);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CLogoWnd::EnableLogo(BOOL bEnable)
{
	BOOL bOldLogoEnabled = m_bLogoEnabled;

	TRACE(_T("CLogoWnd::EnableLogo %d\n"), bEnable);

	m_bLogoEnabled = bEnable;

	Invalidate();

	return bOldLogoEnabled;
}


void CLogoWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

// 	GetWindowRect(&(lpncsp->rgrc[0]));
// 	if (bCalcValidRects)
// 	{
// 		GetWindowRect(&(lpncsp->rgrc[1]));
// 		GetWindowRect(&(lpncsp->rgrc[2]));
// 	}
}

void CLogoWnd::OnNcPaint() 
{
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnNcPaint() for painting messages
}

void CLogoWnd::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanging(lpwndpos);
	
	// TODO: Add your message handler code here
	((LPWINDOWPOS)lpwndpos)->hwndInsertAfter = HWND_BOTTOM;	
}
