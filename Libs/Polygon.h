#ifndef POLYGON_H
#define POLYGON_H
#include "Vertex.h"

class Polygon{
	public:
		vector<Vertex> vertices;
		Vertex box[2];
		bool use;
		int maxvertices;
	Polygon();
	Polygon(int x, int y);
	void insert(Vertex v);
	void Erase();
	};

#endif
