#include "Hasher.h"
#include <iostream>

using namespace std;

// copy from http://www.cse.yorku.ca/~oz/hash.html
unsigned long djb2_hash(char* str, unsigned long length) {
  unsigned long hash = 5381;
  int c;

  for (unsigned long i = 0; i < length; i++) {
    c = *str++;
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

// DEBUG

/*
int main() {
  const char* a = "Hello World!";
  const char* b = "Scooby Doo!";
  const char* c = a;
  cout << jdb2_hash((char*) a, strlen(a)) << endl;
  cout << jdb2_hash((char*) b, strlen(b)) << endl;
  cout << jdb2_hash((char*) c, strlen(c)) << endl;
}
*/
