#include "NetworkHeader.h"
#include "DataHeader.h"

using namespace std; 


int main(int argc, char *argv[]) {
  /* DONE: Command Line arguments */

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
	

	int clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(clientSocket < 0)
		DieWithError("socket() failed");

	// construct the server address structure
	struct sockaddr_in servAddr;						// server address
	memset(&servAddr, 0, sizeof(servAddr));	// zero out structure
	servAddr.sin_family = AF_INET;					// ip4v squadfam
	servAddr.sin_port = htons(serverPort); // server port
	
	// establish TCP connection with server
	if(connect(clientSocket, (struct sockaddr *) &servAddr, sizeof(servAddr))< 0)
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
			if (send(clientSocket, buffer, bufferLen, 0) != bufferLen)
				DieWithError("send() sent a different number of bytes than expected");
		}
		s = "LEAVE";
	}

	free(ph.data);
	free(buffer);

	close(clientSocket);

  return 0;
}
