#include "NetworkHeader.h"
#include "Data.h"
#include "Hasher.h"

#include <set>

using namespace std; 

int main(int argc, char *argv[]) {
  char* serverIP = (char *)SERVER_HOST; // server host from the NetworkHeader.h
  unsigned short serverPort = atoi(SERVER_PORT); // port from NetworkHeader.h
  string dir_name = "client_dir";

  if(argc < 2) {
    cout << "Error: Usage MusicClient [-h <serverIP>] [-p <serverPort>] [-d <directory>]" << endl;
    return(1);
  }

  for(int i = 1; i < argc; ++i) { // parse in the IP and port number
    if(argv[i][0] == '-') {
      char c = argv[i][1];
      switch (c) {
        case 'h': // host
          serverIP = argv[i+1];
          break;
        case 'p': // port number
          serverPort = atoi(argv[i+1]);
          break;
        case 'd': // directory
          dir_name = argv[i+1];
          break;
        default:
          break;
      }
    }
  }


  hostent* remoteHost;


  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sock < 0)
    DieWithError("socket() failed");

  // construct the server address structure
  struct sockaddr_in servAddr;						// server address
  memset(&servAddr, 0, sizeof(servAddr));	// zero out structure
  servAddr.sin_family = AF_INET;					// ip4v squadfam
  servAddr.sin_port = htons(serverPort); // server port
  //servAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());			// serverIP

  // Handles both IP and name address input
  if((remoteHost = gethostbyname(serverIP)) != NULL) {  // If host is domain name
    servAddr.sin_addr.s_addr =  *((unsigned long *) remoteHost->h_addr_list[0]);
  }
  else { // Host address is address
    unsigned int addr = inet_addr(serverIP);   // converts format of address to binary
    remoteHost = gethostbyaddr((char *)&addr, 4, AF_INET);
    servAddr.sin_addr.s_addr = addr;  // Internet Address 32 bits
  }

  // establish TCP connection with server
  if(connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr))< 0)
    DieWithError("connect() failed");

  // connection is successful
  cout << "Connection was successful." << endl;	

  // string holder for sending data
  char* buffer;
  buffer = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES + 3));
  unsigned long bufferLen;

  // packet holder for sending data
  packet_h ph;
  ph.version = 0x5;
  ph.r = 0; // request
  ph.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);

  // for recving packet
  char recv_buffer[SHORT_BUFFSIZE];
  packet_h recv_packet;
  recv_packet.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);

  vector<SongFile> serverSongList, clientSongList, hashedServerSongList, hashedClientSongList;
  // files that only the server and the client have respectively
  vector<SongFile> onlyServer, onlyClient; 

  // ask for input after connection is established.
  string commandInput = "";
  string s = "WAIT"; 

  while(s != "LEAVE") {
    if (s == "WAIT") {
      cout << "\nenter LIST (display the list of files currently stored on the server) ";
      cout << "or LEAVE (end the connection)" << endl;
      cin >> commandInput;
      if (commandInput != "LIST" && commandInput != "LEAVE") {
        continue;
      } 
      else s = commandInput;
    }

    if (s == "LIST") {
      // free old data first
      freeSongFiles(hashedServerSongList);

      ph.type = 0;
      ph.length = 0;
      bufferLen = serializePacket(buffer, ph, false);

      sendTCPMessage(sock, buffer, bufferLen, 0);

      // get a list of files from the server
      recvTCPMessage(sock, buffer, recv_buffer);
      deserializePacket(buffer, recv_packet); 
      deserializeSongList(hashedServerSongList, recv_packet.data, recv_packet.length);
      cout << "Current song files on the server:" << endl;
      for (unsigned int i = 0; i < hashedServerSongList.size(); i++)
        cout << hashedServerSongList[i].name << endl;

      cout << "\nDo you want to get the difference in files between you and the server? (YES/NO)" << endl;
      cin >> commandInput;
      if (commandInput == "YES") s = "DIFF";
      else s = "WAIT";
    }

    if (s == "DIFF") {
      // free old data
      freeSongFiles(clientSongList);
      freeSongFiles(hashedClientSongList);
      onlyServer.clear(); onlyClient.clear();

      bufferLen = getFilesFromDisk(dir_name, buffer);
      deserializeSongList(clientSongList, buffer, bufferLen);

      // hash the song content
      hashSongList(clientSongList, hashedClientSongList);

      // get the diff
      getDiff(onlyServer, hashedClientSongList, hashedServerSongList);
      cout << "Files the server has that you don't have:" << endl;
      for (unsigned int i = 0; i < onlyServer.size(); i++) 
        cout << onlyServer[i].name << endl;

      getDiff(onlyClient, hashedServerSongList, hashedClientSongList);
      cout << "Files you have but the server does not have:" << endl;
      for (unsigned int i = 0; i < onlyClient.size(); i++) 
        cout << onlyClient[i].name << endl;

      // send to the server the list of files the server has but you do not have
      ph.type = 1;
      ph.length = serializeSongList(onlyServer, ph.data, false);
      bufferLen = serializePacket(buffer, ph, false);
      sendTCPMessage(sock, buffer, bufferLen, 0);

      cout << "\nDo you want to sync with the server? (YES/NO)" << endl;
      cin >> commandInput;
      if (commandInput == "YES") s = "SYNC";
      else s = "WAIT";
    }

    if (s == "SYNC") {
      freeSongFiles(serverSongList);

      // get the files to send to the server
      vector<SongFile> filesToSend;
      getSameSongList(filesToSend, onlyClient, clientSongList);
      cout << "\nSend these files to the server:" << endl;
      for (unsigned int i = 0; i < filesToSend.size(); i++) {
        cout << filesToSend[i].name << endl;
      }

      // construct packet and send to the server
      ph.type = 2;
      ph.length = serializeSongList(filesToSend, ph.data, false);
      bufferLen = serializePacket(buffer, ph, false);
      sendTCPMessage(sock, buffer, bufferLen, 0);

      // get message back from the server
      recvTCPMessage(sock, buffer, recv_buffer);
      deserializePacket(buffer, recv_packet); 
      deserializeSongList(serverSongList, recv_packet.data, recv_packet.length);

      // write the files to disk
      for (unsigned int i = 0; i < serverSongList.size(); i++) {
        writeSongToDisk(dir_name, serverSongList[i]);
      } 

      s = "WAIT";
    }

    if (s == "LEAVE") {
      ph.type = 3;
      ph.length = 0;
      bufferLen = serializePacket(buffer, ph, false);
      sendTCPMessage(sock, buffer, bufferLen, 0);
    }
  }

  free(ph.data);
  free(recv_packet.data);
  free(buffer);
  freeSongFiles(clientSongList);
  freeSongFiles(hashedServerSongList);
  freeSongFiles(hashedClientSongList);

  close(sock);

  return 0;
}
