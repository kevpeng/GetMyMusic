#include "DataHeader.h"
#include <dirent.h>
#include <iostream>
#include <stdio.h>
#include <string.h>


using namespace std;

int getFilesFromDisk(string dir_name, char* data) {
  DIR* dir;
  dirent* pdir;

  // file
  FILE* fp;
  long fSize;
  char* buffer;

  dir = opendir(dir_name.c_str());

  int idx = 0;
  int numFiles = 0;

  while ((pdir = readdir(dir))) {
    string fname = pdir->d_name;
    // check if it is really an MP3 file
    if (fname.substr(fname.find_last_of(".") + 1) != "mp3") 
      continue;
    
    numFiles += 1;

    // add song name
    for (int i = 0; i < NAME_BYTES; i++) {
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


vector<SongFile> deserialize(char* data, int numFiles) {
  vector<SongFile> sList;
  int idx = 0;

  for (int i = 0; i < numFiles; i++) {
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


void writeSongToDisk(SongFile& song) {
  FILE* fp;
  fp = fopen(song.name, "wb");
  fwrite(song.data, 1, song.length, fp);
  fclose(fp);
}


int main() {
  char* data;
  data = (char*) malloc(sizeof(char) * MAX_SONG_LIST_BYTES);

  int numFiles = getFilesFromDisk("music_dir_1", data);
  vector<SongFile> v1 = deserialize(data, numFiles);
  cout << "v1 has" << endl;
  for (int i = 0; i < v1.size(); i++)
    cout << v1[i].name << endl;

  cout << "v2 has" << endl;
  numFiles = getFilesFromDisk("music_dir_2", data);
  vector<SongFile> v2 = deserialize(data, numFiles);
  for (int i = 0; i < v2.size(); i++)
    cout << v2[i].name << endl;

  // get diff
  cout << "v2 has but v1 does not have" << endl;
  vector<SongFile> diff = getDiff(v1, v2);
  for (int i = 0; i < diff.size(); i++)
    cout << diff[i].name << endl;

  free(data);
  return 0;
}

