// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E008FAFA_541B_4FA7_AF2A_A48EA8D70FD8__INCLUDED_)
#define AFX_STDAFX_H__E008FAFA_541B_4FA7_AF2A_A48EA8D70FD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <afxmt.h>

#ifdef _DEBUG
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#endif //_DEBUG

/////////////////////////////////////////////////////////////

#include <assert.h>
#undef ASSERT
#define ASSERT assert

/////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include <atlbase.h> 

/////////////////////////////////////////////////////////////////////////////

class CHDCBuffer
{
 public:
 CDC* GetBufferDC(CDC* realDC, int nWidth, int nHeight, COLORREF clBackgroundColor)
  {
    winOK=TRUE;
    cx=nWidth;
    cy=nHeight;
    pRealDC=realDC;
    BOOL dcOK=dc.CreateCompatibleDC(realDC);
    BOOL bmpOK=bmp.CreateCompatibleBitmap(realDC, cx, cy);
    ASSERT(dcOK);
//    ASSERT(bmpOK);
    if(!dcOK || !bmpOK)
     winOK=FALSE;
    if(winOK)
     oldBmp=dc.SelectObject(&bmp);
    CDC* res= winOK ? &dc : realDC;
    CBrush brush;
    brush.CreateSolidBrush(clBackgroundColor);
    res->FillRect(CRect(0,0,cx,cy),&brush);
    brush.DeleteObject();
    return res;
  }
 void RestoreBufferDC()
  {
   if(!winOK) return;
   pRealDC->BitBlt(0,0,cx,cy,&dc,0,0,SRCCOPY);
   dc.SelectObject(oldBmp);
   bmp.DeleteObject();
   dc.DeleteDC();
  }
 protected:
  int cx,cy;
  CDC* pRealDC;
  CDC dc;
  BOOL winOK;
  CBitmap bmp;
  CBitmap* oldBmp;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E008FAFA_541B_4FA7_AF2A_A48EA8D70FD8__INCLUDED_)
