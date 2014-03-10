// ------------------------------------------------------------
// NetSrv.cpp - network server for Applica Server project - TV.
// - Implementation file: 
// -- Find Server functionality --
// ------------------------------------------------------------

#define STRICT

#include "StdAfx.h"
#include "NetSrv.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include "Serial.h"
#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Wspiapi.h>
#else
#include <sys/socket.h>
#include <resolv.h>
#endif

#include "..\Inc\product.h"
#include "..\Presenter.Engine\Commands.h"

#define NETWORK_SERVER_PORT 13325
#define CONNECT_TYPE_TRIGGER 5

enum { EOF_Char = 27 };

#ifdef _DEBUG
#define DEBUG_OUTPUT(x) OutputDebugStringA(x)
#else
#define DEBUG_OUTPUT(x) 
#endif

#pragma pack(push,1)
typedef struct message_header{
	char prefix[2];
	unsigned int packet_length;
	unsigned int connect_type;
	int station_id;
	unsigned int command;
	unsigned int data_length;
} message_header;
#pragma pack(pop)

char debugstring[2480];

int ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Listener"), MB_ICONSTOP|MB_OK);
	return 1;
}

TCHAR* GetIniFolder()
{
	TCHAR szExe[MAX_PATH] = _T("");
	GetModuleFileName(NULL, szExe, MAX_PATH);
	LPTSTR p = _tcsrchr( szExe, _T('\\') );
	if( p != NULL )
	{
		p[1] = _T('\0');
	}
	return _tcsdup(szExe);
}

LPSTR StringToUTF8(LPTSTR strString)
{
	USES_CONVERSION;
	int buf_size = WideCharToMultiByte(CP_UTF8, 0, T2W(strString), -1,
		NULL, 0, NULL, NULL );
	
	LPSTR pBuf = (char *)malloc(buf_size + 1);
	
	DWORD size = 0;
	size = WideCharToMultiByte(CP_UTF8, 0, T2W(strString), -1,
		pBuf, buf_size, NULL, NULL );
	
	if(!size)
	{
		free(pBuf);
		return NULL;
	}

	return pBuf;
}

char * find_signage_server_local(void)
{
	struct sockaddr_in adr;
	SOCKET sock;
	long bBroadcast = 1;
	int res = 0;
	char obuf[1024];
	char inbuf[1024];
	int bytes;
	int count = 5;
	struct sockaddr_in adrServer;
	int rbytes = sizeof(adrServer);

	fd_set rfds;
	struct timeval tv;

	char *product_version = StringToUTF8(CURRENT_PROTOCOL_VERSION);

	bytes = sprintf(obuf, "REQUEST = FindServer\nPRODUCT = %s\nPROTOCOL = %s\n", PRODUCT_NAME, product_version);
	free(product_version);

	DEBUG_OUTPUT(obuf);

	memset(&adrServer, 0, sizeof(adrServer));
	memset(&adr, 0, sizeof(adr));
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1)
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == -1)
		{
			printf("Can't create socket.\n");
			return NULL;
		}
	}

	res = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&bBroadcast, sizeof(bBroadcast));
	if (res != 0)
	{
		DEBUG_OUTPUT("Can't set socket to BROADCAST type.\n");
	}

	adr.sin_family = AF_INET;
	adr.sin_port = htons(NETWORK_SERVER_PORT);
	adr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	while (count-- > 0) {
		res = sendto(sock, obuf, bytes, 0, (struct sockaddr *)&adr, sizeof(adr));
		if (res == -1) {
			sprintf(debugstring, "Send error: %s\n", strerror(errno));
			DEBUG_OUTPUT(debugstring);
			continue;
		}
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		res = select(sock+1, &rfds, NULL, NULL, &tv);
		if(res > 0) {
			memset(inbuf, 0, sizeof(inbuf));
			res = recvfrom(sock, inbuf, 1024, 0, (struct sockaddr *)&adrServer, &rbytes);
			closesocket(sock);
			if (res != -1) {
				/* good
				 * check */
				if (strstr(inbuf, "PROTOCOL") == 0) {
					DEBUG_OUTPUT("[find_signage_server_local] protocol error\n");
					return NULL;
				}
				sprintf(debugstring, "[find_signage_server_local] server %s\n", inet_ntoa(adrServer.sin_addr));
				DEBUG_OUTPUT(debugstring);
				return strdup(inet_ntoa(adrServer.sin_addr));
			} else {
				DEBUG_OUTPUT("[find_signage_server_local] recvfrom error!\n");
				return NULL;
			}
		}
	}
	DEBUG_OUTPUT("[find_signage_server_local] Server not found!");
	closesocket(sock);
	return NULL;
}

