//////////////////////////////////////////////////////////////
// File:		// SerializeStruct.h
// File time:	// 19.05.2004	17:17
// Version: 	// 1.0
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _SERIALIZESTRUCT_H_UID0000000DFEF17429
#define _SERIALIZESTRUCT_H_UID0000000DFEF17429

#ifndef COMPILE_DL_PRESENTER
#include "CLua.h"
#endif // COMPILE_DL_PRESENTER

#include "CBase64.h"

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
#include "ext_php.h"
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

// Базовый класс для упрощения сериализации классов в текстовый ini файл.

BOOL GetSection(CString inBuf, CString SectName, CString &SectParam, CString &SectBody, CString &SectNext);

class CSerializeStruct
{
	friend class CPHP_Helper;

public:
	CSerializeStruct (void);
	virtual ~CSerializeStruct (void);

	virtual BOOL Load(LPCTSTR szIniFileName);
	virtual BOOL Save(LPCTSTR szIniFileName = NULL);


	struct SERIALIZE_CONTEXT
	{
		BOOL	m_bLoadingFromFile; // TRUE - for loading process
		BOOL	m_bLoadBegin;		// TRUE - for first call during loading process
		BOOL	m_bLoadEnd;			// TRUE - for last call during loading process
		CString	m_strIni;	// ini text   - for saving  process
		CString	m_strPrefix;// key prefix - for saving  process
		CString	m_strName;	// key name   - for loading process
		CString	m_strValue;	// key value  - for loading process
		
		BOOL IsSpecialIteration() const
		{
			return m_bLoadBegin || m_bLoadEnd;
		}
	};

	virtual BOOL Serialize( SERIALIZE_CONTEXT& context );
	// return TRUE if found corresponding member during loading,
	// return TRUE during saving.
#ifndef COMPILE_DL_PRESENTER
	virtual void LuaSerialize(CLua &L);
#endif // COMPILE_DL_PRESENTER
#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
	virtual BOOL PHPSerialize(BOOL GetData, zval *val);
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

	CString	m_strFileName;

	virtual void GetValues( CString& strIni ); // Save members to strIni
	virtual void SetValues( CString& strIni ); // Load members from strIni

protected:

	virtual void OnLoadBegin();
	virtual void OnLoadEnd();

	virtual void OnSaveBegin();
	virtual void OnSaveEnd();

	virtual BOOL UpdateValue( LPCTSTR szName, LPCTSTR szValue,
					BOOL bStartLoad, BOOL bEndLoad ); // Update members from szName & szValue

public:

	static void Escape( CString& s );
	static void Unescape( CString& s );

protected:
	static CString FormatKey( const SERIALIZE_CONTEXT& context, LPCTSTR szIniName,
					LPCTSTR szValue );

