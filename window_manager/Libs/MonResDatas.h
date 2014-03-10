// MonResDatas.h
//////////////////////////////////////////////////////////////////////////

#ifndef _DLGDATAS_H_
#define _DLGDATAS_H_

#include <vector>

struct RES
{
	CString m_Txt;
	int		m_W;
	int		m_H;
	BOOL	m_Custom;
	
	RES(CString txt, int w, int h, BOOL Custom = FALSE)
	{
		m_Txt = txt;
		m_W = w;
		m_H = h;
		m_Custom = Custom;
	}
};

class CResolutionList
{
public:
	CResolutionList();

	void GenerateSmallResolutionsList();

	int Size() {return m_ResArray.size();}
	int GetIndex(int x, int y);
	CString GetString(int x, int y);
	CString GetStringFromIndex(int ind);

	int GetX(int ind);
	int GetY(int ind);

	BOOL Add(RES& Res);

	void RemoveAllCustoms();
	
protected:
	std::vector<RES> m_ResArray;
};

extern CResolutionList g_Res;

//////////////////////////////////////////////////////////////////////////

struct BITS
{
	CString	m_Txt;
	int		m_Bits;
	
	BITS(CString txt, int bits)
	{
		m_Txt = txt;
		m_Bits = bits;
	}
};

class CColorList
{
public:
	CColorList();
	
	int Size() {return m_ColorArray.size();}
	int GetIndex(int Bits);
	CString GetString(int Bits);
	CString GetStringFromIndex(int ind);
	
	int GetBits(int ind);
	
protected:
	std::vector<BITS> m_ColorArray;
};

extern CColorList g_Bits;

//////////////////////////////////////////////////////////////////////////

struct FREQ
{
	CString	m_Txt;
	int		m_Freq;
	
	FREQ(CString txt, int freq)
	{
		m_Txt = txt;
		m_Freq = freq;
	}
};

class CFrequencyList
{
public:
	CFrequencyList();
	
	int Size() {return m_FreqArray.size();}
	int GetIndex(int Freq);
	CString GetString(int Freq);
	CString GetStringFromIndex(int ind);
	
	int GetFrequency(int ind);
	
protected:
	std::vector<FREQ> m_FreqArray;
};

extern CFrequencyList g_Freq;

#endif //_DLGDATAS_H_