DWORD passTriggerMessage(TCHAR *user_name, TCHAR *user_pass, char *data, int len)
{
	int sockfd;
    struct sockaddr_in dest;
    char *buffer;
    unsigned int i;
    message_header header;
	message_header recv_hdr;
	struct hostent *hp;

	int ret;
	char *user = NULL;
	char *password = NULL;
	char *product_version = NULL;

	char *server = NULL;

	const char *login_format = "UserName = %s\nPassword = %s\nHostname = host\nProtocol=%s";

	if (!user_name || !user_pass) 
	{
		printf("ATTENTION! You have to provide information about user and password\n");
		return -1;
	}

	if (!data) 
	{
		printf("ATTENTION! You have to provide data information you want to send\n");
		return -1;
	}

	if(!server) 
	{
		server = find_signage_server_local();
		if (!server) 
		{
			sprintf(debugstring, "Unable to find %s Server!\n", PRODUCT_NAME);
			DEBUG_OUTPUT(debugstring);
			return -1;
		}
	}

	/*---Open socket for streaming---*/
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		DEBUG_OUTPUT("Socket");
		return -1;
	}
	
	/*---Initialize server address/port struct---*/
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(NETWORK_SERVER_PORT);
			
	hp = gethostbyname( server );
	if( hp == NULL ) 
	{
		DEBUG_OUTPUT("gethostbyname");
		return -1;
	}

	memcpy(&dest.sin_addr, hp->h_addr_list[0], hp->h_length);

	/*---Connect to server---*/
	if ( connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) != 0 )
	{
		DEBUG_OUTPUT("Connect ");
		exit(errno);
	}

	/*---Get "Hello?"---*/
	header.prefix[0] = '#';
	header.prefix[1] = '#';
	user = StringToUTF8(user_name);
	password = StringToUTF8(user_pass);
	product_version = StringToUTF8(CURRENT_PROTOCOL_VERSION);

	buffer = (char *)malloc(strlen(login_format) + strlen(user) + strlen(password) + strlen(product_version));
	sprintf(buffer, login_format, user, password, product_version);
	header.packet_length = strlen(buffer) + 1 + sizeof(header) + 2;
	header.connect_type = CONNECT_TYPE_TRIGGER;
	header.station_id = 0;
	header.command = MESSAGE_TYPE_LOGIN_INFO;
	header.data_length = header.packet_length - sizeof(header) - 2;

	send(sockfd, (const char *)&header, sizeof(header), 0);
	send(sockfd, buffer, strlen(buffer) + 1, 0);
	send(sockfd, "$$", 2, 0);

	free(buffer);
	free(user);
	free(password);
	free(product_version);

	i =0;
	while (i != sizeof(recv_hdr)) 
	{
		ret = recv(sockfd, ((char *)&recv_hdr) + i, sizeof(recv_hdr) - i, 0);
		if (ret <= 0) 
		{
			DEBUG_OUTPUT("Error while recieving answer\n");
			return -1;
		}
		
		i += ret;
	}

	buffer = (char *)malloc(recv_hdr.data_length + 2);
			
	if(recv_hdr.prefix[0] != '#' || recv_hdr.prefix[1] != '#') 
	{
		DEBUG_OUTPUT("Invalid server\n");
		return -1;
	}

	if(recv_hdr.command != MESSAGE_TYPE_LOGIN_SUCCESS) 
	{
		DEBUG_OUTPUT("Login failed\n");
		closesocket(sockfd);
		return -1;
	}

	i =0;
	while (i != recv_hdr.data_length + 2) 
	{
		ret = recv(sockfd, buffer + i, recv_hdr.data_length + 2 - i, 0);
		if (ret <= 0) 
		{
			DEBUG_OUTPUT("Error while recieving answer\n");
			return -1;
		}
		i += ret;
	}

	free(buffer);

	header.prefix[0] = '#';
	header.prefix[1] = '#';
	header.packet_length = len + sizeof(header) + 2;
	header.connect_type = CONNECT_TYPE_TRIGGER;
	header.station_id = 0;
	header.command = MESSAGE_TYPE_TRIGGER_EVENT;
	header.data_length = header.packet_length - sizeof(header) - 2;

	send(sockfd, (const char *)&header, sizeof(header), 0);
	send(sockfd, data, len, 0);
	send(sockfd, "$$", 2, 0);

	/*---Clean up---*/
	closesocket(sockfd);
	return 0;
}