	static BOOL SerializeString( SERIALIZE_CONTEXT& context, CString& strMember,
					LPCTSTR szIniName )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				Unescape( context.m_strValue );
				strMember = context.m_strValue;
				return TRUE;
			}
		}
		else
		{
			CString s = strMember;
			Escape( s );
			context.m_strIni += FormatKey( context, szIniName, s );
		}
		return FALSE;
	}

	static BOOL SerializeStringBase64( SERIALIZE_CONTEXT& context, CString& strMember,
					LPCTSTR szIniName )
	{
		CBase64 Coder;
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				Unescape( context.m_strValue );
				strMember = context.m_strValue;
				Coder.DecodeString(context.m_strValue, strMember);
				return TRUE;
			}
		}
		else
		{
			CString s = strMember;
			Coder.EncodeString(strMember, s);
			Escape( s );
			context.m_strIni += FormatKey( context, szIniName, s );
		}
		return FALSE;
	}

	static BOOL SerializeStringBuf( SERIALIZE_CONTEXT& context, LPTSTR pszMemberBuf, int iBufSize,
					LPCTSTR szIniName )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				Unescape( context.m_strValue );
				_tcsncpy( pszMemberBuf, context.m_strValue, iBufSize );
				return TRUE;
			}
		}
		else
		{
			CString s = pszMemberBuf;
			Escape( s );
			context.m_strIni += FormatKey( context, szIniName, s );
		}
		return FALSE;
	}

	template<class TYPE>
	static BOOL SerializeInt( SERIALIZE_CONTEXT& context, TYPE& scalarMember,
					LPCTSTR szIniName, TYPE* unused )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				scalarMember = (TYPE) _ttoi( context.m_strValue );
				return TRUE;
			}
		}
		else
		{
			TCHAR buf[100];
			_itot( scalarMember, buf, 10 );
			context.m_strIni += FormatKey( context, szIniName, buf );
		}
		return FALSE;
	}

	static BOOL SerializeLONGLONG( SERIALIZE_CONTEXT& context, LONGLONG& scalarMember,
					LPCTSTR szIniName, LONGLONG* unused )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				scalarMember = (LONGLONG) _ttoi64( context.m_strValue );
				return TRUE;
			}
		}
		else
		{
			TCHAR buf[100];
			_i64tot( scalarMember, buf, 10 );
			context.m_strIni += FormatKey( context, szIniName, buf );
		}
		return FALSE;
	}

	template<class TYPE>
	static BOOL SerializeDouble( SERIALIZE_CONTEXT& context, TYPE& floatMember,
					LPCTSTR szIniName, TYPE* unused )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				floatMember = (TYPE) _tcstod( context.m_strValue, NULL );
				return TRUE;
			}
		}
		else
		{
			TCHAR buf[100];
//			_gcvt( floatMember, 15, buf );
			_stprintf( buf, _T("%G"), (double)floatMember );
			context.m_strIni += FormatKey( context, szIniName, buf );
		}
		return FALSE;
	}

	template<class TYPE>
	static BOOL SerializeHex( SERIALIZE_CONTEXT& context, TYPE& scalarMember,
					LPCTSTR szIniName, TYPE* unused )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				scalarMember = (TYPE) _tcstoul( context.m_strValue, NULL, 16 );
				return TRUE;
			}
		}
		else
		{
			TCHAR buf[100];
			_ultot( scalarMember, buf, 16 );
			context.m_strIni += FormatKey( context, szIniName, buf );
		}
		return FALSE;
	}

	template<class TYPE>
	static BOOL SerializeBin( SERIALIZE_CONTEXT& context, TYPE& scalarMember,
					LPCTSTR szIniName, TYPE* unused )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.m_strName == szIniName )
			{
				scalarMember = (TYPE) _tcstoul( context.m_strValue, NULL, 2 );
				return TRUE;
			}
		}
		else
		{
			TCHAR buf[100];
			_ultot( scalarMember, buf, 2 );
			context.m_strIni += FormatKey( context, szIniName, buf );
		}
		return FALSE;
	}

	template<class TYPE, class ARRAY>
	static BOOL SerializeArray( SERIALIZE_CONTEXT& context, ARRAY& arrMember,
					LPCTSTR szIniName, TYPE* unused )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.IsSpecialIteration() )
			{
				int n = arrMember.GetSize();
				for( int i=0; i<n; i++ )
				{
					BOOL res = arrMember.ElementAt( i ).Serialize( context );
					ASSERT( res == FALSE );
				}

				return FALSE;
			}

			CString strName = szIniName;
			CString strSizeName = strName + _T(".Size");

			if( context.m_strName == strSizeName )
			{
				int size = _ttoi( context.m_strValue );
				arrMember.SetSize( size );
				return TRUE;
			}
			else
			{
				CString old_name = context.m_strName;
				if( context.m_strName.Left(strName.GetLength()) == strName )
				{
					context.m_strName.Delete( 0, strName.GetLength() );
					ASSERT( context.m_strName[0] == _T('[') );
					context.m_strName.Delete( 0 );
					int index = _ttoi( context.m_strName );
					int pos = context.m_strName.Find( _T('.') );
					context.m_strName = context.m_strName.Mid( pos + 1 );
					ASSERT( arrMember.GetSize() > index );
					TYPE element = arrMember.GetAt( index );
					BOOL res = element.Serialize( context );
					if( res )
					{
						arrMember.SetAt( index, element );
						return TRUE;
					}
					context.m_strName = old_name;
				}
			}
		}
		else
		{
			CString s;
			int n = arrMember.GetSize();
			s.Format( _T("%s%s.Size = %d\r\n"),
				context.m_strPrefix, szIniName, n );
			context.m_strIni += s;

			for( int i=0; i<n; i++ )
			{
				CString new_prefix, old_prefix = context.m_strPrefix;
				new_prefix.Format( _T("%s%s[%d]."), old_prefix, szIniName, i );
				context.m_strPrefix = new_prefix;
				BOOL res = arrMember.GetAt( i ).Serialize( context );
				context.m_strPrefix = old_prefix;
				ASSERT( res == FALSE );
			}
		}
		return FALSE;
	}

	static BOOL SerializeStruct( SERIALIZE_CONTEXT& context,
					CSerializeStruct* pSerializedMember, LPCTSTR szIniName )
	{
		if( context.m_bLoadingFromFile )
		{
			if( context.IsSpecialIteration() )
			{
				BOOL res = pSerializedMember->Serialize( context );
				ASSERT( res == FALSE );

				return FALSE;
			}

			CString strName;
			strName.Format( _T("%s."), szIniName );
			CString old_name = context.m_strName;

			if( context.m_strName.Left(strName.GetLength()) == strName )
			{
				context.m_strName.Delete( 0, strName.GetLength() );
				BOOL res = pSerializedMember->Serialize( context );
				if( res )
				{
					return TRUE;
				}

				context.m_strName = old_name;
			}
			return FALSE;
		}
		else
		{
			CString new_prefix, old_prefix = context.m_strPrefix;
			new_prefix.Format( _T("%s%s."), old_prefix, szIniName );
			context.m_strPrefix = new_prefix;
			BOOL res = pSerializedMember->Serialize( context );
			context.m_strPrefix = old_prefix;
			return res;
		}
	}

	static BOOL SerializeStrings( SERIALIZE_CONTEXT& context, CStringArray& arrMember,
					LPCTSTR szIniName)
	{
		if( context.m_bLoadingFromFile )
		{
			if (context.m_strName == szIniName)
			{
				// this is our entry
				CString strTemp(context.m_strValue);
				// strTemp consists parameter value
				// 
				// StringArray - array of double quoted "escaped" strings
				//
				arrMember.RemoveAll();
				int nIndex = 0;
				int nFind = 0;
				while ((nFind = strTemp.Find(_T(','), nIndex)) != -1)
				{
					// found ","
					if (nFind == nIndex)
					{
						// it means that comma(,) is the first char - ignore it
						nIndex++;
						if (nIndex >= strTemp.GetLength())
							break;
						else
							continue;
					}
					arrMember.Add(strTemp.Mid(nIndex, nFind-nIndex));
					nIndex = nFind + 1; // the symbol after comma(,) that was found
				}
				// add the rest only if there is something!!!
				BOOL bSpacesOnly = TRUE;
				for (nFind = nIndex;nFind < strTemp.GetLength();nFind++)
				{
					if (strTemp.GetAt(nFind) == _T(' ') || strTemp.GetAt(nFind) == _T('\t'))
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
					arrMember.Add(strTemp.Mid(nIndex));
				}
				return TRUE;
			}

			return FALSE;
		}
		else
		{
			CString strTemp;
			int i = 0;
			strTemp.Empty();
			for (i = 0;i < arrMember.GetSize() - 1;i++)
			{
				strTemp += arrMember.GetAt(i) + _T(",");
			}
			// add last element now
			if (arrMember.GetSize() > 0) // means that at least there is something
			{
				strTemp += arrMember.GetAt(i);
			}

			context.m_strIni += FormatKey(context, szIniName, strTemp);
		}
		return FALSE;
	}

