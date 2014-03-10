//////////////////////////////////////////////////////////////
// File:		// Bitmap.h
// File time:	// 3.08.2004	17:29
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _BITMAP_H_UID00000006AE4EB61C
#define _BITMAP_H_UID00000006AE4EB61C

/*
class CBitmap
{
public:
	CBitmap (void);
	virtual ~CBitmap (void);

protected:

private:

};
*/

#define ROUND_UP(x,mod_pow_of_2)	((x+mod_pow_of_2-1)&(~(mod_pow_of_2-1)))

//////////////////////////////////////////////////////////////

BOOL LoadBMP( LPCTSTR sBMPFile, HGLOBAL *phDIB, CPalette *pPal );
void DrawDIB( CDC* pDC, HGLOBAL hDIB, CPalette *pPal );

//////////////////////////////////////////////////////////////

BOOL LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal );
void DrawBitmap(CDC* pDC, CBitmap& bitmap, CPalette *pPal, CPoint pt );

//////////////////////////////////////////////////////////////

BOOL WriteWindowToDIB( LPCTSTR szFile, CWnd *pWnd );

//////////////////////////////////////////////////////////////

class CMyException
{
public:
	LONG	m_lLastError;
	CString m_strError;
	int		m_iLine;
	LPCTSTR	m_pszFile;

	CMyException( LONG lLastError, LPCTSTR pszError, int iLine, LPCTSTR pszFile )
	{
		m_lLastError = lLastError;
		m_strError = pszError;
		m_iLine = iLine;
		m_pszFile = pszFile;
	}
};

//////////////////////////////////////////////////////////////

#define RET_TRY			goto my_return
#define ERROR_TRY(err)	throw CMyException( GetLastError(), err, __LINE__, _T(__FILE__) )
#define POS				strPosition.Format( _T("line %d"), __LINE__ );

//////////////////////////////////////////////////////////////

inline void FillBitmapInfoHeader(
		BITMAPINFOHEADER& bmih,
		int width,
		int height,
		int bits,
		int dataSize = 0 )
{
	ZeroMemory( &bmih, sizeof(bmih) );
	bmih.biSize = sizeof(bmih);
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biBitCount = bits;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = dataSize; // may be zero for BI_RGB only
	bmih.biClrUsed = 0;
	if( bits <= 8 )
		bmih.biClrUsed = (1<<bits);
	bmih.biClrImportant = 0;
}

/////////////////////////////////////////////////////////////////////////////

inline CString DumpBitmapInfoHeader(
		const BITMAPINFOHEADER& bmih )
{
	CString s;
	s.Format( _T("BITMAPINFOHEADER: %d x %d x %d bits, %d bytes (%db/info, %d planes, compr: %d, colors: %d, %d)"),
		bmih.biWidth, bmih.biHeight, bmih.biBitCount, bmih.biSizeImage,
		bmih.biSize, bmih.biPlanes, bmih.biCompression,
		bmih.biClrUsed, bmih.biClrImportant );
	return s;
}

/////////////////////////////////////////////////////////////////////////////

#endif //ifndef _BITMAP_H_UID00000006AE4EB61C
