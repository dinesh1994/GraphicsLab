#include <iostream>
#include <SDL/SDL.h>
#include <math.h>

using namespace std;

float xc,yc,r;

void putPixelc(SDL_Surface* dest, int x, int y, int r, int g, int b)
{
if(x>=0 && x<dest->w && y>=0 && y < dest->h)
        ((Uint32*)dest->pixels)[y*dest->pitch/4+x] = SDL_MapRGB(dest->format,r,g,b);
}

void drawCircle(SDL_Surface* dest, float x, float y)
{
        putPixelc(dest, xc+x, (yc+y),255,255,255);
        putPixelc(dest, (yc+y), (xc+x),255,255,255);
        putPixelc(dest, xc-x, (yc+y),255,255,255);
        putPixelc(dest, (yc-y), (xc+x),255,255,255);

        putPixelc(dest, xc+x, (yc-y),255,255,255);
        putPixelc(dest, (yc+y), (xc-x),255,255,255);
        putPixelc(dest, xc-x, (yc-y),255,255,255);
        putPixelc(dest, (yc-y), (xc-x),255,255,255);

}

void midPointAlgo(SDL_Surface* dest, float xcenter, float ycenter, float r)
{
        xc = xcenter;
        yc = ycenter;
        float x=0,y=r;
        float h=1-r;
        float deltaE=3;
        float deltaSE= -2*r+5;
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