DWORD WINAPI asServerMainThread(LPVOID lpParameter)
{
	CSerial serial;
	LONG    lLastError = ERROR_SUCCESS;

	DWORD dwBytesRead = 0;
	int data_len = MAX_PATH;
	char *data = (char *)malloc(data_len);
	char symbol;
	bool start_flag = FALSE;
	int len = 0;

	DEBUG_OUTPUT("\n### Debug start\n");

	TCHAR *folder = GetIniFolder();
	TCHAR *init_path = (TCHAR *)malloc((_tcsclen(folder) + _tcsclen(_T("comm_configuration.ini")) + 1)*sizeof(TCHAR));
	_stprintf(init_path, _T("%s%s"), folder, _T("comm_configuration.ini"));
	free(folder);

	TCHAR user_name[256];
	TCHAR user_pass[256];
	TCHAR comm[16];
	
	GetPrivateProfileString(_T("Connection"), _T("UserName"), _T(""), user_name, sizeof(user_name), init_path);
	GetPrivateProfileString(_T("Connection"), _T("UserPass"), _T(""), user_pass, sizeof(user_pass), init_path);
	GetPrivateProfileString(_T("CommPort Configuration"), _T("CommPort"), _T("COM1"), comm, sizeof(comm), init_path);
	
	int baud = GetPrivateProfileInt(_T("CommPort Configuration"), _T("BaudRate"), 9600, init_path);
	int byte_size = GetPrivateProfileInt(_T("CommPort Configuration"), _T("ByteSize"), 8, init_path);
	int parity = GetPrivateProfileInt(_T("CommPort Configuration"), _T("Parity"), 0, init_path);
	int stopbits = GetPrivateProfileInt(_T("CommPort Configuration"), _T("StopBits"), 0, init_path);
	int command_start = GetPrivateProfileInt(_T("CommPort Configuration"), _T("CommandStart"), 47, init_path);
	int command_stop = GetPrivateProfileInt(_T("CommPort Configuration"), _T("CommandStop"), 13, init_path);

	CSerial::EBaudrate eBaudrate;
	CSerial::EDataBits eDataBits;
	CSerial::EParity eParity;
	CSerial::EStopBits eStopBits;

	switch(baud) 
	{
	case 110:
		eBaudrate = CSerial::EBaud110;
		break; 
	case 300:
		eBaudrate = CSerial::EBaud300;
		break;
	case 600:
		eBaudrate = CSerial::EBaud600;
		break;
	case 1200:
		eBaudrate = CSerial::EBaud1200;
		break;
	case 2400:
		eBaudrate = CSerial::EBaud2400;
		break;
	case 4800:
		eBaudrate = CSerial::EBaud4800;
		break;
	case 9600:
		eBaudrate = CSerial::EBaud9600;
		break;
	case 14400:
		eBaudrate = CSerial::EBaud14400;
		break;
	case 19200:
		eBaudrate = CSerial::EBaud19200;
		break;
	case 38400:
		eBaudrate = CSerial::EBaud38400;
		break;
	case 56000:
		eBaudrate = CSerial::EBaud56000;
		break;
	case 57600:
		eBaudrate = CSerial::EBaud57600;
		break;
	case 115200:
		eBaudrate = CSerial::EBaud115200;
		break;
	case 128000:
		eBaudrate = CSerial::EBaud128000;
		break;
	case 256000:
		eBaudrate = CSerial::EBaud256000;
		break;
	default:
		DEBUG_OUTPUT("Invalid Baudrate\n");
		return -1;
	}

	switch(byte_size)
	{
	case -1:
		eDataBits = CSerial::EDataUnknown;
		break;
	case 5:
		eDataBits = CSerial::EData5;
		break;
	case 6:
		eDataBits = CSerial::EData6;
		break;
	case 7:
		eDataBits = CSerial::EData7;
		break;
	case 8:
		eDataBits = CSerial::EData8;
		break;
	default:
		DEBUG_OUTPUT("Invalid eDataBits\n");
		return -1;
	}

	switch(parity)
	{
	case -1:
		eParity = CSerial::EParUnknown;
		break;
	case 0:
		eParity = CSerial::EParNone;
		break;
	case 1:
		eParity = CSerial::EParOdd;
		break;
	case 2:
		eParity = CSerial::EParEven;
		break;
	case 3:
		eParity = CSerial::EParMark;
		break;
	case 4:
		eParity = CSerial::EParSpace;
		break;
	default:
		DEBUG_OUTPUT("Invalid eParity\n");
		return -1;
	}

	switch(stopbits)
	{
		case -1:
			eStopBits = CSerial::EStopUnknown;
			break;
		case 0:
			eStopBits = CSerial::EStop1;
			break;
		case 1:
			eStopBits = CSerial::EStop1_5;
			break;
		case 2:
			eStopBits = CSerial::EStop2;
			break;
		default:
			DEBUG_OUTPUT("Invalid eStopBits\n");
			return -1;
	}

    // Attempt to open the serial port (COM1)
    lLastError = serial.Open(comm,0,0,false);
	if (lLastError != ERROR_SUCCESS) {
		DEBUG_OUTPUT("Unable to open COM-port\n");
		return ::ShowError(serial.GetLastError(), _T("Unable to open COM-port"));
	}

    // Setup the serial port (9600,8N1, which is the default setting)
    lLastError = serial.Setup(eBaudrate, eDataBits, eParity, eStopBits);

	if (lLastError != ERROR_SUCCESS) {
		DEBUG_OUTPUT("Unable to set COM-port setting\n");
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port setting"));
	}

    // Register only for the receive event
    lLastError = serial.SetMask(CSerial::EEventBreak |
								CSerial::EEventCTS   |
								CSerial::EEventDSR   |
								CSerial::EEventError |
								CSerial::EEventRing  |
								CSerial::EEventRLSD  |
								CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS)
	{
		DEBUG_OUTPUT("Unable to set COM-port event mask\n");
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port event mask"));
	}

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
    lLastError = serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		DEBUG_OUTPUT("Unable to set COM-port read timeout.\n");
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port read timeout."));
	}

    // Keep reading data, until an EOF (CTRL-Z) has been received
	do
	{
		// Wait for an event
		lLastError = serial.WaitEvent();
		if (lLastError != ERROR_SUCCESS)
		{
			DEBUG_OUTPUT("Unable to wait for a COM-port event.\n");
			return ::ShowError(serial.GetLastError(), _T("Unable to wait for a COM-port event."));
		}

		// Save event
		const CSerial::EEvent eEvent = serial.GetEventType();

		// Handle break event
		if (eEvent & CSerial::EEventBreak)
		{
			DEBUG_OUTPUT("\n### BREAK received ###\n");
		}

		// Handle CTS event
		if (eEvent & CSerial::EEventCTS)
		{
			DEBUG_OUTPUT("EEventCTS\n");
			//DEBUG_OUTPUT("\n### Clear to send %s ###\n", serial.GetCTS()?"on":"off");
		}

		// Handle DSR event
		if (eEvent & CSerial::EEventDSR)
		{
			DEBUG_OUTPUT("EEventDSR\n");
			//DEBUG_OUTPUT("\n### Data set ready %s ###\n", serial.GetDSR()?"on":"off");
		}

		// Handle error event
		if (eEvent & CSerial::EEventError)
		{
			DEBUG_OUTPUT("\n### ERROR: ");
			switch (serial.GetError())
			{
			case CSerial::EErrorBreak:		DEBUG_OUTPUT("Break condition");			break;
			case CSerial::EErrorFrame:		DEBUG_OUTPUT("Framing error");			break;
			case CSerial::EErrorIOE:		DEBUG_OUTPUT("IO device error");			break;
			case CSerial::EErrorMode:		DEBUG_OUTPUT("Unsupported mode");			break;
			case CSerial::EErrorOverrun:	DEBUG_OUTPUT("Buffer overrun");			break;
			case CSerial::EErrorRxOver:		DEBUG_OUTPUT("Input buffer overflow");	break;
			case CSerial::EErrorParity:		DEBUG_OUTPUT("Input parity error");		break;
			case CSerial::EErrorTxFull:		DEBUG_OUTPUT("Output buffer full");		break;
			default:						DEBUG_OUTPUT("Unknown");					break;
			}
			DEBUG_OUTPUT(" ###\n");
		}

		// Handle ring event
		if (eEvent & CSerial::EEventRing)
		{
			DEBUG_OUTPUT("\n### RING ###\n");
		}

		// Handle RLSD/CD event
		if (eEvent & CSerial::EEventRLSD)
		{
			DEBUG_OUTPUT("EEventRLSD\n");
			//fprintf(dbg_stream, "\n### RLSD/CD %s ###\n", serial.GetRLSD()?"on":"off");
		}

		// Handle data receive event
		if (eEvent & CSerial::EEventRecv)
		{
			// Read data, until there is nothing left
			DEBUG_OUTPUT("\n### EEventRecv\n");

			do
			{
				// Read data from the COM-port
				lLastError = serial.Read(&symbol, 1,&dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
				{
					free(data);
					return ::ShowError(serial.GetLastError(), _T("Unable to read from COM-port."));
				}

				if (dwBytesRead > 0)
				{
					sprintf(debugstring, "char = '%u', charsym = '%c'\n", (unsigned char)symbol, symbol);
					DEBUG_OUTPUT(debugstring);

					if (start_flag) 
					{
						DEBUG_OUTPUT("Start flag set\n");
						if (symbol == command_stop)
						{
							DWORD pdwWritten;

							DEBUG_OUTPUT("Stop byte, we set now and send trigger message\n");
							passTriggerMessage(user_name, user_pass, data, len);
							start_flag = FALSE;
							len = 0;
							if (serial.Write("\r\n", &pdwWritten) != ERROR_SUCCESS)
							{
								return ::ShowError(serial.GetLastError(), _T("Unable to send data to COM-port"));
							}
						}
						else
						{
							DEBUG_OUTPUT("Stop byte set\n");
							if (len == data_len)
							{
								data_len = data_len + MAX_PATH;
								data = (char *)realloc(data, data_len);
							}
							data[len++] = symbol;
						}
					} 
					else 
					{
						DEBUG_OUTPUT("Stop flag set\n");
						if (symbol == command_start)
						{
							DEBUG_OUTPUT("We set start byte\n");
							start_flag = TRUE;
						}
					}
				}
				else if (dwBytesRead == 0)
				{
					break;
				}
			}
			while (1);
		}
	} 
	while(1);
	free(data);
	free(user_name);
	free(user_pass);

	// Close the port again
	serial.Close();
    return 0;
}

