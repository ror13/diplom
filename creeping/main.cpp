#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>

#ifdef _WIN32
typedef int socklen_t;
#include <winsock.h>

#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include "utils.h"
#include "creeping.h"
#include "debug_utils.h"
#include "NetCommanProtocol.h"

#define OPTION_ARG_NONE 0
#define OPTION_ARG_INT 1



struct OptionEntry {
	const char * 	name;
	const char *	short_name;
	int				with_arg;
	void *			variable;
	const char *	help;
};

std::string path_to_conf;
int o_timeout = -1;
bool o_slave = false;
bool o_once = false;

static struct OptionEntry entries[] = {
	{"--timeout", "-t", OPTION_ARG_INT, &o_timeout, "Quit after that time"},
	{"--slave", "-s", G_OPTION_ARG_NONE, &o_slave, "Read commands from stdin"},
	{"--once", "-o", G_OPTION_ARG_NONE, &o_once, "Don't loop run once"},
	{NULL}
};

bool check_path(char * path){
	struct stat st;
	if(stat(path,&st) == 0 && S_ISREG(st.st_mode) || !strcmp(path, "-"))
		return true;
    return false;
}

void parse_comand_line(int argc, char *argv[]){
	int i;
	for (i = 1; i < argc;i++)
	{
		struct OptionEntry * arg = entries;
		while (arg->name != NULL)
		{
			if (!strcmp(arg->name, argv[i]) ||  !strcmp(arg->short_name, argv[i]) )
			{
				if(arg->with_arg == OPTION_ARG_INT && i + 1 < argc)
					*(int *)arg->variable = atoi(argv[++i]);
				if(arg->with_arg == OPTION_ARG_NONE)
					*(bool *)arg->variable = true;
				break;
			}else if(check_path(argv[i])){
				path_to_conf.assign(argv[i]);
				break;
			}
			++arg;	
		}
	}
}

bool 
find_server(std::string * ip)
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

bool
net_client(const char * serv_adr)
{
	int sockfd = 0;
	struct sockaddr_in serv_addr; 

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(23162); 
	serv_addr.sin_addr.s_addr = inet_addr(serv_adr);

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       return 1;
    } 
	Creeping creeping;
    for (;;)
    {
		int n = 0;
		char msg_send, msg_get;
		msg_send = MSG_STATE;
		msg_get = MSG_ERR;
		sendto(sockfd,&msg_send,1,0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
		n = recvfrom( sockfd, &msg_get, 1, 0, NULL, NULL);
		if(n<= 0 || msg_get != msg_send)
			return 1;
		n = recvfrom( sockfd, &msg_get, 1, 0, NULL, NULL);
		if(n<= 0)
			return 1;
		
		if(msg_get == MSG_STATE_STOP)
		{
			if(creeping.is_run())
				creeping.stop();
			goto continue_work;
		}
		
		if(msg_get == MSG_STATE_START)
		{
			if(creeping.is_run())
				goto continue_work;
			msg_send = MSG_DATA_SIZE;
			sendto(sockfd,&msg_send,1,0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
			n = recvfrom( sockfd, &msg_get, 1, 0, NULL, NULL);
			if(n<= 0 || msg_get != msg_send)
				goto continue_work;
			int data_size;
			n = recvfrom( sockfd, (char*)&data_size, 4, 0, NULL, NULL);
			if(n<= 0 || msg_get <= 0)
				goto continue_work;
			char data[data_size];
			msg_send = MSG_DATA;
			sendto(sockfd,&msg_send,1,0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
			n = recvfrom( sockfd, &msg_get, 1, 0, NULL, NULL);
			if(n<= 0 || msg_get != msg_send)
				goto continue_work;
			n = recvfrom( sockfd, data, data_size, 0, NULL, NULL);
			if(n<= 0)
				goto continue_work;
			creeping.open_conf(data, data_size);
			creeping.start();
		}
		continue_work:
		sleep(1);
	}
	return 0;
}

void 
start_with_net()
{
	#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	#endif
	
	
	std::string serv_ip;
	find_server(&serv_ip);
	net_client(serv_ip.c_str());
	
	#ifdef _WIN32
	WSACleanup();
	#endif
	DEBUG_PRINT_LINE;
}

void
start_with_cmd()
{
	Creeping creeping;
	if(!path_to_conf.empty())
		creeping.open_conf(path_to_conf.c_str());
	if(o_once)
		creeping.play_once();
	
	
	creeping.start();
	
	if(o_timeout >= 0)
		sleep(o_timeout);
	else
		creeping.join();
	
	creeping.stop();
}

int
main(int argc, char *argv[])
{
	utils_setenv("FONTCONFIG_PATH", "fonts", 1);
	//nice(-10);
	if(argc > 1)
	{
		parse_comand_line(argc,argv);
		start_with_cmd();
	}
	else
	{
		start_with_net();
	}
	
	return 0;
}
