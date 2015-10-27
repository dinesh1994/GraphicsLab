#include "Polygon.h"

Polygon::Polygon()
{
    box[0] = Vertex(-1,-1);
    box[1] = Vertex(-1,-1);
}

Polygon::Polygon(int p, int q)
{
    box[0] = Vertex(p,q);
    box[1] = Vertex(p,q);

    vertices.push_back(Vertex(p,q));
}

void Polygon::insert(Vertex v)
{
    if(box[0]==Vertex(-1,-1) && box[1]==Vertex(-1,-1))
     {
         box[0] = v;
         box[1] = v;
     }

    if(v.x < box[0].x)
        box[0].x = v.x;
    if(v.y < box[0].y)
        box[0].y = v.y;

    if(v.x > box[1].x)
        box[1].x = v.x;
    if(v.y > box[1].y)
        box[1].y = v.y;

    vertices.push_back(v);
}

void Polygon::Erase()
{
    if(!vertices.empty())
    {
    vertices.erase(vertices.begin(),vertices.end());
    box[0] = Vertex(-1,-1);
    box[1] = Vertex(-1,-1);
    }

}
