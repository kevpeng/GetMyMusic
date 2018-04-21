################################################################################
## OS
################################################################################
OS = LINUX

################################################################################
## GCC
################################################################################
CC = g++ -std=c++14
OPTS = -O3
#OPTS = -g
FLAGS = #-Wall -Werror 
ifeq "$(OS)" "LINUX"
  DEFS = -DLINUX
else
  ifeq "$(OS)" "OSX"
  DEFS = -DOSX
endif
endif

################################################################################
## Rules
################################################################################
INCL = 
LIBS = 

COBJS = DataOps.o\
       MusicClient.o

SOBJS = DataOps.o\
       MusicServer.o

CEXECUTABLE = MusicClient
SEXECUTABLE = MusicServer

default: $(CEXECUTABLE)

c: $(CEXECUTABLE)

s: $(SEXECUTABLE)

$(CEXECUTABLE): $(COBJS)
	$(CC) $(OPTS) $(FLAGS) $(DEFS) $(COBJS) $(LIBS) -o $(CEXECUTABLE)

$(SEXECUTABLE): $(SOBJS) 
	$(CC) $(OPTS) $(FLAGS) $(DEFS) $(SOBJS) $(LIBS) -o $(SEXECUTABLE)

clean:
	rm -f $(CEXECUTABLE) $(SEXECUTABLE) $(COBJS) $(SOBJS)

.cpp.o:
	$(CC) $(OPTS) $(FLAGS) $(DEFS) -MMD $(INCL) -c $< -o $@
	cat $*.d >> Dependencies
	rm -f $*.d

-include Dependencies

################################################################################
## My Stuff
################################################################################
o:
	vim MusicServer.cpp +":tabf MusicClient.cpp" +":tabf DataHeader.h" +":tabf README.md"

#TODO finish this
rc: 
	./MusicClient 

rs:
	./MusicServer 

# GDB
gc:
	gdb MusicClient

gs:
	gdb MusicServer

vc:
	valgrind MusicClient

vs:
	valgrind MusicServer
