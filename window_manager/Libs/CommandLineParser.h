// CommandLineParser.h: interface for the CCommandLineParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDLINEPARSER_H__B46AA7E4_DF95_4338_8CB9_82F4CD750770__INCLUDED_)
#define AFX_COMMANDLINEPARSER_H__B46AA7E4_DF95_4338_8CB9_82F4CD750770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum EXE_MODE
{
	MODE_UNKNOWN,
	MODE_AGENT,			// side that is waiting for connections
	STATION_MANAGER,
	TASK_MANAGER,
	SPECIAL_ACTION,
	USER_MANAGER
};

class CCommandLineParser : public CCommandLineInfo  
{
public:
	CCommandLineParser();
	virtual ~CCommandLineParser();

	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	BOOL		m_bTestMode;

	BOOL		m_bSetRect;
	int			m_iCorner;
	
	CRect		m_FullViewRect;

	EXE_MODE	m_PresenterExeMode;
	
	CString		m_strArgForSpecial;
};

#endif // !defined(AFX_COMMANDLINEPARSER_H__B46AA7E4_DF95_4338_8CB9_82F4CD750770__INCLUDED_)
