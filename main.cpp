#include <iostream>
#include <fstream>

using namespace std;

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

void parseFile(ifstream& file) {
	cout << "succss" << endl;
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