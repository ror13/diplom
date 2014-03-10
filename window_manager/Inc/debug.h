#ifndef DEBUG_H
#define DEBUG_H

#ifndef _DRIVER_NAME
#define _DRIVER_NAME _T("PRSNT")
#endif

#define _CALL(x) TRACE(_DRIVER_NAME _T(": "x" Call at line %d\n"), __LINE__);
#define _TRACE(x) TRACE(_DRIVER_NAME _T("(%d): %s\n"), __LINE__, x);

#if _DEBUG
#define MAX_ENTER_LEVEL 32*sizeof(TCHAR)
class FUNC_TRACEClass
{
public:
	static ULONG Level;
	PTCHAR m_szFunc;
	FUNC_TRACEClass(PTCHAR szFunc)
	{
		TCHAR Buffer[MAX_ENTER_LEVEL+1];

		for(ULONG i = 0; i < Level; i++)
		{
			Buffer[i] = _T(' ');
		}
		Buffer[i] = 0;
		m_szFunc = szFunc;
		TRACE(_T("%d.%s") _DRIVER_NAME _T(": %s Enter\n"), Level, Buffer, m_szFunc);
		if (Level < MAX_ENTER_LEVEL)
		{
			Level++;
		}
	}
	~FUNC_TRACEClass()
	{
		if (Level > 0)
		{
			Level--;
		}

		TCHAR Buffer[MAX_ENTER_LEVEL+1];
		for(ULONG i = 0; i < Level; i++)
		{
			Buffer[i] = ' ';
		}
		Buffer[i] = 0;
		TRACE(_T("%d.%s") _DRIVER_NAME _T(": %s Leave\n"), Level, Buffer, m_szFunc);
		m_szFunc = NULL;
	}
};
#define FUNC_TRACE(x) FUNC_TRACEClass __FUNC_TRACE__DebugTraceObject(x);
#define FUNC_TRACE_EXIT __FUNC_TRACE__DebugTraceObject.~FUNC_TRACEClass();
#else
#define FUNC_TRACE(x)
#define FUNC_TRACE_EXIT
#endif

#endif // DEBUG_H
