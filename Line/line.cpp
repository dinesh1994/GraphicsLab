#include <iostream>
#include <SDL/SDL.h>
#include "Vertex.h"
#include <stdio.h>

using namespace std;

float w=600,h=600;
int state = 0;

int abs(int x){ return ((x>0)?x:(-x)); }

void putPixel(SDL_Surface* dest, int x, int y, int r, int g, int b)
{
    if(x>=0 && x < dest->w && y>=0 && y < dest->h)
        ((Uint32*)dest->pixels)[y*dest->pitch/4+x]=SDL_MapRGB(dest->format,r,g,b);
}

void swapValue(int &a,int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

void drawLine(SDL_Surface* dest, int x0, int y0, int x1, int y1)
{
    bool step = abs(x1-x0) < abs(y1-y0);
    if(step)
    {
        swapValue(x0,y0);
        swapValue(x1,y1);
    }

    if(x1<x0)
    {
        swapValue(x0,x1);
        swapValue(y0,y1);
    }

    float error=0.0;
    float roundError = (float)abs(y1-y0)/(float)(x1-x0);

    int y=y0;
    int ystep = ((y1>y0) ? 1 : -1);
    printf("step: %d ystep = %d\n",step,ystep);
    for(int i=x0;i<x1;i++)
    {
        if(step)
            putPixel(dest, y, h-i, 255, 255, 255);
        else
            putPixel(dest, i, h-y, 255, 255, 255);
        SDL_Flip(dest);
        error += roundError;

        if(error>=0.5)
        {
            if(ystep==1)
                y++;
            else
                y--;
            error-=1.0;
        }
    }
}

void drawLine(SDL_Surface* dest, vector<Vertex> &points)
{
    if(state != 2)
    {
        cout<<"Press 2 for enabling drawLine"<<endl;
        return ;
    }
    for(int i = 0;i<points.size()-1;i++)
    {
        drawLine(dest, points[i].x, points[i].y, points[i+1].x, points[i+1].y);
        SDL_Flip(dest);
    }
}

int main()
{
    vector<Vertex> points;
    int pointCount =0;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface* screen = SDL_SetVideoMode(w,h,32,SDL_SWSURFACE);
    bool running = true;
    Uint32 start;
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
                     if(state == 1)
                     {
                         points.push_back(Vertex(event.button.x, h-event.button.y));
                         putPixel(screen, event.button.x, event.button.y,255,255,255);
                         SDL_Flip(screen);
                         cout<<"("<<event.button.x<<","<<event.button.y<<")"<<endl;
                         cout<<"("<<event.button.x<<","<<h-event.button.y<<")"<<endl;
                     }
                     else
                        cout<<"Change state to 1 for adding more points"<<endl;
                    break;
            case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_1)
                        state = 1;
                    if(event.key.keysym.sym == SDLK_2)
                        state = 2;
                    if(event.key.keysym.sym == SDLK_c)
                    {
                        state = 0;
                        points.erase(points.begin(),points.end());
                        SDL_FillRect(screen,NULL,0);
                        SDL_Flip(screen);
                    }
                    cout<<"state = "<<state<<endl;
                    drawLine(screen,points);
                    break;
            }
        }

        if(1000.0/30 > (SDL_GetTicks()-start))
            SDL_Delay(1000.0/30-(SDL_GetTicks()-start));
    }
    SDL_Quit();
}
