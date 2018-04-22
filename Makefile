CC = g++ -std=c++11
CFLAGS = -Wall

all: client server

client: MusicClient.cpp NetworkHeader.cpp DataOps.cpp
				${CC} ${CFLAGS} -o MusicClient MusicClient.cpp NetworkHeader.cpp DataOps.cpp

server: MusicServer.cpp NetworkHeader.cpp DataOps.cpp
				${CC} ${CFLAGS} -o MusicServer MusicServer.cpp NetworkHeader.cpp DataOps.cpp

test: DataOps.cpp Hasher.cpp
			${CC} ${CLAGS} DataOps.cpp Hasher.cpp
