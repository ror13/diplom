// LogoWallpaper.cpp: implementation of the CLogoWallpaper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogoWallpaper.h"

#include "WallPaper.h"

#include "WallMonConfig.h"
#include "SignWallHelper.h"

#include "FileOperation.h"

#include "ximage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogoWallpaper::CLogoWallpaper()
{
	m_WallPaper.GetRegistrySettings();
}

CLogoWallpaper::~CLogoWallpaper()
{

}

BOOL CLogoWallpaper::MakeWallPaper(BOOL bDrawLogo, BOOL bUpdateAndSend)
{
	TRACE(_T("CLogoWallpaper::MakeWallPaper bDrawLogo %d bUpdateAndSend %d\n"), bDrawLogo, bUpdateAndSend);
	// Create name of logo.bmp file located in program directory
	CString szPath, szDrive, szDir, szLogoFile;
	CString szLogoWallPaperFile;
	
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
	szLogoWallPaperFile = szDrive + szDir + _T("ClientWallPaperLogo") + _T(".BMP");

	CBitmapEx BitmapLogo;

	TRACE(_T("Desktopinfo GetSystemMetrics width %d, height %d\n"), GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));

	CWallPaper OldWallPaper;
	OldWallPaper.GetRegistrySettings(OLD_WALLPAPER_PREFIX);
	if (OldWallPaper.m_Wallpaper == _T("")
		/* || m_WallPaper.m_Wallpaper.CompareNoCase(OldWallPaper.m_Wallpaper) != 0*/)
	{
		m_WallPaper.SetRegistrySettings(OLD_WALLPAPER_PREFIX);
	}

	//CxImage Load and set to bitmap

	CBitmapEx BitmapCurrentWallpaper;
	//BitmapCurrentWallpaper.Load(m_WallPaper.m_Wallpaper);
	TRACE(_T("%s"), m_WallPaper.m_Wallpaper);

	if (!m_WallPaper.m_Wallpaper.IsEmpty() &&
		m_WallPaper.m_Wallpaper != _T("") &&
		IsFileExists(m_WallPaper.m_Wallpaper))
	{
		TRACE(_T("CLogoWallpaper::MakeWallPaper 1\n"));
		CxImage Image;
		Image.Load(m_WallPaper.m_Wallpaper);

		if (Image.IsValid())
		{
			TRACE(_T("CLogoWallpaper::MakeWallPaper 2\n"));
			HBITMAP hbCurrentWallpaper;
			
			hbCurrentWallpaper = Image.MakeBitmap();
			
			if (hbCurrentWallpaper)
			{
				BitmapCurrentWallpaper.Load(hbCurrentWallpaper);
				DeleteObject(hbCurrentWallpaper);
			}
		}
	}

	CBitmapEx BitmapWall;
	BitmapWall.Create(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
	BitmapWall.Clear();
	if (BitmapCurrentWallpaper.IsValid())
	{
		if (m_WallPaper.m_WallpaperStyle == _T("2"))
		{
			BitmapCurrentWallpaper.Scale2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
	}

	BOOL bIsWallWithoutHost;
	bIsWallWithoutHost = IsWallWithoutHost();
	
	CWallMonConfig Conf;
	Conf.GetSystemConfig();

	MON_CONF_ARRAY::iterator m;
	for(m = Conf.m_Mons.begin(); m != Conf.m_Mons.end(); m++)
	{
		MON_CONF &mon_conf = (*m);
		
		TRACE(_T("DeviceName %s, DeviceString %s, m_MonNum %d m_MonRect %d %d x %d %d, m_bAttached %d, m_bPrimary %d, m_MonPosCol %d, m_MonPosRow %d\n"), 
			mon_conf.m_DeviceName,
			mon_conf.m_DeviceString,
			mon_conf.m_MonNum,
			mon_conf.m_MonRect.left, mon_conf.m_MonRect.top, mon_conf.m_MonRect.right, mon_conf.m_MonRect.bottom,
			mon_conf.m_bAttached,
			mon_conf.m_bPrimary,
			mon_conf.m_MonPosCol,
			mon_conf.m_MonPosRow);

		TRACE(_T("m_MonRect %d %d x %d %d\n"), 
			mon_conf.m_MonRect.left, mon_conf.m_MonRect.top, mon_conf.m_MonRect.right, mon_conf.m_MonRect.bottom);
		
		if (mon_conf.m_bAttached)
		{
			if (!BitmapLogo.IsValid())
			{
				BitmapLogo.Load(szLogoFile);
				BitmapLogo.Scale2(mon_conf.m_MonRect.Width(), mon_conf.m_MonRect.Height());
			}

			if (bIsWallWithoutHost)
			{
				if (bDrawLogo)
				{
					BitmapWall.Draw(mon_conf.m_MonRect.left, mon_conf.m_MonRect.top, 
						mon_conf.m_MonRect.Width(), mon_conf.m_MonRect.Height(), BitmapLogo, 0, 0);
				}
			}
			else
			{
				if (!mon_conf.m_bPrimary)
				{
					if (bDrawLogo)
					{
						BitmapWall.Draw(mon_conf.m_MonRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN), mon_conf.m_MonRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN), 
							mon_conf.m_MonRect.Width(), mon_conf.m_MonRect.Height(), BitmapLogo, 0, 0);
					}
				}
				else
				{
					if (BitmapCurrentWallpaper.IsValid())
					{
						BitmapWall.Draw(mon_conf.m_MonRect.left, mon_conf.m_MonRect.top, 
							mon_conf.m_MonRect.Width(), mon_conf.m_MonRect.Height(), BitmapCurrentWallpaper, 0, 0);
					}
				}
			}

		}
	}

	//CString strWallPaperFileName = _T("WallPaper.bmp");
	
	BitmapWall.Save(szLogoWallPaperFile);

//	m_WallPaper.SetWallPaper(szLogoWallPaperFile, WALLPAPER_Tiled);
	m_WallPaper.SetWallPaper(szLogoWallPaperFile, WALLPAPER_Tiled, bUpdateAndSend, bUpdateAndSend);

	return TRUE;
}

BOOL CLogoWallpaper::RevertWallPaper()
{
	m_WallPaper.GetRegistrySettings(OLD_WALLPAPER_PREFIX);
	m_WallPaper.SetWallPaper();

	return TRUE;
}

BOOL CLogoWallpaper::IsWallPaperEQU(CString strWallPaperName)
{
	return m_WallPaper.m_Wallpaper.CompareNoCase(strWallPaperName) == 0;
}

BOOL CLogoWallpaper::IsWallPaperEQU(CWallPaper &WallPaper)
{
	return m_WallPaper == WallPaper;
}
