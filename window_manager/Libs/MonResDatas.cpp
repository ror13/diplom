// MonResDatas.cpp
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MonResDatas.h"


CResolutionList::CResolutionList()
{
	m_ResArray.push_back(RES(_T("640 by 480 pixels"), 640, 480));
	m_ResArray.push_back(RES(_T("800 by 600 pixels"), 800, 600));
	m_ResArray.push_back(RES(_T("1024 by 768 pixels"), 1024, 768));
	m_ResArray.push_back(RES(_T("1280 by 720 pixels"), 1280, 720));
	m_ResArray.push_back(RES(_T("1280 by 768 pixels"), 1280, 768));
	m_ResArray.push_back(RES(_T("1280 by 800 pixels"), 1280, 800));
	m_ResArray.push_back(RES(_T("1280 by 1024 pixels"), 1280, 1024));
	m_ResArray.push_back(RES(_T("1360 by 768 pixels"), 1360, 768));
	m_ResArray.push_back(RES(_T("1440 by 900 pixels"), 1440, 900));
	m_ResArray.push_back(RES(_T("1600 by 1024 pixels"), 1600, 1024));
	m_ResArray.push_back(RES(_T("1600 by 1200 pixels"), 1600, 1200));
	m_ResArray.push_back(RES(_T("1680 by 1050 pixels"), 1680, 1050));
	m_ResArray.push_back(RES(_T("1920 by 1080 pixels"), 1920, 1080));
	m_ResArray.push_back(RES(_T("1920 by 1200 pixels"), 1920, 1200));
	m_ResArray.push_back(RES(_T("2048 by 1536 pixels"), 2048, 1536));
	m_ResArray.push_back(RES(_T("2560 by 1600 pixels"), 2560, 1600));
	m_ResArray.push_back(RES(_T("3840 by 2160 pixels"), 3840, 2160));
	m_ResArray.push_back(RES(_T("5760 by 2160 pixels"), 5760, 2160));
	m_ResArray.push_back(RES(_T("6400 by 4096 pixels"), 6400, 4096));
}

void CResolutionList::GenerateSmallResolutionsList()
{
	m_ResArray.clear();
	m_ResArray.push_back(RES(_T("640 by 480 pixels"), 640, 480));
	m_ResArray.push_back(RES(_T("1024 by 768 pixels"), 1024, 768));
	m_ResArray.push_back(RES(_T("1280 by 1024 pixels"), 1280, 1024));
	m_ResArray.push_back(RES(_T("1600 by 1200 pixels"), 1600, 1200));
	m_ResArray.push_back(RES(_T("1920 by 1080 pixels"), 1920, 1080));
	m_ResArray.push_back(RES(_T("2048 by 1536 pixels"), 2048, 1536));
	m_ResArray.push_back(RES(_T("2560 by 1600 pixels"), 2560, 1600));
	m_ResArray.push_back(RES(_T("3840 by 2160 pixels"), 3840, 2160));
	m_ResArray.push_back(RES(_T("5760 by 2160 pixels"), 5760, 2160));
	m_ResArray.push_back(RES(_T("6400 by 4096 pixels"), 6400, 4096));
}

int CResolutionList::GetIndex(int x, int y)
{
	for(int i = 0; i < m_ResArray.size(); i++)
	{
		if(m_ResArray[i].m_W == x && m_ResArray[i].m_H == y)
			return i;
	}
	return -1;
}

CString CResolutionList::GetString(int x, int y)
{
	int Index = GetIndex(x, y);
	return GetStringFromIndex(Index);
}

CString CResolutionList::GetStringFromIndex(int ind)
{
	if(ind < 0 || ind >= m_ResArray.size())
		return _T("");

	return m_ResArray[ind].m_Txt;
}

int CResolutionList::GetX(int ind)
{
	if(ind < 0 || ind >= m_ResArray.size())
		return 0;
	
	return m_ResArray[ind].m_W;
}

int CResolutionList::GetY(int ind)
{
	if(ind < 0 || ind >= m_ResArray.size())
		return 0;
	
	return m_ResArray[ind].m_H;
}

BOOL CResolutionList::Add(RES& Res)
{
	for(int i = 0; i < m_ResArray.size(); i++)
	{
		if(m_ResArray[i].m_W == Res.m_W && m_ResArray[i].m_H == Res.m_H)
		{
			return TRUE;
		}
	}

	m_ResArray.push_back(Res);

	return TRUE;
}

void CResolutionList::RemoveAllCustoms()
{
	for(int i = 0; i < m_ResArray.size(); i++)
	{
		if(m_ResArray[i].m_Custom)
		{
			m_ResArray.erase(m_ResArray.begin() + i);
			i = 0;
		}
	}
}

CResolutionList g_Res;
//////////////////////////////////////////////////////////////////////////

CColorList::CColorList()
{
	m_ColorArray.push_back(BITS(_T("256 colors"), 8));
	m_ColorArray.push_back(BITS(_T("16 bits"), 16));
	m_ColorArray.push_back(BITS(_T("32 bits"), 32));
}

int CColorList::GetIndex(int Bits)
{
	for(int i = 0; i < m_ColorArray.size(); i++)
	{
		if(m_ColorArray[i].m_Bits == Bits)
			return i;
	}
	return -1;
}

CString CColorList::GetString(int Bits)
{
	int Index = GetIndex(Bits);
	return GetStringFromIndex(Index);
}

CString CColorList::GetStringFromIndex(int ind)
{
	if(ind < 0 || ind >= m_ColorArray.size())
		return _T("");
	
	return m_ColorArray[ind].m_Txt;
}

int CColorList::GetBits(int ind)
{
	if(ind < 0 || ind >= m_ColorArray.size())
		return 0;
	
	return m_ColorArray[ind].m_Bits;
}

CColorList g_Bits;
//////////////////////////////////////////////////////////////////////////

CFrequencyList::CFrequencyList()
{
	m_FreqArray.push_back(FREQ(_T("60 Hertz"), 60));
	m_FreqArray.push_back(FREQ(_T("70 Hertz"), 70));
	m_FreqArray.push_back(FREQ(_T("72 Hertz"), 72));
	m_FreqArray.push_back(FREQ(_T("75 Hertz"), 75));
	m_FreqArray.push_back(FREQ(_T("85 Hertz"), 85));
	m_FreqArray.push_back(FREQ(_T("100 Hertz"), 100));
	m_FreqArray.push_back(FREQ(_T("120 Hertz"), 120));
}

int CFrequencyList::GetIndex(int Freq)
{
	for(int i = 0; i < m_FreqArray.size(); i++)
	{
		if(m_FreqArray[i].m_Freq == Freq)
			return i;
	}
	return -1;
}

CString CFrequencyList::GetString(int Freq)
{
	int Index = GetIndex(Freq);
	return GetStringFromIndex(Index);
}

CString CFrequencyList::GetStringFromIndex(int ind)
{
	if(ind < 0 || ind >= m_FreqArray.size())
		return _T("");
	
	return m_FreqArray[ind].m_Txt;
}

int CFrequencyList::GetFrequency(int ind)
{
	if(ind < 0 || ind >= m_FreqArray.size())
		return 0;
	
	return m_FreqArray[ind].m_Freq;
}

CFrequencyList g_Freq;
