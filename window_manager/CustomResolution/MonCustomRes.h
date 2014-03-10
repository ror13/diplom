// MonCustomRes.h
//////////////////////////////////////////////////////////////////////////

#ifndef _MONCUSTOMRES_H_
#define _MONCUSTOMRES_H_

#include <vector>
#include "SerializeStruct.h"

class CCustomResolution : public CSerializeStruct
{
public:
	CCustomResolution();
	CCustomResolution(CString txt, int w, int h, BOOL Custom = FALSE);
	CCustomResolution(int w, int h, BOOL Custom = FALSE);
	
	~CCustomResolution();

	CString m_Txt;
	int		m_W;
	int		m_H;
	BOOL	m_Custom;

	virtual BOOL Serialize( SERIALIZE_CONTEXT& context );
};

class CCustomResolutionList : public CSerializeStruct
{
public:
	CCustomResolutionList();

	int Size() {return m_ResArray.GetSize();}
	int GetIndex(int x, int y);
	CString GetString(int x, int y);
	CString GetStringFromIndex(int ind);

	BOOL Remove(int x, int y);
	BOOL Remove(int ind);

	void RemoveAll() { m_ResArray.RemoveAll(); }; 

	int Add(CCustomResolution& Res);
	int Edit(CCustomResolution& Res, CCustomResolution& ResNew);

	int GetX(int ind);
	int GetY(int ind);

	CCustomResolution& CCustomResolutionList::Get(int ind);

	BOOL SaveToFile();
	BOOL LoadFromFile();
	
	virtual BOOL Serialize( SERIALIZE_CONTEXT& context );

protected:
	CArray<CCustomResolution, CCustomResolution&> m_ResArray;

};

extern CCustomResolutionList g_CustomRes;


#endif //_MONCUSTOMRES_H_
