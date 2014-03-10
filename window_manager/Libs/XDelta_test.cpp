//
// Permission to distribute this example by
// Copyright (C) 2007 Ralf Junker
// Ralf Junker <delphi@yunqa.de>
// http://www.yunqa.de/delphi/

//---------------------------------------------------------------------------

#include "stdafx.h"

#ifdef _DEBUG

#include "FileOperation.h"

#include "XDelta.h"

#include <string>

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

static BYTE _data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static BYTE _data2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};


class XDeltaTest: public CppUnit::TestFixture
{
	CString m_f1;
	CString m_f2;
	CString m_p1;
	CString m_r1;


public:
	void setUp()
	{
		m_f1 = GetServerReportsDirectory(NULL) + _T("TestData1.txt");
		m_f2 = GetServerReportsDirectory(NULL) + _T("TestData2.txt");
		m_p1 = GetServerReportsDirectory(NULL) + _T("TestData1.diff");
		m_r1 = GetServerReportsDirectory(NULL) + _T("TestData1.complete");

		BYTE data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		BYTE data12[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		SaveBinFileFromBuf(m_f1, (PBYTE)_data1, sizeof(_data1));
		SaveBinFileFromBuf(m_f2, (PBYTE)_data2, sizeof(_data2));
	}
	
	void testXDeltaOld()
	{ 
		CPPUNIT_ASSERT_EQUAL(TRUE, XDeltaDiffOld(m_f1, m_f2, m_p1));
		CPPUNIT_ASSERT_EQUAL(TRUE, XDeltaPatchOld(m_f1, m_p1, m_r1));
	}

	void testXDelta()
	{ 
		CPPUNIT_ASSERT_EQUAL(TRUE, XDeltaDiff(m_f1, m_f2, m_p1));
		CPPUNIT_ASSERT_EQUAL(TRUE, XDeltaPatch(m_f1, m_p1, m_r1));

		CString strMd5;
		CPPUNIT_ASSERT(XDeltaGetSrcMd5(m_p1, strMd5));
		CPPUNIT_ASSERT_EQUAL(std::string("c56bd5480f6e5413cb62a0ad9666613a"), std::string(StringToUTF8(strMd5)));

		CPPUNIT_ASSERT(XDeltaGetDstMd5(m_p1, strMd5));
		CPPUNIT_ASSERT_EQUAL(std::string("a6e7d3b46fdfaf0bde2a1f832a00d2de"), std::string(StringToUTF8(strMd5)));

		LONGLONG size;
		CPPUNIT_ASSERT(XDeltaGetSrcSize(m_p1, size));
		CPPUNIT_ASSERT_EQUAL((unsigned int)sizeof(_data1), (unsigned int)size);

		CPPUNIT_ASSERT(XDeltaGetDstSize(m_p1, size));
		CPPUNIT_ASSERT_EQUAL((unsigned int)sizeof(_data2), (unsigned int)size);
	}

	void tearDown()
	{
		//DeleteFile(m_f1);
		//DeleteFile(m_f2);
		//DeleteFile(m_p1);
		//DeleteFile(m_r1);
	}
	
	CPPUNIT_TEST_SUITE(XDeltaTest);
	CPPUNIT_TEST(testXDeltaOld);
	CPPUNIT_TEST(testXDelta);
	CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION(XDeltaTest);
	

#endif // _DEBUG


