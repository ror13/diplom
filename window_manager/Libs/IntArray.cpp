#include "stdafx.h"
#include "IntArray.h"

void StrToIntArray(INT_ARRAY &array, LPCTSTR str)
{
	CString tmp(str);
    TCHAR *psz_method, *psz_tmp;
    int mon;
	
	array.clear();
	
	psz_method = (LPTSTR)(LPCTSTR)tmp;
	
	if(psz_method)
	{
		while( *psz_method )
		{
			psz_tmp = psz_method;
			while( *psz_tmp && *psz_tmp != _T(',') )
			{
				psz_tmp++;
			}
			
			if( *psz_tmp )
			{
				*psz_tmp = '\0';
				mon = _ttoi( psz_method );
				array.push_back(mon);
				psz_method = psz_tmp + 1;
			}
			else
			{
				if(psz_method[0] != _T('\"'))
				{
					mon = _ttoi( psz_method );
					array.push_back(mon);
				}
				psz_method = psz_tmp;
			}
		}
	}
}

BOOL IsIntPresent(INT_ARRAY &array, int Num)
{
	INT_ARRAY::iterator i = array.begin();
	while(i != array.end())
	{
		if(*i++ == Num)
			return TRUE;
	}
	return FALSE;
}

