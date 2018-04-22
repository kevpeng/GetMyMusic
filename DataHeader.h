#ifndef __DATAHEADER_H__
#define __DATAHEADER_H__

#include <vector>

// const for song data
const unsigned short NAME_BYTES = 20;
const unsigned short LENGTH_BYTES = sizeof(unsigned long); 
const unsigned long MAX_SONG_BYTES = 6000000;
const unsigned long MAX_SONG_LIST_BYTES = MAX_SONG_BYTES * (1 << 8);


// packet header
struct packet_h {
  // r = 0 : request, r = 1: response
  // length is the number of songs
  unsigned short version, type, r, length;
  char* data;
};

// contain a song's info
struct SongFile {
  char name[NAME_BYTES]; // 1 more for null terminator
  char data[MAX_SONG_BYTES];
  long length; // length of data in bytes
};

/* functions for serializing and deserializing packet */
void deserializePacket(char* packet_str, packet_h& ph);
unsigned long serializePacket(char* packet_str, packet_h& ph);

/* functions for reading and writing files */
unsigned short getFilesFromDisk(std::string dir_name, char* data);
void writeSongToDisk(SongFile& song);

/* utility functions */
std::vector<SongFile> getDiff(std::vector<SongFile>& v1, std::vector<SongFile>& v2);
std::vector<SongFile> deserializeSongList(char* data, int numFiles);

#endif