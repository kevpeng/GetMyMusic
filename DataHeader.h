#ifndef __DATAHEADER_H__
#define __DATAHEADER_H__

#include <vector>

const int NAME_BYTES = 20;
const int LENGTH_BYTES = sizeof(long);
const int MAX_SONG_BYTES = 6000000;
const int MAX_SONG_LIST_BYTES = MAX_SONG_BYTES * 20;

using namespace std;


// packet header
struct packet_h {
  // use the one from project 3 with no checksum
  // data: [file_data_size|file_name|file_data] * n (where n is the number of files), we can force the file_data_size field to be 4 bytes,
  // and the file_name field to be 2 bytes for example, so only file_data field will vary in length.
  unsigned short version, type, length;
  char data[MAX_SONG_LIST_BYTES];
};

struct SongFile {
  char name[NAME_BYTES]; // 1 more for null terminator
  char data[MAX_SONG_BYTES];
  long length;
};

/* functions for serializing and deserializing music list */
packet_h deserialize(char* packet_str);
char* serialize(packet_h ph);

/* functions for reading and writing files */
char* readFiles(char* dir_name);


#endif