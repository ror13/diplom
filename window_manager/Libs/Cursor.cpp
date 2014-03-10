//////////////////////////////////////////////////////////////
// File:		// Cursor.cpp
// File time:	// 28.03.2006	15:19
// Description: //////////////////////////////////////////////
// 
//

#include "stdafx.h"
//#include <windows.h>
#include "Cursor.h"
#include "Bitmap.h"
#include "Gdi.h"

//////////////////////////////////////////////////////////////

#pragma pack( push, 1 )

struct CUR_FILE_HEADER
{
	WORD	Reserved;		// = 0
	WORD	Type;			// = 2
	WORD	CursorCount;
};

struct CUR_HEADER
{
	BYTE	Width;	// most commonly = 32
	BYTE	Height;	// most commonly = 32
	BYTE	ColorCount;	// =0 !
	BYTE	Reserved;	// =0
	WORD	XHotspot;
	WORD	YHotspot;
	DWORD	SizeInBytes;	// Size of (InfoHeader + ANDBitmap + XORBitmap)
	DWORD	FileOffset;		// FilePos, where InfoHeader starts
};

/*
typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;		// Size of InfoHeader structure = 40
        LONG       biWidth;		// Cursor Width
        LONG       biHeight;	// Cursor Height (added height of XORbitmap and ANDbitmap)
        WORD       biPlanes;	// number of planes = 1
        WORD       biBitCount;	// bits per pixel = 1
        DWORD      biCompression;	// Type of Compression = 0
        DWORD      biSizeImage;		// Size of Image in Bytes = 0 (uncompressed)
        LONG       biXPelsPerMeter;	// unused = 0
        LONG       biYPelsPerMeter;	// unused = 0
        DWORD      biClrUsed;		// unused = 0
        DWORD      biClrImportant;	// unused = 0
} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;
*/

typedef RGBQUAD RGB_STRUCT;

// PALETTEENTRY

/*
struct RGB_STRUCT
{
	BYTE	Red;		// ??????????
	BYTE	Green;		// ??????????
	BYTE	Blue;		// ??????????
	BYTE	Reserved;	// ??????????
};
*/

#pragma pack( pop )

//////////////////////////////////////////////////////////////

