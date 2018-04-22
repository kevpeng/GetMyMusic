#include "NetworkHeader.h"
#include "DataHeader.h"

#define MAXPENDING 5

using namespace std;

void HandleTCPClient(int clientSock) {
  char* buffer;
  buffer = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES + 3));
  char recv_buffer[SHORT_BUFFSIZE];
  short recvMsgSize;

  unsigned long idx = 0;

  do {
    cout << "before recv" << endl;
    if ((recvMsgSize = recv(clientSock, recv_buffer, SHORT_BUFFSIZE, 0)) < 0)
      DieWithError("recv() failed");

    for (unsigned short i = 0; i < recvMsgSize; i++) {
      buffer[idx + i] = recv_buffer[i];
    }

    idx += (unsigned short) recvMsgSize;
  } while (recvMsgSize > 0);

  cout << "after do-while" << endl;

  packet_h recv_packet;
  recv_packet.data = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES));
  deserializePacket(buffer, recv_packet);

  packet_h ph;
  ph.version = 0x5;
  ph.type = 0;
  ph.r = 1; // response
  ph.data = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES));

  // cout << "before from disk" << endl;
  // getFilesFromDisk("music_dir_1", ph.data);
  // cout << "after from disk" << endl;
  // unsigned long bufferLen = serializePacket(buffer, ph);
  // cout << "after serialize" << endl;

  // cout << "before send" << endl;

  // if (send(clientSock, buffer, bufferLen, 0) != bufferLen)
  //   DieWithError("send() failed");

  // cout << "sent" << endl;

  free(buffer);
  free(recv_packet.data);
  free(ph.data);
  close(clientSock);
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

    /* clientSock is connected to a client */
    HandleTCPClient(clientSock);
  }
}
