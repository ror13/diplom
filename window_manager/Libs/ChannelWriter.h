//////////////////////////////////////////////////////////////
// File:		// ChannelWriter.h
// File time:	// 26.07.2004	17:49
// Author:		// XXX
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _CHANNELWRITER_H_UID000000009DE185D5
#define _CHANNELWRITER_H_UID000000009DE185D5

#include "Channel.h"

class CChannelWriter: protected CChannel
{
public:
	CChannelWriter ( DWORD nId );
	virtual ~CChannelWriter (void);

	CLASS_FUNC BOOL WritePacket( IN LPCVOID pPacket, IN DWORD dwPacketSize, IN BOOL bBinaryPacket = TRUE )
	{
//#pragma message (WARNING "WritePacket should work with ApplicaServer clients")
		//WriteLog( _T("WritePacket: dwPacketSize = %d"), dwPacketSize );
		ASSERT( m_bOpenOnly );
		if( !m_bOpenOnly )
			return FALSE;

		if( !IsInitialized() )
		{
			Uninitialize();
			Initialize();
			if( !IsInitialized() )
				return FALSE;
		}

		DWORD dwWaitResult; 
		dwWaitResult = MyWaitForSingleObject( m_hWriteMutex, 5000L );
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
		DWORD dwBlocks = (dwPacketSize + dwBlockDataSize - 1 ) / dwBlockDataSize;
		DWORD dwPacketID = 
			InterlockedExchangeAdd( (LONG*)&m_pMemory->m_dwLastWrittenPacketId, 1 );

		DWORD dwCachedTail = m_pMemory->m_dwTail;

		for( DWORD i=0; i<dwBlocks; i++ )
		{
			HANDLE hArray[2] = { m_hStreamNotFullEvent, m_hHasNoReadersEvent };
			dwWaitResult = MyWaitForMultipleObjects( 2, hArray, FALSE, 5000L );
			switch (dwWaitResult) 
			{
				case WAIT_OBJECT_0:
					break;
				default:
					ReleaseMutex(m_hWriteMutex);
					return FALSE; 
			}

			PACKET_BLOCK& block = m_pMemory->m_Blocks[dwCachedTail];
			//ZeroMemory( &block, block.GetHeaderSize() ); // На всякий случай
			block.m_dwPacketID = dwPacketID;
			block.m_dwBlockNumber = i;
			block.m_dwPacketSize = dwPacketSize;
			block.m_dwFlags = bBinaryPacket ? 0 : PACKET_BLOCK_FLAG_TEXT;

			DWORD dwBytes = ( i + 1 == dwBlocks ) ?
							Rest( dwPacketSize, dwBlockDataSize ) : dwBlockDataSize;

			CopyMemory( block.m_chData, PBYTE(pPacket)+i*dwBlockDataSize, dwBytes );

#ifdef FORCE_PACKET_WRITE
			if( i+1 < dwBlocks )
			{
				DWORD next = dwCachedTail;
				IncBlockIndex( next );
				IncBlockIndex( next );
				if( next != m_pMemory->m_dwHead )
				{
					IncBlockIndex( dwCachedTail );
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
					ReleaseMutex(m_hWriteMutex);
					return FALSE; 
			}

			IncBlockIndex( dwCachedTail );
			m_pMemory->m_dwTail = dwCachedTail;

			SetEvent( m_hStreamNotEmptyEvent );

			DWORD next = dwCachedTail;
			IncBlockIndex( next );
			if( next == m_pMemory->m_dwHead )
			{
				ResetEvent( m_hStreamNotFullEvent );
			}

			ReleaseMutex(m_hStreamStatusMutex);
		}

		ReleaseMutex(m_hWriteMutex);

		return TRUE;
	}

protected:

private:

};

#endif //ifndef _CHANNELWRITER_H_UID000000009DE185D5
