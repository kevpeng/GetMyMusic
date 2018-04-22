#include "NetworkHeader.h"
#include "DataHeader.h"

using namespace std;

void DieWithError(string errorMessage) { 
  cerr << errorMessage << endl; 
  exit(1); 
}


unsigned long recvTCPMessage(int sock, char* buffer, char recv_buffer[]) {
  int recvMsgSize;
  unsigned long idx = 0;
  bool getLengthField = false;
  unsigned long dataLength = 0;

  while (true) {
    if ((recvMsgSize = recv(sock, recv_buffer, SHORT_BUFFSIZE, 0)) < 0)
      DieWithError("recv() failed");

    if (recvMsgSize == 0) break;
  
    // now we know it is > 0
    unsigned int unsignedMsgSize = (unsigned int) recvMsgSize;

    for (unsigned int i = 0; i < unsignedMsgSize; i++) {
      buffer[idx + i] = recv_buffer[i];
    }
    idx += unsignedMsgSize;

    // check if we have received all the header fields (except data)
    if (idx >= HEADER_BYTES) {
      // get the length of the data
      memcpy(&dataLength, buffer + 1, LENGTH_BYTES);
      getLengthField = true;
    }
    if (getLengthField && idx == dataLength + HEADER_BYTES)
      break;
  } 

  return idx;
}
