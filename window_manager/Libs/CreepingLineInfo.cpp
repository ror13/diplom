//////////////////////////////////////////////////////////////
// File:		// CreepingLineInfo.cpp
// File time:	// 27.05.2005	14:36
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
#include "CreepingLineInfo.h"
#include "FileOperation.h"
#include "Translate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////

CCreepingLineInfo::CCreepingLineInfo (void)
{
	InitDefault();
}

CCreepingLineInfo::~CCreepingLineInfo (void)
{
}

//////////////////////////////////////////////////////////////

BOOL CCreepingLineInfo::operator== (const CCreepingLineInfo& a)
{
#define COMPARE_CHILD(ch) if( ch != a.ch ) return FALSE
	
	// General:
	COMPARE_CHILD(m_strText);
	COMPARE_CHILD(m_ScrollingSpeed);
	COMPARE_CHILD(m_bResizing);
		
	// Font:
	if( 0 != memcmp( &m_font, &a.m_font, sizeof(m_font)-sizeof(m_font.lfFaceName) ) )
		return FALSE;
	if( 0 != _tcscmp( m_font.lfFaceName, a.m_font.lfFaceName ) )
		return FALSE;
	COMPARE_CHILD(m_clTextColor);
	COMPARE_CHILD(m_clTextSize);
			
	// Window:
	COMPARE_CHILD(m_clBackground);
	COMPARE_CHILD(m_bTransparent);
	COMPARE_CHILD(m_clTransparentColor);
	COMPARE_CHILD(m_TransparencyFactor);
	COMPARE_CHILD(m_Rect);
	COMPARE_CHILD(m_Position);
			
	return TRUE;
}

