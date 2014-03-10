// template_parser.h
//////////////////////////////////////////////////////////////////////////

#ifndef _TEMPLATE_PARSER_H_
#define _TEMPLATE_PARSER_H_

#include "OptionsMap.h"

#include <list>

void StripComment(LPTSTR Buffer);

void StripLeft(LPTSTR Buffer);
void StripRight(LPTSTR Buffer);
void StripLine(LPTSTR Buffer);

void StripHtml(LPTSTR Buffer);

class CHtmlTagParser
{
public:
	BOOL Parse(LPTSTR Buffer);

	virtual void OnHtmlTag(LPCTSTR TagName, LPTSTR TagParam) = 0;
};

class CParamList
{
public:
	struct PARAM
	{
		TCHAR	Name[20];
		TCHAR	Value[256];
	};
	
	CParamList();
	LPTSTR GetParam(LPCTSTR ParamName);
	
	int		m_ParamListSize;
	PARAM	m_ParamList[20];
};

class CTagParamList : public CParamList
{
public:
	void FromString(LPTSTR TagParams);
};

class CStyleParamList : public CParamList
{
public:
	void FromString(LPTSTR Style);
};

#define TEMPLATE_EXT	_T(".gtmpl")

class CViewsTemplate : public CHtmlTagParser
{
public:
	struct VIEW
	{
		DWORD	ViewID;
		RECT	Rect;
	};

	struct IMG
	{
		TCHAR	ImgSource[MAX_PATH];
		BOOL	bExternalLink;
		TCHAR	ImgMD5[33];
	};

	CViewsTemplate();

	BOOL LoadViews(CString TemplateFile);
	BOOL LoadViews(CString TemplateFile, CString &sHtmlFileName);
	BOOL LoadViewsFromHtml(CString HtmlFileFile);

	int GetViewsCount();	// we allways have zero view (background)
	BOOL GetViewInfo(DWORD ViewID, LPRECT pRect);
	
	CString GetBackgroungImageFilePath() {return m_strBackgroungImageFilePath;}
	BOOL GetTemplateResolution(int *pWidth, int *pHeight);

	BOOL CViewsTemplate::CreateScheduleTemplateFile(
		const CString& SourceTemplateFile, const CString& outputFolder, CString &NewTemplateFile);
	
	BOOL ExpandScheduleTemplateFile(CString TemplateFileName,
		CString FolderForTemplates,	CString &strHtmlFileName, LPCTSTR wantedHtmlNam);

	// CHtmlTagParser
	virtual void OnHtmlTag(LPCTSTR TagName, LPTSTR TagParam);

	struct FIND_TAG
	{
		LPTSTR TagName;
		LPTSTR ParamName;
	};

	BOOL FindTagParamVal(CString Html, int BeginPos,
		FIND_TAG Tags[],
		int *pTagParamValBeginPos, int *pTagParamValEndPos);

	CString CreateNewFileName(IMG Img);
	CString CreateAbsFilePath(CString strHtmlDir, CString strItemPath);

	CString			m_DivParams;
	BOOL			m_bIsScript;

	typedef std::list<VIEW>			TVIEW_LIST;
	typedef TVIEW_LIST::iterator	VIEW_ITERATOR;

	TVIEW_LIST		m_ViewList;

	typedef std::list<IMG>		IMG_LIST;
	typedef IMG_LIST::iterator	IMG_ITERATOR;

//	IMG_LIST		m_ImgList;

	CString			m_WorkDir;
	CString			m_strBackgroungImageFilePath;
	CString			m_strHtmlName;
	int				m_Width;
	int				m_Height;
};

#endif //_TEMPLATE_PARSER_H_