#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
	static BOOL PHP_SerializeString( BOOL GetData, zval *val, CString& strMember, LPCTSTR szIniName )
	{
		if ( GetData == TRUE )
		{
			zval **dest;
			
			int retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName, _tcsclen(szIniName)+sizeof(TCHAR), 
				(void **)&dest);

			if (retval == FAILURE || Z_TYPE_PP(dest) != IS_STRING)
			{
				return TRUE;
			}
			strMember = Z_STRVAL_PP(dest);
			if (strMember == _T("---"))
			{
				strMember = _T("");
			}
		} 
		else
		{
			if (strMember.GetLength() == 0)
			{
				strMember = _T("---");
			}
			LPTSTR szMember = strMember.GetBuffer(sizeof(TCHAR));
			add_assoc_string(val, (LPTSTR)szIniName, szMember, TRUE);
			strMember.ReleaseBuffer();
		}
		
		return FALSE;
	}
	
	template<class TYPE>
	static BOOL PHP_SerializeInt( BOOL GetData, zval *val, TYPE& scalarMember,
		LPCTSTR szIniName, TYPE* unused )
	{
		if ( GetData == TRUE )
		{
			zval **dest;
			
			int retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName, _tcsclen(szIniName)+sizeof(TCHAR), 
				(void **)&dest);
			
			if (retval == FAILURE || Z_TYPE_PP(dest) != IS_LONG)
			{
				return TRUE;
			}
			scalarMember = Z_LVAL_PP(dest);
		} 
		else
		{
			add_assoc_long(val, (LPTSTR)szIniName, scalarMember);
		}
		
		return FALSE;
	}
	
	static BOOL PHP_SerializeLONGLONG( BOOL GetData, zval *val, LONGLONG& scalarMember,
						  LPCTSTR szIniName, LONGLONG* unused )
	{
		CString szIniName2;
		
		szIniName2.Format(_T("%s%s"), szIniName, _T("Hi"));

		LARGE_INTEGER scalar;

		
		if ( GetData == TRUE )
		{
			zval **dest;
			
			int retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName, _tcsclen(szIniName)+sizeof(TCHAR), 
				(void **)&dest);
			
			if (retval == FAILURE || Z_TYPE_PP(dest) != IS_LONG)
			{
				return TRUE;
			}
			scalar.LowPart = Z_LVAL_PP(dest);

			retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName2.GetBuffer(0), szIniName2.GetLength()+sizeof(TCHAR), 
				(void **)&dest);
			szIniName2.ReleaseBuffer();
			
			if (retval == FAILURE || Z_TYPE_PP(dest) != IS_LONG)
			{
				return TRUE;
			}
			scalar.HighPart = Z_LVAL_PP(dest);

			scalarMember = scalar.QuadPart;
		} 
		else
		{
			scalar.QuadPart = scalarMember;
			add_assoc_long(val, (LPTSTR)szIniName, scalar.LowPart);
			add_assoc_long(val, (LPTSTR)szIniName2.GetBuffer(0), scalar.HighPart);
			szIniName2.ReleaseBuffer();
		}
		
		return FALSE;
	}

	static BOOL PHP_SerializeStruct( BOOL GetData, zval *val, 
		CSerializeStruct* pSerializedMember, LPCTSTR szIniName )
	{
		if ( GetData == TRUE )
		{
			zval **dest;
			
			int retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName, _tcsclen(szIniName)+sizeof(TCHAR), 
				(void **)&dest);
			
			if (retval == FAILURE || Z_TYPE_PP(dest) != IS_ARRAY)
			{
				return TRUE;
			}

			if (pSerializedMember->PHPSerialize(GetData, *dest))
			{
				return TRUE;
			}
		}
		else
		{
			zval *substruct;

			MAKE_STD_ZVAL(substruct);
			
			//array_init(substruct);

			if (pSerializedMember->PHPSerialize(GetData, substruct))
			{
				return TRUE;
			}
			add_assoc_zval(val, (LPTSTR)szIniName, substruct);
		}
		
		return FALSE;
	}

	static BOOL PHP_SerializeStrings( BOOL GetData, zval *val, CStringArray& arrMember,
		LPCTSTR szIniName)
	{
		if ( GetData == TRUE )
		{
			zval **arr;
			int retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName, _tcsclen(szIniName)+sizeof(TCHAR), 
				(void **)&arr);

			if (retval == FAILURE || Z_TYPE_PP(arr) != IS_ARRAY)
			{
				return TRUE;
			}
			
			int n = zend_hash_num_elements(Z_ARRVAL_PP(arr));
			arrMember.SetSize(n);

			for( int i=0; i<n; i++ )
			{
				CString element;

				zval **dest;

				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				LPTSTR szIniNameN = (LPTSTR)strIniNameN.GetBuffer(0);

				int retval = zend_hash_find(Z_ARRVAL_PP(arr), 
					(LPTSTR)szIniNameN, _tcsclen(szIniNameN)+sizeof(TCHAR), 
					(void **)&dest);
				strIniNameN.ReleaseBuffer();
				
				if (retval == FAILURE || Z_TYPE_PP(dest) != IS_STRING)
				{
					return TRUE;
				}
		
				element = Z_STRVAL_P(*dest);
				arrMember.SetAt(i, element);
			}

		}
		else
		{
			zval *arr;
			MAKE_STD_ZVAL(arr);
			array_init(arr);

			int n = arrMember.GetSize();

			for( int i=0; i<n; i++ )
			{

				CString item = arrMember.GetAt(i);
				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				add_assoc_string(arr, (LPTSTR)strIniNameN.GetBuffer(0), item.GetBuffer(0), TRUE);
				item.ReleaseBuffer();
				strIniNameN.ReleaseBuffer();
			}
			add_assoc_zval(val, (LPTSTR)szIniName, arr);
		}
		
		return FALSE;
	}

	template</*class TYPE, */class ARRAY>
	static BOOL PHP_SerializeInts( BOOL GetData, zval *val, ARRAY& arrMember,
		LPCTSTR szIniName)
	{
		if ( GetData == TRUE )
		{
			zval **arr;
			int retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName, _tcsclen(szIniName)+sizeof(TCHAR), 
				(void **)&arr);

			if (retval == FAILURE || Z_TYPE_PP(arr) != IS_ARRAY)
			{
				return TRUE;
			}
			
			int n = zend_hash_num_elements(Z_ARRVAL_PP(arr));
			arrMember.SetSize(n);

			for( int i=0; i<n; i++ )
			{
				int element;

				zval **dest;

				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				LPTSTR szIniNameN = (LPTSTR)strIniNameN.GetBuffer(0);

				int retval = zend_hash_find(Z_ARRVAL_PP(arr), 
					(LPTSTR)szIniNameN, _tcsclen(szIniNameN)+sizeof(TCHAR), 
					(void **)&dest);
				strIniNameN.ReleaseBuffer();
				
				if (retval == FAILURE || Z_TYPE_PP(dest) != IS_LONG)
				{
					return TRUE;
				}
		
				element = Z_LVAL_P(*dest);
				arrMember.SetAt(i, element);
			}

		}
		else
		{
			zval *arr;
			MAKE_STD_ZVAL(arr);
			array_init(arr);

			int n = arrMember.GetSize();

			for( int i=0; i<n; i++ )
			{

				int item = arrMember.GetAt(i);
				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				add_assoc_long(arr, (LPTSTR)strIniNameN.GetBuffer(0), item);
				strIniNameN.ReleaseBuffer();
			}
			add_assoc_zval(val, (LPTSTR)szIniName, arr);
		}
		
		return FALSE;
	}

	template</*class TYPE, */class ARRAY>
	static BOOL PHP_SerializeSchedulesArray( BOOL GetData, zval *val, ARRAY& arrMember,
		LPCTSTR szIniName/*, TYPE* unused*/ )
	{
		if ( GetData == TRUE )
		{
			ASSERT(FALSE);
/*
			int n = zend_hash_num_elements(Z_ARRVAL_P(val));

			for( int i=0; i<n; i++ )
			{
				TYPE element;

				zval **dest;

				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				LPTSTR szIniNameN = (LPTSTR)strIniNameN.GetBuffer(0);

				int retval = zend_hash_find(Z_ARRVAL_P(val), 
					(LPTSTR)szIniNameN, _tcsclen(szIniNameN)+sizeof(TCHAR), 
					(void **)&dest);
				strIniNameN.ReleaseBuffer();
				
				if (retval == FAILURE || Z_TYPE_PP(dest) != IS_ARRAY)
				{
					return TRUE;
				}
				
				if (element.PHPSerialize(GetData, *dest))
				{
					return TRUE;
				}
				arrMember.Add(element);
			}
*/
		}
		else
		{
			zval *arr;
			MAKE_STD_ZVAL(arr);
			array_init(arr);

			int n = arrMember.GetSize();

			//array_init(item);
			for( int i=0; i<n; i++ )
			{
				zval *item;
				MAKE_STD_ZVAL(item);

				if (arrMember.ElementAt( i )->PHPSerialize( GetData, item ))
				{
					return TRUE;
				}

				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				add_assoc_zval(arr, (LPTSTR)strIniNameN.GetBuffer(0), item);
				strIniNameN.ReleaseBuffer();
			}
			add_assoc_zval(val, (LPTSTR)szIniName, arr);
		}
		
		return FALSE;
	}
	
	template<class TYPE, class ARRAY>
	static BOOL PHP_SerializeArray( BOOL GetData, zval *val, ARRAY& arrMember,
		LPCTSTR szIniName, TYPE* unused )
	{
		if ( GetData == TRUE )
		{
			zval **arr;
			int retval = zend_hash_find(Z_ARRVAL_P(val), 
				(LPTSTR)szIniName, _tcsclen(szIniName)+sizeof(TCHAR), 
				(void **)&arr);

			if (retval == FAILURE || Z_TYPE_PP(arr) != IS_ARRAY)
			{
				return TRUE;
			}
			
			int n = zend_hash_num_elements(Z_ARRVAL_PP(arr));
			arrMember.SetSize(n);

			for( int i=0; i<n; i++ )
			{
				TYPE element;

				zval **dest;

				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				LPTSTR szIniNameN = (LPTSTR)strIniNameN.GetBuffer(0);

				int retval = zend_hash_find(Z_ARRVAL_PP(arr), 
					(LPTSTR)szIniNameN, _tcsclen(szIniNameN)+sizeof(TCHAR), 
					(void **)&dest);
				strIniNameN.ReleaseBuffer();
				
				if (retval == FAILURE || Z_TYPE_PP(dest) != IS_ARRAY)
				{
					return TRUE;
				}
				
				if (element.PHPSerialize(GetData, *dest))
				{
					return TRUE;
				}
				arrMember.SetAt(i, element);
			}

		}
		else
		{
			zval *arr;
			MAKE_STD_ZVAL(arr);
			array_init(arr);

			int n = arrMember.GetSize();
			/*PHP_SerializeInt( GetData, val, n, szCountIniName.GetBuffer(0), (int*)NULL );
			szCountIniName.ReleaseBuffer();*/

			//array_init(item);
			for( int i=0; i<n; i++ )
			{
				zval *item;
				MAKE_STD_ZVAL(item);

				if (arrMember.ElementAt( i ).PHPSerialize( GetData, item ))
				{
					return TRUE;
				}
				CString strIniNameN;
				strIniNameN.Format(_T("%s[%d]"), szIniName, i);
				add_assoc_zval(arr, (LPTSTR)strIniNameN.GetBuffer(0), item);
				strIniNameN.ReleaseBuffer();
			}
			add_assoc_zval(val, (LPTSTR)szIniName, arr);
		}
		
		return FALSE;
	}