BOOL LoadCursorFromFileMy( LPCTSTR szCursorFile, HCURSOR& hCursor,
						  CBitmap& bmp, CPalette* pPalette,
						  CBitmap& bmpMask, CPalette* pPaletteMask )
{
	hCursor = NULL;
//	hCursor = ::LoadCursor( NULL, IDC_WAIT );

	HANDLE	hFile = INVALID_HANDLE_VALUE;
	HANDLE	hMapping = NULL;
	PVOID	pView = NULL;
	DWORD	dwFileSize = 0;

	DWORD dwExitType = 0;
	CString strPosition = _T("Before try!");
	try
	{
		hFile = CreateFile( szCursorFile, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, 0, NULL );

		if( hFile == INVALID_HANDLE_VALUE )
			ERROR_TRY( _T("Open file error") );
		POS

		dwFileSize = GetFileSize( hFile, NULL );

		if( dwFileSize == INVALID_FILE_SIZE )
			ERROR_TRY( _T("Can't get file size") );
		POS

		hMapping = CreateFileMapping( hFile, NULL,
			PAGE_READONLY, 0, 0, NULL );

		if( hMapping == NULL )
			ERROR_TRY( _T("Can't create file mapping") );
		POS

		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;

		pView = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );

		if( pView == NULL )
			ERROR_TRY( _T("Can't map view of file") );
		POS

		CUR_FILE_HEADER* pFile = (CUR_FILE_HEADER*) pView;
		ASSERT( pFile->Type == 2 );
		ASSERT( pFile->Reserved == 0 );

		TRACE( _T("%d cursors\n"), pFile->CursorCount );

		POS

		for( int i=0; i<pFile->CursorCount; i++ )
		{
			POS

			CUR_HEADER* pHeader = (CUR_HEADER*)
				&PBYTE(pView)[ sizeof(CUR_FILE_HEADER) + i*sizeof(CUR_HEADER) ];

			POS

			TRACE( _T("cursor #%d: %d x %d\n"),
				i+1, pHeader->Width, pHeader->Height );
			TRACE( _T("cursor #%d: Hotspot: %d,%d\n"),
				i+1, pHeader->XHotspot, pHeader->YHotspot );
			TRACE( _T("cursor #%d: %d bytes from %d offset\n"),
				i+1, pHeader->SizeInBytes, pHeader->FileOffset );

			BITMAPINFOHEADER* pBmp = (BITMAPINFOHEADER*)
				&PBYTE(pView)[ pHeader->FileOffset ];

			TRACE( _T("cursor #%d bmp: %d x %d, %d bits, %d planes, %d colors\n"),
				i+1, pBmp->biWidth, pBmp->biHeight,
				pBmp->biBitCount, pBmp->biPlanes,
				pBmp->biClrUsed );
			TRACE( _T("cursor #%d bmp: compression: %d; %d bytes\n"),
				i+1, pBmp->biCompression, pBmp->biSizeImage );

			POS
			
			int colors = 
				pBmp->biBitCount > 8 ? 0 :
				( pBmp->biBitCount == 1 ? 2 : pBmp->biClrUsed );

			TRACE( _T("cursor #%d: %d colors in palette\n"),
				i+1, colors );

			if( pBmp->biSizeImage != 0 )
			{
				ASSERT( pHeader->SizeInBytes == 
					pBmp->biSize + colors*sizeof(RGB_STRUCT) + pBmp->biSizeImage );
			}

			POS

			RGB_STRUCT* pRgb = (RGB_STRUCT*)
				&PBYTE(pView)[ pHeader->FileOffset + pBmp->biSize ];

			POS

			PBYTE pXorData = (PBYTE)
				&PBYTE(pView)[ pHeader->FileOffset + pBmp->biSize
					+ colors*sizeof(RGB_STRUCT) ];

			POS

			PBYTE pAndData = NULL; //pXorData;

			BOOL bMaskExists = pHeader->Height*2 == pBmp->biHeight;
			if( bMaskExists )
			{
				DWORD row_length = ROUND_UP(pBmp->biBitCount*pBmp->biWidth,32)/8;
				DWORD offset = row_length * (pBmp->biHeight/2);

				pAndData = &pXorData[offset];

				if( pBmp->biSizeImage != 0 )
				{
					ASSERT( offset + 
						ROUND_UP(pBmp->biWidth*1, 32)*(pBmp->biHeight/2)/8
						== pBmp->biSizeImage );
				}
			}

			POS

			TRACE( _T("===\n") );

			if( i==0 )
			{
				POS
				HWND hDesktop = ::GetDesktopWindow();
				HDC hdc = GetWindowDC( hDesktop );

				ICONINFO icon_info;
				icon_info.fIcon = FALSE;
				icon_info.xHotspot = pHeader->XHotspot;
				icon_info.yHotspot = pHeader->YHotspot;

				char buf[sizeof(BITMAPINFO)+256*sizeof(RGBQUAD)];
				BITMAPINFO& info = *((BITMAPINFO*)&buf);
				info.bmiHeader = *pBmp;
				memcpy( info.bmiColors, pRgb, sizeof(RGB_STRUCT)*colors );

				if( bMaskExists )
				{
					info.bmiHeader.biHeight /=2;
					info.bmiHeader.biSizeImage = 0;
				}


				POS

				icon_info.hbmColor = 
					CreateDIBitmap( hdc, &info.bmiHeader, CBM_INIT, pXorData,
					&info, DIB_RGB_COLORS );

				POS

				bmp.DeleteObject();
				bmp.Detach();
				bmp.Attach( icon_info.hbmColor );

				info.bmiHeader.biSizeImage = 0;
				info.bmiHeader.biBitCount = 1;
				info.bmiHeader.biClrUsed = 2;

				info.bmiColors[0].rgbRed = 0;
				info.bmiColors[0].rgbGreen = 0;
				info.bmiColors[0].rgbBlue = 0;
				info.bmiColors[0].rgbReserved = 0;
				info.bmiColors[1].rgbRed = 0xff;
				info.bmiColors[1].rgbGreen = 0xff;
				info.bmiColors[1].rgbBlue = 0xff;
				info.bmiColors[1].rgbReserved = 0;

				POS

/*
				WriteLog( _T("Mask:\n") );

				DWORD row_length =
					ROUND_UP(info.bmiHeader.biBitCount*info.bmiHeader.biWidth,32)/8;

				for( int i=0; i<info.bmiHeader.biWidth; i++ )
				{
					PBYTE p = &pAndData[row_length*(info.bmiHeader.biWidth-1-i)];

					CString s;
					for( int j=0; j<info.bmiHeader.biHeight; j++ )
					{
						CString d;
						BOOL clr = FALSE;

						BYTE b = p[j/8];
						clr = 0 != (b&(1<<(7-j%8)));

						d.Format( _T("  %x"), clr );
						s += d;
					}
					WriteLog( _T("%s\n"), s );
				}
				WriteLog( _T("===\n") );
				POS
*/

				icon_info.hbmMask = bMaskExists ?
					CreateDIBitmap( hdc, &info.bmiHeader, CBM_INIT, pAndData,
					&info, DIB_RGB_COLORS )
					: NULL;

				POS

				bmpMask.DeleteObject();
				bmpMask.Detach();
				bmpMask.Attach( icon_info.hbmMask );

				hCursor = (HCURSOR) CreateIconIndirect( &icon_info );

				POS

				if( icon_info.hbmColor != NULL )
				{
//					DeleteObject( icon_info.hbmColor );
				}

				if( icon_info.hbmMask != NULL )
				{
//					DeleteObject( icon_info.hbmMask );
				}

				ReleaseDC( hDesktop, hdc );
				POS
			}
			POS
		}

		POS
		TRACE( _T("Cursor opened OK!\n") );
	}
	catch(CMyException e)
    {
		dwExitType = 2;
		TRACE( _T("CMyException: %s: lasterror=%d (line %d, file %s)"),
			e.m_strError, e.m_lLastError, e.m_iLine, e.m_pszFile );
	}
	catch(...)
    {
		dwExitType = 3;
		TRACE( _T("Unknown exception!!! (last pos - %s) %s, %d"), strPosition, _T(__FILE__), __LINE__ );
	}

	if( hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
	}

	if( hMapping != NULL )
	{
		CloseHandle( hMapping );
	}

	if( pView != NULL )
	{
		UnmapViewOfFile( pView );
	}

	return dwExitType == 0;
}

