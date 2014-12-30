#include "vertex.h"

Vertex::Vertex(double x, double y, double z) {
	x = x;
	y = y;
	z = z;
}

Vertex::Vertex(std::vector<double>& v) {
	if (v.size() != 3) {
		std::cerr
			<< "Vector of incorrect length provided for vertex constructor"
			<< std::endl;
	}
	x = v[0];
	y = v[1];
	z = v[2];
}

Vertex::~Vertex() {}