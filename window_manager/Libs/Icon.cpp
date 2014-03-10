// Icon.cpp

#include "StdAfx.h"
#include "Icon.h"

#define ROUND_UP(x,mod_pow_of_2)	((x+mod_pow_of_2-1)&(~(mod_pow_of_2-1)))

BITMAP CaptureBitmap(HBITMAP hBitmap)
{
	BITMAP bitmap = {0};
	GetObject(hBitmap, sizeof(bitmap), &bitmap);
	return bitmap;
}

CSize GetIconSize(HICON hIcon)
{
	CSize size(32,32);
	
	ICONINFO info;
	if(GetIconInfo(hIcon, &info))
	{
		BITMAP bitmap = {0};
		if(GetObject(info.hbmColor, sizeof(bitmap), &bitmap))
		{
			size.cx = bitmap.bmWidth;
			size.cy = bitmap.bmHeight;
		}
		
		DeleteObject(info.hbmColor);
		DeleteObject(info.hbmMask);
	}
	
	return size;
}

BOOL SaveIconToMem(HICON hIcon, LPBYTE pBuf, int &BufSize)
{
	if(hIcon == NULL)
		return FALSE;

	ICONINFO icon_info;
	BOOL res = GetIconInfo(hIcon, &icon_info);
	if(!res)
		return FALSE;

	BITMAP ColorBitmap = CaptureBitmap(icon_info.hbmColor);
	DWORD dwColorLength = ColorBitmap.bmHeight * ColorBitmap.bmWidthBytes;
	
	BITMAP MaskBitmap = CaptureBitmap(icon_info.hbmMask);
	DWORD dwMaskLength = MaskBitmap.bmHeight * MaskBitmap.bmWidthBytes;
	
	ICONDIR icon_dir;
	memset(&icon_dir, 0, sizeof(icon_dir));

	icon_dir.idType = 1;
	icon_dir.idCount = 1;
	icon_dir.idEntries[0].bWidth = (BYTE)ColorBitmap.bmWidth;
	icon_dir.idEntries[0].bHeight = (BYTE)ColorBitmap.bmHeight;
	
	int colors = ColorBitmap.bmBitsPixel <= 8 ? (1 << ColorBitmap.bmBitsPixel) : 0;
	icon_dir.idEntries[0].bColorCount = colors;
	icon_dir.idEntries[0].wPlanes = 1;
	icon_dir.idEntries[0].wBitCount = ColorBitmap.bmBitsPixel;

	int iColorDataSize =
		(ROUND_UP(ColorBitmap.bmBitsPixel * ColorBitmap.bmWidth, 32) / 8) * ColorBitmap.bmHeight;
	int iMaskDataSize =
		dwMaskLength > 0 ? (ROUND_UP(1 * ColorBitmap.bmWidth, 32) / 8) * ColorBitmap.bmHeight : 0;

	int Size = sizeof(BITMAPINFOHEADER) + colors * sizeof(RGBQUAD) + iColorDataSize +
		iMaskDataSize;
	
	int Offset = sizeof(ICONDIR);
	
	icon_dir.idEntries[0].dwBytesInRes = Size;
	icon_dir.idEntries[0].dwImageOffset = Offset;

	int all_size = Offset + Size;

	if(pBuf == NULL)
	{
		BufSize = all_size;

		DeleteObject(icon_info.hbmColor);
		DeleteObject(icon_info.hbmMask);
		return TRUE;
	}

	if(BufSize < all_size)
	{
		DeleteObject(icon_info.hbmColor);
		DeleteObject(icon_info.hbmMask);
		return FALSE;
	}

	LPBYTE lpMemBuf = pBuf;

	memcpy(lpMemBuf, &icon_dir, Offset);
	lpMemBuf += Offset;

	LPBITMAPINFOHEADER lpbmih = (LPBITMAPINFOHEADER)lpMemBuf;
	lpbmih->biSize = sizeof(BITMAPINFOHEADER);
	lpbmih->biWidth = ColorBitmap.bmWidth;
	lpbmih->biHeight = ColorBitmap.bmHeight + MaskBitmap.bmHeight;
	lpbmih->biPlanes = ColorBitmap.bmPlanes;
	lpbmih->biBitCount = ColorBitmap.bmBitsPixel;
	lpbmih->biCompression = BI_RGB;
	lpbmih->biSizeImage = dwColorLength + dwMaskLength;
	lpbmih->biXPelsPerMeter = 0;
	lpbmih->biYPelsPerMeter = 0;
	lpbmih->biClrUsed = colors;
	lpbmih->biClrImportant = 0;

	lpMemBuf += sizeof(BITMAPINFOHEADER);

	if(colors)
	{
		LPRGBQUAD rgb_item = (LPRGBQUAD)lpMemBuf;
		for(int i = 0; i < colors; i++)
		{
		}
	}

	lpMemBuf += sizeof(RGBQUAD) * colors;

	LPBYTE pColorBits = lpMemBuf;
	if(dwColorLength > 0)
		GetBitmapBits(icon_info.hbmColor, dwColorLength, pColorBits);
	
	lpMemBuf += dwColorLength;

	LPBYTE pMaskBits = lpMemBuf;
	if(dwMaskLength > 0)
		GetBitmapBits(icon_info.hbmMask, dwMaskLength, pMaskBits);

	// Invert line position
	int LineLength = ColorBitmap.bmWidthBytes;
	int MaskLineLength = MaskBitmap.bmWidthBytes;

	LPBYTE pTmpLineBuf = new BYTE[LineLength];
	for(int i = 0; i < ColorBitmap.bmHeight/2; i++)
	{
		LPBYTE pLine1 = &pColorBits[LineLength * i];
		LPBYTE pLine2 = &pColorBits[LineLength * (ColorBitmap.bmHeight - 1 - i)];

		memcpy(pTmpLineBuf, pLine1, LineLength);
		memcpy(pLine1, pLine2, LineLength);
		memcpy(pLine2, pTmpLineBuf, LineLength);
		
		pLine1 = &pMaskBits[MaskLineLength * i];
		pLine2 = &pMaskBits[MaskLineLength * (MaskBitmap.bmHeight - 1 - i)];

		memcpy(pTmpLineBuf, pLine1, MaskLineLength);
		memcpy(pLine1, pLine2, MaskLineLength);
		memcpy(pLine2, pTmpLineBuf, MaskLineLength);
	}
	delete []pTmpLineBuf;

	BufSize = all_size;
	
	DeleteObject(icon_info.hbmColor);
	DeleteObject(icon_info.hbmMask);
	
	return TRUE;
}

HICON MakeIconFromMem(LPBYTE pBuf)
{
	LPICONDIR pIconDir = (LPICONDIR)pBuf;
	LPICONIMAGE pIconImage =
		(LPICONIMAGE)(pBuf + pIconDir->idEntries[0].dwImageOffset);
	HICON hIcon = CreateIconFromResource((LPBYTE)pIconImage, pIconDir->idEntries[0].dwBytesInRes, TRUE, 0x00030000);

	return hIcon;
}
