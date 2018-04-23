#include "NetworkHeader.h"
#include "Data.h"

using namespace std;

void DieWithError(string errorMessage) { 
  cerr << errorMessage << endl; 
  exit(1); 
}

unsigned long recvTCPMessage(int sock, char* buffer, char recv_buffer[]) {
  int recvMsgSize;
  unsigned long idx = 0;
  bool getLengthField = false;
  unsigned long dataLength = 0;

  while (true) {
    if ((recvMsgSize = recv(sock, recv_buffer, SHORT_BUFFSIZE, 0)) < 0)
      DieWithError("recv() failed");

    if (recvMsgSize == 0) break;
  
    // now we know it is > 0
    unsigned int unsignedMsgSize = (unsigned int) recvMsgSize;

    for (unsigned int i = 0; i < unsignedMsgSize; i++) {
      buffer[idx + i] = recv_buffer[i];
    }
    idx += unsignedMsgSize;

    // check if we have received all the header fields (except data)
    if (idx >= HEADER_BYTES) {
      // get the length of the data
      memcpy(&dataLength, buffer + 1, LENGTH_BYTES);
      getLengthField = true;
    }
    if (getLengthField && idx == dataLength + HEADER_BYTES)
      break;
  } 

  return idx;
}


void sendTCPMessage(int sock, char* buffer, unsigned long bufferLen, int flags) {
  ssize_t sendStatus = send(sock, buffer, bufferLen, flags);
  if (sendStatus < 0)
    DieWithError("send() failed");
  
  if ((unsigned long) sendStatus != bufferLen)
    DieWithError("send() sent a different number of bytes than expected");
}


int CreateTCPServerSocket(unsigned short port) { 
  int sock;                        /* socket to create */ 
  struct sockaddr_in echoServAddr; /* Local address */ 

  /* Create socket for incoming connections */ 
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
    DieWithError("socket() failed"); 

  /* Construct local address structure */ 
  memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */ 
  echoServAddr.sin_family = AF_INET;                /* Internet address family */ 
  echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */ 
  echoServAddr.sin_port = htons(port);              /* Local port */ 

  /* Bind to the local address */ 
  if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) 
    DieWithError("bind() failed"); 

  /* Mark the socket so it will listen for incoming connections */ 
  if (listen(sock, MAXPENDING) < 0) 
    DieWithError("listen() failed"); 

  return sock; 
}


int AcceptTCPConnection(int servSock) {
  int clntSock;                    /* Socket descriptor for client */
  struct sockaddr_in echoClntAddr; /* Client address */
  unsigned int clntLen;            /* Length of client address data structure */

  /* Set the size of the in-out parameter */
  clntLen = sizeof(echoClntAddr);

  /* Wait for a client to connect */
  if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
          &clntLen)) < 0)
    DieWithError("accept() failed");

  /* clntSock is connected to a client! */

  printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

  return clntSock;
}

