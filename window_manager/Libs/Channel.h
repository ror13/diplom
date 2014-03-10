//////////////////////////////////////////////////////////////
// File:		// Channel.h
// File time:	// 26.07.2004	18:09
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _CHANNEL_H_UID00000001A1ACBCD1
#define _CHANNEL_H_UID00000001A1ACBCD1

//////////////////////////////////////////////////////////////

#define CLASS_FUNC virtual

//////////////////////////////////////////////////////////////

#if 0

#define MyWaitForSingleObject		WaitForSingleObject
#define MyWaitForMultipleObjects	WaitForMultipleObjects

#else

DWORD MyWaitForSingleObject(  // for better debugging
  HANDLE hHandle, 
  DWORD dwMilliseconds 
); 

DWORD MyWaitForMultipleObjects( // for better debugging
  DWORD nCount, 
  CONST HANDLE* lpHandles, 
  BOOL fWaitAll, 
  DWORD dwMilliseconds 
);

#define MyWaitFor_exists

#endif

unsigned long __cdecl my_beginthreadex ( // for better debugging
        void *security,
        unsigned stacksize,
        unsigned (__stdcall * initialcode) (void *),
        void * argument,
        unsigned createflag,
        unsigned *thrdaddr
        );

//////////////////////////////////////////////////////////////

PSECURITY_ATTRIBUTES GetInterSessionSecurityAttributes( DWORD dwRights );
void ReleaseInterSessionSecurityAttributes( PSECURITY_ATTRIBUTES pAttr );

//////////////////////////////////////////////////////////////

#include <process.h>

#define DATA_SIZE 4096 - 16 //4000 - 16
#define DEFAULT_CHANNEL_BLOCK_COUNT 64
#define FORCE_PACKET_READ
#define FORCE_PACKET_WRITE

//////////////////////////////////////////////////////////////

#define PACKET_BLOCK_FLAG_TEXT	1

//////////////////////////////////////////////////////////////

typedef struct sPACKET_BLOCK
{
	DWORD	m_dwPacketID;
	DWORD	m_dwBlockNumber;
	DWORD	m_dwPacketSize;
	DWORD	m_dwFlags;
	BYTE	m_chData[DATA_SIZE];

	DWORD GetDataSize()
	{
		return sizeof(m_chData);
	}

	DWORD GetHeaderSize()
	{
		return sizeof(PACKET_BLOCK) - sizeof(m_chData);
	}
} PACKET_BLOCK, *PPACKET_BLOCK;

//////////////////////////////////////////////////////////////

#pragma warning(push)
#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

typedef struct sCHANNEL_MEMORY
{
	DWORD	m_dwBlocks;		//stream block count
	volatile DWORD	m_dwLastWrittenPacketId; // last packet ID (for internal use!)
	volatile DWORD	m_dwHead;		//current head position (block number)
	volatile DWORD	m_dwTail;		//current tail position (block number after last)
	DWORD	m_nId;			//stream file mapping ID (for read only, debug)
	DWORD	m_dwReaderCount;
	DWORD	m_dwReserved[8-2];
	PACKET_BLOCK	m_Blocks[0];
} CHANNEL_MEMORY, *PCHANNEL_MEMORY;

#pragma warning(pop)

//////////////////////////////////////////////////////////////

typedef void ( __cdecl *CHANNEL_READ_PROC )
		( IN LPCVOID pPacket, IN DWORD dwPacketSize, IN BOOL bBinaryPacket, IN void * pArg );

//////////////////////////////////////////////////////////////

class CChannel
{
public:
	CChannel ( DWORD nId, BOOL bWriter, DWORD dwBlocks = DEFAULT_CHANNEL_BLOCK_COUNT );
	virtual ~CChannel (void);

protected:

	CHANNEL_MEMORY* m_pMemory;	//current memory buffer

	CString	m_strKernelObjectsPrefix;
	CString	m_strPrefix;
	DWORD	m_nId;   //stream file mapping ID
	BOOL	m_bOpenOnly;

	void SignalRead();
	void SignalWrite();
	void WaitForRead();
	void WaitForWrite();

	HANDLE	m_hInitializeMutex;
	HANDLE	m_hInitializedEvent;		// if all other objects initialized;

	HANDLE	m_hHasReadersDataMutex;	// protect m_hHasReaders & m_pMemory->m_dwReaderCount
	HANDLE	m_hHasNoReadersEvent;

