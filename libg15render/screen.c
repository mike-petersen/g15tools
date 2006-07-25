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

/**
 * Retrieves the value of the pixel at (x, y)
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param x X offset for pixel to be retrieved.
 * \param y Y offset for pixel to be retrieved.
 */
int g15r_getPixel(g15canvas * canvas, unsigned int x, unsigned int y)
{
   if (x >= G15_LCD_WIDTH || y >= G15_LCD_HEIGHT)
      return 0;
   
   int row = y / BYTE_SIZE;
   int offset = G15_LCD_WIDTH * row + x;
   int bit = y % BYTE_SIZE;
    
   return (canvas->buffer[offset] & (1 << bit)) >> bit;
}

/**
 * Sets the value of the pixel at (x, y)
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param x X offset for pixel to be set.
 * \param y Y offset for pixel to be set.
 * \param val Value to which pixel should be set.
 */
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

/**
 * Clears the screen and fills it with pixels of color
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param color Screen will be filled with this color.
 */
void g15r_clearScreen(g15canvas * canvas, int color)
{
   memset(canvas->buffer, (color ? 0xFF: 0), G15_BUFFER_LEN);
}

/**
 * Clears the screen and resets the mode values for a canvas
 * 
 * \param canvas A pointer to a g15canvas struct
 */
void g15r_initCanvas(g15canvas * canvas)
{
   memset(canvas->buffer, 0, G15_BUFFER_LEN);
   canvas->mode_cache = 0;
   canvas->mode_reverse = 0;
   canvas->mode_xor = 0;
}
