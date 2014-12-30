#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <vector>

class Vertex {
	double x, y, z;
	public:
		Vertex(double, double, double);
		Vertex(std::vector<double>&);
		~Vertex();
};
#endif