	HANDLE	m_hWriteMutex;
	HANDLE	m_hReadMutex;
	HANDLE	m_hStreamStatusMutex;
	HANDLE	m_hStreamNotFullEvent;
	HANDLE	m_hStreamNotEmptyEvent;

	HANDLE	m_hStopReadThreadEvent;	// unnamed! Only for this reader object example
	HANDLE	m_hReaderThread;		// Only for reader object
	DWORD	m_dwBlocks;				// Only for reader object
	HANDLE	m_hFileMapping;

	LONG		m_lReadThreadStarted;
	CHANNEL_READ_PROC	m_pUserReadFunc;
	void*		m_pUserReadFuncArg;

	BOOL IsInitialized()
	{
		BOOL bHandlesOK =
				m_hInitializeMutex != NULL &&
				m_hInitializedEvent != NULL &&
				m_hHasReadersDataMutex != NULL &&
				m_hHasNoReadersEvent != NULL &&
				m_hWriteMutex != NULL &&
				m_hReadMutex != NULL &&
				m_hStreamStatusMutex != NULL &&
				m_hStreamNotFullEvent != NULL &&
				m_hStreamNotEmptyEvent != NULL &&
				m_hFileMapping != NULL &&
				( m_hStopReadThreadEvent != NULL || m_bOpenOnly ) &&
				( m_dwBlocks != 0 || m_bOpenOnly ) &&
				m_pMemory != NULL;
		return bHandlesOK;
	}

	void IncBlockIndex( volatile DWORD& dwIndex)
	{
		_ASSERT(dwIndex < m_pMemory->m_dwBlocks );
		dwIndex++;
		if( dwIndex >= m_pMemory->m_dwBlocks )
			dwIndex = 0;
	}

	DWORD Rest( DWORD dwAllSize, DWORD dwBlockSize )
	{
		DWORD res = dwAllSize % dwBlockSize;
		if( res == 0 )
			return dwBlockSize;
		return res;
	}

