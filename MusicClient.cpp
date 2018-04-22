#include "NetworkHeader.h"
#include "Data.h"

using namespace std; 


int main(int argc, char *argv[]) {
  string serverIP = SERVER_HOST; // server host from the NetworkHeader.h
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


  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sock < 0)
    DieWithError("socket() failed");

  // construct the server address structure
  struct sockaddr_in servAddr;						// server address
  memset(&servAddr, 0, sizeof(servAddr));	// zero out structure
  servAddr.sin_family = AF_INET;					// ip4v squadfam
  servAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());			// serverIP
  servAddr.sin_port = htons(serverPort); // server port

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

  // for recving packet
  char recv_buffer[SHORT_BUFFSIZE];
  packet_h recv_packet;
  recv_packet.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);

  // ask for input after connection is established.
  string s = ""; 

  while(s != "LEAVE") {
    cout << "Please type a function name (LIST, DIFF, PULL, LEAVE): ";
    s = "";
    cin >> s;
    cout << "The command you issued was: " << s << endl;
    if (s == "LIST") {
      ph.type = 0;
      ph.length = 0;
      unsigned long bufferLen = serializePacket(buffer, ph);
      if (send(sock, buffer, bufferLen, 0) != (unsigned int)bufferLen)
        DieWithError("send() sent a different number of bytes than expected");

      recvTCPMessage(sock, buffer, recv_buffer);
      deserializePacket(buffer, recv_packet); 
      cout << recv_packet.version << endl;
      cout << recv_packet.type << endl;
      cout << recv_packet.r << endl;
      cout << recv_packet.length << endl;

      vector<SongFile> sList = deserializeSongList(recv_packet.data, recv_packet.length);
      for (unsigned int i = 0; i < sList.size(); i++) {
        writeSongToDisk(sList[i]);
      }
    }
    s = "LEAVE";
  }

  free(ph.data);
  free(recv_packet.data);
  free(buffer);

  close(sock);

  return 0;
}