//////////////////////////////////////////////////////////////

void CaptureBitmap(
					IN	HBITMAP				hBmp,
					OUT	LPDWORD				pdwBitsLength,
					OUT	PBITMAP				pBitmap )
{
	ASSERT( pdwBitsLength!=NULL );
	*pdwBitsLength = 0;
	BOOL res;
	BITMAP& bitmap = *pBitmap;
	ZeroMemory( &bitmap, sizeof(bitmap) );

	//WriteLog( _T("GetObject( %X, %d, %X );"), hBmp, sizeof(bitmap), &bitmap );

	if( hBmp == NULL )
		return;

	res = GetObject( hBmp, sizeof(bitmap), &bitmap );
	if( !res )
		ERROR_TRY( _T("GetObject (BITMAP) error") );

	ASSERT( bitmap.bmPlanes == 1 );

	*pdwBitsLength = bitmap.bmWidthBytes * bitmap.bmHeight;

	TRACE( _T("Captured bitmap: %dx%dx%d (line - %d bytes), planes - %d, pBits - %X\n"),
		bitmap.bmWidth, bitmap.bmHeight, bitmap.bmBitsPixel, bitmap.bmWidthBytes, bitmap.bmPlanes, bitmap.bmBits );
}

//////////////////////////////////////////////////////////////////////////

