#include <SDL/SDL.h>
#include <stdio.h>
#include <iostream>
#include "Polygon.h"
#include "Vertex.h"
#include "line.h"
#include "circle.h"
#include <cmath>
#include <algorithm>

using namespace std;

int ww=600,wh=600;
float xcenter,ycenter;
int state=0;
SDL_Surface* screen;

Polygon P1,P2,clippedP;

bool isInsideP(Vertex v, Polygon &p);


void putPixel(SDL_Surface* dest, int x, int y)
{
    int r,b,g;
    switch(state)
    {
    case 1:
            r = 255;
            g = 255;
            b = 100;
            break;
    case 2:
            r = 255;
            g = 255;
            b = 0;
            break;
    case 3:
            r = 255;
            g = 0;
            b = 0;
            break;
    default:
            r = 255;
            g = 255;
            b = 255;
    }

    if(x>=0 && x<dest->w && y>=0 && y < dest->h)
        ((Uint32*)dest->pixels)[y*dest->pitch/4+x] = SDL_MapRGB(dest->format,r,g,b);
}

bool compare(Vertex a, Vertex b)
{
    float dx1 = a.x - xcenter;
    float dy1 = a.y - ycenter;
    float dx2 = b.x - xcenter;
    float dy2 = b.y - ycenter;

    if (dx1 >= 0 && dx2 < 0)
        return false;
    if (dx1 < 0 && dx2 >= 0)
        return true;
    if (dx1 == 0 && dx2 == 0)
    {
        if (dy1 >= 0 || dy2 >= 0)
            return a.y < b.y;
        return b.y < a.y;
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (dx1) * (dy2) - (dx2) * (dy1);
    if (det < 0)
        return false;
    if (det > 0)
        return true;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    float d1 = (dx1) * (dx1) + (dy1) * (dy1);
    float d2 = (dx2) * (dx2) + (dy2) * (dy2);
    return d1 < d2;
}

void sortClockwise(Polygon &p)
{
    if(p.vertices.size()==0)
        return ;

    float xsum=0,ysum=0;

    for(int i=0;i<p.vertices.size();i++)
    {
        xsum += p.vertices[i].x;
        ysum += p.vertices[i].y;
    }
    //Finding mean point for clockwise sorting
    xcenter = xsum/p.vertices.size();
    ycenter = ysum/p.vertices.size();

    sort(p.vertices.begin(),p.vertices.end(), compare);
}

void removeEndingEdge(Vertex v, AEL &ael)
{
    for(unsigned int i=0;i<ael.edgeList.size();i++)
    {
        if(v == ael.edgeList[i].vend)
         {
              //printf("Removing edge (%f,%f)-(%f,%f)\n", ael.edgeList[i].vstart.x, ael.edgeList[i].vstart.y, ael.edgeList[i].vend.x, ael.edgeList[i].vend.y);
              ael.edgeList.erase(ael.edgeList.begin()+i);
         }
    }
}

bool sortByX(Vertex a, Vertex b)
{
    return (a.x<b.x);
}

void updateIntersectionPoints(float y,AEL &ael)
{
    if(!ael.POI.empty())
    {
        ael.POI.erase(ael.POI.begin(),ael.POI.end());
        //cout<<"\n\t OLD VALUES OF POI CLEARED !!\n"<<endl;
    }

    for(int i=0;i<ael.edgeList.size();i++)
    {
        float x;
        if(ael.edgeList[i].m)
            x = (y - ael.edgeList[i].c)/ael.edgeList[i].m;
        else
            cout<<"Slope is 0"<<endl;

        ael.POI.push_back(Vertex(x,y));
    }
}

void drawPixels(AEL &ael)
{
    if(ael.POI.empty())
    {
      //  cout<<"POI empty\n"<<endl;
        return ;
    }

    sort(ael.POI.begin(), ael.POI.end(), &sortByX);
    //cout<<"OUTSIDE THE LOOP\n"<<endl;

    for(int i1=0,i2=0;i2<ael.POI.size();i1++)
        {
            i2 = i1+1;
            int x1 = ael.POI[i1].x;
            int x2 = ael.POI[i2].x;
            int y = ael.POI[i1].y;
            //cout<<"INSIDE THE LOOP\n"<<endl;
            for(int x=x1;x<=x2;x++)
            {
                //printf("Pixels (%d,%d)-(%d,%d)\n",x1,y,x2,y);
                putPixel(screen, x, y);
            }

            SDL_Flip(screen);
            //SDL_Delay(50);
        }

}

void ScanFill(Polygon &p)
{
	if(p.vertices.empty())
	{
		cout<<"Polygon is Emtpy\n"<<endl;
		return ;
	}

    vector<Vertex> bucket[wh];                              //Making bucket for each scanline for holding vertices

    for(int i=0;i<p.vertices.size();i++)
        bucket[(int)p.vertices[i].y].push_back(p.vertices[i]);

    AEL ael;

    for(int y=(p.box[1].y + 1);y>=p.box[0].y;y--)                                //For every vertex at current line update AEL and intersection point :)
    {
        if(!bucket[y].empty())
        {
            for(int j=0;j<bucket[y].size();j++)
            {
                Vertex v = bucket[y][j];

                    for(int k=0;k<p.vertices.size();k++)
                    {
                        int k1 = (k+1) % p.vertices.size();
                        int k2;
                        if(k==0)
                            k2 = p.vertices.size()-1;
                        else
                            k2 = k-1;

                        if(v == p.vertices[k])
                        {
                            ael.insert(Edge(v,p.vertices[k1]));
                            ael.insert(Edge(v,p.vertices[k2]));
                        }
                    }
                removeEndingEdge(v, ael);
            }
        }
        updateIntersectionPoints(y,ael);
        //cout<<"\n\t\tSIZE-POI :"<<ael.POI.size()<<endl;
        drawPixels(ael);
    }
}

void drawPolygon(Polygon &p)
{
    sortClockwise(p);
    for(int i=0;i<p.vertices.size();i++)
    {
        putPixel(screen,p.vertices[i].x, p.vertices[i].y);
        SDL_Flip(screen);
        SDL_Delay(500);
    }

   // ScanFill(p);
}

bool isInBox(Vertex v, Vertex v1, Vertex v2, Vertex box[])
{
    if(v.y>box[1].y || v.x > box[1].x || v.y < box[0].y || v.x < box[0].x)
        return false;
    else if(v == v1)
        return false;
    else if(v == v2)
        return true;
    else
        return true;
}

Vertex findIntersectionPoint(Vertex v1, Vertex v2, Vertex v3, Vertex v4)
{
        float m1,m2,c1,c2,dx1,dx2,x,y;

        dx1 = v2.x - v1.x;

        dx2 = v4.x - v3.x;

        if(dx1 == 0 && dx2 == 0)
        {
            if(v3.x == v1.x)
            {
                clippedP.insert(v3);
                return v4;
            }
            else
            {
                return Vertex(-1,-1);
            }
        }
        else if( dx1 != 0 && dx2 ==0 )
        {
            m1 = (v2.y - v1.y)/(dx1);
            c1 = v1.y - (m1*v1.x);

            x = v3.x;
            y = (m1*x) + c1;
        }
        else if( dx2 != 0 && dx1 ==0 )
        {
            m2 = (v4.y - v3.y)/(dx2);
            c2 = v3.y - (m2*v3.x);

            x = v1.x;
            y = (m2*x) + c2;
        }
        else
        {
            m1 = (v2.y - v1.y)/(dx1);
            c1 = v1.y - (m1*v1.x);

            m2 = (v4.y - v3.y)/(dx2);
            c2 = v3.y - (m2*v3.x);

            if(m2!=m1)
            {
                x = (c2 - c1)/(m1 - m2);
                y = ((m1*c2) - (m2*c1))/(m1-m2);
            }
            else
                ;
           /* {//In case lines are overlapping(NOTE: v3 & v4 are vertices of clipping Polygon)
                if(c1 == c2 )
                {
                    if(v1.x > v3.x && v2.x < v4.x)                      //case when both v1 and v2 are interior of v3 and v4 edge
                    {
                        clippedP.insert(v2);
                    }
                    else if(v1.x < v3.x && v2.x > v3.x && v2.x < v4.x)  //when v1 at left of v3 and v2 is in between v3 & v4
                    {
                        clippedP.insert(v3);
                        clippedP.insert(v2);
                    }
                    else if(v2.x > v4.x && v1.x > v3.x && v1.x < v4.x)  //when v2 at right of v4 and v1 is in between v3 & v4
                    {
                        clippedP.insert(v4);
                    }
                    else                                                //when v3 & v4 in between v1 & v2
                    {
                        clippedP.insert(v3);
                        clippedP.insert(v4);
                    }

                }
                cout<<"Slope is Same"<<endl;
                return Vertex(-1,-1);
            }*/
        }



        printf("\nPoint of intersection (%f,%f) is ",x,y);
            if(isInBox(Vertex(x,y), v1, v2, P1.box)&&isInBox(Vertex(x,y), v3, v4, P2.box))
            {
                //putPixel(screen,x,y);
                //midPointAlgo(screen,x,y,100);
                //SDL_Flip(screen);
                cout<<"INSIDE BOTH THE BOX"<<endl;
                return Vertex(x,y);
            }
            else
            {
                cout<<"OUTSIDE one of the box"<<endl;
                return Vertex(-1,-1);
            }

}


void checkForIntersection(Vertex v1, Vertex v2)
{
    for(int i1=0,i2;i1<P2.vertices.size();i1++)
    {
        i2 = (i1+1) % P2.vertices.size();

        Vertex p2v1 = P2.vertices[i1];
        Vertex p2v2 = P2.vertices[i2];

        Vertex intersect = findIntersectionPoint(v1, v2, p2v1, p2v2);

        if(!(intersect == Vertex(-1,-1)))
        {
            clippedP.insert(intersect);
        }
    }
}

void getClippedVertices()
{
     cout<<"\n P1 & P2 sorted\n"<<endl;
                   sortClockwise(P1);
                   //drawPolygon(P1);
                   sortClockwise(P2);
                   //drawPolygon(P2);

    if(P1.vertices.size()<3||P2.vertices.size()<3)
    {
        cerr<<"Insert more Edges in P1 or P2"<<endl;
        return ;
    }

    bool vinsideP2[P1.vertices.size()];
    bool vinsideP1[P2.vertices.size()];

    for(int i=0;i<P1.vertices.size();i++)
        vinsideP2[i] = isInsideP(P1.vertices[i], P2);

    for(int i=0;i<P2.vertices.size();i++)
    {
        if(isInsideP(P2.vertices[i], P1))
        {
            printf("Vertex P2 inside P1 (%f,%f)\n",P2.vertices[i].x,P2.vertices[i].y);
            clippedP.insert(P2.vertices[i]);
        }
    }

    for(int i1=0,i2;i1<P1.vertices.size();i1++)
    {
        i2 = (i1+1)%P1.vertices.size();

        bool st1 = vinsideP2[i1];
        bool st2 = vinsideP2[i2];

        if(!st1 && !st2)
        {
            cout<<"case 1: Both the vertices are outside"<<endl;
            checkForIntersection(P1.vertices[i1], P1.vertices[i2]);
        }
        else if(st1 && st2)
        {
            cout<<"case 2: Both the vertices are inside"<<endl;
            clippedP.insert(P1.vertices[i2]);
        }
        else
        {
            cout<<"case 3: One out one in"<<endl;
            checkForIntersection(P1.vertices[i1], P1.vertices[i2]);
            if(vinsideP2[i2])
                clippedP.insert(P1.vertices[i2]);
        }
    }
    drawPolygon(clippedP);
}


//Using Half-Ray method for finding if point intersect P2 or not
bool isIntersectingP(Vertex v,Vertex v1,Vertex v2, Polygon &p)
{
    if(v.y >= p.box[1].y || v.y <= p.box[0].y || v.x <= p.box[0].x || v.x >= p.box[1].x)
    {
        //cout << "outside box" << endl;
        return false;
    }
    else
    {
        float m = (v2.y - v1.y)/(v2.x - v1.x);
        if(m!=0)
        {
            float c = v1.y - (m*v1.x);

            float x = (v.y - c)/m;

            //return false if point of intersection is outside p.box
            if(v.y > p.box[1].y || v.y < p.box[0].y || x < p.box[0].x || x > p.box[1].x)
                return false;
            else if(x >= v.x)
            {
                //Here logic is if intersecting point is vertex then
                //if it is 1st in cyclic order then return true and
                //if it's 2nd vertex then return false
                if(Vertex(x, v.y) == v1)
                    return true;
                else if(Vertex(x, v.y) == v2)
                    return false;
                else
                    return true; //return true if point of intersection is at right side of point in ray method.
            }
            else
            {
                return false;
            }
        }
        else
            return false;
    }
    //debugin
    //cout << "none match" << endl;
    return false;
}



//Using Half-Ray method for finding if point is inside P2 or not
bool isInsideP(Vertex v, Polygon &p)
{
    int count=0;

    for(int i=0,i1=-1,i2=-1;i2!=0;i++)
    {
        i1 = i;
        i2 = (i+1) % p.vertices.size();

        //cout<<"edge ("<<p.vertices[i1].x<<","<<p.vertices[i1].y<<")-("<<p.vertices[i2].x<<","<<p.vertices[i2].y<<")---\n"<<endl;

        bool var = isIntersectingP(v,p.vertices[i1],p.vertices[i2], p);
        //cout<<var;
        if(var)
            count+=1;

        //cout<<"\ncount: " << count<<endl;
    }

    if(count == 3)
        return false;

    return ((count%2)!=0);

}


void fillVertices(float x, float y)
{
    Vertex v(x,y);
    vector<Vertex> lines;
    switch(state)
    {
        case 1:
                P1.insert(v);
                cout<<"P1->vertex = "<<x<<","<<y<<endl;
                lines = P1.vertices;
                break;
        case 2:
                P2.insert(v);
                cout<<"P2->vertex = "<<x<<","<<y<<endl;
                lines = P2.vertices;
                break;
        case 3:
               {
                if(P2.vertices.empty())
                        return;
                if(isInsideP(Vertex(x,y), P2))
                      ;//printf("\nPoint (%f,%f) is inside P2:\n",x,y);
                else
                      ;//printf("\nPoint (%f,%f) is outside P2:\n",x,y);
                break;
               }
        Default:
                cout<<"Please press 1 or 2 for valid state"<<endl;
    }
/*
    if(lines.size()>=2&&(state==1||state==2))
                {
                    for(int i=0,j;i<lines.size();i++)
                    {
                        j = (i+1)% lines.size();
                        float x0 = lines[i].x;
                        float y0 = lines[i].y;
                        float x1 = lines[j].x;
                        float y1 = lines[j].y;

                        drawLine(screen, x0, y0, x1, y1);
                    }
                    SDL_Flip(screen);
                }
*/
}

void detectVertices(Polygon &p)
{
    if(p.vertices.empty())
    {
        cout<<"Polygon empty Insert some vertices\n"<<endl;
        return;
    }

    for(int i=0;i<p.vertices.size();i++)
    {
        midPointAlgo(screen, p.vertices[i].x, p.vertices[i].y, 5);
    }
    SDL_Flip(screen);
}

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode(ww,wh,32,SDL_SWSURFACE);

    Uint32 start;
    bool running = true;
    SDL_Event event;

    SDL_FillRect(screen,NULL,0);

    while(running)
    {
        start = SDL_GetTicks();
        while(SDL_PollEvent(&event))
        {
            //Interesting error if we remove scope from case statements
            //Then cross-initialization error will occur
            //as int x & int y are visible in other cases at each time x&y are declared even if that case does't occur
            switch(event.type)
            {
              case SDL_QUIT:
                  {
                        running = false;
                        break;
                  }
              case SDL_MOUSEBUTTONDOWN:
                  {
                        putPixel(screen, event.button.x, event.button.y);
                        SDL_Flip(screen);
                        float x = event.button.x;
                        float y = event.button.y; /*need to revert as top-left corner is 0,0 in display devices*/
                        fillVertices( x, y);
                        break;
                  }
              case SDL_KEYDOWN:
                  {
                        if(event.key.keysym.sym == SDLK_1)
                                state = 1;
                        else if(event.key.keysym.sym == SDLK_2)
                                state = 2;
                        else if(event.key.keysym.sym == SDLK_3)
                                state = 3;
                        else if(event.key.keysym.sym == SDLK_s)
                                getClippedVertices();
                        else if(event.key.keysym.sym == SDLK_ESCAPE)
                                running = false;
                        else if(event.key.keysym.sym == SDLK_c)
                        {
                            SDL_FillRect(screen,NULL,0);
                            SDL_Flip(screen);
                            P1.Erase();
                            P2.Erase();
                            clippedP.Erase();
                        }
                        else if(event.key.keysym.sym == SDLK_f)
                        {
                            switch(state)
                            {
                            case 1:
                                    ScanFill(P1);
                                    break;
                            case 2:
                                    ScanFill(P2);
                                    break;
                            case 3:
                                    ScanFill(clippedP);
                                    break;
                            default:
                                    cout<<"----Select State\n\t 1-P1 \n\t2-P2\n\t 3-P3\n"<<endl;
                            }
                        }
                        else if(event.key.keysym.sym == SDLK_v)
                        {
                            switch(state)
                            {
                            case 1:
                                    detectVertices(P1);
                                    break;
                            case 2:
                                    detectVertices(P2);
                                    break;
                            case 3:
                                    detectVertices(clippedP);
                                    break;
                            default:
                                    cout<<"----Select State\n\t 1-P1 \n\t2-P2\n\t 3-P3\n"<<endl;
                            }
                        }
                        else
                        {
                         drawLine(screen,P2.box[0].x,P2.box[0].y,P2.box[0].x,P2.box[1].y);
                         drawLine(screen,P2.box[0].x,P2.box[1].y,P2.box[1].x,P2.box[1].y);
                         drawLine(screen,P2.box[1].x,P2.box[1].y,P2.box[1].x,P2.box[0].y);
                         drawLine(screen,P2.box[1].x,P2.box[0].y,P2.box[0].x,P2.box[0].y);
                         SDL_Flip(screen);
                        }
                        cout<<"State:"<<state<<endl;
                        break;
                  }
            }
        }


        if(100.0/30>(SDL_GetTicks()-start))
            SDL_Delay(100.0-(SDL_GetTicks()-start));
    }
    SDL_Quit();

    return 1;
}
