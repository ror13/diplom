// TemplateFile.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TemplateFile.h"

CTemplateFile::CTemplateFile()
{
	m_hFile = INVALID_HANDLE_VALUE;
	memset(&m_FileHeader, 0, sizeof(m_FileHeader));
	memset(&m_CurItem, 0, sizeof(m_CurItem));
}

CTemplateFile::~CTemplateFile()
{
	Close();
}

BOOL CTemplateFile::Create(LPCTSTR FilePath)
{
	Close();

	m_hFile = CreateFile(FilePath, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// write file header
	memset(&m_FileHeader, 0, sizeof(m_FileHeader));

	memcpy(m_FileHeader.Signature, TEMPLATE_SIGNATURE, sizeof(TEMPLATE_SIGNATURE));
	m_FileHeader.HeaderSize = sizeof(m_FileHeader);
	m_FileHeader.OffsetAfterLastItem.LowPart = sizeof(m_FileHeader);
	DWORD written = 0;
	WriteFile(m_hFile, &m_FileHeader, m_FileHeader.HeaderSize, &written, NULL);

	return TRUE;
}

BOOL CTemplateFile::Open(LPCTSTR FilePath)
{
	Close();

	m_hFile = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	
	if(m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	
	// read file signature
	memset(&m_FileHeader, 0, sizeof(m_FileHeader));
	DWORD readed = 0;
	ReadFile(m_hFile, &m_FileHeader, sizeof(m_FileHeader), &readed, NULL);

	if(strncmp(m_FileHeader.Signature, TEMPLATE_SIGNATURE,
		sizeof(TEMPLATE_SIGNATURE)) != 0)
		return FALSE;
	
	return TRUE;
}

BOOL CTemplateFile::Close()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		memset(&m_FileHeader, 0, sizeof(m_FileHeader));
		memset(&m_CurItem, 0, sizeof(m_CurItem));
	}
	return TRUE;
}

BOOL CTemplateFile::GetItemCount(int *pCount)
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	if(pCount)
		*pCount = m_FileHeader.CountItems;

	return TRUE;
}

BOOL CTemplateFile::LoadCurItemHeader()
{
	// read item size
	DWORD readed = 0;
	ReadFile(m_hFile, &m_CurItem.ItemSize, sizeof(m_CurItem.ItemSize),
		&readed, NULL);
	
	// read item name from UTF8 coding in pascal string format
	m_CurItem.ItemName[0] = _T('\0');
	BYTE ItemNameSize = 0;
	ReadFile(m_hFile, &ItemNameSize, sizeof(ItemNameSize),
		&readed, NULL);
	
	char ItemNameBuf[256];
	ReadFile(m_hFile, &ItemNameBuf, ItemNameSize,
		&readed, NULL);
	
	int wSize = MultiByteToWideChar(CP_UTF8, 0, ItemNameBuf, ItemNameSize, NULL, 0);
	LPWSTR wszBuffer = new WCHAR[wSize+1];
	int Count = MultiByteToWideChar(CP_UTF8, 0, ItemNameBuf, ItemNameSize, wszBuffer, wSize+1);
	wszBuffer[Count] = 0;
	USES_CONVERSION;
	lstrcpy(m_CurItem.ItemName, W2T(wszBuffer));
	delete[] wszBuffer;

	return TRUE;
}

BOOL CTemplateFile::LoadCurItemData()
{
	// read item data
	if(m_CurItem.pItemBuf)
	{
		delete[] m_CurItem.pItemBuf;
		m_CurItem.pItemBuf = NULL;
	}
	
	if(m_CurItem.ItemSize > 0)
	{
		m_CurItem.pItemBuf = new BYTE[m_CurItem.ItemSize];
		DWORD readed = 0;
		ReadFile(m_hFile, m_CurItem.pItemBuf, m_CurItem.ItemSize,
			&readed, NULL);
	}

	return TRUE;
}

BOOL CTemplateFile::SkipLoadCurItemData()
{
	if(m_CurItem.ItemSize > 0)
	{
		DWORD res = SetFilePointer(m_hFile, m_CurItem.ItemSize,
			NULL, FILE_CURRENT);
		if(res == INVALID_SET_FILE_POINTER)
			return FALSE;
	}
	
	return TRUE;
}

BOOL CTemplateFile::SetPointerToFirstItem()
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD res = SetFilePointer(m_hFile, m_FileHeader.HeaderSize,
		NULL, FILE_BEGIN);
	if(res == INVALID_SET_FILE_POINTER)
		return FALSE;

	LoadCurItemHeader();
	LoadCurItemData();
	return TRUE;
}

BOOL CTemplateFile::AddItem(LPCTSTR ItemName, LPVOID pBuf, int BufSize)
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	
	DWORD res = SetFilePointerEx(m_hFile, m_FileHeader.OffsetAfterLastItem,
		NULL, FILE_BEGIN);
	if(res == INVALID_SET_FILE_POINTER)
		return FALSE;

	// write item size
	DWORD written = 0;
	WriteFile(m_hFile, &BufSize, sizeof(BufSize), &written, NULL);
	
	// convert item name to UTF8 coding and write in pascal string format
	USES_CONVERSION;
	LPWSTR wItemName = T2W((LPTSTR)ItemName);
	int buf_size = WideCharToMultiByte(CP_UTF8, 0, wItemName, -1,
		NULL, 0, NULL, NULL );;
	LPSTR pUTF8Buf = new char[buf_size+1];
	DWORD size = 0;
	size = WideCharToMultiByte(CP_UTF8, 0, wItemName, -1,
		pUTF8Buf, buf_size, NULL, NULL );
	
	if(!size)
	{
		delete[] pUTF8Buf;
		return FALSE;
	}

	BYTE ItemNameSize = (BYTE)size - 1;
	WriteFile(m_hFile, &ItemNameSize, sizeof(ItemNameSize),
		&written, NULL);
	
	WriteFile(m_hFile, pUTF8Buf, ItemNameSize, &written, NULL);
	
	delete[] pUTF8Buf;

	// write item data
	WriteFile(m_hFile, pBuf, BufSize, &written, NULL);

	// update file header
	m_FileHeader.CountItems++;

	LARGE_INTEGER zero = {0, 0};
	res = SetFilePointerEx(m_hFile, zero,
		&m_FileHeader.OffsetAfterLastItem, FILE_CURRENT);

	res = SetFilePointerEx(m_hFile, zero,
		NULL, FILE_BEGIN);

	WriteFile(m_hFile, &m_FileHeader, sizeof(m_FileHeader), &written, NULL);

	return TRUE;
}