void CCreepingLineInfo::InitDefault()
{
	// General:
	m_bUseRSS = FALSE;
	m_strText = _TRANS(_T("Test text Creeping Line"));
#if _DEBUG
	//m_strText = _TRANS(_T("Test text Creeping Line1Test text Creeping Line2Test text Creeping Line3Test text Creeping Line4Test text Creeping Line5Test text Creeping Line6Test text Creeping Line7Test text Creeping Line8"));
	m_strText = _TRANS(_T("<-<-<===A=!!=A==<-<-<  TEST LINE 0_o ^$%^%&><><>@@"));
#endif
	m_ScrollingSpeed = 100;

	// Font:

	LPCTSTR szFont = _T("Arial");
	m_clTextSize = 36;

	HDC hDC = ::GetDC( NULL );
	int nHeight = -MulDiv(m_clTextSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC( NULL, hDC );
	hDC = NULL;

	m_font.lfHeight = nHeight;
	m_font.lfWidth = 0;
	m_font.lfEscapement = 0;
	m_font.lfOrientation = 0;
	m_font.lfWeight = FW_NORMAL;
	m_font.lfItalic = FALSE;
	m_font.lfUnderline = FALSE;
	m_font.lfStrikeOut = FALSE;
	m_font.lfCharSet = DEFAULT_CHARSET;
	m_font.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_font.lfQuality = DEFAULT_QUALITY;
	m_font.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcsncpy( m_font.lfFaceName, szFont, LF_FACESIZE );

	m_clTextColor = GetSysColor( COLOR_BTNTEXT );

	// Window:
	m_clBackground = GetSysColor( COLOR_3DFACE );
	m_bResizing = TRUE;
	m_bTransparent = TRUE;
	m_clTransparentColor = RGB(220,220,220);
	m_TransparencyFactor = 200;
	m_Rect = CRect( 0, 0, 600, 70 );
	m_Position = DOCKING_POS_TOP;

	// Text Layout left-to-right or other 
	m_iTextLayout = CREEPING_LINE_TEXT_LAYOUT_LEFT_TO_RIGHT;
}

//////////////////////////////////////////////////////////////

BOOL CCreepingLineInfo::Serialize( SERIALIZE_CONTEXT& context )
{
	SERIALIZE_START

	// General:
	SERIALIZE_INT(	BOOL,	m_bUseRSS,					_T("General.bUseRSS") )
	SERIALIZE_STRING(		m_strText,					_T("General.LineText") )

	SERIALIZE_INT(	BOOL,	m_bResizing,				_T("General.bResizing") )
	SERIALIZE_DOUBLE(	double,	m_ScrollingSpeed,		_T("General.ScrollingSpeed") )

	// Font:
	SERIALIZE_INT(	LONG,		m_font.lfHeight,		_T("Font.lfHeight") )
	SERIALIZE_INT(	LONG,		m_font.lfWidth,			_T("Font.lfWidth") )
	SERIALIZE_INT(	LONG,		m_font.lfEscapement,	_T("Font.lfEscapement") )
	SERIALIZE_INT(	LONG,		m_font.lfOrientation,	_T("Font.lfOrientation") )
	SERIALIZE_INT(	LONG,		m_font.lfWeight,		_T("Font.lfWeight") )
	SERIALIZE_INT(	BYTE,		m_font.lfItalic,		_T("Font.lfItalic") )
	SERIALIZE_INT(	BYTE,		m_font.lfUnderline,		_T("Font.lfUnderline") )
	SERIALIZE_INT(	BYTE,		m_font.lfStrikeOut,		_T("Font.lfStrikeOut") )
	SERIALIZE_INT(	BYTE,		m_font.lfCharSet,		_T("Font.lfCharSet") )
	SERIALIZE_INT(	BYTE,		m_font.lfOutPrecision,	_T("Font.lfOutPrecision") )
	SERIALIZE_INT(	BYTE,		m_font.lfClipPrecision,	_T("Font.lfClipPrecision") )
	SERIALIZE_INT(	BYTE,		m_font.lfQuality,		_T("Font.lfQuality") )
	SERIALIZE_INT(	BYTE,		m_font.lfPitchAndFamily,_T("Font.lfPitchAndFamily") )
	SERIALIZE_STR_BUF(			m_font.lfFaceName,		_T("Font.lfFaceName") )
	SERIALIZE_HEX(	COLORREF,	m_clTextColor,			_T("Font.clTextColor") )
	SERIALIZE_INT(	LONG,		m_clTextSize,			_T("Font.clTextSize") )

	// Window:
	SERIALIZE_HEX(	COLORREF,	m_clBackground,			_T("Window.clBackground") )
	SERIALIZE_HEX(	COLORREF,	m_clTransparentColor,	_T("Window.clTransparentColor") )
	SERIALIZE_INT(	BYTE,		m_TransparencyFactor,	_T("Window.TransparencyFactor") )
	SERIALIZE_INT(	LONG,		m_Rect.left,			_T("Window.Rect.left") )
	SERIALIZE_INT(	LONG,		m_Rect.top,				_T("Window.Rect.top") )
	SERIALIZE_INT(	LONG,		m_Rect.right,			_T("Window.Rect.right") )
	SERIALIZE_INT(	LONG,		m_Rect.bottom,			_T("Window.Rect.bottom") )
	SERIALIZE_INT(	int,		m_Position,				_T("Window.Position") )

	// Direction
	SERIALIZE_INT(	int,		m_iTextLayout,			_T("General.TextLayout") )

	SERIALIZE_END
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#ifdef _DEBUG

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class CCreepingLineInfoTest: public CppUnit::TestFixture
{
private:
	CString		m_TestFileName;
public:
	void setUp()
	{
		m_TestFileName = GetRootFolder() + _T("test.clf");
	}
	
	void testGetSetVal()
	{
		CCreepingLineInfo info;
		info.m_strText = _T("Test русского текста");

		CString str;
		info.GetValues(str);

		CCreepingLineInfo info2;
		info2.SetValues(str);

		CPPUNIT_ASSERT(info == info2);
	}
	
	void testSaveLoad()
	{
		CCreepingLineInfo info;
		info.m_strText = _T("Test русского текста");
		info.Save(m_TestFileName);

		CCreepingLineInfo info2;
		info2.Load(m_TestFileName);

		CPPUNIT_ASSERT(info == info2);
	}
	
	
	void test()
	{
		CPPUNIT_ASSERT_MESSAGE("Important Message", 1);
		CPPUNIT_ASSERT(1);
		CPPUNIT_ASSERT_EQUAL(13, 13);
	}
	
	void tearDown()
	{
		DeleteFile(m_TestFileName);
	}
	
	CPPUNIT_TEST_SUITE(CCreepingLineInfoTest);
	CPPUNIT_TEST(testGetSetVal);
	CPPUNIT_TEST(testSaveLoad);
	CPPUNIT_TEST_SUITE_END();
	
};
CPPUNIT_TEST_SUITE_REGISTRATION(CCreepingLineInfoTest);

#endif // _DEBUG
