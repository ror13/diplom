// WallPaper.h: interface for the CWallPaper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WALLPAPER_H__5BAD2C4C_9444_43CB_89C7_387038AB1103__INCLUDED_)
#define AFX_WALLPAPER_H__5BAD2C4C_9444_43CB_89C7_387038AB1103__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum WALLPAPER_POSITION
{
	WALLPAPER_Saved,
	WALLPAPER_Stretched,
	WALLPAPER_Centered,
	WALLPAPER_Tiled,
};

class CWallPaper  
{
public:
	CWallPaper();
	virtual ~CWallPaper();

	bool operator ==(const CWallPaper &src);
	bool operator !=(const CWallPaper &src);
	
	// Read wallpaper data from registry
	BOOL	GetRegistrySettings(CString strPrefix = _T(""));
	// Write wallpaper data to registry
	BOOL	SetRegistrySettings(CString strPrefix = _T(""));

	// attach new wallpaper to desktop
	BOOL	SetWallPaper(BOOL bSendUpdate = TRUE, BOOL bUpdateIni = TRUE);
	BOOL	SetWallPaper(CString strFileName, WALLPAPER_POSITION pos, BOOL bSendUpdate = TRUE, BOOL bUpdateIni = TRUE);

//private:
	CString		m_Wallpaper;
	CString		m_WallpaperStyle;
	CString		m_TileWallpaper;

};



#endif // !defined(AFX_WALLPAPER_H__5BAD2C4C_9444_43CB_89C7_387038AB1103__INCLUDED_)