BOOL CTemplateFile::GetCurItemInfo(LPTSTR ItemNameBuf, int ItemNameBufSize, DWORD *pItemSize)
{
	if(ItemNameBufSize <= lstrlen(m_CurItem.ItemName))
		return FALSE;
	
	lstrcpy(ItemNameBuf, m_CurItem.ItemName);
	if(pItemSize)
		*pItemSize = m_CurItem.ItemSize;

	return TRUE;
}

BOOL CTemplateFile::GetCurItemName(CString & ItemNameBuf)
{
	ItemNameBuf = m_CurItem.ItemName;
	return TRUE;
}

BOOL CTemplateFile::GetCurItemData(LPVOID ItemDataBuf, int *ItemDataBufSize)
{
	if(*ItemDataBufSize < m_CurItem.ItemSize)
	{
		*ItemDataBufSize = m_CurItem.ItemSize;
		return FALSE;
	}
	*ItemDataBufSize = m_CurItem.ItemSize;	
	memcpy(ItemDataBuf, m_CurItem.pItemBuf, m_CurItem.ItemSize);
	return TRUE;
}


BOOL CTemplateFile::GetCurItem(LPVOID pBuf, DWORD BufSize)
{
	if(BufSize < m_CurItem.ItemSize)
		return FALSE;
	memcpy(pBuf, m_CurItem.pItemBuf, m_CurItem.ItemSize);
	return TRUE;
}

BOOL CTemplateFile::MoveToNextItem(int SkipItems)
{
	if(SkipItems == 1)
	{
		LoadCurItemHeader();
		LoadCurItemData();
	} else
	{
		for(int i = 1; i < SkipItems; i++)
		{
			LoadCurItemHeader();
			SkipLoadCurItemData();
		}
		LoadCurItemHeader();
		LoadCurItemData();
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CViewsTemplateTest

#ifdef _DEBUG

class CTemplateFileTest: public CppUnit::TestFixture
{
public:
	void setUp()
	{
	}
	
	void testCreateOpen()
	{
		CTemplateFile templ;

		CPPUNIT_ASSERT(templ.Create(_T("../../TestFiles/template.gtmpl")));
		CPPUNIT_ASSERT(templ.Close());
		CPPUNIT_ASSERT(templ.Open(_T("../../TestFiles/template.gtmpl")));
	}
	
	void testAddGet()
	{
		CTemplateFile templ;
		
		CPPUNIT_ASSERT(templ.Create(_T("../../TestFiles/template.gtmpl")));
		for(int i = 0 ; i < 10; i++)
		{
			CString str;
			str.Format(_T("Hi World %d! Привет мир %d!"), i, i);
			CPPUNIT_ASSERT(templ.AddItem(str.GetBuffer(0), &i, sizeof(i)));
		}
		CPPUNIT_ASSERT(templ.Close());

		CPPUNIT_ASSERT(templ.Open(_T("../../TestFiles/template.gtmpl")));

		int ItemCount = 0;
		CPPUNIT_ASSERT(templ.GetItemCount(&ItemCount));
		CPPUNIT_ASSERT_EQUAL(10, ItemCount);
		
		CPPUNIT_ASSERT(templ.SetPointerToFirstItem());

		CString ItemName1(_T("Hi World 0! Привет мир 0!"));
		CString Name;
		DWORD data_size;
		CPPUNIT_ASSERT(templ.GetCurItemInfo(Name.GetBuffer(256), 256, &data_size));
		CPPUNIT_ASSERT(Name == ItemName1);
		CPPUNIT_ASSERT_EQUAL((DWORD)sizeof(i), data_size);

		int data = 4444;
		CPPUNIT_ASSERT(templ.GetCurItem(&data, sizeof(data)));
		CPPUNIT_ASSERT_EQUAL(0, data);

		CPPUNIT_ASSERT(templ.MoveToNextItem(9));

		CString ItemName10(_T("Hi World 9! Привет мир 9!"));
		CPPUNIT_ASSERT(templ.GetCurItemInfo(Name.GetBuffer(256), 256, &data_size));
		CPPUNIT_ASSERT(Name == ItemName10);
		CPPUNIT_ASSERT_EQUAL((DWORD)sizeof(i), data_size);
		
		data = 4444;
		CPPUNIT_ASSERT(templ.GetCurItem((LPBYTE)&data, sizeof(data)));
		CPPUNIT_ASSERT_EQUAL(9, data);
	}

	void tearDown()
	{
	}
	
	CPPUNIT_TEST_SUITE(CTemplateFileTest);
	CPPUNIT_TEST(testCreateOpen);
	CPPUNIT_TEST(testAddGet);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CTemplateFileTest);

#endif // _DEBUG
