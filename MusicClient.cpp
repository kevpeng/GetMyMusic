#include "NetworkHeader.h" // get this

using namespace std; 

int main(int argc, char *argv[]) {
  /* DONE: Command Line arguments */

  string serverIP = SERVER_HOST; // server host from the NetworkHeader.h
	unsigned short serverPort = atoi(SERVER_PORT); // port from NetworkHeader.h
	
	if(argc < 2) 
  {
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

	// convert address
	int rtnVal = inet_pton(AF_INET,serverIP.c_str(), &servAddr.sin_addr.s_addr);
	if(rtnVal <= 0)
		DieWithError("inet_pton() failed");

	servAddr.sin_port = htons(serverPort); // server port
	
	// establish TCP connection with server
	if(connect(clientSocket, (struct sockaddr *) &servAddr, sizeof(servAddr))< 0)
		DieWithError("connect() failed");

	// connection is successful
	cout << "Connection was successful." << endl;	

	// ask for input after connection is established.
	cout << "Please type a function name (LIST, DIFF, PULL, LEAVE): ";
	string s = ""; 
  cin >> s;
	cout << "The command you issued was: " << s << endl;

	while(s != "LEAVE") {
		cout << "Please type a function name (LIST, DIFF, PULL, LEAVE): ";
		s = "";
		cin >> s;
		cout << "The command you issued was: " << s << endl;
		
		
		
		
	//	ssize_t numBytes = send(clientSocket, s, s.length(), 0);
//		if(numBytes < 0)
	//		DieWithError("send() failed");
		
		
			
	}
	close(clientSocket);
	return(0);



  /* TODO: Set up TCP connection with the server */

  /* TODO: Waiting for input from the user and send LIST, DIFF, SYNC, LEAVE */

  return 0;
}
