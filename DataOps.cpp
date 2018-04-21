#include "DataHeader.h"
#include <dirent.h>
#include <iostream>
#include <stdio.h>

using namespace std;

void getFiles(string dir_name, char* data) {
  DIR* dir;
  dirent* pdir;

  // file
  FILE* fp;
  long fSize;
  char* buffer;

  dir = opendir(dir_name.c_str());

  int idx = 0;

  while ((pdir = readdir(dir))) {
    string fname = pdir->d_name;
    // check if it is really an MP3 file
    if (fname.substr(fname.find_last_of(".") + 1) != "mp3") 
      continue;
    
    // add song name
    for (int i = 0; i < fname.length(); i++) {
      data[idx + i] = fname[i];
    }

    idx += NAME_BYTES;

    // read file content
    fp = fopen((dir_name + "/" + fname).c_str(), "rb");

    buffer = (char*) malloc(sizeof(char) * MAX_SONG_BYTES);
    fSize = fread(buffer, MAX_SONG_BYTES, 1, fp);

    strncpy(data + idx, buffer, fSize);

    idx += fSize;

    fclose(fp);
    free(buffer);
  }
}


int main() {
  char data[MAX_SONG_LIST_BYTES];
  getFiles("music_files", data);
  return 0;
}