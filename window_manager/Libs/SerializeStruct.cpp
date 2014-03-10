//////////////////////////////////////////////////////////////
// File:		// SerializeStruct.cpp
// File time:	// 19.05.2004	17:17
// Version: 	// 1.0
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "SerializeStruct.h"
#include "FileOperation.h"

BOOL GetSection(CString inBuf, CString SectName, CString &SectParam, CString &SectBody, CString &SectNext)
{
	BOOL bFound = FALSE;
	SectBody.Empty();
	
	while(!inBuf.IsEmpty())
	{
		CString line = GetLine(inBuf);
		
		if(!bFound)
		{
			int ind_section_begin = line.Find(_T("["));
			if(ind_section_begin != 0)
				continue;
			int ind_section_end = line.Find(_T("]"));
			if(ind_section_end == -1)
				continue;
			
			CString section_name =
				line.Mid(ind_section_begin + 1, ind_section_end - ind_section_begin - 1);
			
			int ind_section_param = section_name.Find(_T("="));
			if(ind_section_param != -1)
				SectParam = section_name.Mid(ind_section_param + 1);
			else
				SectParam.Empty();
			
			if(ind_section_param != -1)
			{
				if(section_name.Left(ind_section_param) != SectName)
					continue;
			} else
			{
				if(section_name != SectName)
					continue;
			}
			
			bFound = TRUE;
			continue;
		}
		
		int ind_next_section_begin = line.Find(_T("["));
		if(ind_next_section_begin == 0)
		{
			inBuf = line + _T("\n") + inBuf;
			break;
		}
		
		SectBody += line + _T("\n");
	}
	
	SectNext = inBuf;
	
	return bFound;
}

//////////////////////////////////////////////////////////////

CSerializeStruct::CSerializeStruct (void)
{
}

CSerializeStruct::~CSerializeStruct (void)
{
}

//////////////////////////////////////////////////////////////

void CSerializeStruct::Escape( CString& s )
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

//////////////////////////////////////////////////////////////

void CSerializeStruct::Unescape( CString& s )
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

CString CSerializeStruct::FormatKey( const SERIALIZE_CONTEXT& context, LPCTSTR szIniName, LPCTSTR szValue )
{
	CString s;
	s.Format( _T("%s%s = %s\r\n"), context.m_strPrefix, szIniName, szValue );
	return s;
}

//////////////////////////////////////////////////////////////

void CSerializeStruct::OnLoadBegin()
{
}

//////////////////////////////////////////////////////////////

void CSerializeStruct::OnLoadEnd()
{
}

//////////////////////////////////////////////////////////////

BOOL CSerializeStruct::Load( LPCTSTR szIniFileName )
{
	m_strFileName = GetFileName(szIniFileName);
	CString strIni;

	if(LoadUTF8FileToString(szIniFileName, strIni))
	{
		SetValues( strIni );
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////

void CSerializeStruct::SetValues( CString& strIni ) // Load members from strIni
{
	CString strIniCopy = strIni, line;
	CStringArray names, values;

	while( !strIni.IsEmpty() )
	{
		line = GetLine( strIni );
		if( line.IsEmpty() )
			continue;

		CString name, value;
		if( IsValue( line, name, value ) )
		{
			names.Add( name );
			values.Add( value );
		} 
	}

	int n = names.GetSize();

	LPCTSTR szNonExistentName = _T("#name#that#don't#exist");
	UpdateValue( szNonExistentName, _T(""), TRUE, FALSE );

	for( int i=0; i<n; i++ )
	{
		CString name = names[i];
		CString value = values[i];

		BOOL bFound = UpdateValue( name, value, FALSE, FALSE );

		if( !bFound )
		{
//			ASSERT( FALSE && "Unknown task .ini argument!" );
		}
	}

	UpdateValue( szNonExistentName, _T(""), FALSE, TRUE );
}

//////////////////////////////////////////////////////////////

void CSerializeStruct::OnSaveBegin()
{
}

//////////////////////////////////////////////////////////////

void CSerializeStruct::OnSaveEnd()
{
}

//////////////////////////////////////////////////////////////

BOOL CSerializeStruct::Save(LPCTSTR szIniFileName)
{
	CString strIni;
	CString strSavedIniFileName = m_strFileName;

	if( szIniFileName )
	{
		m_strFileName = GetFileName(szIniFileName);
		strSavedIniFileName = szIniFileName;
	}

	GetValues( strIni );

	if(SaveUTF8FileFromString(strSavedIniFileName, strIni))
	{
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////

void CSerializeStruct::GetValues( CString& strIni ) // Save members to strIni
{
	SERIALIZE_CONTEXT context;
	context.m_bLoadingFromFile = FALSE;
	context.m_bLoadBegin = FALSE;
	context.m_bLoadEnd = FALSE;
	context.m_strIni.Empty();
	context.m_strPrefix.Empty();

	BOOL res = Serialize( context );
	ASSERT( res == FALSE );

	strIni += context.m_strIni;
}

//////////////////////////////////////////////////////////////

BOOL CSerializeStruct::UpdateValue( LPCTSTR szName, LPCTSTR szValue,
					BOOL bStartLoad, BOOL bEndLoad ) // Update members from szName & szValue
{
	SERIALIZE_CONTEXT context;
	context.m_bLoadingFromFile = TRUE;
	context.m_bLoadBegin = bStartLoad;
	context.m_bLoadEnd = bEndLoad;
	context.m_strName = szName;
	context.m_strValue = szValue;

	BOOL res = Serialize( context );

	return res;
}

//////////////////////////////////////////////////////////////

BOOL CSerializeStruct::Serialize( SERIALIZE_CONTEXT& context )
{
	SERIALIZE_START

	SERIALIZE_END
}

//////////////////////////////////////////////////////////////

#ifndef COMPILE_DL_PRESENTER
void CSerializeStruct::LuaSerialize(CLua &L)
{
	LUA_SERIALIZE(L, "FileName", m_strFileName);
}
#endif // COMPILE_DL_PRESENTER

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
BOOL CSerializeStruct::PHPSerialize(BOOL GetData, zval *val)
{
	PHP_SERIALIZE_START

	PHP_SERIALIZE_END
}
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
