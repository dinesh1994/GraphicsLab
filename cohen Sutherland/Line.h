#ifndef LINE_H
#define LINE_H
#include <iostream>
#include <SDL/SDL.h>
#include "Vertex.h"
#include <stdio.h>


int abs(int x);
void putPixel(SDL_Surface* dest, int x, int y, int r, int g, int b);
void swapValue(int &a,int &b);
void drawLine(SDL_Surface* dest, int x0, int y0, int x1, int y1);

#endif

