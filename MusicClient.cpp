#include "NetworkHeader.h"
#include "Data.h"
#include "Hasher.h"

#include <set>

using namespace std; 


int main(int argc, char *argv[]) {
  char* serverIP = (char *)SERVER_HOST; // server host from the NetworkHeader.h
  unsigned short serverPort = atoi(SERVER_PORT); // port from NetworkHeader.h

  if(argc < 2) {
    cout << "Error: Usage MusicClient [-h <serverIP>] [-p <serverPort>]" << endl;
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

  // packet header
  packet_h ph;
  ph.version = 0x5;
  ph.r = 0; // request
  ph.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);

  char* buffer;
  buffer = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES + 3));
  unsigned long bufferLen;

  // for recving packet
  char recv_buffer[SHORT_BUFFSIZE];
  packet_h recv_packet;
  recv_packet.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);

  vector<SongFile> clientSongList, hashedServerSongList, hashedClientSongList;
  vector<SongFile> diff;

  // ask for input after connection is established.
  string s = ""; 

  while(s != "LEAVE") {
    cout << "Please type a function name (LIST, DIFF, PULL, LEAVE): ";
    s = "";
    cin >> s;
    cout << "The command you issued was: " << s << endl;

    if (s == "LIST" || s ==  "list") {
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
    }
    else if (s == "DIFF" || s == "diff") {
      freeSongFiles(clientSongList);
      freeSongFiles(hashedClientSongList);

      bufferLen = getFilesFromDisk("client_dir", buffer);
      deserializeSongList(clientSongList, buffer, bufferLen);

      // hash the song content
      for (unsigned int i = 0; i < clientSongList.size(); i++) {
        SongFile song;
        song.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);
        unsigned long hash = djb2_hash(clientSongList[i].data, clientSongList[i].length);
        song.length = sizeof(hash);
        memcpy(song.data, &hash, sizeof(hash));
      }
    }
    else if (s == "PULL") {
    }
    else if (s == "LEAVE" || s == "leave") {
      s = "LEAVE";
    }
  }

  free(ph.data);
  free(recv_packet.data);
  free(buffer);
  freeSongFiles(clientSongList);
  freeSongFiles(hashedServerSongList);
  freeSongFiles(hashedClientSongList);
  freeSongFiles(diff);

  close(sock);

  return 0;
}
