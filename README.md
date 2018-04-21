# GetMyMusic
GetMyMusic!, a file/music synchronization application. CS332 Final Project.

Makefile Instructions
# Compiling
make || make c - compiles client executable
make s         - compiles server executable
# Executing
make rc        - runs client executable
make rs        - runs server executable
# Debugging with GDB
make gc        - runs gdb on client
make gs        - runs gdb on server
# Debugging with Valgrind
make vc        - runs client with valgrind
make vs        - runs server with valgrind

Overview:
Functions to implement:
  LIST, DIFF, PULL, LEAVE,
  Init function to read MP3 files into a vector of song data.
  Functions to convert data from char array to vector and back.
  Maybe a CLOSE function to write data from vector to files before closing
  client/server.

Format of char array : [ 1 byte unsigned int length of song in bits | song data ]

TODO:
Decide on Multithreading approach
