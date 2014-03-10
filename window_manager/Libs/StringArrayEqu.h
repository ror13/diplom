// StringArrayEqu.h
//////////////////////////////////////////////////////////////////////////

#ifndef _STRINGARRAYEQU_H_
#define _STRINGARRAYEQU_H_

#include "SerializeStruct.h"

class CStringArrayEqu: public CStringArray, public CSerializeStruct
{
public:
	CStringArrayEqu();
	CStringArrayEqu(const CStringArrayEqu& op);
	const CStringArrayEqu& operator =(const CStringArrayEqu& op);
	bool operator ==(const CStringArrayEqu& op);

	BOOL Contain(CString str);
	BOOL FromString(CString str);
	CString ToString();

#ifndef COMPILE_DL_PRESENTER
	virtual void LuaSerialize(CLua &L);
#endif // COMPILE_DL_PRESENTER
#ifdef COMPILE_DL_PRESENTER
#ifndef PRESENTER_NO_PHP
	virtual BOOL PHPSerialize(BOOL GetData, zval *val);
#endif // PRESENTER_NO_PHP
#endif // COMPILE_DL_PRESENTER
};

#endif // _STRINGARRAYEQU_H_
