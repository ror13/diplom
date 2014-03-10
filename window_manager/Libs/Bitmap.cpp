//////////////////////////////////////////////////////////////
// File:		// Bitmap.cpp
// File time:	// 3.08.2004	17:29
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "Bitmap.h"

//////////////////////////////////////////////////////////////
/*
CBitmap::CBitmap (void)
{
}

CBitmap::~CBitmap (void)
{
}
*/

//////////////////////////////////////////////////////////////

HBITMAP LoadBitmap(LPCTSTR szFilename)
{
	ASSERT(szFilename);

	HBITMAP hBitmap = NULL;
	hBitmap = (HBITMAP)LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	return hBitmap;
}

//////////////////////////////////////////////////////////////

// LoadBMP		- Loads a BMP file and creates a logical palette for it.
// Returns		- TRUE for success
// sBMPFile		- Full path of the BMP file
// phDIB		- Pointer to a HGLOBAL variable to hold the loaded bitmap
//			  Memory is allocated by this function but should be 
//			  released by the caller.
// pPal			- Will hold the logical palette
BOOL LoadBMP( LPCTSTR sBMPFile, HGLOBAL *phDIB, CPalette *pPal )
{
	CFile file;
	if( !file.Open( sBMPFile, CFile::modeRead) )
		return FALSE;

	BITMAPFILEHEADER bmfHeader;
	long nFileLen;

	nFileLen = file.GetLength();


	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;

	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;

	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nFileLen);
	if (hDIB == 0)
		return FALSE;

	// Read the remainder of the bitmap file.
	if (file.ReadHuge((LPSTR)hDIB, nFileLen - sizeof(BITMAPFILEHEADER)) !=
		nFileLen - sizeof(BITMAPFILEHEADER) )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}



	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;

	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed :
						1 << bmInfo.bmiHeader.biBitCount;

	// Create the palette
	if( nColors <= 256 )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}

		pPal->CreatePalette( pLP );

		delete[] pLP;
	}

	*phDIB = hDIB;
	return TRUE;
}

//////////////////////////////////////////////////////////////

void DrawDIB( CDC* pDC, HGLOBAL hDIB, CPalette *pPal )
{
	LPVOID	lpDIBBits;		// Pointer to DIB bits
	BOOL	bSuccess=FALSE; 	// Success/fail flag

	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed :
						1 << bmInfo.bmiHeader.biBitCount;

	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors +
			bmInfo.bmiHeader.biClrUsed) +
			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	if( pPal && (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) )
	{
		pDC->SelectPalette(pPal, FALSE);
		pDC->RealizePalette();
	}

	::SetDIBitsToDevice(pDC->m_hDC, 				// hDC
				   0,					// DestX
				   0,					// DestY
				   bmInfo.bmiHeader.biWidth,		// nDestWidth
				   bmInfo.bmiHeader.biHeight,		// nDestHeight
				   0,					// SrcX
				   0,					// SrcY
				   0,					// nStartScan
				   bmInfo.bmiHeader.biHeight,		// nNumScans
				   lpDIBBits,				// lpBits
				   (LPBITMAPINFO)hDIB,			// lpBitsInfo
				   DIB_RGB_COLORS); 			// wUsage
}

//////////////////////////////////////////////////////////////

// LoadBMPImage	- Loads a BMP file and creates a bitmap GDI object
//		  also creates logical palette for it.
// Returns	- TRUE for success
// sBMPFile	- Full path of the BMP file
// bitmap	- The bitmap object to initialize
// pPal		- Will hold the logical palette. Can be NULL
BOOL LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal )
{
	CFile file;
	if( !file.Open( sBMPFile, CFile::modeRead) )
		return FALSE;

	BITMAPFILEHEADER bmfHeader;

	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;

	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;

	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = file.GetLength() - sizeof(BITMAPFILEHEADER);
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
		return FALSE;

	// Read the remainder of the bitmap file.
	if (file.ReadHuge((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}


	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;

	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed :
						1 << bmiHeader.biBitCount;

	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) +
			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	// Create the logical palette
	if( pPal != NULL )
	{
		// Create the palette
		if( nColors <= 256 )
		{
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;

			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
				pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}

			pPal->CreatePalette( pLP );

			delete[] pLP;
		}
	}

	CClientDC dc(NULL);
	CPalette* pOldPalette = NULL;
	if( pPal )
	{
		pOldPalette = dc.SelectPalette( pPal, FALSE );
		dc.RealizePalette();
	}

	HBITMAP hBmp = CreateDIBitmap( dc.m_hDC,		// handle to device context 
				&bmiHeader,	// pointer to bitmap size and format data 
				CBM_INIT,	// initialization flag 
				lpDIBBits,	// pointer to initialization data 
				&bmInfo,	// pointer to bitmap color-format data 
				DIB_RGB_COLORS);		// color-data usage 
	bitmap.Attach( hBmp );

	if( pOldPalette )
		dc.SelectPalette( pOldPalette, FALSE );

	::GlobalFree(hDIB);
	return TRUE;
}

