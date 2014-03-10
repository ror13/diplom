// BitmapBuf.h

#ifndef _BITMAPBUF_H_
#define _BITMAPBUF_H_

/////////////////////////////////////////////////////////////////////////////
// CCreepingLineWnd window

class CBitmapBuf
{
public:
	int				m_Offset;
	int				m_TxtPos;
	int				m_Size;
	CBitmap			m_Bitmap;
	CDC				m_CompatibleDc;
	
	CBitmapBuf();
	
	void SetData(CDC *pDC, CFont *pFont,
		COLORREF clBackground, COLORREF clTextColor);
	
	BOOL CreateNewBuf(int ImageOffset, int ImageWidth, int ImageHeight,
		int TxtOffset, int TxtWidth, 
		CString &srcTxt,
		BOOL IsRTLReading = FALSE);

	void SetTxtPos(int TxtPos);

	void FreeBuf();

	CRect GetTxtRect(CString &srcTxt);
	
	int FindCountSymbolsForWidth(CString &srcTxt, int NeedTxtWidth,
		BOOL bWithPartialVisibleSymbol);
	
protected:
	CDC				*m_pDC;
	CFont			*m_pFont;
	
	COLORREF		m_clBackground;
	COLORREF		m_clTextColor;
};

#endif //_BITMAPBUF_H_
