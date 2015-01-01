#include <iostream>
#include <string>

#include "vertex.h"

void print(vertex& v) {
	std::string start 	= "Vector { ";
	std::string comma 	= ", ";
	std::string end 	= " }\n";

	std::cout
		<< start
		<< v.x
		<< comma
		<< v.y
		<< comma
		<< v.z
		<< end;
}