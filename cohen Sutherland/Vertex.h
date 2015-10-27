#ifndef VERTEX_H
#define VERTEX_H
#include <algorithm>
#include <iostream>
#include <vector>
#include <stdio.h>

using namespace std;

class Vertex{
  public:
    float x,y;
    bool in,out;
    Vertex();
    Vertex(float x, float y);
  bool operator==(Vertex v);
  bool operator>=(Vertex v);
  bool operator<=(Vertex v);
  void operator=(Vertex v);

};

class Edge
{
public:
    Vertex vstart,vend;
    float m;                                //slope of edge
    float c;
    Edge(Vertex v1, Vertex v2);
    bool operator==(Edge e);
    void showEdges();
};

class AEL
{
public:
    vector<Edge> edgeList;
    vector<Vertex> POI;                     //Points of intersection at given scanline

    void insert(Edge e);
    void insert(Vertex v);
};

#endif
