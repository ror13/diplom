// CLua.h: interface for the CLua class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LUA_H__890F139E_A523_4AA6_95B1_81F8D657CBDF__INCLUDED_)
#define AFX_LUA_H__890F139E_A523_4AA6_95B1_81F8D657CBDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern "C"
{
#include "lua\lua.h"
#include "lua\lauxlib.h"
}

typedef lua_State *PLUA_STATE;

class CSerializeStruct;

class CLua  
{
public:
	CLua();
	CLua(lua_State *luaVM);
	virtual ~CLua();

	BOOL	m_bToLua;

	operator PLUA_STATE();

	void SetGlobal(CString Name, CString Value);
	CString GetGlobal(CString Name);
	BOOL GetGlobalBool(CString Name);

	void AddNewTable(CString Name);
	void AddNewTable(int Name);

	void AddKey(CString Name, CString Value);
	void AddKey(CString Name, char* Value);
	void AddKey(CString Name, lua_Number Value);
	void AddKey(CString Name, bool Value);
	void AddKey(CString Name, int Value);
	void AddKey(CString Name, long Value);
	void AddKey(CString Name, DWORD Value);
	void AddKey(CString Name, LONGLONG Value);
	void AddKey(int Name, CString Value);
	void AddKey(int Name, CSerializeStruct *pValue);
	void AddKey(CString Name, CSerializeStruct *pValue);
	void AddKey(CString Name, lua_CFunction Funct);
	void AddKeyBOOL(CString Name, int Value);

	bool GetTable(CString Name);

	void GetKey(CString Name, CString &Value);
	void GetKey(CString Name, lua_Number &Value);
	void GetKey(CString Name, bool &Value);
	void GetKey(CString Name, int &Value);
	void GetKey(CString Name, long &Value);
	void GetKey(CString Name, DWORD &Value);
	void GetKey(CString Name, LONGLONG &Value);
	void GetKey(int Name, CString &Value);
	void GetKey(int Name, CSerializeStruct *pValue);
	void GetKey(CString Name, CSerializeStruct *pValue);
	void GetKeyBOOL(CString Name, int &Value);

	void ToLua(CSerializeStruct *pValue);
	void FromLua(CSerializeStruct *pValue);


protected:
	BOOL		m_bAttached;
	lua_State	*m_luaVM;
};

#endif // !defined(AFX_LUA_H__890F139E_A523_4AA6_95B1_81F8D657CBDF__INCLUDED_)
