CC = g++ -std=c++11
CFLAGS = -Wall

all: client server

client: MusicClient.cpp NetworkHeader.cpp Data.cpp
				${CC} ${CFLAGS} -o MusicClient MusicClient.cpp NetworkHeader.cpp Data.cpp

server: MusicServer.cpp NetworkHeader.cpp Data.cpp
				${CC} ${CFLAGS} -o MusicServer MusicServer.cpp NetworkHeader.cpp Data.cpp

################################################################################
## My Stuff
################################################################################
o:
	vim MusicServer.cpp +":tabf MusicClient.cpp" +":tabf Data.cpp" + ":vsplit Data.h" +":tabf README.md"

rc: #Usage MusicClient [-h <serverIP>] [-p <serverPort>]
	./MusicClient

rs: #Usage MusicServer -p <port>
	./MusicServer -p 31400 

# GDB
gc:
	gdb MusicClient

gs:
	gdb MusicServer

vc:
	valgrind MusicClient

vs:
	valgrind MusicServer
