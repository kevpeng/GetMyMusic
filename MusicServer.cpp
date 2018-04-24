#include "NetworkHeader.h"
#include "Data.h"
#include <pthread.h>
#include <time.h>
#include <fstream>
#include <csignal>

using namespace std;

// mutex lock
static const bool DEBUG = true;
static pthread_mutex_t writeToDiskLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t logLock = PTHREAD_MUTEX_INITIALIZER;

static ofstream logFile;

void* ThreadMain(void* args);
void closeSignal(int signum);

void waitFor (unsigned int secs) {
  unsigned int retTime = time(0) + secs;
  while (time(0) < retTime);
}

void logData(string msg) {
  pthread_mutex_lock(&logLock);
  time_t rawtime;
  time(&rawtime);
  cout << ctime(&rawtime) << msg << endl;
  logFile << ctime(&rawtime) << msg << endl;
  pthread_mutex_unlock(&logLock);
}

void HandleTCPClient(int clientSock, string clientAddr) {
  vector<SongFile> serverSongList, hashedServerSongList;
  vector<SongFile> clientSongList, hashedClientSongList;
  vector<SongFile> onlyServer;

  logData("Connect with client " + clientAddr);

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
      logData("Receive a LIST packet from " + clientAddr);
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
      logData("Receive a DIFF packet from " + clientAddr);

      // free old data first
      freeSongFiles(onlyServer);

      deserializeSongList(onlyServer, recv_packet.data, recv_packet.length);
    } 
    else if (recv_packet.type == 2) { // SYNC
      logData("Receive a SYNC packet from " + clientAddr);
      string filesStr = "";

      /* send back data to the client */
      vector<SongFile> filesToSend;
      getSameSongList(filesToSend, onlyServer, serverSongList);
      for (unsigned int i = 0; i < filesToSend.size(); i++) {
        if (i > 0) filesStr += ", ";
        filesStr += filesToSend[i].name;
      }
      logData("Send these files: " + filesStr + " to " + clientAddr);

      ph.type = 2;
      ph.length = serializeSongList(filesToSend, ph.data, false);
      bufferLen = serializePacket(buffer, ph, false);
      sendTCPMessage(clientSock, buffer, bufferLen, 0);

      /* write data from the client to disk (put locks here) */
      pthread_mutex_lock(&writeToDiskLock);

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

      filesStr = "";
      // write the files to disk
      for (unsigned int i = 0; i < filesToWrite.size(); i++) {
        if (i > 0) filesStr += ", ";
        filesStr += filesToWrite[i].name;
        writeSongToDisk("server_dir", filesToWrite[i]);
      } 
      logData("Write these files: " + filesStr + " from " + clientAddr);

      if (DEBUG) {
        // test lock
        waitFor(5);
      }

      // free data for next query from the client
      freeSongFiles(clientSongList);
      freeSongFiles(serverSongList);
      freeSongFiles(hashedClientSongList);
      freeSongFiles(hashedServerSongList);

      logData("Finish writing files from " + clientAddr);

      pthread_mutex_unlock(&writeToDiskLock);
    } 
    else if (recv_packet.type == 3) { // LEAVE
      break;
    }
  }

  free(buffer);
  free(recv_packet.data);
  free(ph.data);
  freeSongFiles(serverSongList);
  freeSongFiles(clientSongList);
  freeSongFiles(onlyServer);

  logData("Client " + clientAddr + " ends connection");
  close(clientSock);
}


struct ThreadArgs {
	int clientSock; // socket descript for client
  string clientAddr;
};


int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Error: Usage MusicServer -p <port>\n");
    exit(1);
  }

  char c;
  unsigned short servPort;

  // open the log file
  logFile.open("log.txt", fstream::out | fstream::app);

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

  // register interrupting signal
  signal(SIGINT, closeSignal);

  for (;;) {
    clientSock = AcceptTCPConnection(servSock, clientAddr);  
		
		// create separate memory for client argument
    threadArgs = new ThreadArgs;
		if(threadArgs == NULL)
			DieWithError("failed to allocate memory for thread");

		threadArgs->clientSock = clientSock;
    string clientAddrStr(inet_ntoa(clientAddr.sin_addr));
    threadArgs->clientAddr = clientAddrStr;


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
	string clntAddr = ((ThreadArgs*)threadArgs)->clientAddr;

  delete (ThreadArgs*)threadArgs;

	HandleTCPClient(clntSock, clntAddr);
  return NULL;
}


// handling interrupting signal
void closeSignal(int signum) {
  cout << "Close the server" << endl;
  logFile.close();
  exit(signum);
}
