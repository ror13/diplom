#include "StdAfx.h"

#include "NetworkHelper.h"
#include "tchar.h"

#include <Iphlpapi.h>
#pragma comment (lib, "Iphlpapi.lib")

#include "PresenterRegistry.h"
#include "CommonApi.h"

/*
Sample code how to get network infos

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		GlobalFree (pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc ( sizeof(ulOutBufLen) );
	}

	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
			printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
			printf("\tAdapter Addr: \t%ld\n", pAdapter->Address);
			printf("\tIP Address: \t%s\n", pAdapter->IpAddressList.IpAddress.String);
			printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);
			
			printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
			printf("\t***\n");
			if (pAdapter->DhcpEnabled) {
				printf("\tDHCP Enabled: Yes\n");
				printf("\t\tDHCP Server: \t%s\n", pAdapter->DhcpServer.IpAddress.String);
				printf("\tLease Obtained: %ld\n", pAdapter->LeaseObtained);
			}
			else
				printf("\tDHCP Enabled: No\n");
			
			if (pAdapter->HaveWins) {
				printf("\tHave Wins: Yes\n");
				printf("\t\tPrimary Wins Server: \t%s\n", pAdapter->PrimaryWinsServer.IpAddress.String);
				printf("\t\tSecondary Wins Server: \t%s\n", pAdapter->SecondaryWinsServer.IpAddress.String);
			}
			else
				printf("\tHave Wins: No\n");
			pAdapter = pAdapter->Next;
		}
	}
	else {
		printf("Call to GetAdaptersInfo failed.\n");
	}
*/



ULONG GetLocalIP()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
		
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free (pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc ( ulOutBufLen );
	}

	dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen);
	if (dwRetVal != NO_ERROR)
	{
		return -1;
	}

	pAdapter = pAdapterInfo;

	ULONG ip_addr = inet_addr(pAdapter->IpAddressList.IpAddress.String);

	free (pAdapterInfo);

	return ip_addr;
}

ULONG GetLocalBroadcastIP()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
		
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free (pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc ( ulOutBufLen );
	}

	dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen);
	if (dwRetVal != NO_ERROR)
	{
		return -1;
	}

	pAdapter = pAdapterInfo;

	u_long host_addr = inet_addr(pAdapter->IpAddressList.IpAddress.String);   // local IP addr
	u_long net_mask = inet_addr(pAdapter->IpAddressList.IpMask.String);   // LAN netmask
 	u_long net_addr = host_addr & net_mask;         // 172.16.64.0
 	u_long dir_bcast_addr = net_addr | (~net_mask); // 172.16.95.255

	free (pAdapterInfo);

	return dir_bcast_addr;
}
