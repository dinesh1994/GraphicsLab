#include "Vertex.h"

Vertex::Vertex(){
}

Vertex::Vertex(float p, float q)
{
  x = p;
  y = q;
}

bool Vertex::operator==(Vertex v){    return ((x == v.x)&&(y == v.y));}

void Vertex::operator=(Vertex v)
{
    x = v.x;
    y = v.y;
}

bool Vertex::operator>=(Vertex v)
{
    return ((x >= v.x)&&(y >= v.y));
}

bool Vertex::operator<=(Vertex v)
{
    return ((x <= v.x)&&(y <= v.y));
}


Edge::Edge(Vertex v1, Vertex v2)
{
    if(v1.y > v2.y)
    {
        vstart = v1;
        vend = v2;
    }
    else
    {
        vstart = v2;
        vend = v1;
    }

    m = (vstart.y - vend.y)/(vstart.x - vend.x);
    c = (vend.y - (m*vend.x));
}

bool Edge::operator==(Edge e)
{
    if(vstart == e.vstart && vend == e.vend)
        return true;
    else
        return false;
}

void Edge::showEdges()
{
    printf("Edge - (%f,%f)-(%f,%f) :\n",vstart.x, vstart.y, vend.x, vend.y);
}

void AEL::insert(Edge e)
{
    if(edgeList.empty())
     {
         edgeList.push_back(e);
         e.showEdges();
     }
    else
    {
        for(int i=0;i<edgeList.size();i++)
        {
            if(e == edgeList[i])
                return ;
        }
        edgeList.push_back(e);

    }
}

void AEL::insert(Vertex e)
{
    if(POI.empty())
        POI.push_back(e);
    else
    {
        for(int i=0;i<POI.size();i++)
        {
            if(e == POI[i])
                return ;
        }
        POI.push_back(e);
    }
}
