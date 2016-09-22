UNAME := $(shell uname)
CC = g++
CFLAGS = -Wall -g

ifeq ($(UNAME), SunOS)
# do something Solaris-y
LIBS= -lm -lnsl -lsocket -lpthread -lrt
FILE=runSolaris
else
LIBS= -lpthread 
FILE=run
endif

HFILES = udpsocket.h udpsocket2.h main.h packet.h FileReader.h FileWriter.h md5.h Sender.h Receiver.h

CCFILES = main.cpp md5.cpp

all: $(CCFILES)
	$(CC) $(CFLAGS) -o $(FILE) $(LIBS) $(CCFILES)

test: all
	valgrind -v --leak-check=yes --show-reachable=yes ./$(FILE)

clean:
	rm -f *.o *.*~ $(FILE)

