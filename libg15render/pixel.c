/*
    This file is part of g15tools.

    g15tools is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15tools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15lcd; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "libg15render.h"

void swap(int *x, int *y)
{
    int tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;
}

void g15r_pixelReverseFill(g15canvas * canvas, int x1, int y1, int x2, int y2, int fill, int color)
{
   int x=0;
   int y=0;
      
   for(x=x1; x<=x2; ++x)
   {
      for(y=y1; y<=y2; ++y)
      {
         if(!fill) color = !g15r_getPixel(canvas, x, y);
         g15r_setPixel(canvas, x, y, color);
      }
   }
}

void g15r_pixelOverlay(g15canvas * canvas, int x1, int y1, int width, int height, int colormap[])
{
   int i=0;
   
   for (i=0;i<(width*height);++i)
   {
      int color=0;
      if (colormap[i] != 0)
         color = 1;
      int x = x1 + i % width;
      int y = y1 + i / width;
      g15r_setPixel(canvas, x, y, color);
   }
}

void g15r_drawLine(g15canvas * canvas, int px1, int py1, int px2, int py2, const int c)
{
   // Bresenham's Line Algorithm
   // http://en.wikipedia.org/wiki/Bresenham's_algorithm
   
   int steep=0;
   
   if (abs(py2-py1) > abs(px2-px1))
      steep = 1;
   
   if(steep)
   {
      swap(&px1,&py1);
      swap(&px2,&py2);
   }
   
   if(px1>px2)
   {
      swap(&px1,&px2);
      swap(&py1,&py2);
   }
   
   int dx = px2 - px1;
   int dy = abs(py2 - py1);
   
   int error = 0;
   int y = py1;
   int ystep = (py1 < py2) ? 1 : -1;
   int x=0;
   
   for(x=px1; x<=px2; ++x) 
   {
      if(steep)
         g15r_setPixel(canvas, y,x,c);
      else
         g15r_setPixel(canvas, x,y,c);
         
      error += dy;
      if(2*error >= dx)
      {
         y += ystep;
         error -= dx;
      }
   }
}

void g15r_pixelBox(g15canvas * canvas, int x1, int y1, int x2, int y2, int color, int thick, int fill)
{
   int i=0;
   for(i=0; i<thick; ++i)
   {
      g15r_drawLine(canvas, x1,y1,x2,y1,color); // Top
      g15r_drawLine(canvas, x1,y1,x1,y2,color); // Left
      g15r_drawLine(canvas, x2,y1,x2,y2,color); // Right
      g15r_drawLine(canvas, x1,y2,x2,y2,color); // Bottom
      x1++; y1++;
      x2--; y2--;
  }
  
  int x=0, y=0;
  
   if(fill)
   {
      for(x=x1; x<=x2; ++x)
         for(y=y1; y<=y2; ++y)
            g15r_setPixel(canvas, x, y, color);
   }

}

