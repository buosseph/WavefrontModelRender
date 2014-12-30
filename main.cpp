#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "vertex.h"

using namespace std;

enum Input {
	Comment = (int)'#',
	Vertex	= (int)'v',
	Face	= (int)'f',
	Object	= (int)'o',
};

// vector<Vertex> vertices;

void parseComment(string line) {
	cout << line << '\n';
}

void parseVertex(string line) {
	vector<double> tokens;
	size_t found = line.find(' ');
	if (found != string::npos) {
		string values = line.substr(found);
		size_t lastPos = found;
		for (int i = lastPos; i < line.length(); i++) {
			if (values[i] == ' ' || values[i] == '\0') {
				string value = values.substr(lastPos, i);
				lastPos = i;
				tokens.push_back(atof(value.c_str()));
			}
		}
	}

	cout << "Vertex: { ";
	for (int i = 0; i < 3; i++) {
		cout << tokens[i];
		if (i != 2) {
			cout << ", ";
		}
	}
	cout << " };" << endl;
}

void parseFace(string line) {
	vector<int> tokens;
	size_t found = line.find(' ');
	if (found != string::npos) {
		string values = line.substr(found);
		size_t lastPos = found;
		for (int i = lastPos; i < line.length(); i++) {
			if (values[i] == ' ' || values[i] == '\0') {
				string value = values.substr(lastPos, i);
				lastPos = i;
				tokens.push_back(atoi(value.c_str()));
			}
		}
	}

	cout << "Face: { ";
	for (int i = 0; i < tokens.size(); i++) {
		cout << tokens[i];
		if (i != tokens.size() - 1) {
			cout << ", ";
		}
	}
	cout << " };" << endl;
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