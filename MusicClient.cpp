#include "NetworkHeader.h" // get this

using namespace std; 

int main(int argc, char *argv[]) {
  /* TODO: Command Line arguments */

  string serverIP = SERVER_HOST; // server host from the NetworkHeader.h
	unsigned short serverPort = atoi(SERVER_PORT); // port from NetworkHeader.h
	
	if((argc < 4)) 
  {
    cout << "Error: Usage MusicClient [-h <serverIP>] [-p <serverPort>]" << endl;
		exit(1);
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
	
	cout << "Please type a function name (LIST, DIFF, PULL, LEAVE, HELP): ";
	string s = ""; 
  cin >> s;
	cout << "The command you issued was: " << s << endl;




  /* TODO: Set up TCP connection with the server */

  /* TODO: Waiting for input from the user and send LIST, DIFF, SYNC, LEAVE */

  return 0;
}
