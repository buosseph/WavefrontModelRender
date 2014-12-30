#include <iostream>
#include <fstream>
#include <string>

using namespace std;

enum Input {
	Comment = (int)'#',
	Vertex	= (int)'v',
	Face	= (int)'f',
	Object	= (int)'o',

};

void parseComment(string line) {
	cout << line << '\n';
}

void parseVertex(string line) {
	cout << line << '\n';
}

void parseFace(string line) {
	cout << line << '\n';
}

void parseObject(string line) {
	cout << line << '\n';
}

void parseFile(ifstream& file) {
	string line;
	while(getline(file, line)) {
		Input input = (Input)(int)line[0];

		switch(input) {
			case Comment:
				parseComment(line);
				break;
			case Vertex:
				parseVertex(line);
				break;
			case Face:
				parseFace(line);
				break;
			case Object:
				parseObject(line);
				break;
			default:
				break;
		}
	}
}

bool isWavefront(char* filename) {
	if (filename[0] == '\0') {
		return false;
	}

	int i, j;
	i = j = 0;
	while(filename[j] != '\0') {
		if (filename[j] == '.') {
			i = j;
		}
		j++;
	}
	char* suffix = filename + i;
	char obj[] = ".obj";
	return (strcmp(suffix, obj) == 0);
}

bool openFile(char* filename) {
	if (!isWavefront(filename)) {
		return false;
	}

	ifstream file;
	file.open(filename);
	if (file.is_open()) {
		parseFile(file);
	}
	else {
		cerr << "Error opening file" << endl;
		return false;
	}

	file.close();
	return true;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout
			<< "Usage:\n"
			<< "a.out [.obj]"
			<< endl;
		return -1;
	}
	openFile(argv[1]);
	return 0;	
}