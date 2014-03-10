// LogoWallpaper.h: interface for the CLogoWallpaper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGOWALLPAPER_H__CA4BEBE9_6EFD_439B_A909_6772D5701BFF__INCLUDED_)
#define AFX_LOGOWALLPAPER_H__CA4BEBE9_6EFD_439B_A909_6772D5701BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define OLD_WALLPAPER_PREFIX	_T("Old")

#include "WallPaper.h"
#include "BitmapEx.h"

class CLogoWallpaper  
{
public:
	CLogoWallpaper();
	virtual ~CLogoWallpaper();

	BOOL	MakeWallPaper(BOOL bDrawLogo = TRUE, BOOL bUpdateAndSend = TRUE);
	BOOL	RevertWallPaper();
	BOOL	IsWallPaperEQU(CString strWallPaperName);
	BOOL	IsWallPaperEQU(CWallPaper &WallPaper);

//private:
	CWallPaper	m_WallPaper;

};

#endif // !defined(AFX_LOGOWALLPAPER_H__CA4BEBE9_6EFD_439B_A909_6772D5701BFF__INCLUDED_)
