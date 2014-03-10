// StringArrayEqu.cpp
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringArrayEqu.h"

CStringArrayEqu::CStringArrayEqu()
{
}
CStringArrayEqu::CStringArrayEqu(const CStringArrayEqu& op)
{
	this->Copy(op);
}
const CStringArrayEqu& CStringArrayEqu::operator =(const CStringArrayEqu& op)
{
	RemoveAll();
	Copy(op);
	return *this;
}

bool CStringArrayEqu::operator ==(const CStringArrayEqu& op)
{
	if(GetSize() != op.GetSize())
		return false;

	for(int i = 0; i < GetSize(); i++)
	{
		if(ElementAt(i) != op.GetAt(i))
			return false;
	}

	return true;
}

BOOL CStringArrayEqu::Contain(CString str)
{
	for(int i = 0; i < GetSize(); i++)
	{
		if(GetAt(i).CompareNoCase(str) == 0)
			return TRUE;
	}
	return FALSE;
}

BOOL CStringArrayEqu::FromString(CString str)
{
	// StringArray - array of double quoted "escaped" strings
	//
	RemoveAll();
	int nIndex = 0;
	int nFind = 0;
	while ((nFind = str.Find(_T(','), nIndex)) != -1)
	{
		// found ","
		if (nFind == nIndex)
		{
			// it means that comma(,) is the first char - ignore it
			nIndex++;
			if (nIndex >= str.GetLength())
				break;
			else
				continue;
		}
		Add(str.Mid(nIndex, nFind-nIndex));
		nIndex = nFind + 1; // the symbol after comma(,) that was found
	}
	// add the rest only if there is something!!!
	BOOL bSpacesOnly = TRUE;
	for(nFind = nIndex; nFind < str.GetLength(); nFind++)
	{
		if(str.GetAt(nFind) == _T(' ') || str.GetAt(nFind) == _T('\t'))
		{
			// this is whitespace
			nIndex++;
		}
		else
		{
			bSpacesOnly = FALSE;
			break;
		}
	}
	if (!bSpacesOnly)
	{
		Add(str.Mid(nIndex));
	}
	return TRUE;
}

CString CStringArrayEqu::ToString()
{
	CString strTemp;
	for(int i = 0; i < GetSize() - 1; i++)
		strTemp += GetAt(i) + _T(",");

	// add last element now
	if(GetSize() > 0) // means that at least there is something
		strTemp += GetAt(i);
	
	return strTemp;
}

#ifndef COMPILE_DL_PRESENTER
void CStringArrayEqu::LuaSerialize(CLua &L)
{
	L.AddKey("Count", GetSize());
	for(int i = 0; i < GetSize(); i++)
		L.AddKey(i + 1, GetAt(i));
}
#endif // COMPILE_DL_PRESENTER

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
BOOL CStringArrayEqu::PHPSerialize(BOOL GetData, zval *val)
{
	PHP_SERIALIZE_START

	PHP_SERIALIZE_STRINGS( *this,	_T("StringArrayEqu"))
	
	PHP_SERIALIZE_END
}
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

