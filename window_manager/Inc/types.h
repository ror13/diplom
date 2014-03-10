#pragma once

#ifndef WARNING
#define INT_TO_STR(x) #x
#define INT_TO_STR2(x) INT_TO_STR((x))
#define WARNING __FILE__  INT_TO_STR2(__LINE__) " : warning: "
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0501
#else
#if _WIN32_WINNT < 0x0501
#pragma message ( WARNING "_WIN32_WINNT is defined to very old value!!!")
#endif
#endif


//#include <wtypes.h>

#ifndef _WINDEF_
#else // for other windows programs:
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS STATUS_SUCCESS
#endif

#ifndef ERROR_NO_MORE_ITEMS
#define ERROR_NO_MORE_ITEMS STATUS_NO_MORE_ENTRIES
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef NTSTATUS
typedef long NTSTATUS;
#endif

#ifndef LPDWORD
typedef unsigned long* LPDWORD;
#endif

#ifndef LPVOID
typedef void* LPVOID;
#endif

#ifndef LPCVOID
typedef const void* LPCVOID;
#endif

#ifndef PVOID
typedef void *PVOID;
#endif

#include <devioctl.h>
#include <windef.h>

