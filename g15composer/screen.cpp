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
#include "screen.h"
#include "util.h"

static bool mode_xor = false;
static bool mode_reverse = false;
static bool mode_cache = false;
static unsigned char buffer[ 160*43 ];

using namespace std;

int getPixel(unsigned int x, unsigned int y)
{
   if (x >= 160 || y >= 43)
      return 0;
   
   int row = y / 8;
   int offset = 160*row + x;
   int bit = y % 8;
    
   return (buffer[offset] & (1 << bit)) >> bit;
}


void setPixel(unsigned int x, unsigned int y, int val)
{
   if (x >= 160 || y >= 43)
      return;
   
   int row = y / 8;
   int offset = 160*row + x;
   int bit = y % 8;
    
   if (mode_xor) val ^= getPixel(x, y);
   if (mode_reverse) val = !val;

   if (val)
      buffer[offset] = buffer[offset] | 1 << bit;
   else
      buffer[offset] = buffer[offset]  &  ~(1 << bit);
   
}

void clearScreen(int color)
{
   memset(buffer, (color ? 0xFF: 0), 160*43);
}

void updateScreen(bool force)
{
   string pix = "";
   if(force || !mode_cache)
   {
      for(int y=0; y<43; ++y)
         for(int x=0; x<160; ++x)
            pix += getPixel(x, y) ? '1' : '0';
       
      cout << "P " << pix << endl;
   }
}

void handleModeCommand(string const &input_line)
{
   bool stat = input_line.substr(3,1) == "1";

   if(input_line[1] == 'C')
   {
      bool was_cached = mode_cache;
      mode_cache = stat;
      if(was_cached) 
         updateScreen(true);
   }
   else if(input_line[1] == 'R')
      mode_reverse = stat;
   else if(input_line[1] == 'X')
      mode_xor = stat;
}
