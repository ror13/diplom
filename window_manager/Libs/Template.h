// Template.h

#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include <list>

struct TEMPLATE_VIEW
{
	DWORD	m_ViewID;
	CRect	m_Rect;
	
	TEMPLATE_VIEW()
	{
		m_ViewID = 0;
	}
	
	BOOL operator ==(const TEMPLATE_VIEW &tv)
	{
		return m_ViewID == tv.m_ViewID && m_Rect == tv.m_Rect;
	}
	
	BOOL operator ==(const DWORD ViewID)
	{
		return m_ViewID == ViewID;
	}
};

typedef std::list<TEMPLATE_VIEW>		TEMPLATE_VIEW_LIST;
typedef TEMPLATE_VIEW_LIST::iterator	TEMPLATE_VIEW_LIST_ITERATOR;

class CTemplate
{
public:
	int						m_Width;
	int						m_Height;
	CString					m_strBackgroundImgPath;
	TEMPLATE_VIEW_LIST		m_ViewList;

	CTemplate();
	CTemplate(const CTemplate& templ);

	CTemplate& operator =(const CTemplate& templ);
	
	TEMPLATE_VIEW& GetViewByIndex(int Index);
	TEMPLATE_VIEW& GetViewByID(DWORD ViewID);

	void Init();
	CString	MakeHTML(CString strTitle);
};

#endif //_TEMPLATE_H_
