// CommandLineParser.cpp: implementation of the CCommandLineParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandLineParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandLineParser::CCommandLineParser()
{
	m_bTestMode = FALSE;
	m_bSetRect = FALSE;
}

CCommandLineParser::~CCommandLineParser()
{

}

void CCommandLineParser::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	BOOL res;
	res = FALSE;
	
	if(bFlag && lstrcmpi(pszParam, _T("Test")) == 0)
	{
		m_bTestMode = TRUE;
		res = TRUE;
	}
	
	if(bFlag && lstrcmpi(pszParam, _T("Rect")) == 0)
	{
		m_bSetRect = TRUE;
		m_iCorner = 0;
		res = TRUE;
	} else if(m_bSetRect && m_iCorner > -1)
	{
		if(bFlag) pszParam--;
		
		TCHAR *end;
		switch(m_iCorner++)
		{
		case 0:	m_FullViewRect.left = _tcstol(pszParam, &end, 10); break;
		case 1:	m_FullViewRect.right = _tcstol(pszParam, &end, 10); break;
		case 2:	m_FullViewRect.top = _tcstol(pszParam, &end, 10); break;
		case 3:
			m_FullViewRect.bottom = _tcstol(pszParam, &end, 10);
			m_iCorner = -1;
			break;
		}
		res = TRUE;
	}

	if(bFlag)
	{
		CString Param(pszParam);
		int pos = Param.Find(_T('='));
		CString key, value;
		if(pos == -1)
		{
			key = Param;
			value.Empty();
		} else
		{
			key = Param.Left(pos);
			value = Param.Mid(pos + 1);
		}
		
		if(key.CompareNoCase(_T("mode") ) == 0)
		{
			if(value.CompareNoCase( _T("client") ) == 0)
				m_PresenterExeMode = MODE_AGENT;
			else if(value.CompareNoCase( _T("term") ) == 0)
				m_PresenterExeMode = STATION_MANAGER;
			else if(value.CompareNoCase( _T("manage") ) == 0)
				m_PresenterExeMode = STATION_MANAGER;
			else if(value.CompareNoCase( _T("station") ) == 0)
				m_PresenterExeMode = STATION_MANAGER;
			else if(value.CompareNoCase( _T("task") ) == 0)
				m_PresenterExeMode = TASK_MANAGER;
			else if(value.CompareNoCase( _T("user") ) == 0)
				m_PresenterExeMode = USER_MANAGER;
			else if(value.CompareNoCase( _T("special") ) == 0)
				m_PresenterExeMode = SPECIAL_ACTION;
			else
			{
				m_PresenterExeMode = MODE_UNKNOWN;
				ASSERT( FALSE && "Unknown command line /mode option!" );
			}

			res = TRUE;
		}
		
		if(key.CompareNoCase(_T("arg")) == 0)
		{
			m_strArgForSpecial = value;
			res = TRUE;
		}
	}
	
	if(!res)
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}
