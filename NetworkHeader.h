#ifndef __NETWORKHEADER_H__
#define __NETWORKHEADER_H__

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <netdb.h>
#include <errno.h>      // for errno, EINTR
#include <signal.h>     // for sigaction()
#include <iostream>     // iostream

#define SERVER_HOST "141.166.207.144"  /* mathcs01 IP address */
#define SERVER_PORT "31400"
#define SA struct sockaddr

#define MAXPENDING 5

/* Miscellaneous constants */
#define MAXLINE         4096    /* max text line length */
#define MAXSOCKADDR     128     /* max socket address structure size */
#define BUFFSIZE        8192    /* buffer size for reads and writes */
#define SHORT_BUFFSIZE  100     /* For messages I know are short */

void DieWithError(std::string errorMessage);

int CreateTCPServerSocket(unsigned short port);
int AcceptTCPConnection(int servSock);

unsigned long recvTCPMessage(int sock, char* buffer, char recv_buffer[]);
void sendTCPMessage(int sock, char* buffer, unsigned long bufferLen, int flags);

#endif
