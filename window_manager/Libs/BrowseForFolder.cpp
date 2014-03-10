//////////////////////////////////////////////////////////////
// File:		// BrowseForFolder.cpp
// File time:	// 19.11.2003	23:51
// Version: 	// 1.0
// Description: //////////////////////////////////////////////
//   SHBrowseForFolder wrapper
//   

#include "stdafx.h"
//#include <windows.h>
#include "BrowseForFolder.h"

//////////////////////////////////////////////////////////////

static int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg) 
	{
		case BFFM_INITIALIZED: 
		{
			LPCTSTR szStartFolder = (LPCTSTR) lpData;

			// WParam is TRUE since you are passing a path.
			// It would be FALSE if you were passing a pidl.
			if(szStartFolder!=NULL)
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szStartFolder);
			break;
		}

        case BFFM_SELCHANGED: 
        {
			// Set the status window to the currently selected path.
			BOOL res = SHGetPathFromIDList((LPITEMIDLIST) lParam, szDir);
			if( res )
			{
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
			}
			else
			{
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)_T("<empty selection>") );
			}
			break;
        }

		default:
			break;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////

#ifndef	BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
                                        // Caller needs to call OleInitialize() before using this API
#define BIF_USENEWUI           (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
#endif

/////////////////////////////////////////////////////////////////////////////

BOOL	BrowseForFolder( HWND hParent, CString& szFolder, LPCTSTR szStartFolder, LPCTSTR szTitle )
{
	szFolder.Empty();

	CString s;
	LPITEMIDLIST	pidl=NULL;
	BROWSEINFO bi;
	LPMALLOC pMalloc = NULL;
	SHGetMalloc(&pMalloc);

	SHGetSpecialFolderLocation(0,CSIDL_DRIVES,&pidl);
	//pidl = ConvertPathToLpItemIdList( szStartFolder );

	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = hParent;
	if( szTitle!=NULL )
		bi.lpszTitle = szTitle;
	else
		bi.lpszTitle = _T("Choose folder:");
	bi.ulFlags =	BIF_EDITBOX |
					BIF_NEWDIALOGSTYLE |
					//BIF_RETURNONLYFSDIRS |
					BIF_STATUSTEXT;
	bi.pidlRoot = pidl;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)szStartFolder;
	
	bi.pszDisplayName = s.GetBuffer(MAX_PATH);
	CoInitialize(NULL);
	LPITEMIDLIST dir = SHBrowseForFolder(&bi);
	CoUninitialize();
	s.ReleaseBuffer();

	if(pidl)
	{
		pMalloc->Free(pidl);
	}

	if(dir!=NULL)
	{
		SHGetPathFromIDList( dir, s.GetBuffer(MAX_PATH) );
		s.ReleaseBuffer();
		szFolder = s;
		pMalloc->Free(dir);
		pMalloc->Release();
		return TRUE;
	}

	pMalloc->Release();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
