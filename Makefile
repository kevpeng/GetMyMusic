CC = g++ -std=c++11
CFLAGS = -Wall
INCL = -pthread

all: client server

client: MusicClient.cpp NetworkHeader.cpp Data.cpp Hasher.cpp
				${CC} ${CFLAGS} -o MusicClient MusicClient.cpp NetworkHeader.cpp Data.cpp Hasher.cpp

server: MusicServer.cpp NetworkHeader.cpp Data.cpp Hasher.cpp
				${CC} ${CFLAGS} $(INCL) -o MusicServer MusicServer.cpp NetworkHeader.cpp Data.cpp Hasher.cpp

################################################################################
## My Stuff
################################################################################
o:
	vim MusicServer.cpp +":tabf MusicClient.cpp" +":tabf Data.cpp" + ":vsplit Data.h" +":tabf README.md"

# mathcs01 port 31400
rc: #Usage MusicClient [-h <serverIP>] [-p <serverPort>]
	./MusicClient -h 141.166.207.144 -p 31400

rs: #Usage MusicServer -p <port>
	./MusicServer -p 31400 

# Are's IP
rca: #Usage MusicClient [-h <serverIP>] [-p <serverPort>]
	./MusicClient -h 141.166.130.179 -p 31400

# GDB
gc:
	gdb MusicClient

gs:
	gdb MusicServer

vc:
	valgrind MusicClient

vs:
	valgrind MusicServer
