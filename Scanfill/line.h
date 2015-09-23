#ifndef LINE_H
#define LINE_H

int abs( int, int);
void putPixel(SDL_Surface *dest, int x, int y);
void swapValue(int &a, int &b);
void drawLine(SDL_Surface* dest, float x0, float y0, float x1, float y1);

#endif // LINE_H
