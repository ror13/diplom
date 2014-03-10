// WallPaper.cpp: implementation of the CWallPaper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallPaper.h"

#include "CommonApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWallPaper::CWallPaper()
{
	m_Wallpaper = _T("");
	m_WallpaperStyle = _T("0");
	m_TileWallpaper = _T("0");
}

CWallPaper::~CWallPaper()
{

}

bool CWallPaper::operator ==(const CWallPaper &src)
{
	bool res = true;
	
	res &= (m_Wallpaper == src.m_Wallpaper);
	res &= (m_WallpaperStyle == src.m_WallpaperStyle);
	res &= (m_TileWallpaper == src.m_TileWallpaper);
	
	return res;
}

bool CWallPaper::operator !=(const CWallPaper &src)
{
	return !(*this == src);
}

BOOL CWallPaper::GetRegistrySettings(CString strPrefix)
{
	BOOL res;
	TCHAR	szWallpaper[MAX_PATH];
	res = regReadRegString(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 
		strPrefix + _T("Wallpaper"), szWallpaper, SIZEOF_ARRAY(szWallpaper));
	if (!res)
		return FALSE;

	TCHAR	szWallpaperStyle[MAX_PATH];;
	res = regReadRegString(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 
		strPrefix + _T("WallpaperStyle"), szWallpaperStyle, SIZEOF_ARRAY(szWallpaperStyle));
	if (!res)
		return FALSE;

	TCHAR	szTileWallpaper[MAX_PATH];;
	res = regReadRegString(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 
		strPrefix + _T("TileWallpaper"), szTileWallpaper, SIZEOF_ARRAY(szTileWallpaper));
	if (!res)
		return FALSE;

	m_Wallpaper = szWallpaper;
	m_WallpaperStyle = szWallpaperStyle;
	m_TileWallpaper = szTileWallpaper;

	return TRUE;
}

BOOL CWallPaper::SetRegistrySettings(CString strPrefix)
{
	BOOL res;
	res = regWriteRegString(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 
		strPrefix + _T("Wallpaper"), m_Wallpaper);
	if (!res)
		return FALSE;
	
	res = regWriteRegString(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 
		strPrefix + _T("WallpaperStyle"), m_WallpaperStyle);
	if (!res)
		return FALSE;
	
	res = regWriteRegString(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 
		strPrefix + _T("TileWallpaper"), m_TileWallpaper);
	if (!res)
		return FALSE;
	
	return TRUE;
}

BOOL CWallPaper::SetWallPaper(BOOL bSendUpdate, BOOL bUpdateIni)
{
	return  SetWallPaper(m_Wallpaper, WALLPAPER_Saved, bSendUpdate, bUpdateIni);
}

BOOL CWallPaper::SetWallPaper(CString strFileName, WALLPAPER_POSITION pos, BOOL bSendUpdate, BOOL bUpdateIni)
{
	switch (pos)
	{
	case WALLPAPER_Saved:
		break;
	case WALLPAPER_Stretched:
		m_WallpaperStyle = _T("2");
		m_TileWallpaper = _T("0");
		break;
		
	case WALLPAPER_Centered:
		m_WallpaperStyle = _T("1");
		m_TileWallpaper = _T("0");
		break;
		
	case WALLPAPER_Tiled:
		m_WallpaperStyle = _T("1");
		m_TileWallpaper = _T("1");
		break;
	}

	m_Wallpaper = strFileName;

	SetRegistrySettings();

	UINT fWinIni = 0;
	if (bSendUpdate)
	{
		fWinIni |= SPIF_SENDWININICHANGE;
	}
	if (bUpdateIni)
	{
		fWinIni |= SPIF_UPDATEINIFILE;
	}

	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, 
		(PVOID)((LPCTSTR)strFileName),  
		fWinIni /*SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE*/ );

	return TRUE;
}

