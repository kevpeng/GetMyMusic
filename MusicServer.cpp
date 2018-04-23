#include "NetworkHeader.h"
#include "Data.h"
#include <pthread.h>

using namespace std;

void* ThreadMain(void* args);

void HandleTCPClient(int clientSock, char* clientAddr) {
  vector<SongFile> serverSongList, hashedServerSongList;
  vector<SongFile> clientSongList, hashedClientSongList;
  vector<SongFile> onlyServer;

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
    bufferLen = recvTCPMessage(clientSock, buffer, recv_buffer);
    if (bufferLen == 0)
      break;

    deserializePacket(buffer, recv_packet);

    if (recv_packet.type == 0) { // LIST
      cout << "Receive a LIST packet from " << clientAddr << endl;
      // free old data first
      freeSongFiles(serverSongList);

      // read data from disk
      ph.length = getFilesFromDisk("server_dir", ph.data);
      deserializeSongList(serverSongList, ph.data, ph.length);

      // serialize hashed data
      bufferLen = serializePacket(buffer, ph, true);

      sendTCPMessage(clientSock, buffer, bufferLen, 0);
    } 
    else if (recv_packet.type == 1) { // DIFF
      cout << "Receive a DIFF packet from " << clientAddr << endl;
      // free old data first
      freeSongFiles(onlyServer);

      deserializeSongList(onlyServer, recv_packet.data, recv_packet.length);

      for (unsigned int i = 0; i < onlyServer.size(); i++) {
        cout << onlyServer[i].name << endl;
      }
    } 
    else if (recv_packet.type == 2) { // SYNC
      cout << "Receive a SYNC packet from " << clientAddr << endl;

      /* send back data to the client */
      vector<SongFile> filesToSend;
      getSameSongList(filesToSend, onlyServer, serverSongList);
      cout << "\nSend these files to the client:" << endl;
      for (unsigned int i = 0; i < filesToSend.size(); i++) {
        cout << filesToSend[i].name << endl;
      }
      ph.type = 2;
      ph.length = serializeSongList(filesToSend, ph.data, false);
      bufferLen = serializePacket(buffer, ph, false);
      sendTCPMessage(clientSock, buffer, bufferLen, 0);

      /* write data from the client to disk (put locks here) */
      // free old data first 
      freeSongFiles(onlyServer);
      freeSongFiles(clientSongList);
      freeSongFiles(serverSongList);

      // read data from disk
      ph.length = getFilesFromDisk("server_dir", ph.data);
      deserializeSongList(serverSongList, ph.data, ph.length);
  
      // get client data
      deserializeSongList(clientSongList, recv_packet.data, recv_packet.length);

      // hash content
      hashSongList(serverSongList, hashedServerSongList);
      hashSongList(clientSongList, hashedClientSongList);

      // get up-to-date difference
      vector<SongFile> diff;
      // file the client has but the server does not
      getDiff(diff, hashedServerSongList, hashedClientSongList);

      vector<SongFile> filesToWrite;
      getSameSongList(filesToWrite, diff, clientSongList);

      // write the files to disk
      for (unsigned int i = 0; i < filesToWrite.size(); i++) {
        writeSongToDisk("server_dir", filesToWrite[i]);
      } 

      // free data for next query from the client
      freeSongFiles(clientSongList);
      freeSongFiles(serverSongList);
      freeSongFiles(hashedClientSongList);
      freeSongFiles(hashedServerSongList);

      cout << "Finish writing files from " << clientAddr << endl;
    }
  }

  free(buffer);
  free(recv_packet.data);
  free(ph.data);
  freeSongFiles(serverSongList);
  freeSongFiles(clientSongList);
  freeSongFiles(onlyServer);

  close(clientSock);
}


struct ThreadArgs {
	int clientSock; // socket descript for client
  char* clientAddr;
};


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
  sockaddr_in clientAddr;
  pthread_t threadID;
  ThreadArgs* threadArgs;
  
  /* Create socket for incoming connections */
  servSock = CreateTCPServerSocket(servPort);

  for (;;) {
    clientSock = AcceptTCPConnection(servSock, clientAddr);  
		
		// create separate memory for client argument
		if((threadArgs = (ThreadArgs*) malloc(sizeof(ThreadArgs))) == NULL)
			DieWithError((char*)"malloc() failed");

		threadArgs->clientSock = clientSock;
    threadArgs->clientAddr = inet_ntoa(clientAddr.sin_addr);


		// Create client thread
		if(pthread_create(&threadID, NULL, ThreadMain, (void*)threadArgs) != 0)
			DieWithError("pthread_create() failed");
  }
}


// for multithreading
void* ThreadMain(void* threadArgs) {
	// Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self());
	// extract socket file descriptor from argument
	int clntSock = ((ThreadArgs*)threadArgs)->clientSock;
	char* clntAddr = ((ThreadArgs*)threadArgs)->clientAddr;
	free(threadArgs); // deallocate memory for argument 

	HandleTCPClient(clntSock, clntAddr);
  return NULL;
}
