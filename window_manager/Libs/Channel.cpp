//////////////////////////////////////////////////////////////
// File:		// Channel.cpp
// File time:	// 26.07.2004	18:09
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "Channel.h"

//////////////////////////////////////////////////////////////

#ifndef _PROGRAM_OBJECTS_UID_DEFINED_
#error Macro _PROGRAM_OBJECTS_UID_DEFINED_ & _PROGRAM_OBJECTS_UID_ must be defined! (for example "MyProgram")
/*
Example:
#define _PROGRAM_OBJECTS_UID_DEFINED_
#define _PROGRAM_OBJECTS_UID_				_T("MyProgram")
*/
#endif

//////////////////////////////////////////////////////////////

#ifdef MyWaitFor_exists

DWORD MyWaitForSingleObject(  // for better debugging
  HANDLE hHandle, 
  DWORD dwMilliseconds 
)
{
	DWORD res = WaitForSingleObject( hHandle, dwMilliseconds );
	return res;
}

DWORD MyWaitForMultipleObjects( // for better debugging
  DWORD nCount, 
  CONST HANDLE* lpHandles, 
  BOOL fWaitAll, 
  DWORD dwMilliseconds 
)
{
	DWORD res = WaitForMultipleObjects( nCount, lpHandles, fWaitAll, dwMilliseconds );
	return res;
}

#endif //ifdef MyWaitFor_exists

unsigned long __cdecl my_beginthreadex ( // for better debugging
        void *security,
        unsigned stacksize,
        unsigned (__stdcall * initialcode) (void *),
        void * argument,
        unsigned createflag,
        unsigned *thrdaddr
        )
{
	unsigned long res = _beginthreadex(
		security,
		stacksize,
		initialcode,
		argument,
		createflag,
		thrdaddr
		);
	return res;
}

//////////////////////////////////////////////////////////////

#include <aclapi.h>

//////////////////////////////////////////////////////////////

struct SECURITY_ATTR_INFO
{
	SECURITY_ATTRIBUTES		attr;
	PACL					pACL;

	SECURITY_ATTR_INFO()
	{
		attr.nLength = sizeof(attr);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle = FALSE;
		pACL = NULL;
	}
};

//////////////////////////////////////////////////////////////

void ReleaseInterSessionSecurityAttributes( PSECURITY_ATTRIBUTES pAttr )
{
//	TRACE( _T("call ReleaseInterSessionSecurityAttributes()\n") );

	SECURITY_ATTR_INFO* pSAI = NULL;
	ASSERT( 0 == (DWORD)&pSAI->attr );

	pSAI = (SECURITY_ATTR_INFO*)( ((PBYTE)pAttr) - (DWORD)&pSAI->attr );

	if( pAttr != NULL && pSAI != NULL )
	{
		if( pSAI->attr.lpSecurityDescriptor != NULL ) 
		{
			LocalFree( pSAI->attr.lpSecurityDescriptor );
			pSAI->attr.lpSecurityDescriptor = NULL;
		}

		if( pSAI->pACL != NULL ) 
		{
			LocalFree(pSAI->pACL);
			pSAI->pACL = NULL;
		}

		delete pSAI;
	}
}

//////////////////////////////////////////////////////////////

