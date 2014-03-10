#include "StdAfx.h"
#include "GUIHelpers.h"

void HideWindowsTaskBar(BOOL bHide)
{
	CWnd* pTrayWnd = CWnd::FindWindow(_T("Shell_TrayWnd"), _T(""));

	CWnd* pStartWnd = CWnd::FromHandle(::FindWindowEx( NULL, NULL, ((const TCHAR*)0xC017), NULL));

	if ( bHide )
	{
		if (pTrayWnd)
		{
			pTrayWnd->ShowWindow(SW_HIDE);
		}
		if (pStartWnd)
		{
			pStartWnd->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (pTrayWnd)
		{
			pTrayWnd->ShowWindow(SW_SHOW);
		}
		if (pStartWnd)
		{
			pStartWnd->ShowWindow(SW_SHOW);
		}
	}
}
