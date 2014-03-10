// MonCustomRes.cpp
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MonCustomRes.h"

#include "FileOperation.h"
#include "PresenterGlobals.h"


CCustomResolution::CCustomResolution()
{
	m_Txt = _T("");
	m_W = 0;
	m_H = 0;
	m_Custom = FALSE;
}

CCustomResolution::CCustomResolution(CString txt, int w, int h, BOOL Custom)
{
	m_Txt = txt;
	m_W = w;
	m_H = h;
	m_Custom = Custom;
}

CCustomResolution::CCustomResolution(int w, int h, BOOL Custom)
{
	m_Txt.Format(_T("%d by %d pixels"), w, h);
	if (Custom)
	{
		m_Txt += _T(" (custom)");
	}
	m_W = w;
	m_H = h;
	m_Custom = Custom;
}

CCustomResolution::~CCustomResolution()
{

}

BOOL CCustomResolution::Serialize( SERIALIZE_CONTEXT& context )
{
	SERIALIZE_START
		
		SERIALIZE_STRING(		m_Txt,		_T("TXT") )
		SERIALIZE_INT( INT,		m_W,		_T("W") )
		SERIALIZE_INT( INT,		m_H,		_T("H") )
		SERIALIZE_INT( BOOL,	m_Custom,	_T("CUSTOM") )
		
	SERIALIZE_END
}

CCustomResolutionList::CCustomResolutionList()
{
// 	m_ResArray.Add(CCustomResolution(_T("640 by 480 pixels"), 640, 480));
// 	m_ResArray.Add(CCustomResolution(_T("800 by 600 pixels"), 800, 600));
// 	m_ResArray.Add(CCustomResolution(_T("1024 by 768 pixels"), 1024, 768));
// 	m_ResArray.Add(CCustomResolution(_T("1280 by 720 pixels"), 1280, 720));
// 	m_ResArray.Add(CCustomResolution(_T("1280 by 768 pixels"), 1280, 768));
// 	m_ResArray.Add(CCustomResolution(_T("1280 by 800 pixels"), 1280, 800));
// 	m_ResArray.Add(CCustomResolution(_T("1280 by 1024 pixels"), 1280, 1024));
// 	m_ResArray.Add(CCustomResolution(_T("1360 by 768 pixels"), 1360, 768));
// 	m_ResArray.Add(CCustomResolution(_T("1440 by 900 pixels"), 1440, 900));
// 	m_ResArray.Add(CCustomResolution(_T("1600 by 1024 pixels"), 1600, 1024));
// 	m_ResArray.Add(CCustomResolution(_T("1600 by 1200 pixels"), 1600, 1200));
// 	m_ResArray.Add(CCustomResolution(_T("1680 by 1050 pixels"), 1680, 1050));
// 	m_ResArray.Add(CCustomResolution(_T("1920 by 1080 pixels"), 1920, 1080));
// 	m_ResArray.Add(CCustomResolution(_T("2048 by 1536 pixels"), 2048, 1536));
// 	m_ResArray.Add(CCustomResolution(_T("3840 by 2160 pixels"), 3840, 2160));
// 	m_ResArray.Add(CCustomResolution(_T("5760 by 2160 pixels"), 5760, 2160));
}

int CCustomResolutionList::Add(CCustomResolution& Res)
{
	for (int i = 0; i < m_ResArray.GetSize(); i++)
	{
		if (m_ResArray[i].m_W == Res.m_W &&
			m_ResArray[i].m_H == Res.m_H)
		{
			return TRUE;
		}
	}

	return m_ResArray.Add(Res);
}

int CCustomResolutionList::Edit(CCustomResolution& Res, CCustomResolution& ResNew)
{
	for (int i = 0; i < m_ResArray.GetSize(); i++)
	{
		if (m_ResArray[i].m_W == Res.m_W &&
			m_ResArray[i].m_H == Res.m_H)
		{
			m_ResArray[i].m_W = ResNew.m_W;
			m_ResArray[i].m_H = ResNew.m_H;
			m_ResArray[i].m_Txt = ResNew.m_Txt;
			m_ResArray[i].m_Custom = ResNew.m_Custom;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CCustomResolutionList::Remove(int x, int y)
{
	int ind = GetIndex(x, y);

	if (ind == -1)
	{
		return FALSE;
	}

	return Remove(ind);
}

BOOL CCustomResolutionList::Remove(int ind)
{
	m_ResArray.RemoveAt(ind);

	return TRUE;
}

int CCustomResolutionList::GetIndex(int x, int y)
{
	for(int i = 0; i < m_ResArray.GetSize(); i++)
	{
		if(m_ResArray[i].m_W == x && m_ResArray[i].m_H == y)
			return i;
	}
	return -1;
}

CString CCustomResolutionList::GetString(int x, int y)
{
	int Index = GetIndex(x, y);
	return GetStringFromIndex(Index);
}

CString CCustomResolutionList::GetStringFromIndex(int ind)
{
	if(ind < 0 || ind >= m_ResArray.GetSize())
		return _T("");
	
	return m_ResArray[ind].m_Txt;
}

int CCustomResolutionList::GetX(int ind)
{
	if(ind < 0 || ind >= m_ResArray.GetSize())
		return 0;
	
	return m_ResArray[ind].m_W;
}

int CCustomResolutionList::GetY(int ind)
{
	if(ind < 0 || ind >= m_ResArray.GetSize())
		return 0;
	
	return m_ResArray[ind].m_H;
}

static CCustomResolution CustomResolutionZero(0, 0, 0);
CCustomResolution& CCustomResolutionList::Get(int ind)
{
	if(ind < 0 || ind >= m_ResArray.GetSize())
	{
		return CustomResolutionZero;
	}
	
	return m_ResArray[ind];
}

BOOL CCustomResolutionList::Serialize(SERIALIZE_CONTEXT& context)
{
	SERIALIZE_START

		SERIALIZE_ARRAY(CCustomResolution, m_ResArray,		_T("arrCustomResolution") )

	SERIALIZE_END
}

BOOL CCustomResolutionList::SaveToFile()
{
	CString strCustomResolutionList;
	GetValues(strCustomResolutionList);

	BOOL Res = SaveUTF8FileFromString(GetRootFolder() + CUSTOM_RESOLUTIONS_FILE_NAME, strCustomResolutionList);

	return Res;
}

BOOL CCustomResolutionList::LoadFromFile()
{
	CString strCustomResolutionList;
	
	BOOL Res = LoadUTF8FileToString(GetRootFolder() + CUSTOM_RESOLUTIONS_FILE_NAME, strCustomResolutionList);

	if (Res)
	{
		SetValues(strCustomResolutionList);
	}
		
	return Res;

}

CCustomResolutionList g_CustomRes;