//////////////////////////////////////////////////////////////

void DrawBitmap(CDC* pDC, CBitmap& bitmap, CPalette *pPal, CPoint pt )
{
	// Create a compatible memory DC
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	memDC.SelectObject( &bitmap );

	// Select and realize the palette
	if( pPal != NULL && pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		pDC->SelectPalette( pPal, FALSE );
		pDC->RealizePalette();
	}

	BITMAP bm;
	bitmap.GetBitmap( &bm );

	pDC->BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, &memDC, 0, 0,SRCCOPY);
}

//////////////////////////////////////////////////////////////

// Function LoadAnImage: accepts a file name and returns a HBITMAP.
// On error, it returns 0.
HBITMAP LoadAnImage(char* FileName)
{
	// Use IPicture stuff to use JPG / GIF files
	IPicture* p;
	IStream* s;
//	IPersistStream* ps;
	HGLOBAL hG;
	void* pp;
	FILE* fp;

	// Read file in memory
	fp = fopen(FileName,"rb");
	if (!fp)
		return NULL;

	fseek(fp,0,SEEK_END);
	int fs = ftell(fp);
	fseek(fp,0,SEEK_SET);
	hG = GlobalAlloc(GPTR,fs);

	if (!hG)
	{
		fclose(fp);
		return NULL;
	}
	pp = (void*)hG;
	fread(pp,1,fs,fp);
	fclose(fp);

	// Create an IStream so IPicture can
	CreateStreamOnHGlobal(hG,false,&s);
	if (!s)
	{
		GlobalFree(hG);
		return NULL;
	}

	OleLoadPicture(s,0,false,IID_IPicture,(void**)&p);

	if (!p)
	{
		s->Release();
		GlobalFree(hG);
		return NULL;
	}
	s->Release();
	GlobalFree(hG);

	HBITMAP hB = 0;
	p->get_Handle((unsigned int*)&hB);

	// Copy the image. Necessary, because upon p's release,
	// the handle is destroyed.
	HBITMAP hBB = (HBITMAP)CopyImage(hB,IMAGE_BITMAP,0,0,
						LR_COPYRETURNORG);

	p->Release();
	return hBB;
}

//////////////////////////////////////////////////////////////
//Function Name :HANDLE DDBToDIB(CBitmap &bitmap,
//                               DWORD dwCompression,
//                               CPalette *pPal)
// URL:			http://www.codeguru.com/Cpp/G-M/bitmap/capturing/article.php/c4919/
// Author:		Shailesh N. Kanzariya
//
//Parameters:
//  CBitmap &bitmap : Compatible Device Dependent Bitmap
//  DWORD dwCompression : Compression format for Bitmap must not be
//  BI_BITFIELDS in this case.
//  CPalette *pPal : Pointer to Palette. If this is NULL, the
//  default system palette will be used.
//
//////////////////////////////////////////////////////////////

