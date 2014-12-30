CC=g++
CFLAGS=-c -Wall
SOURCES=*.cpp
OBJECTS=$(Sources:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -rf *o main