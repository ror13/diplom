//////////////////////////////////////////////////////////////
// File:		// Gdi.h
// File time:	// 3.08.2004	17:29
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _GDI_H_UID000008357638F
#define _GDI_H_UID000008357638F


//////////////////////////////////////////////////////////////

inline SIZE GetDesktopSize()
{
	CRect r;
	::GetWindowRect( ::GetDesktopWindow(), &r );
	return CSize( r.Width(), r.Height() );
/*
	SIZE sz;
	HWND hDesktopWnd = NULL; //::GetDesktopWindow();
	{
		HDC hDc = ::GetDC(hDesktopWnd);
		sz.cx = ::GetDeviceCaps(hDc, HORZRES);
		sz.cy = ::GetDeviceCaps(hDc, VERTRES);
		::ReleaseDC(hDesktopWnd, hDc);
	}
	return sz;
*/
}

//////////////////////////////////////////////////////////////

inline int GetDesktopPixels()
{
//	CRect r;
//	::GetWindowRect( ::GetDesktopWindow(), &r );
//	return CSize( r.Width(), r.Height() );
//*
//	SIZE sz;
	int pixels = 0;
	HWND hDesktopWnd = NULL; //::GetDesktopWindow();
	{
		HDC hDc = ::GetDC(hDesktopWnd);
//		sz.cx = ::GetDeviceCaps(hDc, HORZRES);
//		sz.cy = ::GetDeviceCaps(hDc, VERTRES);
		pixels = ::GetDeviceCaps(hDc, BITSPIXEL);
		::ReleaseDC(hDesktopWnd, hDc);
	}
//	return sz;
	return pixels;
//*/
}

/////////////////////////////////////////////////////////////////////////////

inline CString DumpRect( CRect r )
{
	CString s;
//	s.Format( _T("x1=%3d, y1=%3d,  x2=%3d, y2=%3d"),
//		r.left, r.top, r.right, r.bottom );
	s.Format( _T("x1=%3d, y1=%3d,  width=%3d, height=%3d"),
		r.left, r.top, r.Width(), r.Height() );
	return s;
}

/////////////////////////////////////////////////////////////////////////////

#ifndef NO_WriteLog_FUNC

inline BOOL copyDevicePalette( HDC dc, RGBQUAD* pRgbPalette, UINT ColorCount )
{
	ASSERT( ColorCount>=0 && ColorCount <= 256 );
	PALETTEENTRY syspalette[256];
	ZeroMemory( &syspalette, sizeof(syspalette) );

	UINT entries;
	entries = ::GetSystemPaletteEntries( dc, 0, 256, NULL );
//	ASSERT( entries == ColorCount );
	entries = ::GetSystemPaletteEntries( dc, 0, ColorCount, syspalette );

//	CDC* pDC = CDC::FromHandle( dc );
//	CPalette* pPalette = pDC->GetCurrentPalette();
//	UINT entries = pPalette->GetPaletteEntries( 0, 256, syspalette );

	if (entries == 0)
	{
//		static BOOL bFirst = TRUE;
//		if( bFirst )
//		{
//			bFirst = FALSE;
//		}

		// resorting to standard 16 colour palette:
//		ASSERT( ColorCount == 16 );

		if( ColorCount == 16 )
		{
			TRACE( _T("!!!Resorting to standard 16 colour palette...") );
			for (unsigned int i=0;i<ColorCount;i++)
			{
				int v = (i%16) >= 8 ? 127 : 255;
				pRgbPalette[i].rgbRed		= i & 1 ? v : 0;
				pRgbPalette[i].rgbGreen		= i & 2 ? v : 0;
				pRgbPalette[i].rgbBlue		= i & 4 ? v : 0;
				pRgbPalette[i].rgbReserved	= 0;
			}
			return TRUE;
		}

		return FALSE;
	}

	// - Update the bitmap's stored copy of the palette
	for( UINT i=0; i<entries; i++ )
	{
		ASSERT( syspalette[i].peFlags == 0 );
		pRgbPalette[i].rgbRed		= syspalette[i].peRed;
		pRgbPalette[i].rgbGreen		= syspalette[i].peGreen;
		pRgbPalette[i].rgbBlue		= syspalette[i].peBlue;
		pRgbPalette[i].rgbReserved	= 0;
	}

	return ColorCount == entries;
}

#endif //ifdef PRESENTER_PRJ

/////////////////////////////////////////////////////////////////////////////

#endif //ifndef _GDI_H_UID000008357638F
