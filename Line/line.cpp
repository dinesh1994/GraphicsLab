#include <iostream>
#include <SDL/SDL.h>

using namespace std;

float w=600,h=600;

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
    for(int i=x0;i<x1;i++)
    {
        if(step)
            putPixel(dest, y, h-i, 255, 255, 255);
        else
            putPixel(dest, i, h-y, 255, 255, 255);
        error += roundError;
        if(error>=0.5)
        {
            y++;
            error-=1.0;
        }
    }
}

int main()
{
    int x0,y0,x1,y1;
    cin>>x0>>y0>>x1>>y1;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface* screen = SDL_SetVideoMode(w,h,32,SDL_SWSURFACE);
    bool running = true;
    Uint32 start;
    SDL_Event event;
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
            }
        }
        SDL_FillRect(screen,NULL,0);

            drawLine(screen, x0, y0, x1, y1);

        SDL_Flip(screen);

        if(1000.0/30 > (SDL_GetTicks()-start))
            SDL_Delay(1000.0/30-(SDL_GetTicks()-start));
    }
    SDL_Quit();
}
