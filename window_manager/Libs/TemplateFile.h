// TemplateFile.h
//////////////////////////////////////////////////////////////////////////

#ifndef _TEMPLATEFILE_H_
#define _TEMPLATEFILE_H_

#define TEMPLATE_SIGNATURE	"IntegratedTemplate"

class CTemplateFile
{
public:
	struct FILE_HEADER
	{
		char			Signature[sizeof(TEMPLATE_SIGNATURE)];
		DWORD			HeaderSize;
		DWORD			CountItems;
		LARGE_INTEGER	OffsetAfterLastItem;
	};

	struct ITEM
	{
		DWORD			ItemSize;
		TCHAR			ItemName[256];
		LPBYTE			pItemBuf;
	};

	CTemplateFile();
	~CTemplateFile();

	BOOL Create(LPCTSTR FilePath);
	BOOL Open(LPCTSTR FilePath);
	BOOL Close();

	BOOL GetItemCount(int *pCount);
	BOOL SetPointerToFirstItem();
	BOOL AddItem(LPCTSTR ItemName, LPVOID pBuf, int BufSize);
	BOOL GetCurItemInfo(LPTSTR BufItemName, int BufItemNameSize, DWORD *pItemSize);
	BOOL GetCurItem(LPVOID pBuf, DWORD BufSize);
	BOOL GetCurItemName(CString & ItemNameBuf);
	BOOL GetCurItemData(LPVOID ItemDataBuf, int *ItemDataBufSize);

	BOOL MoveToNextItem(int SkipItems = 1);

protected:
	BOOL LoadCurItemHeader();
	BOOL LoadCurItemData();
	BOOL SkipLoadCurItemData();

	HANDLE		m_hFile;
	FILE_HEADER	m_FileHeader;

	ITEM		m_CurItem;
};

#endif //_TEMPLATEFILE_H_