#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER



private:

};

//////////////////////////////////////////////////////////////

#define SERIALIZE_START \
	if( context.m_bLoadBegin ) \
		OnLoadBegin(); \
	if( !context.m_bLoadingFromFile ) \
		OnSaveBegin();

#define SERIALIZE_END \
	if( context.m_bLoadEnd ) \
		OnLoadEnd(); \
	if( !context.m_bLoadingFromFile ) \
		OnSaveEnd(); \
	if( context.m_bLoadingFromFile ) \
		return FALSE; \
	return FALSE;

#define SERIALIZE_STRING( member, ini_name ) \
	if( SerializeString( context, member, ini_name ) ) \
		return TRUE;

#define SERIALIZE_STRING_BASE64( member, ini_name ) \
	if( SerializeStringBase64( context, member, ini_name ) ) \
		return TRUE;

#define SERIALIZE_STR_BUF( member, ini_name ) \
	if( SerializeStringBuf( context, member, sizeof(member)/sizeof(member[0]), ini_name ) ) \
		return TRUE;

#define SERIALIZE_INT( type, member, ini_name ) \
	if( SerializeInt( context, member, ini_name, (type*)NULL ) ) \
		return TRUE;

#define SERIALIZE_LONGLONG( member, ini_name ) \
	if( SerializeLONGLONG( context, member, ini_name, NULL ) ) \
		return TRUE;

