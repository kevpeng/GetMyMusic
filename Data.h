#ifndef __DATA_H__
#define __DATA_H__

#include <vector>
#include <string>

// const for packet
const unsigned short LENGTH_BYTES = 8; 
const unsigned short HEADER_BYTES = 1 + LENGTH_BYTES;

// const for song data
const unsigned short NAME_BYTES = 20;
const unsigned long MAX_SONG_BYTES = 6000000;
const unsigned long MAX_SONG_LIST_BYTES = MAX_SONG_BYTES * (1 << 8);


// packet header
struct packet_h {
  // r = 0 : request, r = 1: response
  // length is the number of songs
  unsigned short version, type, r;
  unsigned long length;
  char* data;
};

// contain a song's info
struct SongFile {
  char name[NAME_BYTES]; // 1 more for null terminator
  char data[MAX_SONG_BYTES];
  unsigned long length; // length of data in bytes
};

void copyData(char* data, char* buffer, unsigned long totalBytes);

/* functions for serializing and deserializing packet */
void deserializePacket(char* packet_str, packet_h& ph);
unsigned long serializePacket(char* packet_str, packet_h& ph);

/* functions for reading and writing files */
unsigned long getFilesFromDisk(std::string dir_name, char* data);
void writeSongToDisk(SongFile& song);

/* utility functions */
std::vector<SongFile> getDiff(std::vector<SongFile>& v1, std::vector<SongFile>& v2);
std::vector<SongFile> deserializeSongList(char* data, unsigned long totalBytes);

#endif
