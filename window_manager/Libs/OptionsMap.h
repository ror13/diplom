//////////////////////////////////////////////////////////////
// File:		// OptionsMap.h
// File time:	// 30.04.2004	15:24
// Version: 	// 1.0
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _OPTIONSMAP_H_UID00000002E4090494
#define _OPTIONSMAP_H_UID00000002E4090494

/*
inline UINT HashKey(LPCTSTR key, int UniqueFunctionSignature = 0)
{
	TRACE1( "HashKey( \"%s\" )\n", key );
	register TCHAR d1 = 0;
	register TCHAR d2 = 0;
	const TCHAR first = *key - _T('A');
	while( *key )
	{
		key++;
		d2 = d1;
		d1 = *key;
	}
	d1 -= _T('0');
	d2 -= _T('0');
	UINT res = first*100 + d2*10 + d1;
	return res;
}
*/

class COptionsMap;

class COptionsMapStruct
{
public:
	virtual CString Serialize() = 0;
	virtual BOOL Unserialize(CString strStruct) = 0;

	static void StripLeft(LPTSTR Buffer);
	static void StripRight(LPTSTR Buffer);
	static void StripLine(LPTSTR Buffer);
	static void FromStringToMap(CString strOpt, COptionsMap &map);
};

class CRectMapStruct : public COptionsMapStruct
{
public:
	CRectMapStruct(RECT *pRect)
	{
		m_pRect = pRect;
	}

	// COptionsMapStruct
	virtual CString Serialize();
	virtual BOOL Unserialize(CString strStruct);

protected:
	RECT	*m_pRect;
};

class COptionsMap: protected CMap<CString, LPCTSTR, CString, LPCTSTR>
{
	friend struct SESSION_COMMAND;
	friend struct COMMAND_ANSWER;
public:

	COptionsMap (void);
//	virtual ~COptionsMap (void);

	CString GetOption( LPCTSTR szOptionName ) const;
	int GetOptionInt( LPCTSTR szOptionName ) const;
	LONGLONG GetOptionLONGLONG( LPCTSTR szOptionName ) const;


	int GetOptionInt( LPCTSTR szOptionName, DWORD dwOptionIndex) const
	{ // this function is inline, because it is frequently usable
		CString name, s;
		name.Format( _T("%s%d"), szOptionName, dwOptionIndex );
		Lookup( name, s );
		Unescape( s );
		return _ttoi(s);
	}
	
	CString GetOption( LPCTSTR szOptionName, DWORD dwOptionIndex ) const
	{ // this function is inline, because it is frequently usable
		CString name, s;
		name.Format( _T("%s%d"), szOptionName, dwOptionIndex );
		Lookup( name, s );
		Unescape( s );
		return s;
	}

	BOOL DelOption( LPCTSTR szOptionName );
	
	BOOL DelOption( LPCTSTR szOptionName, DWORD dwOptionIndex );

///////////////////////
	void Escape( CString& s )  const;
	void Unescape( CString& s ) const;

	void SetOption( LPCTSTR szOptionName, CString szText );
	void SetOption( LPCTSTR szOptionName, int dwNumber );
	void SetOption( LPCTSTR szOptionName, DWORD dwNumber );
	void SetOption( LPCTSTR szOptionName, UINT dwNumber );
	void SetOption( LPCTSTR szOptionName, LONGLONG dwNumber );
	void SetOption( LPCTSTR szOptionName, COptionsMapStruct *pStruct);

	void SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, LPCTSTR szText);
	void SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, DWORD dwNumber );
 	void SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, int dwNumber );
	void SetOption( LPCTSTR szOptionName, DWORD dwOptionIndex, COptionsMapStruct *pStruct);

///////////////////////

	BOOL IsOptionExists( LPCTSTR szOptionName ) const;

	void RemoveAll();

	CString ToString();

	void FromString(CString str);

	BOOL SaveToRegistry(HKEY hRootKey, LPCTSTR szSubKey);
	BOOL LoadFromRegistry(HKEY hRootKey, LPCTSTR szSubKey);
};

#endif //ifndef _OPTIONSMAP_H_UID00000002E4090494
