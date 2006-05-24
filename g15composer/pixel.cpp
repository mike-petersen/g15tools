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

#include <iostream>

#include "pixel.h"
#include "screen.h"
#include "util.h"

using namespace std;

void pixelReverseFill(string const &input_line)
{
   int params[5] = { 0, 0, 0, 0, 0 };

   bool fill = input_line[1] == 'F';
   int color = 0;
   
   if(fill)
   {
      get_params(params, input_line, 3, 5);
      color = params[4];
   } else
      get_params(params, input_line, 3, 4);
   
   for(int x=params[0]; x<=params[2]; ++x)
   {
      for(int y=params[1]; y<=params[3]; ++y)
      {
         if(!fill) color = !getPixel(x, y);
         setPixel(x, y, color);
      }
   }
}

void pixelOverlay(string const &input_line)
{
   int params[4] = { 0, 0, 0, 0 };
   int ofs = get_params(params, input_line, 3, 4);
   int exp = params[2]*params[3];
   if(exp != (int) input_line.length()-ofs)
   {
      cout << "Error in Pixel Overlay Command (" << params[2] << "x" << params[3] << ")" << endl;
      cout << "I expect a total of " << exp << " 1s or 0s" << endl;
      return;
   }
   
   for (int i=0;i<exp;++i)
   {
      int color = input_line[ofs+i] == '1';
      int x = params[0]+i%params[2];
      int y = params[1]+i/params[2];
      setPixel(x, y, color);
   }
}

void drawLine(int px1, int py1, int px2, int py2, const int c)
{
   // Bresenham's Line Algorithm
   // http://en.wikipedia.org/wiki/Bresenham's_algorithm
   
   bool steep = abs(py2-py1) > abs(px2-px1);
   
   if(steep)
   {
      swap(px1,py1);
      swap(px2,py2);
   }
   
   if(px1>px2)
   {
      swap(px1,px2);
      swap(py1,py2);
   }
   
   int dx = px2 - px1;
   int dy = abs(py2 - py1);
   
   int error = 0;
   int y = py1;
   int ystep = (py1 < py2) ? 1 : -1;
   
   for(int x=px1; x<=px2; ++x) 
   {
      if(steep)
         setPixel(y,x,c);
      else
         setPixel(x,y,c);
         
      error += dy;
      if(2*error >= dx)
      {
         y += ystep;
         error -= dx;
      }
   }
}

void pixelLine(string const &input_line)
{
   int params[5] = { 0, 0, 0, 0, 1 };
   get_params(params, input_line, 3, 5);
   
   drawLine(params[0], params[1], params[2], params[3], params[4]);

}

void pixelBox(string const &input_line)
{
   int params[7] = { 0, 0, 0, 0, 1, 1, 0 };
   get_params(params, input_line, 3, 7);
   
   int x1 = params[0];
   int y1 = params[1];
   int x2 = params[2];
   int y2 = params[3];
   int color = params[4];
   int thick = params[5];
   int fill  = params[6];
   
   for(int i=0; i<thick; ++i)
   {
      drawLine(x1,y1,x2,y1,color); // Top
      drawLine(x1,y1,x1,y2,color); // Left
      drawLine(x2,y1,x2,y2,color); // Right
      drawLine(x1,y2,x2,y2,color); // Bottom
      x1++; y1++;
      x2--; y2--;
  }
  
   if(fill)
   {
      for(int x=x1; x<=x2; ++x)
         for(int y=y1; y<=y2; ++y)
            setPixel(x, y, color);
   }

}

void handlePixelCommand(string const &input_line)
{
   if (input_line[1] == 'S') // Set
   {
      int params[3] = { 0, 0, 0 };
      get_params(params, input_line, 3, 3);
      setPixel(params[0], params[1], params[2]);
   }
   else if (input_line[1] == 'R' || input_line[1] == 'F')
   {
      pixelReverseFill(input_line);
   }
   else if (input_line[1] == 'O')
   {
      pixelOverlay(input_line);
   }
   else if (input_line[1] == 'L')
   {
      pixelLine(input_line);
   }
   else if (input_line[1] == 'B')
   {
      pixelBox(input_line);
   }
   else if (input_line[1] == 'C')
   {
      clearScreen(input_line.length() < 4 || input_line[3] == '1');
   }
   updateScreen();
}
