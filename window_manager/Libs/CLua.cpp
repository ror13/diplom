// Lua.cpp: implementation of the CLua class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "shlwapi.h"
#include "CLua.h"
#include "FileOperation.h"
#include "SerializeStruct.h"

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLua::CLua()
{
	m_bToLua = TRUE;
	m_bAttached = FALSE;
	m_luaVM = lua_open();
}

CLua::CLua(lua_State *luaVM )
{
	m_bToLua = TRUE;
	m_bAttached = TRUE;
	m_luaVM = luaVM;
}

CLua::~CLua()
{
	if(!m_bAttached)
		lua_close(m_luaVM);
}

CLua::operator PLUA_STATE()
{
	return m_luaVM;
}

void CLua::SetGlobal(CString Name, CString Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Value));
	lua_setglobal(m_luaVM, CUTF8String(Name));
}

CString CLua::GetGlobal(CString Name)
{
	lua_getglobal(m_luaVM, CUTF8String(Name));
	if(lua_isnil(m_luaVM, -1))
	{
		lua_pop(m_luaVM, 1);
		return _T("");
	}
	const char *luaStr = lua_tostring(m_luaVM, -1);
	lua_pop(m_luaVM, 1);
	return UTF8ToString(luaStr);
}

BOOL CLua::GetGlobalBool(CString Name)
{
	lua_getglobal(m_luaVM, CUTF8String(Name));
	if(lua_isnil(m_luaVM, -1))
	{
		lua_pop(m_luaVM, 1);
		return FALSE;
	}
	BOOL bRes = lua_toboolean(m_luaVM, -1);
	lua_pop(m_luaVM, 1);

	return bRes;
}

void CLua::AddNewTable(CString Name)
{
	lua_newtable(m_luaVM);
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushvalue(m_luaVM, -2);
	lua_settable(m_luaVM, -4);
}

void CLua::AddNewTable(int Name)
{
	lua_newtable(m_luaVM);
	lua_pushnumber(m_luaVM, Name);
	lua_pushvalue(m_luaVM, -2);
	lua_settable(m_luaVM, -4);
}

void CLua::AddKey(CString Name, CString Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushstring(m_luaVM, CUTF8String(Value));
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(CString Name, char* Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushstring(m_luaVM, Value);
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(CString Name, lua_Number Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushnumber(m_luaVM, Value);
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(CString Name, bool Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushboolean(m_luaVM, Value);
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(CString Name, DWORD Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushnumber(m_luaVM, Value);
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(CString Name, int Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushnumber(m_luaVM, Value);
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(CString Name, long Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushnumber(m_luaVM, Value);
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(CString Name, LONGLONG Value)
{
	char buf[128];
	_i64toa(Value, buf, 10);
	AddKey(Name, buf);
}

void CLua::AddKey(int Name, CString Value)
{
	lua_pushnumber(m_luaVM, Name);
	lua_pushstring(m_luaVM, CUTF8String(Value));
	lua_settable(m_luaVM, -3);
}

void CLua::AddKey(int Name, CSerializeStruct *pValue)
{
	AddNewTable(Name);
	pValue->LuaSerialize(*this);
	lua_pop(m_luaVM, 1);
}

void CLua::AddKey(CString Name, CSerializeStruct *pValue)
{
	AddNewTable(Name);
	pValue->LuaSerialize(*this);
	lua_pop(m_luaVM, 1);
}

void CLua::AddKey(CString Name, lua_CFunction Funct)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushcfunction(m_luaVM, Funct);
	lua_settable(m_luaVM, -3);
}

void CLua::AddKeyBOOL(CString Name, int Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_pushboolean(m_luaVM, Value);
	lua_settable(m_luaVM, -3);
}

//////////////////////////////////////////////////////////////////////////

bool CLua::GetTable(CString Name)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	if(lua_istable(m_luaVM, -1))
	{
		return true;
	}
	else
	{
		lua_pop(m_luaVM, 1);
		return false;
	}
}

void CLua::GetKey(CString Name, CString &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	Value = UTF8ToString(lua_tostring(m_luaVM, -1));
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(CString Name, lua_Number &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	Value = lua_tonumber(m_luaVM, -1);
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(CString Name, bool &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	Value = lua_toboolean(m_luaVM, -1) == TRUE;
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(CString Name, DWORD &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	Value = (DWORD)lua_tonumber(m_luaVM, -1);
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(CString Name, int &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	Value = (int)lua_tonumber(m_luaVM, -1);
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(CString Name, long &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	Value = (long)lua_tonumber(m_luaVM, -1);
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(CString Name, LONGLONG &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	const char *str = lua_tostring(m_luaVM, -1);
	Value = _atoi64(str);
//	StrToInt64ExA(str, STIF_DEFAULT, &Value);
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(int Name, CString &Value)
{
	lua_pushnumber(m_luaVM, Name);
	lua_gettable(m_luaVM, -2);
	Value = UTF8ToString(lua_tostring(m_luaVM, -1));
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(int Name, CSerializeStruct *pValue)
{
	lua_pushnumber(m_luaVM, Name);
	lua_gettable(m_luaVM, -2);
	pValue->LuaSerialize(*this);
	lua_pop(m_luaVM, 1);
}

void CLua::GetKey(CString Name, CSerializeStruct *pValue)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	pValue->LuaSerialize(*this);
	lua_pop(m_luaVM, 1);
}

void CLua::GetKeyBOOL(CString Name, int &Value)
{
	lua_pushstring(m_luaVM, CUTF8String(Name));
	lua_gettable(m_luaVM, -2);
	Value = lua_toboolean(m_luaVM, -1);
	lua_pop(m_luaVM, 1);
}

void CLua::ToLua(CSerializeStruct *pValue)
{
	m_bToLua = TRUE;
	lua_newtable(m_luaVM);
	pValue->LuaSerialize(*this);
}

void CLua::FromLua(CSerializeStruct *pValue)
{
	m_bToLua = FALSE;
	pValue->LuaSerialize(*this);
}