BOOL SaveCursorToFile( HCURSOR hCursor, LPCTSTR szCursorFile )
{
	BOOL res;
	if( hCursor == NULL )
	{
		return FALSE;
	}

	ICONINFO info;
	ZeroMemory( &info, sizeof(info) );
	PBYTE p = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	DWORD dwExitType = 0;
	CString strPosition = _T("Before try!");
	try
	{
		POS;
		res = GetIconInfo( (HICON) hCursor, &info );
		if( !res )
			ERROR_TRY( _T("GetIconInfo error") );

		POS;
		DWORD dwColorLength = 0;
		BITMAP ColorBitmap = {0};
		CaptureBitmap( info.hbmColor, &dwColorLength, &ColorBitmap );

		POS;
		DWORD dwMaskLength = 0;
		BITMAP MaskBitmap = {0};
		CaptureBitmap( info.hbmMask, &dwMaskLength, &MaskBitmap );

		POS;

		ASSERT( MaskBitmap.bmBitsPixel == 1 );	// AND mask is always monochrome!

		POS;
		DWORD bytes = dwColorLength + dwMaskLength;
		p = new BYTE[ bytes ];
		ASSERT(p!=NULL);
		if( p == NULL )
		{
			TRACE( _T("Not enough memory to allocate %d bytes for icon buffer (%d + %d bytes)!"),
				bytes, dwMaskLength, dwColorLength );
			ERROR_TRY( _T("Not enough memory to allocate icon buffer") );
		}

		BOOL bMaskExists = dwMaskLength > 0;

		PBYTE pColorBits = p;
		PBYTE pMaskBits = bMaskExists ? p + dwColorLength : NULL;

		DWORD copyed = 0;
		POS;
		ASSERT( dwColorLength > 0 );
		if( dwColorLength > 0 )
		{
			copyed = GetBitmapBits( info.hbmColor, dwColorLength, pColorBits );
			if( copyed != dwColorLength )
				ERROR_TRY( _T("GetBitmapBits error") );
		}
		POS;
		if( dwMaskLength > 0 )
		{
			copyed = GetBitmapBits( info.hbmMask, dwMaskLength, pMaskBits );
			if( copyed != dwMaskLength )
				ERROR_TRY( _T("GetBitmapBits error") );
		}

		int width = ColorBitmap.bmWidth;
		int height = ColorBitmap.bmHeight;
		int bits = ColorBitmap.bmBitsPixel;
		int colors = bits <=8 ? (1<<bits) : 0;

		int iColorDataSize = (ROUND_UP(bits*width,32)/8) * height;
		int iMaskDataSize = bMaskExists ? (ROUND_UP(1*width,32)/8) * height : 0;

		int CursorSize =
			sizeof(BITMAPINFOHEADER) +
			colors*sizeof(RGB_STRUCT) +
			iColorDataSize +
			iMaskDataSize;

		int CursorOffset =
			sizeof(CUR_FILE_HEADER) +
			sizeof(CUR_HEADER);

		int iFileSize = CursorOffset +	CursorSize;

		hFile = CreateFile( szCursorFile, FILE_SHARE_WRITE, 0,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

		if( hFile == INVALID_HANDLE_VALUE )
			ERROR_TRY( _T("Opening file error") );

		BOOL res;
		DWORD written = 0;
		CUR_FILE_HEADER CursorFileHeader;
		CursorFileHeader.Reserved = 0;
		CursorFileHeader.Type = 2;
		CursorFileHeader.CursorCount = 1;

		res = WriteFile( hFile, &CursorFileHeader, sizeof(CursorFileHeader),
			&written, NULL );
		if( !res )
			ERROR_TRY( _T("Write file error") );

		CUR_HEADER CursorHeader;
		CursorHeader.Width = width;
		CursorHeader.Height = height;
		CursorHeader.ColorCount = 0;	// reserved
		CursorHeader.Reserved = 0;		// reserved
		CursorHeader.XHotspot = (WORD)info.xHotspot;
		CursorHeader.YHotspot = (WORD)info.yHotspot;
		CursorHeader.SizeInBytes = CursorSize;
		CursorHeader.FileOffset = CursorOffset;

		res = WriteFile( hFile, &CursorHeader, sizeof(CursorHeader),
			&written, NULL );
		if( !res )
			ERROR_TRY( _T("Write file error") );

		if( colors > 0 )
		{
			if( colors == 2 )
			{
				RGBQUAD bmiColors[2];
				bmiColors[0].rgbRed = 0;
				bmiColors[0].rgbGreen = 0;
				bmiColors[0].rgbBlue = 0;
				bmiColors[0].rgbReserved = 0;
				bmiColors[1].rgbRed = 0xff;
				bmiColors[1].rgbGreen = 0xff;
				bmiColors[1].rgbBlue = 0xff;
				bmiColors[1].rgbReserved = 0;

				res = WriteFile( hFile, &bmiColors, sizeof(bmiColors),
					&written, NULL );
				if( !res )
					ERROR_TRY( _T("Write file error") );
			}
			else
			{
				RGBQUAD bmiColors[256] = {0};
				HWND hWnd = NULL;
				HDC hdc = GetDC( hWnd );

				ASSERT( colors > 0 );
				BOOL res = copyDevicePalette( hdc, bmiColors, colors );
				ASSERT( res );
				ReleaseDC( hWnd, hdc );

//				ASSERT( FALSE );
//				// TODO: Don't know how to get cursor's palette!
				ERROR_TRY( _T("Don't know how to get cursor's palette!") );
			}
		}

		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof(bmih);
		bmih.biWidth = width;
		bmih.biHeight = bMaskExists ? height*2 : height;
		bmih.biPlanes = ColorBitmap.bmPlanes;
		bmih.biBitCount = ColorBitmap.bmBitsPixel;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = bytes;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = colors;
		bmih.biClrImportant = 0;

		DumpBitmapInfoHeader( bmih );

		res = WriteFile( hFile, &bmih, sizeof(bmih),
			&written, NULL );
		if( !res )
			ERROR_TRY( _T("Write file error") );

//		res = WriteFile( hFile, p, bytes,
//			&written, NULL );
//		if( !res )
//			ERROR_TRY( _T("Write file error") );

		int y;

		// Save Bitmaps in reverse line order:

		int ColorLineLength = ColorBitmap.bmWidthBytes;
		int MaskLineLength = MaskBitmap.bmWidthBytes;
		int LineLength = 0;

		LineLength = ColorLineLength;
		for( y=height-1; y>=0; y-- )
		{
			PBYTE pLine = &pColorBits[ y * LineLength ];

			if( bits == 32 )
			{
				RGBQUAD* pRgb = (RGBQUAD*) pLine;
				int pixels = LineLength / sizeof(RGBQUAD);
				DWORD* pMaskLine = (DWORD*) &pMaskBits[ y * MaskLineLength ];
				for( int i=0; i<pixels; i++ )
				{
					const int dwordIndex = i / (8*sizeof(*pMaskLine));
					int dwordBitNumber = i % (8*sizeof(*pMaskLine));
					dwordBitNumber = 8 * (dwordBitNumber / 8) +
						7 - dwordBitNumber % 8;
					BOOL bMaskColor =
						0 == (pMaskLine[dwordIndex] & (1<<dwordBitNumber) );

					if( bMaskColor && pRgb[i].rgbReserved == 0 )
					{
						// Если в системе 32 бита цвет:
						// 32-битный курсор загружается и сохраняется ОК
						// 256-цветный курсор при сохранении имеет
						// формат 32 бита, а в нём альфаканал у него весь
						// сброшен в ноль (прозрачно). Для этого можно сделать
						// след. поправку:
						// В режиме виднды 256 цветов надо дополнительно определить
						// палитру, а как - не знаю..... :-(
//						pRgb[i].rgbReserved = 0xff;		// ????????????
					}
				}
			}

			res = WriteFile( hFile, pLine, LineLength,
				&written, NULL );
			if( !res )
				ERROR_TRY( _T("Write file error") );
		}

		// Save Bitmaps in reverse line order:

		if( bMaskExists )
		{
			LineLength = MaskLineLength;
			for( y=height-1; y>=0; y-- )
			{
				PBYTE pLine = &pMaskBits[ y * LineLength ];
				res = WriteFile( hFile, pLine, LineLength,
					&written, NULL );
				if( !res )
					ERROR_TRY( _T("Write file error") );
			}
		}

		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
 	}
	catch(CMyException e)
    {
		dwExitType = 2;
		TRACE( _T("CMyException: %s: lasterror=%d (line %d, file %s)"),
			e.m_strError, e.m_lLastError, e.m_iLine, e.m_pszFile );
	}
	catch(...)
    {
		dwExitType = 3;
		TRACE( _T("Unknown exception!!! (last pos - %s) %s, %d"), strPosition, _T(__FILE__), __LINE__ );
	}

	if( info.hbmMask != NULL )
	{
		DeleteObject( info.hbmMask );
	}
	if( info.hbmColor != NULL )
	{
		DeleteObject( info.hbmColor );
	}

	if( hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
	}

	// Free resources:
	delete[] p;
	p = NULL;

	return dwExitType == 0;
}

//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
