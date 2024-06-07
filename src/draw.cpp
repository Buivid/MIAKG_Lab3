#include "draw.h"




#define RGB32(r, g, b) static_cast<uint32_t>((((static_cast<uint32_t>(b) << 8) | g) << 8) | r)

void put_pixel32(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  if((x > 0 && x < SCREEN_WIDTH) && (y > 0 && y < SCREEN_HEIGHT))
   {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = pixel;
   }
}

Uint32 get_pixel32(SDL_Surface *surface, int x, int y)
{
  assert(NULL != surface);
  assert(x < SCREEN_WIDTH);
  assert(y < SCREEN_HEIGHT);

  Uint32 *pixels = (Uint32 *)surface->pixels;
  return pixels[(y * surface->w) + x];
}

void CirclePoints (SDL_Surface *s, int x, int y, int color)
{
  int xCenter=320;
  int yCenter=240;
  put_pixel32 ( s, xCenter + x, yCenter + y, color);
  put_pixel32 ( s, xCenter + y, yCenter + x, color);
  put_pixel32( s, xCenter + y, yCenter - x, color);
  put_pixel32 ( s, xCenter + x, yCenter - y, color);
  put_pixel32( s, xCenter - x, yCenter - y, color);
  put_pixel32 ( s, xCenter - y, yCenter - x, color);
  put_pixel32 ( s, xCenter - y, yCenter + x, color);
 put_pixel32( s, xCenter - x, yCenter + y, color);
}




int computeCode(int x, int y) {
    int code = INSIDE;
    if (x < X_MIN) code |= LEFT;
    else if (x > X_MAX) code |= RIGHT;
    if (y < Y_MIN) code |= BOTTOM;
    else if (y > Y_MAX) code |= TOP;
    return code;
}

void cohenSutherland(float *xstart, float *ystart, float *xend, float *yend, int *flag)
{
  int codeStart = computeCode(*xstart, *ystart);
  int codeEnd = computeCode(*xend, *yend);
  int has_hanged = 0;

  bool accept = false;

  while (true) {
    if (codeStart == 0 && codeEnd == 0) {
      if (has_hanged==0)
        *flag = 1;
      else
        *flag = 3;
      accept = true;
      break;
    }
    else if (codeStart & codeEnd) {

      if (has_hanged == 0)
        *flag = 0;
      else
        *flag = 4;
      break;
    } else {
      int code = (codeStart != 0) ? codeStart : codeEnd;
      double x, y;
      if (code & TOP) {
        x = *xstart + (*xend - *xstart) * (Y_MAX - *ystart) / (*yend - *ystart);
        y = Y_MAX;
      } else if (code & BOTTOM) {
        x = *xstart + (*xend - *xstart) * (Y_MIN - *ystart) / (*yend - *ystart);
        y = Y_MIN;
      }
      else if (code & RIGHT) {
        y = *ystart + (*yend - *ystart) * (X_MAX - *xstart) / (*xend - *xstart);
        x = X_MAX;
      } else if (code & LEFT) {
        y = *ystart + (*yend - *ystart) * (X_MIN - *xstart) / (*xend - *xstart);
        x = X_MIN;
      }
        if (code == codeStart) {
          *xstart = x;
          *ystart = y;
          codeStart = computeCode(*xstart, *ystart);
        } else {
          *xend = x;
          *yend = y;
          codeEnd = computeCode(*xend, *yend);
        }
        has_hanged = 1;
    }

  }
}

void line (SDL_Surface *s, int x1, int y1, int x2, int y2, int color)
{
 int dx = abs ( x2 - x1 );
 int dy = abs ( y2 - y1 );
 int sx = x2 >= x1 ? 1 : -1;
 int sy = y2 >= y1 ? 1 : -1;

 if ( dy <= dx ) {
 int d = ( dy << 1 ) - dx;
 int d1 = dy << 1;
 int d2 = (dy-dx)<<1;
 put_pixel32( s, x1, y1, color);
 for (int x=x1 +sx, y=y1, i=1; i <= dx; i++, x+=sx) {
 if (d > 0 ) {
 d += d2; y += sy;
 }
 else
 d+=d1;
 put_pixel32( s, x, y, color);
 }
 }
 else {
 int d = ( dx << 1 ) - dy;
 int d1 = dx << 1;
 int d2 = (dx-dy)<<1;

 put_pixel32(s, x1, y1, color);
 for (int x=x1, y=y1+sy, i=1; i <= dy; i++, y+=sy) {
 if ( d > 0 ) {
 d += d2; x += sx;
 }
 else
 d +=d1;
 put_pixel32(s, x, y, color);
 }
 }
}


void draw(SDL_Surface *s, Circle circle, Rectangle window, MODE mode)
{
  // line(s, X_MIN, Y_MIN, X_MAX, Y_MIN, RGB32(10, 150, 200));
  // line(s, X_MAX, Y_MIN, X_MAX, Y_MAX, RGB32(10, 150, 200));
  // line(s, X_MAX, Y_MAX, X_MIN, Y_MAX, RGB32(10, 150, 200));
  // line(s, X_MIN, Y_MAX, X_MIN, Y_MIN, RGB32(10, 150, 200));

  window.transform();
  window.draw(s);

  circle.transform();
  circle.draw(s, mode);
  circle.fill(s);



  // или использу¤ макрос можно получить код цвета:
  //   RGB32(0, 255, 0) эквивалентно записи 0x0000FF00
}
