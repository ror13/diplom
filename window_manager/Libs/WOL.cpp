#include "stdafx.h"

#include "WOL.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  #define USE_WINSOCKAPI
  #define delay Sleep
#include <winsock.h>
#define close(s) { \
closesocket(s); \
WSACleanup(); \
}

unsigned int port = 60000;
unsigned long bcast = 0xffffffff;

int in_ether (LPCTSTR bufp, BYTE *addr) 
{
    TCHAR c;
	LPCTSTR orig;
    int i;
    BYTE *ptr = addr;
    unsigned val;
	
    i = 0;
    orig = bufp;
    while ((*bufp != '\0') && (i < 6)) {
        val = 0;
        c = *bufp++;
        if (isdigit(c))
            val = c - '0';
        else if (c >= 'a' && c <= 'f')
            val = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            val = c - 'A' + 10;
        else {
#ifdef DEBUG
            //fprintf (stderr, "\rin_ether(%s): invalid ether address!\n", orig);
#endif
            errno = EINVAL;
            return (-1);
        }
        val <<= 4;
        c = *bufp;
        if (isdigit(c))
            val |= c - '0';
        else if (c >= 'a' && c <= 'f')
            val |= c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            val |= c - 'A' + 10;
        else if (c == ':' || c == 0)
            val >>= 4;
        else {
#ifdef DEBUG
            //fprintf (stderr, "\rin_ether(%s): invalid ether address!\n", orig);
#endif
            errno = EINVAL;
            return (-1);
        }
        if (c != 0)
            bufp++;
        *ptr++ = (BYTE) (val & 0377);
        i++;
		
        /* We might get a semicolon here - not required. */
        if (*bufp == ':') {
            if (i == 6) {
                ;           /* nothing */
            }
            bufp++;
        }
    }
    if (bufp - orig != 17) {
        return (-1);
    } else {
        return (0);
    }
} /* in_ether */


int send_wol (LPCTSTR dest, LPCTSTR host) 
{
    int i, j;
    int packet;
    struct sockaddr_in sap;
    BYTE ethaddr[8];
    BYTE *ptr;
    BYTE buf [128];
    unsigned long bc;

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    int optval = 1;

	bc = bcast;
	
    /* Fetch the hardware address. */
    if (in_ether (dest, ethaddr) < 0) 
	{
        //fprintf (stderr, "\r%s: invalid hardware address\n", progname);
        return (-1);
    }
	
    /* I would like to have Socket Vers. 1.1 */
    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup (wVersionRequested, &wsaData);
    if (err != 0) 
	{
        //fprintf (stderr, "\r%s: couldn't init Winsock Version 1.1\n", progname);
        WSACleanup ();
        return (-1);
    }
	
    /* setup the packet socket */
    if ((packet = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
	{
        //fprintf (stderr, "\r%s: socket failed\n", progname);
        WSACleanup ();
        return (-1);
    }
	
    /* Set socket options */
    if (setsockopt (packet, SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof (optval)) < 0) 
	{
        //fprintf (stderr, "\r%s: setsocket failed %s\n", progname, strerror (errno));
        close (packet);
        return (-1);
    }
	
    /* Set up broadcast address */
    sap.sin_family = AF_INET;
    sap.sin_addr.s_addr = bc;                 /* broadcast address */
    sap.sin_port = htons(port);
	
    /* Build the message to send - 6 x 0xff then 16 x dest address */
    ptr = buf;
    for (i = 0; i < 6; i++)
        *ptr++ = 0xff;
    for (j = 0; j < 16; j++)
        for (i = 0; i < 6; i++)
            *ptr++ = ethaddr [i];
		
	/* Send the packet out */
	if (sendto (packet, (char *)buf, 102, 0, (struct sockaddr *)&sap, sizeof (sap)) < 0) 
	{
		//fprintf (stderr, "\r%s: sendto failed, %s\n", progname, strerror(errno));
		close (packet);
		return (-1);
	}
	close (packet);
	
	return (0);
}