#define SERIALIZE_DOUBLE( type, member, ini_name ) \
	if( SerializeDouble( context, member, ini_name, (type*)NULL ) ) \
		return TRUE;

#define SERIALIZE_HEX( type, member, ini_name ) \
	if( SerializeHex( context, member, ini_name, (type*)NULL ) ) \
		return TRUE;

#define SERIALIZE_BIN( type, member, ini_name ) \
	if( SerializeBin( context, member, ini_name, (type*)NULL ) ) \
		return TRUE;

#define SERIALIZE_ARRAY( type, member, ini_name ) \
	if( SerializeArray( context, member, ini_name, (type*)NULL ) ) \
		return TRUE;

#define SERIALIZE_STRINGS( member, ini_name ) \
	if( SerializeStrings( context, member, ini_name )) \
		return TRUE;

#define SERIALIZE_STRUCT( member, ini_name ) \
	if( SerializeStruct( context, &member, ini_name ) ) \
		return TRUE;

#define LUA_SERIALIZE(L, key_name, member) \
	if(L.m_bToLua) \
		L.AddKey(key_name, member); \
	else \
		L.GetKey(key_name, member);

#define LUA_SERIALIZE_BOOL(L, key_name, member) \
	if(L.m_bToLua) \
		L.AddKeyBOOL(key_name, member); \
	else \
		L.GetKeyBOOL(key_name, member);

