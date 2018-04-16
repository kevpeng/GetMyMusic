#include <vector>
#include <map> // I prefer using this over vector


// packet header
struct packet_h {
	// use the one from project 3 with no checksum
	// data: [file_data_size|file_name|file_data] * n (where n is the number of files), we can force the file_data_size field to be 4 bytes,
	// and the file_name field to be 2 bytes for example, so only file_data field will vary in length.
};


struct music {
	unsigned char* name;
	unsigned char* data;
};


/* functions for serializing and deserializing music list */
std::vector<music> charArrayToMusicList(unsigned char *data); // deserialize char array to a vector of music
void musicListToCharArray(unsigned char *buffer, std::vector<music> musicList); // serialize a list of music to the char buffer

/* functions for reading and writing music files from/to disk */
std::vector<music> getMusicFromDir(); // get a list of music from the current directory or in the music dir if we want to
void storeMusicToDir(std::vector<music> musicList);

/* some other helper functions */
std::vector<music> removeDuplicates(std::vector<music> musicList); // remove all duplicates (music files with the same contents)
vector<music> getDiff(std::vector<music> first, std::vector<music> second); // get the list of music that is in the first one but not in the second

/* 
Note: I am thinking of using a hash map instead of a vector to store the music list,
so we wont allow 2 music files with the same name. It will make things easier.
*/