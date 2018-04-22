#include "NetworkHeader.h"
#include "Data.h"
#include <pthread.h>
#define MAXPENDING 5

using namespace std;

void HandleTCPClient(int clientSock) {
  char* buffer;
  buffer = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES + 3));
  char recv_buffer[SHORT_BUFFSIZE];

  recvTCPMessage(clientSock, buffer, recv_buffer);

  // recv packet
  packet_h recv_packet;
  recv_packet.data = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES));
  deserializePacket(buffer, recv_packet);

  cout << recv_packet.version << endl;
  cout << recv_packet.type << endl;
  cout << recv_packet.r << endl;
  cout << recv_packet.length << endl;

  // packet to send
  packet_h ph;
  ph.version = 0x5;
  ph.type = 0;
  ph.r = 1; // response
  ph.data = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES));

  ph.length = getFilesFromDisk("music_dir_1", ph.data);
  unsigned long bufferLen = serializePacket(buffer, ph, true);

  long sendStatus = send(clientSock, buffer, bufferLen, 0);
  if (sendStatus < 0)
    DieWithError("send() failed");
  if (bufferLen != (unsigned long) sendStatus)
    DieWithError("send() sent a different number of bytes than expected");

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
  unsigned int clientLen;
  struct sockaddr_in servAddr;
  struct sockaddr_in clientAddr;
  
  /* Create socket for incoming connections */
  if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
    DieWithError("socket() failed");

  /* Construct local address structure */
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(servPort);

  /* Bind to the local address */
  if (::bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) 
    DieWithError("bind() failed");

  /* Mark the socket so it will listen for incoming connections */
  if (listen(servSock, MAXPENDING) < 0)
    DieWithError("listen() failed");

  for (;;) {
    clientLen = sizeof(clientAddr);
    
    /* Wait for client to connect */
    if ((clientSock = accept(servSock, (struct sockaddr *) &clientAddr, &clientLen)) < 0)
      DieWithError("accept() failed");
		
    // clntSock connected to client!!!
		char clntName[INET_ADDRSTRLEN];         // string to contain client address
    if(inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
      printf("Handling client %s/%d\n", clntName, ntohs(clientAddr.sin_port));
    else
      puts("Unable to get client address");

		// create separate memory for client argument
		struct ThreadArgs * threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
		if(threadArgs == NULL)
			DieWithError((char*)"malloc() failed");

		threadArgs->clientSock = clientSock;

		// Create client thread
		pthread_t threadID;
		int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
		if(returnValue != 0)
			DieWithError((char*)"pthread_create() failed");


    /* clientSock is connected to a client */
    //HandleTCPClient(clientSock);
  }
}
