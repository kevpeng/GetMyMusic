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
const unsigned long MAX_SONG_LIST_BYTES = MAX_SONG_BYTES * 10;


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
  char* data;
  unsigned long length; // length of data in bytes
};

/* functions for serializing and deserializing packet */
void deserializePacket(char* packet_str, packet_h& ph);
unsigned long serializePacket(char* packet_str, packet_h& ph, bool hash_data);

/* functions for reading and writing files */
unsigned long getFilesFromDisk(std::string dir_name, char* data);
void writeSongToDisk(std::string dir_name, SongFile& song);

/* utility functions */
void hashSongList(std::vector<SongFile>& original, std::vector<SongFile>& hashed);
void getDiff(std::vector<SongFile>& diff, std::vector<SongFile>& v1, std::vector<SongFile>& v2);
void deserializeSongList(std::vector<SongFile>& sList, char* data, unsigned long totalBytes);
unsigned long serializeSongList(std::vector<SongFile>& sList, char* data, bool hash_data);
void freeSongFiles(std::vector<SongFile>& v);
void getSameSongList(std::vector<SongFile>& sList, std::vector<SongFile>& first, 
    std::vector<SongFile>& second);

#endif