PSECURITY_ATTRIBUTES GetInterSessionSecurityAttributes( DWORD dwRights )
{
	//return NULL; // return NULL if you want to disable advanced security

	DWORD dwRes;
	PSID pEveryoneSID = NULL;
//	PSID pAdminSID = NULL;
	BOOL bAllOK = FALSE;
	PSECURITY_DESCRIPTOR pSD = NULL;
//	EXPLICIT_ACCESS ea[2] = {0};
	EXPLICIT_ACCESS ea[1] = {0};
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
//	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

//	TRACE( _T("Enter GetInterSessionSecurityAttributes()\n") );

	//dwRights |= GENERIC_ALL | GENERIC_READ | GENERIC_WRITE |
	//	SYNCHRONIZE | STANDARD_RIGHTS_ALL | MAXIMUM_ALLOWED |
	//	ACCESS_SYSTEM_SECURITY | GENERIC_EXECUTE;

	SECURITY_ATTR_INFO* pSAI = new SECURITY_ATTR_INFO;
	PSECURITY_ATTRIBUTES pSa = NULL;

	if( pSAI == NULL )
	{
		TRACE( _T("SECURITY> Can't allocate SECURITY_ATTR_INFO!\n") );
		goto Cleanup;
	}

	pSa = &pSAI->attr;
	pSa->nLength = sizeof(*pSa);
	pSa->lpSecurityDescriptor = NULL;
	pSa->bInheritHandle = FALSE;

	// Create a well-known SID for the Everyone group.
	if(!AllocateAndInitializeSid(&SIDAuthWorld, 1,
					 SECURITY_WORLD_RID,
					 0, 0, 0, 0, 0, 0, 0,
					 &pEveryoneSID))
	{
		TRACE1("SECURITY> AllocateAndInitializeSid Error %u\n", GetLastError());
		goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow Everyone read access to the key.
	ZeroMemory(ea, sizeof(ea));
	ea[0].grfAccessPermissions = dwRights;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance= NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;
/*
	// Create a SID for the BUILTIN\Administrators group.
	if(! AllocateAndInitializeSid(&SIDAuthNT, 2,
					 SECURITY_BUILTIN_DOMAIN_RID,
					 DOMAIN_ALIAS_RID_ADMINS,
					 0, 0, 0, 0, 0, 0,
					 &pAdminSID)) 
	{
		TRACE1("SECURITY> AllocateAndInitializeSid Error %u\n", GetLastError());
		goto Cleanup; 
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow the Administrators group full access to the key.
	ea[1].grfAccessPermissions = dwRights;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance= NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[1].Trustee.ptstrName  = (LPTSTR) pAdminSID;
*/

	// Create a new ACL that contains the new ACEs.
//	dwRes = SetEntriesInAcl(2, ea, NULL, &pSAI->pACL);
	dwRes = SetEntriesInAcl(1, ea, NULL, &pSAI->pACL);
	if (ERROR_SUCCESS != dwRes) 
	{
		TRACE1("SECURITY> SetEntriesInAcl Error %u\n", GetLastError());
		goto Cleanup;
	}

	// Initialize a security descriptor.  
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, 
							 SECURITY_DESCRIPTOR_MIN_LENGTH); 
	if (NULL == pSD) 
	{ 
		TRACE1("SECURITY> LocalAlloc Error %u\n", GetLastError());
		goto Cleanup; 
	} 

	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
	{  
		TRACE1("SECURITY> InitializeSecurityDescriptor Error %u\n",
								GetLastError());
		goto Cleanup; 
	} 

	// Add the ACL to the security descriptor. 
	if (!SetSecurityDescriptorDacl(pSD, 
			TRUE,     // bDaclPresent flag   
			pSAI->pACL, 
			FALSE))   // not a default DACL 
	{  
		TRACE1("SECURITY> SetSecurityDescriptorDacl Error %u\n", GetLastError());
		goto Cleanup; 
	} 

	// Initialize a security attributes structure.
	bAllOK = TRUE;
	pSa->lpSecurityDescriptor = pSD;

	// Use the security attributes to set the security descriptor 
	// when you create a key.

Cleanup:

	if( pEveryoneSID != NULL ) 
	{
		FreeSid(pEveryoneSID);
		pEveryoneSID = NULL;
	}
/*
	if( pAdminSID != NULL ) 
	{
		FreeSid(pAdminSID);
		pAdminSID = NULL;
	}
*/
	if( !bAllOK )
	{
		if( pSD != NULL ) 
		{
			LocalFree(pSD);
			pSD = NULL;
		}
		if( pSAI != NULL )
		{
			if( pSAI->pACL != NULL ) 
			{
				LocalFree(pSAI->pACL);
				pSAI->pACL = NULL;
			}
			delete pSAI;
			pSAI = NULL;
			pSa = NULL;
		}
		ASSERT( pSa == NULL );
	}

//	TRACE( _T("Leave GetInterSessionSecurityAttributes()\n") );
	return pSa;
}

//////////////////////////////////////////////////////////////

CChannel::CChannel ( DWORD nId, BOOL bOpenHandlesOnly, DWORD dwBlocks )
{
	ASSERT( sizeof(CHANNEL_MEMORY) < sizeof(PACKET_BLOCK) );
	m_pMemory = NULL;
	if( bOpenHandlesOnly )
		m_dwBlocks = 0;
	else
		m_dwBlocks = dwBlocks;
	m_nId = nId;
	m_bOpenOnly = bOpenHandlesOnly;
	m_strKernelObjectsPrefix.Format( _T("Global\\InterprocessChannel_%s_%d_"),
		_PROGRAM_OBJECTS_UID_, nId );

	m_lReadThreadStarted = 0;
	m_pUserReadFunc = NULL;
	m_pUserReadFuncArg = NULL;

	m_hReaderThread = NULL;

#define InitHandle(x)	x = NULL

	InitHandle( m_hInitializedEvent );
	InitHandle( m_hHasReadersDataMutex );
	InitHandle( m_hHasNoReadersEvent );
	InitHandle( m_hWriteMutex );
	InitHandle( m_hReadMutex );
	InitHandle( m_hStreamStatusMutex );
	InitHandle( m_hStreamNotFullEvent );
	InitHandle( m_hStreamNotEmptyEvent );
	InitHandle( m_hStopReadThreadEvent );
	InitHandle( m_hFileMapping );
	InitHandle( m_hInitializeMutex );
	
	Initialize();
}

CChannel::~CChannel (void)
{
	Uninitialize();
}

//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

