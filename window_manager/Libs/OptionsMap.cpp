//////////////////////////////////////////////////////////////
// File:		// OptionsMap.cpp
// File time:	// 30.04.2004	15:24
// Version: 	// 1.0
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "OptionsMap.h"
#include "FileOperation.h"

#include "CommonApi.h"

//////////////////////////////////////////////////////////////

void COptionsMapStruct::StripLeft(LPTSTR Buffer)
{
	// strip begin white spaces and tabs
	int iLen = lstrlen(Buffer);
	int iPos = 0;
	while(iLen-- > 0)
	{
		if(Buffer[iPos] != _T(' ') && Buffer[iPos] != _T('\t'))
			break;
		
		iPos++;
	}
	
	if(iPos > 0)
		lstrcpy(Buffer, Buffer + iPos);
}

void COptionsMapStruct::StripRight(LPTSTR Buffer)
{
	// strip end white spaces, tabs and \r
	int iLen = lstrlen(Buffer);
	while(iLen-- > 0)
	{
		if( Buffer[iLen] != _T(' ') &&
			Buffer[iLen] != _T('\t') &&
			Buffer[iLen] != _T('\r')
			)
			break;
	}
	Buffer[iLen + 1] = _T('\0');
}

void COptionsMapStruct::StripLine(LPTSTR Buffer)
{
	StripLeft(Buffer);
	StripRight(Buffer);
}

void COptionsMapStruct::FromStringToMap(CString strOpt, COptionsMap &map)
{
	map.RemoveAll();
	
	LPTSTR NextParam = strOpt.GetBuffer(0);
	StripLine(NextParam);

	LPTSTR ParamVal;
	while((ParamVal = _tcsstr(NextParam, _T(":"))) != NULL)
	{
		LPTSTR Name = NextParam;
		LPTSTR Val = CharNext(ParamVal);
		
		*ParamVal = _T('\0');
		
		StripLine(Name);
		StripLeft(Val);
		
		LPTSTR EndParamVal;
		EndParamVal = _tcsstr(Val, _T(";"));
		
		if(EndParamVal)
		{
			NextParam = CharNext(EndParamVal);
			*EndParamVal = _T('\0');
		} else
			NextParam = Val + lstrlen(Val);
		
		map.SetOption(Name, Val);
	}
}

//////////////////////////////////////////////////////////////////////////

CString CRectMapStruct::Serialize()
{
	CString str;
	str.Format(_T("top:%d; left:%d; bottom:%d; right:%d"),
		m_pRect->top, m_pRect->left, m_pRect->bottom, m_pRect->right);
	return str;
}

BOOL CRectMapStruct::Unserialize(CString strStruct)
{
	COptionsMap map;
	FromStringToMap(strStruct, map);
	m_pRect->top = _ttoi(map.GetOption(_T("top")));
	m_pRect->left = _ttoi(map.GetOption(_T("left")));
	m_pRect->bottom = _ttoi(map.GetOption(_T("bottom")));
	m_pRect->right = _ttoi(map.GetOption(_T("right")));
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////

COptionsMap::COptionsMap (void)
{
//	this->InitHashTable( 1001 );
}

/*
COptionsMap::~COptionsMap (void)
{
}
*/

BOOL COptionsMap::DelOption( LPCTSTR szOptionName )
{
	return RemoveKey(szOptionName);
}

BOOL COptionsMap::DelOption( LPCTSTR szOptionName, DWORD dwOptionIndex )
{
	CString name, s;
	name.Format(_T("%s%d"), szOptionName, dwOptionIndex);
	return DelOption(name);
}

void COptionsMap::Escape( CString& s ) const
{
	BOOL bQuoted = FALSE;
	if( !s.IsEmpty() && ( !_istalnum(s[0]) || !_istalnum(s[s.GetLength()-1]) ) )
	{
		bQuoted = TRUE;
	}
	for( int i=0; i<s.GetLength(); i++ )
	{
		if( s[i] == _T('\\') )
		{
			s.SetAt( i, _T('\\') );
			s.Insert( i+1, _T('\\') );
			i++;
			continue;
		}
		if( s[i] == _T('\r') )
		{
			s.SetAt( i, _T('\\') );
			s.Insert( i+1, _T('r') );
			i++;
			continue;
		}
		if( s[i] == _T('\n') )
		{
			s.SetAt( i, _T('\\') );
			s.Insert( i+1, _T('n') );
			i++;
			continue;
		}
		if( bQuoted && s[i] == _T('\"') )
		{
			s.SetAt( i, _T('\\') );
			s.Insert( i+1, _T('\"') );
			i++;
			continue;
		}
	}
	
	if( bQuoted )
	{
		s = _T("\"") + s + _T("\"");
	}
}

void COptionsMap::Unescape( CString& s ) const
{
	BOOL bQuoted = FALSE;
	if( s.GetLength()>=2 && s[0] == _T('\"') && s[s.GetLength()-1] == _T('\"') )
	{
		s = s.Mid( 1, s.GetLength()-2 );
		bQuoted = TRUE;
	}
	
	for( int i=0; i<s.GetLength(); i++ )
	{
		if( s[i] == _T('\\') )
		{
			if( s[i+1] == _T('r') )
			{
				s.SetAt( i, _T('\r') );
				s.Delete( i+1 );
				continue;
			}
			if( s[i+1] == _T('\\') )
			{
				s.SetAt( i, _T('\\') );
				s.Delete( i+1 );
				continue;
			}
			if( s[i+1] == _T('n') )
			{
				s.SetAt( i, _T('\n') );
				s.Delete( i+1 );
				continue;
			}
			if( bQuoted && s[i+1] == _T('\"') )
			{
				s.SetAt( i, _T('\"') );
				s.Delete( i+1 );
				continue;
			}
		}
	}
}

//////////////////////////////////////////////////////////////

CString COptionsMap::GetOption( LPCTSTR szOptionName ) const
{
	CString s;
	Lookup( szOptionName, s );
	Unescape( s );
	return s;
}

int COptionsMap::GetOptionInt( LPCTSTR szOptionName ) const
{
	return _ttoi(GetOption(szOptionName));
}

LONGLONG COptionsMap::GetOptionLONGLONG( LPCTSTR szOptionName ) const
{
	return _ttoi64(GetOption(szOptionName));
}

///////////////////////
void COptionsMap::SetOption( LPCTSTR szOptionName, CString szText )
{
	Escape(szText);
	SetAt( szOptionName, szText );
}

void COptionsMap::SetOption( LPCTSTR szOptionName, int dwNumber )
{
	CString s;
	s.Format( _T("%d"), dwNumber );
	SetOption( szOptionName, s );
}

void COptionsMap::SetOption( LPCTSTR szOptionName, DWORD dwNumber )
{
	CString s;
	s.Format( _T("%d"), dwNumber );
	SetOption( szOptionName, s );
}

void COptionsMap::SetOption( LPCTSTR szOptionName, UINT dwNumber )
{
	CString s;
	s.Format( _T("%d"), dwNumber );
	SetOption( szOptionName, s );
}

void COptionsMap::SetOption( LPCTSTR szOptionName, LONGLONG dwNumber )
{
	CString s;
	char buf[128];
	_i64toa(dwNumber, buf, 10);
	s.Format(_T("%hs"), buf);
	SetOption(szOptionName, s);
}

void COptionsMap::SetOption( LPCTSTR szOptionName, COptionsMapStruct *pStruct)
{
	CString val;
	val = pStruct->Serialize();
	SetOption(szOptionName, val);
}

void COptionsMap::SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, LPCTSTR szText )
{
	CString name;
	name.Format( _T("%s%d"), szOptionName, dwOptionIndex );
	SetOption( name, szText );
}

