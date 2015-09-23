#include <iostream>
#include <SDL/SDL.h>

using namespace std;

float w=600,h=600;

float abs(float x){ return ((x>0)?x:(-x)); }

void putPixel(SDL_Surface* dest, int x, int y, int r, int g, int b)
{
    if(x>=0 && x < dest->w && y>=0 && y < dest->h)
        ((float*)dest->pixels)[y*dest->pitch/4+x]=SDL_MapRGB(dest->format,r,g,b);
}

void swapValue(float &a,float &b)
{
    float temp = a;
    a = b;
    b = temp;
}

void drawLine(SDL_Surface* dest, float x0, float y0, float x1, float y1)
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

    float y=y0;
    float ystep = ((y1>y0) ? 1 : -1);
    for(float i=x0;i<x1;i++)
    {
        if(step)
            putPixel(dest, y, i, 255, 255, 255);
        else
            putPixel(dest, i, y, 255, 255, 255);
        error += roundError;
        if(error>=0.5)
        {
            y++;
            error-=1.0;
        }
    }
}