HANDLE DDBToDIB(CBitmap &bitmap, DWORD dwCompression,
                CPalette *pPal)
{

  BITMAP              bm;
  BITMAPINFOHEADER    bi;
  LPBITMAPINFOHEADER  lpbi;
  DWORD               dwLen;
  HANDLE              hDIB;
  HANDLE              handle;
  HDC                 hDC;
  HPALETTE            hPal;


  ASSERT( bitmap.GetSafeHandle() );

  // The function has no arg for bitfields
  if( dwCompression == BI_BITFIELDS )
    return NULL;

  // If a palette has not been supplied, use default palette
  hPal = (HPALETTE) pPal->GetSafeHandle();
  if (hPal==NULL)
    hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

  // Get bitmap information
  bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

  // Initialize the bitmap infoheader
  bi.biSize          = sizeof(BITMAPINFOHEADER);
  bi.biWidth         = bm.bmWidth;
  bi.biHeight        = bm.bmHeight;
  bi.biPlanes        = 1;
  bi.biBitCount      = bm.bmPlanes * bm.bmBitsPixel;
    //bm.bmPlanes    * bm.bmBitsPixel;
  bi.biCompression   = dwCompression;
  bi.biSizeImage     = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrUsed       = 0;
  bi.biClrImportant  = 0;

  // Compute the size of the infoheader and the color table
  int nColors = (1 << bi.biBitCount);
  if( nColors > 256 )
    nColors = 0;
  dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

  // We need a device context to get the DIB from
  hDC = ::GetDC(NULL);
  hPal = SelectPalette(hDC,hPal,FALSE);
  RealizePalette(hDC);

  // Allocate enough memory to hold bitmap infoheader and
  // color table
  hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

  if (!hDIB){
    SelectPalette(hDC,hPal,FALSE);
    ::ReleaseDC(NULL,hDC);
    return NULL;
  }

  lpbi = (LPBITMAPINFOHEADER)hDIB;

  *lpbi = bi;

  // Call GetDIBits with a NULL lpBits param, so the device
  // driver will calculate the biSizeImage field
  GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L,
                 (DWORD)bi.biHeight,
                 (LPBYTE)NULL, (LPBITMAPINFO)lpbi,
                 (DWORD)DIB_RGB_COLORS);

  bi = *lpbi;

  // If the driver did not fill in the biSizeImage field, then
  // compute it
  // Each scan line of the image is aligned on a DWORD (32bit)
  // boundary
  if (bi.biSizeImage == 0){
    bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31)
                        & ~31) / 8) * bi.biHeight;

    // If a compression scheme is used, the result may in fact
    // be larger
    // Increase the size to account for this.
    if (dwCompression != BI_RGB)
      bi.biSizeImage = (bi.biSizeImage * 3) / 2;
  }

  // Realloc the buffer so that it can hold all the bits
  dwLen += bi.biSizeImage;
  if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
    hDIB = handle;
  else{
    GlobalFree(hDIB);

    // Reselect the original palette
    SelectPalette(hDC,hPal,FALSE);
    ::ReleaseDC(NULL,hDC);
    return NULL;
  }

  // Get the bitmap bits
  lpbi = (LPBITMAPINFOHEADER)hDIB;

  // FINALLY get the DIB
  BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
        0L,                        // Start scan line
        (DWORD)bi.biHeight,        // # of scan lines
        (LPBYTE)lpbi               // address for bitmap bits
        + (bi.biSize + nColors * sizeof(RGBQUAD)),
        (LPBITMAPINFO)lpbi,        // address of bitmapinfo
        (DWORD)DIB_RGB_COLORS);    // Use RGB for color table

  if( !bGotBits )
  {
    GlobalFree(hDIB);

    SelectPalette(hDC,hPal,FALSE);
    ::ReleaseDC(NULL,hDC);
    return NULL;
  }

  SelectPalette(hDC,hPal,FALSE);
  ::ReleaseDC(NULL,hDC);

  return hDIB;

//End of the function
}

//////////////////////////////////////////////////////////////
// Function:	WriteDIB
// URL:			http://www.codeguru.com/Cpp/G-M/bitmap/article.php/c1697/
// Author:		Zafir Anjum
//
// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
BOOL WriteDIB( LPCTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	return TRUE;
}

//////////////////////////////////////////////////////////////
// Function:	WriteWindowToDIB
// URL:			http://www.codeguru.com/cpp/g-m/bitmap/article.php/c1741/
// Author:		Zafir Anjum
//

BOOL WriteWindowToDIB( LPCTSTR szFile, CWnd *pWnd )
{
	CBitmap 	bitmap;
	CWindowDC	dc(pWnd);
	CDC 		memDC;

	memDC.CreateCompatibleDC(&dc);

//	CRect		rect;
//	pWnd->GetWindowRect(rect);
	int width = dc.GetDeviceCaps(HORZRES);
	int heigth = dc.GetDeviceCaps(VERTRES);

	bitmap.CreateCompatibleBitmap(&dc, width, heigth );

	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, width, heigth, &dc, 0, 0, SRCCOPY);

	// Create logical palette if device support a palette
	CPalette pal;
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;

		pLP->palNumEntries =
			GetSystemPaletteEntries( dc, 0, 255, pLP->palPalEntry );

		// Create the palette
		pal.CreatePalette( pLP );

		delete[] pLP;
	}

	memDC.SelectObject(pOldBitmap);

	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB( bitmap, BI_RGB, &pal );

	if( hDIB == NULL )
		return FALSE;

	// Write it to file
	WriteDIB( szFile, hDIB );

	// Free the memory allocated by DDBToDIB for the DIB
	GlobalFree( hDIB );
	return TRUE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
