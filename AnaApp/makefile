# Instructions for compiling Anagame's TrecLib in a Linux Environment
INC=-I../TrecLib -I../TcGraph -I../AnaWidget
LIB=-L../Debug
CC	= g++
CFLAGS  = $(INC) $(LIB) -Wall -Wextra -fpermissive -std=c++17 -g -c
LD	= ld

SOURCE_FILES := $(wildcard *.cpp)
# Set up All Object files in this directory
OBJFILES := $(patsubst %.cpp,%.o,$(SOURCE_FILES))

.cpp.o:
	$(CC) $(CFLAGS) -o $@ $<
	
all: AnaApp.o

AnaApp.o: $(OBJFILES)
	$(LD) -r -o AnaApp.o $(OBJFILES) ; mv AnaApp.o ../Debug/AnaApp.o 

clean:
	rm $(OBJFILES)
