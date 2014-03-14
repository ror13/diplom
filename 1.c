#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
typedef int socklen_t;
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

bool find_server(std::string * ip)
{
	int sockfd,n;
	struct sockaddr_in addr, server_addr;
	long bBroadcast = 1;
	socklen_t rbytes = sizeof(server_addr);
	sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

	memset(&addr,0x0,sizeof(struct sockaddr_in));
	memset(&server_addr,0x0,sizeof(struct sockaddr_in));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_BROADCAST);
	addr.sin_port=htons(23162);
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(char*)&tv,sizeof(struct timeval)) < 0)
	{
		printf("SO_RCVTIMEO %s\n");
		return false;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char *)&bBroadcast, sizeof(bBroadcast)) < 0)
	{
		printf("SO_BROADCAST %s\n");
		return false;
	}
	
	char recvline[4];
	while((n=recvfrom(	sockfd,	recvline, 4, 0, (struct sockaddr *)&server_addr, &rbytes))!=4 || 
			memcmp(recvline,"ping",4)!=0 )
	{
		char sendline = 0;
		sendto(sockfd,&sendline,1,0,
					(struct sockaddr *)&addr,sizeof(addr));
	}
	*ip =  inet_ntoa(server_addr.sin_addr);
	return true;
}
int main(int argc, char**argv)
{
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

	std::string asr;
	find_server(&asr);
	printf("IP %s",asr.c_str());

#ifdef _WIN32
	WSACleanup();
#endif
}
