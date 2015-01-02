# g++ *.cpp -o main -framework glut -framework opengl

CC				= clang++
CXX_CFLAGS		= -c -Wall
LIBS			= -framework glut -framework opengl
SOURCES 		= *.cpp
OBJECTS 		= $(Sources:.cpp=.o)
EXECUTABLE		= main

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(SOURCES) -o $(EXECUTABLE) $(LIBS)

clean:
	rm -rf *o main