#define ARRAY_LUA_SERIALIZE(L) \
	if(L.m_bToLua) \
	{ \
		L.AddKey("Count", GetSize()); \
		for(int i = 0; i < GetSize(); i++) \
			L.AddKey(i + 1, &GetAt(i)); \
	} else \
	{ \
		int c = 0; \
		L.GetKey("Count", c); \
		SetSize(c); \
		for(int i = 0; i < c; i++) \
			L.GetKey(i + 1, &ElementAt(i));\
	}
//////////////////////////////////////////////////////////////
#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
#define PHP_SERIALIZE_START \
	if ( GetData == TRUE ) \
	{ \
		OnLoadBegin(); \
	} \
	else \
	{ \
		OnSaveBegin(); \
		array_init(val); \
	}

#define PHP_SERIALIZE_END \
	if ( GetData == TRUE ) \
	{ \
		OnLoadEnd(); \
	} \
	else \
	{ \
		OnSaveEnd(); \
	} \
	return FALSE;

#define PHP_SERIALIZE_STRING( member, ini_name ) \
	if( PHP_SerializeString( GetData, val, member, ini_name ) ) \
		return TRUE;

#define PHP_SERIALIZE_STRING_BASE64	PHP_SERIALIZE_STRING

#define PHP_SERIALIZE_BIN PHP_SERIALIZE_INT

