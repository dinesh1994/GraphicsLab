#include <iostream>
#include <SDL/SDL.h>
#include <cmath>

using namespace std;

float ww=600,wh=600;
int xc,yc,a,b;

void putPixel(SDL_Surface* dest, int x, int y, int r, int g, int b)
{
    if(x>=0 && x<dest->w && y>=0 && y < dest->h)
        ((Uint32*)dest->pixels)[y*dest->pitch/4+x] = SDL_MapRGB(dest->format,r,g,b);
}

void drawEllipse(SDL_Surface* dest, int x, int y)
{
        putPixel(dest,xc+x,yc+y,255,255,255);
        putPixel(dest,xc+x,yc-y,255,255,255);
        putPixel(dest,xc-x,yc+y,255,255,255);
        putPixel(dest,xc-x,yc-y,255,255,255);

}

void midPointAlgo(SDL_Surface* dest, int a, int b)
{
    int x=0,y=b;
    int sa = a*a;
    int sb = b*b;
    double d1 = sb - sa*b + 0.25*sa;   //?? refer notes

        drawEllipse(dest, x, y);

    while(sa*(y-0.5) > sb*(x+1))
    {
        if(d1<0)
        {
            d1+= sb*((x<<1) + 3);
        }
        else
        {
            d1+= sb*((x<<1) + 3) + sa*(-(y<<1)+2);
            y--;
        }
        x++;
        drawEllipse(dest, x, y);
    }

    double d2 = sb*pow((x+0.5),2) + (sa*pow((y-1),2)) - (sa*sb);

    while(y>0)
    {
        if(d2<0)
        {
            d2+= sa*(-(y<<1)+3) + sb*((x<<1)+2);
            x++;
        }
        else
        {
            d2+= sa*(-(y<<1)+3);
        }
        y-=1;
        drawEllipse(dest,x,y);
    }

}


int main()
{
    cin>>xc>>yc>>a>>b;
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
               midPointAlgo(screen, a, b);
        SDL_Flip(screen);

        if(1000.0/30 > (SDL_GetTicks()-start))
            SDL_Delay(1000.0 - (SDL_GetTicks() - start));
    }
    SDL_Quit();
}
