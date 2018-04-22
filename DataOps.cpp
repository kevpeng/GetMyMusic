#include "DataHeader.h"
#include <dirent.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;


/******* functions for serializing and deserializing packet ********/
unsigned long copyData(char* data, char* buffer, unsigned short numFiles) {
  unsigned long idx = 0;

  for (unsigned short i = 0; i < numFiles; i++) {
    // name
    memcpy(buffer + idx, data + idx, NAME_BYTES);
    idx += NAME_BYTES;

    // length
    unsigned long length;
    memcpy(buffer + idx, data + idx, LENGTH_BYTES);
    memcpy(&length, data + idx, LENGTH_BYTES);

    idx += LENGTH_BYTES;

    // data
    memcpy(buffer + idx, data + idx, length);

    idx += length;
  }

  return idx;
}

unsigned long serializePacket(char* packet_str, packet_h& ph) {
  // version + type
  packet_str[0] = (ph.version << 4) | (ph.type << 1) | (ph.r);
  // length 
  packet_str[1] = ph.length;
  return copyData(ph.data, packet_str + 2, ph.length) + 2;
}


void deserializePacket(char* packet_str, packet_h& ph) {
  ph.version = ((unsigned char) packet_str[0]) >> 4;
  ph.type = (((unsigned char) packet_str[0]) >> 1) & 0x7;
  ph.r = packet_str[0] & 0x1;
  ph.length = packet_str[1];
  copyData(packet_str + 2, ph.data, ph.length);
} 


/******* functions for reading and writing files ********/
unsigned short getFilesFromDisk(string dir_name, char* data) {
  DIR* dir;
  dirent* pdir;

  // file
  FILE* fp;
  unsigned long fSize;
  char* buffer;

  dir = opendir(dir_name.c_str());

  unsigned long idx = 0;
  unsigned short numFiles = 0;

  while ((pdir = readdir(dir))) {
    string fname = pdir->d_name;
    // check if it is really an MP3 file
    if (fname.substr(fname.find_last_of(".") + 1) != "mp3") 
      continue;
    
    numFiles += 1;

    // add song name
    for (unsigned short i = 0; i < NAME_BYTES; i++) {
      data[idx + i] = fname[i];
      // pad with null pointer
      if (i >= fname.length())
        data[idx + i] = '\0';
    }

    idx += NAME_BYTES;

    // read file content
    fp = fopen((dir_name + "/" + fname).c_str(), "rb");

    // get the size
    fseek(fp, 0, SEEK_END);
    fSize = ftell(fp);
    rewind(fp);

    // store the size
    memcpy(data + idx, &fSize, sizeof(fSize));

    idx += LENGTH_BYTES;

    buffer = (char*) malloc(sizeof(char) * fSize);
    fread(buffer, 1, fSize, fp);
    memcpy(data + idx, buffer, fSize);

    idx += fSize;

    fclose(fp);
    free(buffer);
  }

  return numFiles;
}


void writeSongToDisk(SongFile& song) {
  FILE* fp;
  fp = fopen(song.name, "wb");
  fwrite(song.data, 1, song.length, fp);
  fclose(fp);
}


/******* utility functions ********/

vector<SongFile> deserializeSongList(char* data, unsigned short numFiles) {
  vector<SongFile> sList;
  unsigned long idx = 0;

  for (unsigned short i = 0; i < numFiles; i++) {
    SongFile song;

    memcpy(song.name, data + idx, NAME_BYTES);
    idx += NAME_BYTES;

    // find the length of the song
    memcpy(&song.length, data+idx, LENGTH_BYTES);
    idx += LENGTH_BYTES;

    // copy data
    memcpy(song.data, data + idx, song.length);

    idx += song.length;

    sList.push_back(song);
  }

  return sList;
}


bool compareSong(SongFile& s1, SongFile& s2) {
  // compare name
  if (strncmp(s1.name, s2.name, NAME_BYTES) == 0)
    return true;

  // compare content
  if (s1.length == s2.length && memcmp(s1.data, s2.data, s1.length) == 0)
    return true;

  return false;
}


bool hasSong(vector<SongFile>& v, SongFile& song) {
  for (int i = 0; i < v.size(); i++) {
    if (compareSong(v[i], song)) {
      return true;
    }
  }
  return false;
}


// get all the songs in the second list that the first one does not have
vector<SongFile> getDiff(vector<SongFile>& v1, vector<SongFile>& v2) {
  vector<SongFile> diff;

  for (int i = 0; i < v2.size(); i++) {
    if (!hasSong(v1, v2[i]))
      diff.push_back(v2[i]);
  }

  return diff;
}


// DEBUG
// int main() {
//   packet_h ph;
//   ph.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);

//   unsigned short numFiles = getFilesFromDisk("music_dir_1", ph.data);
//   ph.length = numFiles;
//   ph.version = 0x5;
//   ph.type = 0x1;
//   ph.r = 1;

//   char* packet_str;
//   packet_str = (char*) malloc(sizeof(char) * (MAX_SONG_LIST_BYTES + 3));
//   serializePacket(packet_str, ph);

//   packet_h other;
//   other.data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);
//   deserializePacket(packet_str, other);

//   cout << (ph.version == other.version) << endl;
//   cout << (ph.type == other.type) << endl;
//   cout << (ph.r == other.r) << endl;
//   cout << (ph.length == other.length) << endl;

//   vector<SongFile> v = deserializeSongList(other.data, other.length);
//   for (int i = 0; i < v.size(); i++) {
//     writeSongToDisk(v[i]);
//   }

//   free(ph.data);
//   return 0;
// }
