//////////////////////////////////////////////////////////////
// File:		// ChannelReader.h
// File time:	// 26.07.2004	17:49
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _CHANNELREADER_H_UID000000009C9E2DC9
#define _CHANNELREADER_H_UID000000009C9E2DC9

#include "Channel.h"

class CChannelReader: protected CChannel
{
public:
	CChannelReader ( DWORD nId );
	virtual ~CChannelReader (void);

	CLASS_FUNC void StopReadingThread()
	{
		ASSERT( !m_bOpenOnly );
		if( m_bOpenOnly )
			return;

		SetEvent( m_hStopReadThreadEvent );

		ASSERT( m_hReaderThread != (HANDLE)0xcccccccc );
		if( m_hReaderThread != NULL )
		{
			DWORD dwWaitResult = MyWaitForSingleObject( m_hReaderThread, 10000L );
			ASSERT( dwWaitResult == WAIT_OBJECT_0 );
			CloseHandle( m_hReaderThread );
			m_hReaderThread = NULL;
		}
	}

	CLASS_FUNC BOOL BeginReadingThread(CHANNEL_READ_PROC proc, void* argument)
	{
		ASSERT( !m_bOpenOnly );
		if( m_bOpenOnly )
			return FALSE;

		LONG prev = InterlockedCompareExchange( &m_lReadThreadStarted, 1, 0 );
		if( prev != 0 )
		{
			ASSERT( FALSE && "Can't start multiple threads for single Reader object!" );
			return FALSE;
		}

		ASSERT( m_hReaderThread == NULL );

		m_pUserReadFunc = proc;
		m_pUserReadFuncArg = argument;
		unsigned threadID = 0;

		HANDLE hThread = (HANDLE) my_beginthreadex( NULL, 0, ReadThread, this, 0, &threadID );
		if( hThread == NULL )
		{
			InterlockedExchange( &m_lReadThreadStarted, 0 );
			return FALSE;
		}
		else
		{
			m_hReaderThread = hThread;
		}

		return TRUE;
	}

	CLASS_FUNC BOOL ReadPacket( OUT PVOID* ppPacket, OUT PDWORD pdwPacketSize, OUT PBOOL pbBinaryPacket )
	{
		ASSERT( !m_bOpenOnly );
		if( m_bOpenOnly )
			return FALSE;

		ASSERT( ppPacket != NULL );

		DWORD dwWaitResult; 
		dwWaitResult = MyWaitForSingleObject( m_hReadMutex, INFINITE );
		switch (dwWaitResult) 
		{
			case WAIT_OBJECT_0: 
				break;
			case WAIT_TIMEOUT: 
			case WAIT_ABANDONED: 
			default:
				return FALSE; 
		}

		const DWORD dwBlockDataSize = m_pMemory->m_Blocks->GetDataSize();

		DWORD	dwBlocksRead = 0;
		DWORD	dwPacketID = 0;
		DWORD	dwPacketSize = 0;
		BOOL	bBinaryPacket = TRUE;
		PBYTE	pBuffer = NULL;
		DWORD	dwBlocks = 0;

		*ppPacket = NULL;
		*pdwPacketSize = 0;
		*pbBinaryPacket = TRUE;

		DWORD dwCachedHead = m_pMemory->m_dwHead;
		
		while(TRUE)
		{
			HANDLE hArray[2] = { m_hStreamNotEmptyEvent, m_hStopReadThreadEvent };
			dwWaitResult = MyWaitForMultipleObjects( 2, hArray, FALSE, INFINITE );
			switch (dwWaitResult) 
			{
				case WAIT_OBJECT_0:
					break;
				default:
					ReleaseMutex(m_hReadMutex);
					delete[] pBuffer;
					return FALSE; 
			}
			
			PACKET_BLOCK& block = m_pMemory->m_Blocks[dwCachedHead];
			if( dwBlocksRead == 0 )
			{
				dwPacketID = block.m_dwPacketID;
				dwPacketSize = block.m_dwPacketSize;
				bBinaryPacket = ( block.m_dwFlags & PACKET_BLOCK_FLAG_TEXT ) == 0;
				ASSERT( pBuffer == NULL );
				pBuffer = new BYTE[dwPacketSize];
				ASSERT( pBuffer != NULL );
				dwBlocks = (dwPacketSize + dwBlockDataSize - 1 ) / dwBlockDataSize;
			}

			if( dwPacketID != block.m_dwPacketID ||
				dwBlocksRead != block.m_dwBlockNumber ||
				dwPacketSize != block.m_dwPacketSize ||
				bBinaryPacket != (( block.m_dwFlags & PACKET_BLOCK_FLAG_TEXT ) == 0)
				)
			{
				delete[] pBuffer;
				pBuffer = NULL;
				dwBlocksRead = 0;
				continue;
			}

			DWORD dwBytes = ( dwBlocksRead + 1 == dwBlocks ) ?
							Rest( dwPacketSize, dwBlockDataSize ) : dwBlockDataSize;
			ASSERT( dwBytes <= dwBlockDataSize );
			ASSERT( dwBlocksRead*dwBlockDataSize+dwBytes <= dwPacketSize );
			CopyMemory( pBuffer+dwBlocksRead*dwBlockDataSize, block.m_chData, dwBytes );
			//ZeroMemory( &block, block.GetHeaderSize() ); // на всякий случай

#ifdef FORCE_PACKET_READ
			if( dwBlocksRead+1 < dwBlocks )
			{
				DWORD next = dwCachedHead;
				IncBlockIndex( next );
				if( next != m_pMemory->m_dwTail )
				{
					IncBlockIndex( dwCachedHead );
					dwBlocksRead ++;
					continue;
				}
			}
#endif

			dwWaitResult = MyWaitForSingleObject( m_hStreamStatusMutex, 5000L );
			switch (dwWaitResult) 
			{
				case WAIT_OBJECT_0: 
					break;
				default:
					ReleaseMutex(m_hReadMutex);
					delete[] pBuffer;
					return FALSE; 
			}

			IncBlockIndex( dwCachedHead );
			m_pMemory->m_dwHead = dwCachedHead;

			SetEvent( m_hStreamNotFullEvent );

			if( m_pMemory->m_dwTail == dwCachedHead )
			{
				ResetEvent( m_hStreamNotEmptyEvent );
			}

			ReleaseMutex(m_hStreamStatusMutex);

			dwBlocksRead ++;

			if( dwBlocksRead == dwBlocks )
				break;
		};

		*ppPacket = pBuffer;
		*pdwPacketSize = dwPacketSize;
		*pbBinaryPacket = bBinaryPacket;

		ReleaseMutex(m_hReadMutex);

		//WriteLog( _T("ReadPacket: *pdwPacketSize = %d"), *pdwPacketSize );

		return TRUE;
	}

