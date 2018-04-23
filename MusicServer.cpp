#include "NetworkHeader.h"
#include "Data.h"
#include <pthread.h>
#define MAXPENDING 5

using namespace std;


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


void HandleTCPClient(int clientSock) {
  vector<SongFile> serverSongList;
  vector<SongFile> clientSongList;

  // for storing an entire packet
  char* buffer;
  buffer = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES + 3));
  unsigned long bufferLen;
  char recv_buffer[SHORT_BUFFSIZE];

  // recv packet
  packet_h recv_packet;
  recv_packet.data = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES));

  // packet for send
  packet_h ph;
  ph.version = 0x5;
  ph.type = 0;
  ph.r = 1; // response
  ph.data = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES));

  while (true) {
    recvTCPMessage(clientSock, buffer, recv_buffer);

    deserializePacket(buffer, recv_packet);

    // read data from disk
    ph.length = getFilesFromDisk("music_dir_1", ph.data);
    deserializeSongList(ph.data, ph.length);

    // serialize hashed data
    bufferLen = serializePacket(buffer, ph, true);

    long sendStatus = send(clientSock, buffer, bufferLen, 0);
    if (sendStatus < 0)
      DieWithError("send() failed");
    if (bufferLen != (unsigned long) sendStatus)
      DieWithError("send() sent a different number of bytes than expected");
  }

  free(buffer);
  free(recv_packet.data);
  free(ph.data);
  close(clientSock);
}

struct ThreadArgs {
	int clientSock; // socket descript for client
};


// for multithreading
void *ThreadMain(void *threadArgs) {
	// Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self());
	// extract socket file descriptor from argument
	int clntSock = ((struct ThreadArgs *)threadArgs)->clientSock;
	free(threadArgs); // deallocate memory for argument 
	HandleTCPClient(clntSock);
	return(NULL);
}



int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Error: Usage MusicServer -p <port>\n");
    exit(1);
  }

  char c;
  unsigned short servPort;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      c = argv[i][1];

      /* found an option, so look at next
       * argument to get the value of 
       * the option */
      switch (c) {
        case 'p':
          servPort = atoi(argv[i+1]);
          break;
        default:
          break;
      }
    }
  }

  int servSock;
  int clientSock;
  pthread_t threadID;
  ThreadArgs 
  
  /* Create socket for incoming connections */
  servSock = CreateTCPServerSocket(servPort);

  for (;;) {
    clientSock = AcceptTCPConnection(servSock);  
		
		// create separate memory for client argument
		ThreadArgs * threadArgs = (ThreadArgs *) malloc(sizeof(ThreadArgs));
		if(threadArgs == NULL)
			DieWithError((char*)"malloc() failed");

		threadArgs->clientSock = clientSock;

		// Create client thread
		int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
		if(returnValue != 0)
			DieWithError((char*)"pthread_create() failed");

    cout << "Here" << endl;
    /* clientSock is connected to a client */
    //HandleTCPClient(clientSock);
  }
}
