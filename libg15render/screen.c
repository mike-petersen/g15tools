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

int g15r_getPixel(g15canvas * canvas, unsigned int x, unsigned int y)
{
   if (x >= G15_LCD_WIDTH || y >= G15_LCD_HEIGHT)
      return 0;
   
   int row = y / BYTE_SIZE;
   int offset = G15_LCD_WIDTH * row + x;
   int bit = y % BYTE_SIZE;
    
   return (canvas->buffer[offset] & (1 << bit)) >> bit;
}


void g15r_setPixel(g15canvas * canvas, unsigned int x, unsigned int y, int val)
{
   if (x >= G15_LCD_WIDTH || y >= G15_LCD_HEIGHT)
      return;
   
   int row = y / BYTE_SIZE;
   int offset = G15_LCD_WIDTH * row + x;
   int bit = y % BYTE_SIZE;
    
   if (canvas->mode_xor) val ^= g15r_getPixel(canvas, x, y);
   if (canvas->mode_reverse) val = !val;

   if (val)
      canvas->buffer[G15_LCD_OFFSET + offset] = canvas->buffer[G15_LCD_OFFSET + offset] | 1 << bit;
   else
      canvas->buffer[G15_LCD_OFFSET + offset] = canvas->buffer[G15_LCD_OFFSET + offset]  &  ~(1 << bit);
   
}

void g15r_clearScreen(g15canvas * canvas, int color)
{
   memset(canvas->buffer, (color ? 0xFF: 0), G15_LCD_WIDTH * G15_LCD_HEIGHT);
}

void g15r_initCanvas(g15canvas * canvas)
{
   memset(canvas->buffer, 0, G15_LCD_WIDTH * G15_LCD_HEIGHT);
   canvas->mode_cache = 0;
   canvas->mode_reverse = 0;
   canvas->mode_xor = 0;
}