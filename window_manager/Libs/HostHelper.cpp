#include "stdafx.h"

#include "HostHelper.h"
#include <WinIOCTL.h>

#include "..\Inc\Applica.h"
#include "ApplicaInfo.h"

HANDLE HostOpen()
{
	HANDLE hHost;

	if(DefineDosDevice (DDD_RAW_TARGET_PATH, _T("Host"), GetApplicaHostDevice() ))
	{
		hHost = CreateFile(_T("\\\\.\\Host"), GENERIC_WRITE|GENERIC_READ, 0,
					NULL,	OPEN_EXISTING,	0,	NULL);

		if(hHost == INVALID_HANDLE_VALUE)
		{
			DefineDosDevice(DDD_REMOVE_DEFINITION, _T("Host"), NULL);

			return INVALID_HANDLE_VALUE;
		}

		return hHost;
	}

	return INVALID_HANDLE_VALUE;
}

void HostClose(HANDLE hHost)
{
	CloseHandle(hHost);
	DefineDosDevice (DDD_REMOVE_DEFINITION, _T("Host"), NULL);
}


int HostGetStationCount(HANDLE hHost)
{
	int Count = 0;
	STATION_REGISTRATION Station;
	DWORD BytesReturned;

	if(hHost == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	while(DeviceIoControl(hHost, IOCTL_APPLICA_GET_STATION, &Count, sizeof(Count), 
		&Station, sizeof(Station), &BytesReturned, NULL))
	{
		Count++;
	}

	return Count;
}

int HostGetCurrentStation(HANDLE hHost)
{
	int uId = INVALID_STATION_ID;
	DWORD BytesReturned;

	if(hHost == INVALID_HANDLE_VALUE)
	{
		return INVALID_STATION_ID;
	}

	DeviceIoControl(hHost, IOCTL_APPLICA_GET_STATION_ID, NULL, 0,
		&uId, sizeof(uId), &BytesReturned, NULL);

	return uId;
}

int HostGetStationCount(void)
{
	HANDLE hHost;
	int Count;

	hHost = HostOpen();

	if(hHost == INVALID_HANDLE_VALUE)
	{
//		MessageBox( NULL, _T("INVALID_HANDLE_VALUE"), _T("HostGetStationCount"), 0 );
		return 1;
	}

	Count = HostGetStationCount(hHost);

	HostClose(hHost);

	return Count + 1;
}

int HostGetCurrentStation(void)
{
	HANDLE hHost;
	int iId;

	hHost = HostOpen();

	if(hHost == INVALID_HANDLE_VALUE)
	{
//		MessageBox( NULL, _T("INVALID_HANDLE_VALUE"), _T("HostGetCurrentStation"), 0 );
		return -1;
	}

	iId = HostGetCurrentStation(hHost);

	HostClose(hHost);

	if( iId == INVALID_STATION_ID )
	{
//		MessageBox( NULL, _T("INVALID_STATION_ID"), _T("HostGetCurrentStation"), 0 );
//		ASSERT( FALSE && "INVALID_STATION_ID");
		return 0;
	}

	return iId + 1;
}

bool IsDriverPresent(void)
{
	HANDLE hHost = HostOpen();

	if(hHost == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	HostClose(hHost);

	return true;
}
