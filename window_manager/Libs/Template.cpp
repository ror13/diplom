// Template.cpp

#include "StdAfx.h"
#include "Template.h"

CTemplate::CTemplate()
{
	Init();
}

CTemplate::CTemplate(const CTemplate& templ)
{
	Init();
	*this = templ;
}

CTemplate& CTemplate::operator =(const CTemplate& templ)
{
	m_Width = templ.m_Width;
	m_Height = templ.m_Height;
	m_ViewList = templ.m_ViewList;
	m_strBackgroundImgPath = templ.m_strBackgroundImgPath;

	return *this;
}

TEMPLATE_VIEW& CTemplate::GetViewByIndex(int Index)
{
	TEMPLATE_VIEW_LIST_ITERATOR v = m_ViewList.begin();
	for(int i = 0; i < Index; i++)
		v++;
	return *v;
}

TEMPLATE_VIEW& CTemplate::GetViewByID(DWORD ViewID)
{
	TEMPLATE_VIEW_LIST_ITERATOR v = m_ViewList.begin();
	while(v != m_ViewList.end())
	{
		if(*v == ViewID)
			break;
		v++;
	}
	return *v;
}

void CTemplate::Init()
{
	m_Width = 0;
	m_Height = 0;
	m_ViewList.clear();
	m_strBackgroundImgPath.Empty();
}

CString	CTemplate::MakeHTML(CString strTitle)
{
	CString strWindowSize;
	strWindowSize.Format(_T("width:%dpx;height:%dpx;"), m_Width, m_Height);
	
	// html header
	CString strOut = 
		_T("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n")
		_T("<html xmlns=\"http://www.w3.org/1999/xhtml\" >\n")
		_T("  <head>\n");
	strOut += 
		_T("    <title>") + strTitle + _T("</title>\n");
	strOut +=
		_T("  </head>\n")
		_T("  <body scroll=no bgcolor=\"#000000\" style=\"margin-left:0;margin-right:0px;margin-top:0;margin-bottom:0;");
	strOut += strWindowSize;			
	strOut += _T("\">\n");
	
	// add background image
	if(!m_strBackgroundImgPath.IsEmpty())
	{
		CString strPath = m_strBackgroundImgPath;
		strPath.Replace(_T("\\"), _T("/"));
		strOut +=
			_T("  <img id=backgroung_img src=\"file://") + strPath + _T("\" />\n");
	}
	
	// add views
	TEMPLATE_VIEW_LIST_ITERATOR i;
	for(i = m_ViewList.begin(); i != m_ViewList.end(); i++)
	{
		TEMPLATE_VIEW &view = *i;
		
		CString str_div;
		str_div.Format(
			_T("    <div id=%d ")
			_T("style=\"left:%dpx; top:%dpx; width:%dpx; height:%dpx; ")
			_T("position: absolute\">")
			_T("</div>\n"),
			view.m_ViewID,
			view.m_Rect.left, view.m_Rect.top, view.m_Rect.Width(), view.m_Rect.Height()
			);
		strOut += str_div;
	}
	
	strOut +=
		_T("  </body>\n")
		_T("</html>\n");

	return strOut;
}
