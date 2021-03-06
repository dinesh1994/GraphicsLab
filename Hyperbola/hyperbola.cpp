#include <iostream>
#include <SDL/SDL.h>
#include <cmath>
#define SCALE 10
using namespace std;

float ww=600,wh=600;
int xc,yc,c,K;

void putPixel(SDL_Surface* dest, int x, int y, int r, int g, int b)
{
    if(x>=0 && x<dest->w && y>=0 && y < dest->h)
        ((Uint32*)dest->pixels)[y*dest->pitch/4+x] = SDL_MapRGB(dest->format,r,g,b);
}

void drawHyperbola(SDL_Surface* dest, int x, int y)
{
    //yc=wh-yc;
        putPixel(dest,xc+x,wh-(yc+y),255,255,255);
        putPixel(dest,yc+y,wh-(xc+x),255,255,255);
        putPixel(dest,xc-x,wh-(yc-y),255,255,255);
        putPixel(dest,yc-y,wh-(xc-x),255,255,255);
}

void midPointAlgo(SDL_Surface* dest, int c)
{
    int coord = sqrt(c);
    int x=coord*K,y=coord*K;

    double d1 = ((coord>>1)-0.5);   //delta for increasing.

    drawHyperbola(dest, x, y);

    while(x<400 && y>1)
    {
        if(d1<0)
        {
            d1+=y-0.5;
        }
        else
        {
            d1+= -x+y-2.5;
            y--;
        }
        x++;
        drawHyperbola(dest, x, y);
    }
}


int main()
{
    cin>>xc>>yc>>c>>K;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface* screen = SDL_SetVideoMode(ww,wh,32,SDL_SWSURFACE);
    bool running = true;
    Uint32 start;
    SDL_Event event;

    while(running)
    {
        start =  SDL_GetTicks();
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

              midPointAlgo(screen, c);			//calling midPoint Algo

        SDL_Flip(screen);

        if(1000.0/30 > (SDL_GetTicks()-start))
            SDL_Delay(1000.0 - (SDL_GetTicks() - start));
    }
    SDL_Quit();
}
