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


bool sortByX(Vertex a, Vertex b)
{
    return (a.x<b.x);
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
