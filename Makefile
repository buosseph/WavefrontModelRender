# clang++ *.cpp -o main -framework opengl -lglew -framework cocoa -lglfw3 -framework IOKit -framework CoreVideo
# See http://www.glfw.org/docs/latest/build.html#build_link_osx for all dependencies for glfw3

CC				= clang++
CXX_CFLAGS		= -c -Wall
LIBS			= -framework opengl -lglew -framework cocoa -lglfw3 -framework IOKit -framework CoreVideo
SOURCES 		= *.cpp
OBJECTS 		= $(Sources:.cpp=.o)
EXECUTABLE		= main

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(SOURCES) -o $(EXECUTABLE) $(LIBS)

clean:
	rm -rf *o main