#define PHP_SERIALIZE_INT( type, member, ini_name ) \
	if( PHP_SerializeInt( GetData, val, member, ini_name, (type*)NULL ) ) \
		return TRUE;

#define PHP_SERIALIZE_LONGLONG( member, ini_name ) \
	if( PHP_SerializeLONGLONG( GetData, val, member, ini_name, NULL ) ) \
		return TRUE;

#define PHP_SERIALIZE_STRUCT( member, ini_name ) \
	if( PHP_SerializeStruct( GetData, val, &member, ini_name ) ) \
		return TRUE;

#define PHP_SERIALIZE_ARRAY( type, member, ini_name ) \
	if( PHP_SerializeArray( GetData, val, member, ini_name, (type*)NULL ) ) \
		return TRUE;

#define PHP_SERIALIZE_INTS_ARRAY( member, ini_name ) \
	if( PHP_SerializeInts( GetData, val, member, ini_name ) ) \
		return TRUE;

#define PHP_SERIALIZE_SCHEDULES_ARRAY( member, ini_name ) \
	if( PHP_SerializeSchedulesArray( GetData, val, member, ini_name ) ) \
		return TRUE;

#define PHP_SERIALIZE_STRINGS( member, ini_name ) \
	if( PHP_SerializeStrings( GetData, val, member, ini_name )) \
		return TRUE;

#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER

//////////////////////////////////////////////////////////////

#endif //ifndef _SERIALIZESTRUCT_H_UID0000000DFEF17429
