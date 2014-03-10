// BitmapBuf.cpp

#include "StdAfx.h"
#include "BitmapBuf.h"

#include "BitmapEx.h"

//////////////////////////////////////////////////////////////////////////
// CBitmapBuf

CBitmapBuf::CBitmapBuf()
{
	m_Offset = 0;
	m_Size = 0;
	m_clBackground = RGB(0, 0, 0);
	m_clTextColor = RGB(255, 255, 255);
}

void CBitmapBuf::SetData(CDC *pDC, CFont *pFont,
						 COLORREF clBackground, COLORREF clTextColor)
{
	m_pDC = pDC;
	m_pFont = pFont;
	
	m_clBackground = clBackground;
	m_clTextColor = clTextColor;
}

BOOL CBitmapBuf::CreateNewBuf(int ImageOffset, int ImageWidth, int ImageHeight,
							  int TxtOffset, int TxtWidth, 
							  CString &srcTxt, BOOL IsRTLReading)
{
	FreeBuf();

	m_Offset = ImageOffset;
	m_Size = ImageWidth;
	
	m_CompatibleDc.CreateCompatibleDC(m_pDC);
	
	m_Bitmap.CreateCompatibleBitmap(m_pDC, ImageWidth, ImageHeight);
	
	CBitmap *pOldBitMap = m_CompatibleDc.SelectObject(&m_Bitmap);
	pOldBitMap->DeleteObject();

	// Fill Bitmap:
	CBrush Brush;
	Brush.CreateSolidBrush(m_clBackground);
	CRect BitmapRect(0, 0, ImageWidth, ImageHeight);
	m_CompatibleDc.FillRect(&BitmapRect, &Brush);
	
	CFont *pOldFont = m_CompatibleDc.SelectObject(m_pFont);
	pOldFont->DeleteObject();
	
	//int NeedTxtWidth = ImageWidth - TxtOffset;
	int NeedTxtWidth = TxtWidth - TxtOffset;

	int CountSymbolsForDraw = FindCountSymbolsForWidth(srcTxt, NeedTxtWidth, TRUE);
	
	CString TxtForDraw = srcTxt.Mid(0, CountSymbolsForDraw);
	
#if _DEBUG
	//TxtForDraw.MakeReverse();
#endif
	
	CRect TxtRect(TxtOffset, 0, TxtWidth, ImageHeight);
			
	m_CompatibleDc.SetBkColor(m_clBackground);
	m_CompatibleDc.SetTextColor(m_clTextColor);
	
// 	m_CompatibleDc.DrawText(TxtForDraw, TxtForDraw.GetLength(),
// 		&TxtRect, DT_SINGLELINE | DT_VCENTER | DT_RTLREADING);
	//m_CompatibleDc.SetLayout( LAYOUT_RTL );
// 	m_CompatibleDc.DrawText(TxtForDraw, TxtForDraw.GetLength(),
// 		&TxtRect, DT_SINGLELINE | DT_VCENTER | DT_RTLREADING);
// 	m_CompatibleDc.DrawText(TxtForDraw, TxtForDraw.GetLength(),
// 		&TxtRect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	m_CompatibleDc.DrawText(TxtForDraw, TxtForDraw.GetLength(),
		&TxtRect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | (IsRTLReading?DT_RTLREADING:0));
	
#if _DEBUG && 0
static iCreateNewBuf = 0;
	CBitmapEx img;
	
	CString strFilename;
	
	img.Load(m_Bitmap);
	
	strFilename.Format(_T("Img_CreateNewBuf%.8x.bmp"), iCreateNewBuf++);
	
	img.Save(strFilename);
#endif
	
	return TRUE;
}

void CBitmapBuf::SetTxtPos(int TxtPos)
{
	m_TxtPos = TxtPos;
}

void CBitmapBuf::FreeBuf()
{
	m_CompatibleDc.DeleteDC();
	m_Bitmap.DeleteObject();
}

CRect CBitmapBuf::GetTxtRect(CString &srcTxt)
{
	CRect TextRect(0,0,0,0);
	m_CompatibleDc.DrawText(srcTxt, srcTxt.GetLength(), &TextRect,
		DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);
	return TextRect;
}

int CBitmapBuf::FindCountSymbolsForWidth(CString &srcTxt, int NeedTxtWidth,
										  BOOL bWithPartialVisibleSymbol)
{
	int BeginPos = 0;
	int EndPos = srcTxt.GetLength() - 1;

	if(EndPos < 0)
		return 0;
	
	while(EndPos != BeginPos)
	{
		int CurPos = BeginPos + (EndPos - BeginPos + 1) / 2;
		if(CurPos == EndPos)
			break;
		
		CRect TextRect(0,0,0,0);
		m_CompatibleDc.DrawText((LPCTSTR)srcTxt, CurPos+1, &TextRect,
			DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);
		
		int CurTxtWidth = TextRect.Width();
		
		if(CurTxtWidth < NeedTxtWidth)
			BeginPos = CurPos;
		else
			EndPos = CurPos;
	}
	if(bWithPartialVisibleSymbol && (EndPos - BeginPos) == 1)
		BeginPos++;

	return BeginPos + 1;
}
