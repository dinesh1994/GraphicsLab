#include <iostream>
#include <SDL/SDL.h>
#include <math.h>

using namespace std;

float ww=600,wh=600;
int xc,yc,r;

void putPixel(SDL_Surface* dest, int x, int y, int r, int g, int b)
{
    if(x>=0 && x<dest->w && y>=0 && y < dest->h)
        ((Uint32*)dest->pixels)[y*dest->pitch/4+x] = SDL_MapRGB(dest->format,r,g,b);
}

void drawCircle(SDL_Surface* dest, int x, int y)
{
        putPixel(dest, xc+x, wh-(yc+y),255,255,255);
        putPixel(dest, (yc+y), wh-(xc+x),255,255,255);
        putPixel(dest, xc-x, wh-(yc+y),255,255,255);
        putPixel(dest, (yc-y), wh-(xc+x),255,255,255);

        putPixel(dest, xc+x, wh-(yc-y),255,255,255);
        putPixel(dest, (yc+y), wh-(xc-x),255,255,255);
        putPixel(dest, xc-x, wh-(yc-y),255,255,255);
        putPixel(dest, (yc-y), wh-(xc-x),255,255,255);

}

void midPointAlgo(SDL_Surface* dest, int xc, int yc, int r)
{
        int x=0,y=r;
        int h=1-r;
        int deltaE=3;
        int deltaSE= -2*r+5;
        //float dSE= 2*xc - 2*yc + 5;
        //float dE = 2*xc + 3;

        drawCircle(dest,x,y);
        while(y>=x)
        {
            if(h<0)
            {
                //h+=deltaE;

                //deltaE+=2;
                //deltaSE+=2;
                h += 2*x+3; // version1
            }
            else
            {
                //h+=deltaSE;

                //deltaE+=2;
                //deltaSE+=4;
                //y-=1;

                h += 2*(x-y)+5;  // version1
                y = y-1;
            }
            x+=1;
            drawCircle(dest,x,y);
        }
}


int main()
{
    cin>>xc>>yc>>r;
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
               midPointAlgo(screen, xc, yc, r);
        SDL_Flip(screen);

        if(1000.0/30 > (SDL_GetTicks()-start))
            SDL_Delay(1000.0 - (SDL_GetTicks() - start));
    }
    SDL_Quit();
}
