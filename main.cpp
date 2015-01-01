#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <png.h>

// #include "vertex.h"

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

inline void setRGB(png_byte *ptr, float val) {
	int v = (int)(val * 767);
	if (v < 0) v = 0;
	if (v > 767) v = 767;
	int offset = v % 256;

	if (v<256) {
		ptr[0] = 0; ptr[1] = 0; ptr[2] = offset;
	}
	else if (v<512) {
		ptr[0] = 0; ptr[1] = offset; ptr[2] = 255-offset;
	}
	else {
		ptr[0] = offset; ptr[1] = 255-offset; ptr[2] = 0;
	}
}

bool saveScene(char* filename, int width, int height, float* buffer, char* title) {
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open %s for writing\n", filename);

		fclose(fp);
		return false;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");

		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		fclose(fp);
		return false;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");

		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		fclose(fp);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");

		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		fclose(fp);
		return false;
	}

	png_init_io(png_ptr, fp);
	// Write header (8-bit color)
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);

	// Memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			setRGB(&(row[x*3]), buffer[y*width + x]);
		}
		png_write_row(png_ptr, row);
	}

	png_write_end(png_ptr, NULL);

	if (fp != NULL) {
		fclose(fp);
	}
	if (info_ptr != NULL) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	}
	if (png_ptr != NULL) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	}
	if (row != NULL) {
		free(row);
	}

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

	char* filename = "output.png";
	int width = 4;
	int height = 4;
	char* title = "Test";
	float buffer [width*height*3];	// all 0.

	cout << saveScene(filename, width, height, buffer, title) << endl;
	return 0;	
}