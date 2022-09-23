# Instructions for compiling Anagame's TrecLib in a Linux Environment
INC=-I../TrecLib -I../../stb -I/usr/include/freetype2
LIB=-L../Debug
CC	= g++
CFLAGS  = $(INC) $(LIB) -Wall -Wextra -fpermissive -std=c++17 -g -c
LD	= ld


SOURCE_FILES := $(wildcard *.cpp)
# Set up All Object files in this directory
OBJFILES := $(patsubst %.cpp,%.o,$(SOURCE_FILES))

.cpp.o:
	$(CC) $(CFLAGS) -o $@ $<
	
all: TcGraph.o

TcGraph.o: $(OBJFILES)
	$(LD) -r -o TcGraph.o $(OBJFILES) ; mv TcGraph.o ../Debug/TcGraph.o 

clean:
	rm $(OBJFILES)