	CLASS_FUNC void ReleasePacket( IN LPVOID pPacket )
	{
		delete[] pPacket;
	}

	CLASS_FUNC BOOL AddReader()
	{
		DWORD dwWaitResult; 
		dwWaitResult = MyWaitForSingleObject( m_hHasReadersDataMutex, 5000L );
		switch (dwWaitResult) 
		{
			case WAIT_OBJECT_0: 
				break;
			default:
				return FALSE; 
		}
		ResetEvent( m_hHasNoReadersEvent );
		m_pMemory->m_dwReaderCount++;
		ReleaseMutex( m_hHasReadersDataMutex );
		return TRUE;
	}

	CLASS_FUNC BOOL RemoveReader()
	{
		DWORD dwWaitResult; 
		dwWaitResult = MyWaitForSingleObject( m_hHasReadersDataMutex, 5000L );
		switch (dwWaitResult) 
		{
			case WAIT_OBJECT_0: 
				break;
			default:
				return FALSE; 
		}
		m_pMemory->m_dwReaderCount--;
		if( m_pMemory->m_dwReaderCount == 0 )
			SetEvent( m_hHasNoReadersEvent );
		ReleaseMutex( m_hHasReadersDataMutex );
		return TRUE;
	}

protected:

	static unsigned __stdcall ReadThread( void* pArg )
	{
		unsigned res = ReadThreadProc( pArg );
		return res;
	}

	static unsigned __stdcall ReadThreadProc( void* pArg )
	{
		CChannelReader* that = (CChannelReader*) pArg;
		ASSERT( that != NULL );

		TRACE( _T(">>> ReadThreadProc begins ( %d )\n"), that->m_nId );

		if( !that->AddReader() )
		{
			InterlockedExchange( &that->m_lReadThreadStarted, 0 );
			return 0;
		}

		LPVOID pPacket = NULL;
		DWORD dwPacketSize = 0;
		BOOL bBinaryPacket = TRUE;

		while( that->ReadPacket( &pPacket, &dwPacketSize, &bBinaryPacket ) )
		{
			that->m_pUserReadFunc( pPacket, dwPacketSize, bBinaryPacket, that->m_pUserReadFuncArg );
			that->ReleasePacket( pPacket );
			pPacket = NULL;
			dwPacketSize = 0;
			bBinaryPacket = TRUE;
		};

		ASSERT( pPacket == NULL );
		if( !that->RemoveReader() )
		{
			InterlockedExchange( &that->m_lReadThreadStarted, 0 );
			return 0;
		}

		InterlockedExchange( &that->m_lReadThreadStarted, 0 );

		TRACE( _T("<<< ReadThreadProc ends ( %d )\n"), that->m_nId );

		return 0;
	}

private:

};

#endif //ifndef _CHANNELREADER_H_UID000000009C9E2DC9