void COptionsMap::SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, DWORD dwNumber )
{
	CString name, s;
	name.Format( _T("%s%d"), szOptionName, dwOptionIndex );
	s.Format( _T("%d"), dwNumber );
	SetOption( name, s );
}

void COptionsMap::SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, int dwNumber )
{
	CString name, s;
	name.Format( _T("%s%d"), szOptionName, dwOptionIndex );
	s.Format( _T("%d"), dwNumber );
	SetOption( name, s );
}

void COptionsMap::SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, COptionsMapStruct *pStruct)
{
	CString name;
	name.Format( _T("%s%d"), szOptionName, dwOptionIndex );
	CString val;
	val = pStruct->Serialize();
	SetOption(name, val);
}

///////////////////////
BOOL COptionsMap::IsOptionExists( LPCTSTR szOptionName ) const
{
	CString s;
	BOOL res = Lookup( szOptionName, s );
	return res;
}

void COptionsMap::RemoveAll()
{
	CMap<CString, LPCTSTR, CString, LPCTSTR>::RemoveAll();
}

//////////////////////////////////////////////////////////////
CString COptionsMap::ToString()
{
	CString s, d;
	POSITION pos = GetStartPosition();
	while(pos != NULL)
	{
		CString key, value;
		GetNextAssoc(pos, key, value);
		d.Format(_T("%s = %s\r\n"), key, value);
		s += d;
	}
	return s;
}

void COptionsMap::FromString(CString str)
{
	RemoveAll();

	CString line;
	while(!str.IsEmpty())
	{
		line = GetLine(str);
		if(line.IsEmpty())
			continue;
		if( line.Left(1) == _T(";") )
			continue;

		CString name, value;
		if(IsValue(line, name, value))
		{
			SetAt( name, value );
		}
	}
}

//////////////////////////////////////////////////////////////

BOOL COptionsMap::SaveToRegistry(HKEY hRootKey, LPCTSTR szSubKey)
{
	CString s, d;
	POSITION pos = GetStartPosition();
	while(pos != NULL)
	{
		CString key, value;
		GetNextAssoc(pos, key, value);

		BOOL res = regWriteRegString(hRootKey, szSubKey, key, value);
		if(!res)
			return FALSE;
	}

	return TRUE;
}

BOOL COptionsMap::LoadFromRegistry(HKEY hRootKey, LPCTSTR szSubKey)
{
	BOOL bRes = FALSE;

	// open registry
	HREGKEY hKey = regOpenRegKey(hRootKey,	szSubKey, KEY_QUERY_VALUE);
	if( hKey == NULL )
		return FALSE;

	DWORD Index = 0;

	CString strOtions;
	LONG res = ERROR_SUCCESS;
	while(res == ERROR_SUCCESS)
	{
		CString strKey;
		DWORD strKeySize = 256;
		
		CString strVal;
		DWORD strValSize = 16384;
		DWORD keyType = REG_SZ;
		res = RegEnumValue(hKey, Index++, strKey.GetBuffer(strKeySize), &strKeySize,
			NULL, 
			&keyType, (LPBYTE)strVal.GetBuffer(strValSize), &strValSize);
		strKey.ReleaseBuffer();
		strVal.ReleaseBuffer();

		if(res == ERROR_SUCCESS && keyType == REG_SZ)
		{
			SetAt(strKey, strVal);
			continue;
		}

		if(res == ERROR_NO_MORE_ITEMS)
		{
			bRes = TRUE;
			break;
		}
	}

	return bRes;
}

//////////////////////////////////////////////////////////////
