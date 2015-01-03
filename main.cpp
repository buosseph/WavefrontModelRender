#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "render.h"

using namespace std;

enum Input {
	Comment = (int)'#',
	Vertex	= (int)'v',
	Face	= (int)'f',
	Object	= (int)'o',
};

vector<float> vertices;
vector<int> faces;

void parseComment(string line) {
	cout << line << '\n';
}

void parseVertex(string line) {
	size_t found = line.find(' ');
	if (found != string::npos) {
		string values = line.substr(found);
		size_t lastPos = found;
		for (int i = lastPos; i < line.length(); i++) {
			if (values[i] == ' ' || values[i] == '\0') {
				string value = values.substr(lastPos, i);
				lastPos = i;
				vertices.push_back(atof(value.c_str()));
			}
		}
	}
}

void parseFace(string line) {
	size_t found = line.find(' ');
	if (found != string::npos) {
		string values = line.substr(found);
		size_t lastPos = found;
		if (lastPos == 3) {
			for (int i = lastPos; i < line.length(); i++) {
				if (values[i] == ' ' || values[i] == '\0') {
					string value = values.substr(lastPos, i);
					lastPos = i;
					faces.push_back(atoi(value.c_str()) - 1);
						// vertices are 1-indexed in file
				}
			}			
		}
		else {
			// Could remove unnecessary vector and iteration

			vector<int> tokens;
			for (int i = lastPos; i < line.length(); i++) {
				if (values[i] == ' ' || values[i] == '\0') {
					string value = values.substr(lastPos, i);
					lastPos = i;
					tokens.push_back(atoi(value.c_str()) - 1);
						// vertices are 1-indexed in file
				}
			}

			for (int i = 2; i < tokens.size(); i++) {
				faces.push_back(tokens[0]);
				faces.push_back(tokens[i-1]);
				faces.push_back(tokens[i]);

				// cout << "Face: { "
				// 	<< tokens[0] << ", "
				// 	<< tokens[i-1] << ", "
				// 	<< tokens[i] << " };"
				// 	<< endl;
			}
		}
	}
}

void parseObject(string line) {
	string name;
	size_t found = line.find(' ');
	if (found != string::npos) {
		name = line.substr(found + 1);
	}
	cout << name << ": " << endl;
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

int main(int argc, char** argv) {
	if (argc < 2) {
		cout
			<< "Usage:\n"
			<< "a.out [.obj]"
			<< endl;
		return -1;
	}
	bool opened = openFile(argv[1]);
	if (!opened) {
		return -1;
	}

	string filename(argv[1]);
	const char* title = ("Model Render: " + filename).c_str();

	int status = render(title, vertices.size(), &vertices[0], faces.size(), &faces[0]);
	return status;	
}