	void KernelDiag( BOOL bSuccess, LPCTSTR szName, LONG LastError )
	{
/*
		{
			CString errDesc;
			if( !bSuccess )
			{
				LPVOID lpMsgBuf = NULL;
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					LastError,
					0, // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL
				);
				// Process any inserts in lpMsgBuf.
				// ...
				// Display the string.
				if( lpMsgBuf != NULL )
				{
					errDesc = (LPCTSTR) lpMsgBuf;
					errDesc.TrimRight( _T("\r\n") );
				}
				else
				{
					errDesc.Format( _T("Unknown error #%d"), LastError );
				}
				errDesc = _T("  Error: ") + errDesc;

				// Free the buffer.
				LocalFree( lpMsgBuf );
			}

			CString s;
			s.Format(
					_T("%s %s object \"%s\"!%s\n"),
					( bSuccess ? _T("Succeeded") : _T("Failed") ),
					( m_bOpenOnly ? _T("opening") : _T("creating") ),
					szName, errDesc );

			//TRACE( s );
			OutputDebugString( s );
		}
*/
		static BOOL bMessage = TRUE;
		if( !bSuccess && bMessage && LastError != ERROR_FILE_NOT_FOUND )
		{
			CString errDesc;
			{
				LPVOID lpMsgBuf = NULL;
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					LastError,
					0, // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL
				);
				// Process any inserts in lpMsgBuf.
				// ...
				// Display the string.
				if( lpMsgBuf != NULL )
				{
					errDesc = (LPCTSTR) lpMsgBuf;
					errDesc.TrimRight( _T("\r\n") );
				}
				else
				{
					errDesc.Format( _T("Unknown error #%d"), LastError );
				}
				// Free the buffer.
				LocalFree( lpMsgBuf );
			}

			CString s;
			s.Format(
					_T("Can't %s object \"%s\"!")
					_T("\nError: %s\n")
					_T("If you don't want this message to be displayed press \"Cancel\" button."),
					( m_bOpenOnly ? _T("open") : _T("create") ),
					szName, errDesc );

			int res = AfxMessageBox( s, MB_OKCANCEL | MB_ICONERROR );
			if( res == IDCANCEL )
			{
				bMessage = FALSE;
			}
		}
	}

	void InitEvent( HANDLE& hObject, LPCTSTR szObjectName )
	{
		ASSERT( szObjectName != NULL );
		DWORD dwRights = EVENT_ALL_ACCESS;
		PSECURITY_ATTRIBUTES pSa = NULL;
		if( !m_bOpenOnly )
		{
			pSa = GetInterSessionSecurityAttributes( dwRights );
			if( pSa == NULL || pSa->lpSecurityDescriptor == NULL )
			{
				TRACE2( "GetInterSessionSecurityAttributes() error! (line %d, file \"%s\")\n", __LINE__, _T(__FILE__) );
			}
		}

		CString strObjectName = m_strKernelObjectsPrefix + szObjectName + _T("_Event");
		if( m_bOpenOnly )
			hObject = OpenEvent( dwRights, FALSE, strObjectName );
		else
			hObject = CreateEvent( pSa, TRUE, FALSE, strObjectName );

		LONG le = GetLastError();

		if( pSa != NULL )
		{
			ReleaseInterSessionSecurityAttributes( pSa );
			pSa = NULL;
		}
		KernelDiag( hObject!=NULL, strObjectName, le );
	}

	void InitMutex( HANDLE& hObject, LPCTSTR szObjectName )
	{
		ASSERT( szObjectName != NULL );
		DWORD dwRights = MUTEX_ALL_ACCESS;
		PSECURITY_ATTRIBUTES pSa = NULL;
		if( !m_bOpenOnly )
		{
			pSa = GetInterSessionSecurityAttributes( dwRights );
			if( pSa == NULL || pSa->lpSecurityDescriptor == NULL )
			{
				TRACE2( "GetInterSessionSecurityAttributes() error! (line %d, file \"%s\")\n", __LINE__, _T(__FILE__) );
			}
		}

		CString strObjectName = m_strKernelObjectsPrefix + szObjectName + _T("_Mutex");
		if( m_bOpenOnly )
			hObject = OpenMutex( dwRights, FALSE, strObjectName );
		else
			hObject = CreateMutex( pSa, FALSE, strObjectName );

		LONG le = GetLastError();

		if( pSa != NULL )
		{
			ReleaseInterSessionSecurityAttributes( pSa );
			pSa = NULL;
		}
		KernelDiag( hObject!=NULL, strObjectName, le );
	}

	void InitFileMapping( HANDLE& hObject, LPCTSTR szObjectName )
	{
		ASSERT( szObjectName != NULL );
		DWORD dwRights = FILE_MAP_ALL_ACCESS;
		PSECURITY_ATTRIBUTES pSa = NULL;
		if( !m_bOpenOnly )
		{
			pSa = GetInterSessionSecurityAttributes( dwRights );
			if( pSa == NULL || pSa->lpSecurityDescriptor == NULL )
			{
				TRACE2( "GetInterSessionSecurityAttributes() error! (line %d, file \"%s\")\n", __LINE__, _T(__FILE__) );
			}
		}

		CString strObjectName = m_strKernelObjectsPrefix + szObjectName + _T("_FileMapping");
		DWORD dwBytes = m_dwBlocks * sizeof(PACKET_BLOCK) + sizeof(CHANNEL_MEMORY);
		if( m_bOpenOnly )
			hObject = OpenFileMapping( dwRights, FALSE, strObjectName );
		else
			hObject = CreateFileMapping( INVALID_HANDLE_VALUE, pSa,
							PAGE_READWRITE, 0,
							dwBytes, strObjectName );

		LONG le = GetLastError();

		if( pSa != NULL )
		{
			ReleaseInterSessionSecurityAttributes( pSa );
			pSa = NULL;
		}

		if( hObject != NULL )
		{
			LONG err = GetLastError();

			m_pMemory = (PCHANNEL_MEMORY) MapViewOfFile(
				hObject, FILE_MAP_ALL_ACCESS, 0, 0, dwBytes );
			ASSERT( m_pMemory != NULL );

			if( m_bOpenOnly )
			{
				m_dwBlocks = m_pMemory->m_dwBlocks;
				dwBytes = m_dwBlocks * sizeof(PACKET_BLOCK) + sizeof(CHANNEL_MEMORY);

				UnmapViewOfFile( m_pMemory );
				m_pMemory = (PCHANNEL_MEMORY) MapViewOfFile(
					hObject, FILE_MAP_ALL_ACCESS, 0, 0, dwBytes );
				ASSERT( m_pMemory != NULL );
			}

			if( !m_bOpenOnly && err != ERROR_ALREADY_EXISTS )
			{
				ASSERT( m_pMemory != NULL );
				ZeroMemory( m_pMemory, sizeof(CHANNEL_MEMORY) ); // На всякий случай.
				m_pMemory->m_dwBlocks = m_dwBlocks;
				m_pMemory->m_nId = m_nId;
				ASSERT( m_hInitializedEvent != NULL );
				SetEvent( m_hStreamNotFullEvent );
				SetEvent( m_hInitializedEvent );
			}
		}
		KernelDiag( hObject!=NULL, strObjectName, le );
	}

	void Initialize()
	{
		{
			DWORD dwRights = MUTEX_ALL_ACCESS;
			PSECURITY_ATTRIBUTES pSa = NULL;
			pSa = GetInterSessionSecurityAttributes( dwRights );
			if( pSa == NULL || pSa->lpSecurityDescriptor == NULL )
			{
				TRACE2( "GetInterSessionSecurityAttributes() error! (line %d, file \"%s\")\n", __LINE__, _T(__FILE__) );
			}

			m_hInitializeMutex = CreateMutex( pSa, FALSE,
				m_strKernelObjectsPrefix + _T("Initialize_Mutex") );

			LONG le = GetLastError();

			ReleaseInterSessionSecurityAttributes( pSa );
		}

		DWORD dwWaitResult; 
		dwWaitResult = MyWaitForSingleObject( m_hInitializeMutex, 5000L );
		switch (dwWaitResult) 
		{
			case WAIT_OBJECT_0: 
				break;
			case WAIT_TIMEOUT: 
				return; 
			case WAIT_ABANDONED: 
				return; 
		}

		InitEvent( m_hInitializedEvent,  _T("Initialized") );

		InitMutex( m_hHasReadersDataMutex, _T("HasReadersData") );
		InitEvent( m_hHasNoReadersEvent,  _T("HasNoReaders") );

		InitMutex( m_hWriteMutex, _T("Write") );
		InitMutex( m_hReadMutex, _T("Read") );

		InitMutex( m_hStreamStatusMutex, _T("StreamStatus") );

		InitEvent( m_hStreamNotFullEvent,  _T("StreamNotFull") );
		InitEvent( m_hStreamNotEmptyEvent,  _T("StreamNotEmpty") );

		if( m_bOpenOnly )
			m_hStopReadThreadEvent = NULL;
		else
			m_hStopReadThreadEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

		InitFileMapping( m_hFileMapping, _T("Memory") );

		ReleaseMutex(m_hInitializeMutex);
	}

	void CloseKernelHandle( HANDLE& handle )
	{
		if( handle != NULL && handle != INVALID_HANDLE_VALUE )
		{
			CloseHandle( handle );
		}
		handle=NULL;
	}

	void Uninitialize()
	{
		DWORD dwWaitResult; 
		dwWaitResult = MyWaitForSingleObject( m_hInitializeMutex, 5000L );
		switch (dwWaitResult) 
		{
			case WAIT_OBJECT_0: 
				break;
			case WAIT_TIMEOUT: 
				return; 
			case WAIT_ABANDONED: 
				return; 
		}

		if( m_pMemory != NULL )
		{
			UnmapViewOfFile( m_pMemory );
			m_pMemory = NULL;
		}

		CloseKernelHandle( m_hInitializedEvent );
		CloseKernelHandle( m_hHasReadersDataMutex );
		CloseKernelHandle( m_hHasNoReadersEvent );
		CloseKernelHandle( m_hWriteMutex );
		CloseKernelHandle( m_hReadMutex );
		CloseKernelHandle( m_hStreamStatusMutex );
		CloseKernelHandle( m_hStreamNotFullEvent );
		CloseKernelHandle( m_hStreamNotEmptyEvent );
		CloseKernelHandle( m_hStopReadThreadEvent );
		CloseKernelHandle( m_hFileMapping );

		ReleaseMutex(m_hInitializeMutex);

		CloseKernelHandle( m_hInitializeMutex );
	}


public:


private:

};

//////////////////////////////////////////////////////////////

#endif //ifndef _CHANNEL_H_UID00000001A1ACBCD1
