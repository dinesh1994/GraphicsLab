#include <iostream>
#include <vector>
#include <SDL/SDL.h>

#include "Polygon.h"
#include "Vertex.h"
#include "Line.h"
#include "external.h"

#define ROUND(a)	((int)(a+0.5))
#define LEFT_EDGE 0x1
#define RIGHT_EDGE 0x2
#define BOTTOM_EDGE 0x4
#define TOP_EDGE 0x8

#define INSIDE(a) (!a)
#define REJECT(a,b) (a&b)
#define ACCEPT(a,b) (!(a|b))

using namespace std;

Polygon window,P1,clipPolygon;
vector<Polygon> winPolygons;
SDL_Surface *screen;
float ww = 600,wh = 600,xcenter,ycenter;
int state = 0;
Polygon boundPts;
Polygon polyPts;

void drawPolygon(Polygon &P);

void putPixel(SDL_Surface* dest, int x, int y)
{
    int r,b,g;
    switch(state)
    {
    case 1:
            r = 100;
            g = 100;
            b = 100;
            break;
    case 2:
            r = 0;
            g = 0;
            b = 100;
            break;
    case 3:
            r = 255;
            g = 255;
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

unsigned char encode (Vertex &p,Polygon &win)
{
	unsigned char code = 0x00;

	if(p.x<win.box[0].x)
		code = (code|LEFT_EDGE);
	if(p.y<win.box[0].y)
		code = (code|BOTTOM_EDGE);
	if(p.x>win.box[1].x)
		code = (code|RIGHT_EDGE);
	if(p.y>win.box[1].y)
		code = (code|TOP_EDGE);

	return code;
}

void swapPt(Vertex &a, Vertex &b)
{
	Vertex temp = a;
	a = b;
	b = temp;
}

void swapCodes(unsigned char &c1, unsigned char &c2)
{
	unsigned char c = c1;
	c1 = c2;
	c2 = c;
}

bool compare(Vertex a, Vertex b)
{
    float dx1 = a.x - xcenter;
    float dy1 = a.y - ycenter;
    float dx2 = b.x - xcenter;
    float dy2 = b.y - ycenter;

    if (dx1 >= 0 && dx2 < 0)
        return true;
    if (dx1 < 0 && dx2 >= 0)
        return false;
    if (dx1 == 0 && dx2 == 0)
    {
        if (dy1 >= 0 || dy2 >= 0)
            return a.y > b.y;
        return b.y > a.y;
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (dx1) * (dy2) - (dx2) * (dy1);
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    float d1 = (dx1) * (dx1) + (dy1) * (dy1);
    float d2 = (dx2) * (dx2) + (dy2) * (dy2);
    return d1 > d2;
}

//Sortclockwise
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

void clipLine(Polygon &win, Vertex p1, Vertex p2)
{
	unsigned char code1,code2;
	int done = false, draw = false,swape = false,rej = false;
	float m;

	while(!done)
	{
		code1 = encode(p1, win);
		code2 = encode(p2, win);
		if(ACCEPT(code1,code2))
		{
			done = true;
			draw = true;
		}
		else
		{
			if(REJECT(code1, code2))                        //Reject the line segment directly
			{
			    done = true;
			    rej = true;
			}
			else
				if(INSIDE(code1))                           //Checking if point inside or not
				{
					swapPt(p1,p2);
					swapCodes(code1,code2);
					swape = true;
				}

				if(p1.x != p2.x)
					m = (p2.y - p1.y)/(p2.x - p1.x);

				if(code1 & LEFT_EDGE)
				{
					p1.y += (win.box[0].x - p1.x)*m;
					p1.x = win.box[0].x;
					if(swape)
                        p1.out = true;
                    else
                        p1.in = true;
				}
				else
					if(code1 & RIGHT_EDGE)
					{
						p1.y += (win.box[1].x - p1.x)*m;
						p1.x = win.box[1].x;
                        if(swape)
                            p1.out = true;
                        else
                            p1.in = true;
					}
					else
						if(code1 & BOTTOM_EDGE)
							{
								if(p1.x != p2.x)
									p1.x += (win.box[0].y - p1.y)/m;
								p1.y = win.box[0].y;
                                if(swape)
                                    p1.out = true;
                                else
                                    p1.in = true;
							}
						else
							if(code1 & TOP_EDGE)
							{
								if(p1.x != p2.x)
									p1.x += (win.box[1].y - p1.y)/m;
								p1.y = win.box[1].y;
                                if(swape)
                                    p1.out = true;
                                else
                                    p1.in = true;
							}
		}/*
		if(done&&!rej)
        {
            cout<<"Reject appended"<<endl;
            if(swape)
            {
                 polyPts.vertices.push_back(p2);
                 polyPts.vertices.push_back(p1);
            }
            else
            {
                polyPts.vertices.push_back(p1);
                polyPts.vertices.push_back(p2);
            }

        }*/
	}
	if(draw)
    {
        //drawLine(screen, p1.x, p1.y, p2.x, p2.y);
        if(p1.in || p1.out)
            boundPts.vertices.push_back(p1);
        if(p2.in || p2.out)
            boundPts.vertices.push_back(p2);

            //cout<<"Reject appended"<<endl;
            if(swape)
            {
                 polyPts.vertices.push_back(p2);
                 polyPts.vertices.push_back(p1);
            }
            else
            {
                polyPts.vertices.push_back(p1);
                polyPts.vertices.push_back(p2);
            }
    }
}

bool func(Vertex &a, Vertex &b)
{
    return (a==b);
}

void getWinPolygons()
{
    if(boundPts.vertices.empty() || polyPts.vertices.empty())
    {
        cout<<"Boundary points or polypoints empty"<<endl;
        return;
    }
    else
    {
        int i=0,j=0;
            Polygon temp1,temp2;
            int count = 0;
            for(i=0;;i++)
            {
                cout<<"i = "<<i<<"j ="<<j<<endl;

                while((!polyPts.vertices[i].out))
                {
                    if(polyPts.vertices[i].in || polyPts.vertices[i].out)
                     {
                         temp1.insert(polyPts.vertices[i]);
                         cout<<"i = "<<i<<"j = "<<j<<" Inserted from Poly "<<endl;
                         //cout<<"temp->insert "<<temp1.vertices.back().x<<","<<temp1.vertices.back().y<<endl;
                     }

                    i = (i+1)%polyPts.vertices.size();

                    if(i>=polyPts.vertices.size())
                      {
                          if(count ==2)
                            return ;
                          else
                            count++;
                      }
                }

                while(!(polyPts.vertices[i] == boundPts.vertices[j]))
                {
                    j = (j+1)%boundPts.vertices.size();
                }

                cout<<"Comparision"<<j<<endl;

                while(!boundPts.vertices[j].in)
                {
                    int k = (j+1)%boundPts.vertices.size();
                    if(boundPts.vertices[k] == temp1.vertices[0])
                    {
                        cout<<"Polygon completed"<<endl;
                        winPolygons.push_back(temp1);
                        temp1.Erase();

                        return;
                    }
                        temp1.insert(boundPts.vertices[j]);
                        //cout<<"temp->insert "<<temp1.vertices.back().x<<","<<temp1.vertices.back().y<<endl;
                        cout<<"i = "<<i<<"j = "<<j<<" Inserted in Bound "<<endl;
                        //drawPolygon(temp);
                    j = (j+1)%boundPts.vertices.size();
                }

            }
    }
}

void getClippedLines(Polygon &win,Polygon &p)
{
    if(p.vertices.empty()||win.vertices.empty())
        cout<<"Insert some edges in polygon or select window"<<endl;
    else
    {
        if(p.vertices.size()==1)
            return;
        for(int i=0,j;i<p.vertices.size();i++)
        {
            j = (i+1)%p.vertices.size();
            polyPts.vertices.push_back(p.vertices[i]);
            clipLine(win,p.vertices[i],p.vertices[j]);
        }

        boundPts.vertices.push_back(win.vertices[0]);
        boundPts.vertices.push_back(win.vertices[1]);
        boundPts.vertices.push_back(win.vertices[2]);
        boundPts.vertices.push_back(win.vertices[3]);
        //sortClockwise(boundPts);

        unique(boundPts.vertices.begin(),boundPts.vertices.end(),func);
        unique(polyPts.vertices.begin(),polyPts.vertices.end(),func);
        sortClockwise(boundPts);

        for(int i=0;i<boundPts.vertices.size();i++)
        {
            putPixel(screen, boundPts.vertices[i].x, wh-boundPts.vertices[i].y);
            SDL_Flip(screen);
            //SDL_Delay(300);
            cout<<i<<"\t in "<<boundPts.vertices[i].in<<"\t out"<<boundPts.vertices[i].out<<endl;
        }

        cout<<"POLYGON Array"<<endl;

        for(int i=0;i<polyPts.vertices.size();i++)
        {
            putPixel(screen, polyPts.vertices[i].x, wh-polyPts.vertices[i].y);
            SDL_Flip(screen);
            //SDL_Delay(300);
            cout<<i<<"\t in "<<polyPts.vertices[i].in<<"\t out"<<polyPts.vertices[i].out<<endl;
        }

        cout<<"calling getWinPolygon"<<endl;
        getWinPolygons();
        cout<<"clipped polygons = "<<winPolygons.size()<<endl;
        for(int i=0;i<winPolygons.size();i++)
        {
        cout<<"Size of polygon "<<i<<" = "<<winPolygons[i].vertices.size()<<endl;
        //sortClockwise(winPolygons[i]);
        drawPolygon(winPolygons[i]);
        SDL_Flip(screen);
        }

    }

}

void drawPolygon(Polygon &P)
{
   //sortClockwise(P);
    cout<<"Polygon size: "<<P.vertices.size()<<endl;
     if(P.vertices.size()>=2&&(state==2 || state==3))
                {
                    for(int i=0,j;i<P.vertices.size();i++)
                    {
                        j = (i+1)% P.vertices.size();
                        float x0 = P.vertices[i].x;
                        float y0 = P.vertices[i].y;
                        float x1 = P.vertices[j].x;
                        float y1 = P.vertices[j].y;

                        drawLine(screen, x0, y0, x1, y1);
                    }
                    SDL_Flip(screen);
                }

}

void fillVertices(float x, float y)
{
    switch(state)
    {
        case 1:
              {
                if(!window.vertices.empty())
                    window.Erase();
                cout<<"Enter the size of window"<<endl;
                int a=100;
                //cin>>a;
                window.insert(Vertex(x - a, y - a));
                window.insert(Vertex(x - a, y + a));
                window.insert(Vertex(x + a, y + a));
                window.insert(Vertex(x + a, y - a));
                cout<<"Window->center = "<<x<<","<<y<<endl;
                drawLine(screen,window.vertices[0].x,window.vertices[0].y,window.vertices[1].x,window.vertices[1].y);
                drawLine(screen,window.vertices[1].x,window.vertices[1].y,window.vertices[2].x,window.vertices[2].y);
                drawLine(screen,window.vertices[2].x,window.vertices[2].y,window.vertices[3].x,window.vertices[3].y);
                drawLine(screen,window.vertices[3].x,window.vertices[3].y,window.vertices[0].x,window.vertices[0].y);


                //drawLine(screen,window.box[0].x,window.box[0].y,window.box[0].x,window.box[1].y);
                //drawLine(screen,window.box[0].x,window.box[1].y,window.box[1].x,window.box[1].y);
                //drawLine(screen,window.box[1].x,window.box[1].y,window.box[1].x,window.box[0].y);
                //drawLine(screen,window.box[1].x,window.box[0].y,window.box[0].x,window.box[0].y);
                SDL_Flip(screen);
                break;
              }
        case 2:
                P1.insert(Vertex(x,y));
                cout<<"P1->vertex = "<<x<<","<<y<<endl;
                break;
        case 3:
               {

                break;
               }
        Default:
                cout<<"Please press 1 or 2 for valid state"<<endl;
    }

}

int main(int argv,char **argc)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode(ww, wh, 32, SDL_SWSURFACE);

    Uint32 start;
    bool running = true;
    SDL_Event event;

    SDL_FillRect(screen,NULL,0);
    while(running)
    {
        start = SDL_GetTicks();
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                        running = false;
                        break;
            case SDL_MOUSEBUTTONDOWN:
                  {
                      if(state != 0)
                      {
                        putPixel(screen, event.button.x, event.button.y);
                        SDL_Flip(screen);
                        float x = event.button.x;
                        float y = wh - event.button.y;
                        fillVertices( x, y);
                      }
                        break;
                  }
            case SDL_KEYDOWN:
                  {
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        running = false;
                    if(event.key.keysym.sym == SDLK_1)
                        state = 1;
                    if(event.key.keysym.sym == SDLK_2)
                        state = 2;
                    if(event.key.keysym.sym == SDLK_3)
                        state = 3;
                    if(event.key.keysym.sym == SDLK_c)
                    {
                        state = 0;
                        window.Erase();
                        P1.Erase();
                        clipPolygon.Erase();
                        winPolygons.erase(winPolygons.begin(), winPolygons.end());
                        boundPts.Erase();
                        polyPts.Erase();
                        SDL_FillRect(screen,NULL,0);
                        SDL_Flip(screen);
                    }
                    if(event.key.keysym.sym == SDLK_d)
                        {
                            switch(state)
                            {
                            case 1:
                                    drawPolygon(window);
                                    break;
                            case 2:
                                    drawPolygon(P1);
                                    break;
                            case 3:
                                    break;
                            default:
                                    cout<<"----Select State\n\t 1-P1 \n\t2-P2\n\t 3-P3\n"<<endl;
                            }
                        }
                    if(event.key.keysym.sym == SDLK_s)
                    {
                        if(state==3)
                            getClippedLines(window,P1);
                        else
                            cout<<"Press 3 for clipping"<<endl;
                    }
                    cout<<"state = "<<state<<endl;
                      break;
                  }
            }
        }

        if(100.0/30>(SDL_GetTicks()-start))
            SDL_Delay(100.0-(SDL_GetTicks()-start));
    }
    SDL_Quit();

	return 0;
}
