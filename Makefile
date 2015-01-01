# g++ *.cpp -o main -lpng

CC				= g++
CXX_CFLAGS		= -c -Wall
LIBS			= -lpng
SOURCES 		= *.cpp
OBJECTS 		= $(Sources:.cpp=.o)
EXECUTABLE		= main

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(SOURCES) -o $(EXECUTABLE) $(LIBS)

clean:
	rm -rf *o main *png