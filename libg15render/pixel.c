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

#include <fcntl.h>
#include "libg15render.h"

void
swap (int *x, int *y)
{
  int tmp;

  tmp = *x;
  *x = *y;
  *y = tmp;
}

/** 
 *  The area with an upper left corner at (x1, y1) and lower right corner at (x2, y2) will be
 *  filled with color if fill>0 or the current contents of the area will be reversed if fill==0.
 * 
 *  \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found. 
 *  \param x1 Defines leftmost bound of area to be filled.
 *  \param y1 Defines uppermost bound of area to be filled.
 *  \param x2 Defines rightmost bound of area to be filled.
 *  \param y2 Defines bottommost bound of area to be filled.
 *  \param fill Area will be filled with color if fill != 0, else contents of area will have color values reversed.
 *  \param color If fill != 0, then area will be filled if color == 1 and emptied if color == 0.
 */
void
g15r_pixelReverseFill (g15canvas * canvas, int x1, int y1, int x2, int y2,
		       int fill, int color)
{
  int x = 0;
  int y = 0;

  for (x = x1; x <= x2; ++x)
    {
      for (y = y1; y <= y2; ++y)
	{
	  if (!fill)
	    color = !g15r_getPixel (canvas, x, y);
	  g15r_setPixel (canvas, x, y, color);
	}
    }
}

/**
 * A 1-bit bitmap defined in colormap[] is drawn to the canvas with an upper left corner at (x1, y1)
 * and a lower right corner at (x1+width, y1+height).
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param x1 Defines the leftmost bound of the area to be drawn.
 * \param y1 Defines the uppermost bound of the area to be drawn.
 * \param width Defines the width of the bitmap to be drawn.
 * \param height Defines the height of the bitmap to be drawn.
 * \param colormap An array containing width*height entries of value 0 for pixel off or != 0 for pixel on.
 */
void
g15r_pixelOverlay (g15canvas * canvas, int x1, int y1, int width, int height,
		   short colormap[])
{
  int i = 0;

  for (i = 0; i < (width * height); ++i)
    {
      int color = (colormap[i] ? G15_COLOR_BLACK : G15_COLOR_WHITE);
      int x = x1 + i % width;
      int y = y1 + i / width;
      g15r_setPixel (canvas, x, y, color);
    }
}

/**
 * A line of color is drawn from (px1, py1) to (px2, py2).
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param px1 X component of point 1.
 * \param py1 Y component of point 1.
 * \param px2 X component of point 2.
 * \param py2 Y component of point 2.
 * \param color Line will be drawn this color.
 */
void
g15r_drawLine (g15canvas * canvas, int px1, int py1, int px2, int py2,
	       const int color)
{
  /* 
   * Bresenham's Line Algorithm
   * http://en.wikipedia.org/wiki/Bresenham's_algorithm
   */

  int steep = 0;

  if (abs (py2 - py1) > abs (px2 - px1))
    steep = 1;

  if (steep)
    {
      swap (&px1, &py1);
      swap (&px2, &py2);
    }

  if (px1 > px2)
    {
      swap (&px1, &px2);
      swap (&py1, &py2);
    }

  int dx = px2 - px1;
  int dy = abs (py2 - py1);

  int error = 0;
  int y = py1;
  int ystep = (py1 < py2) ? 1 : -1;
  int x = 0;

  for (x = px1; x <= px2; ++x)
    {
      if (steep)
	g15r_setPixel (canvas, y, x, color);
      else
	g15r_setPixel (canvas, x, y, color);

      error += dy;
      if (2 * error >= dx)
	{
	  y += ystep;
	  error -= dx;
	}
    }
}

/**
 * Draws a box around the area bounded by (x1, y1) and (x2, y2).
 * 
 * The box will be filled if fill != 0 and the sides will be thick pixels wide.
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found. 
 * \param x1 Defines leftmost bound of the box.
 * \param y1 Defines uppermost bound of the box.
 * \param x2 Defines rightmost bound of the box.
 * \param y2 Defines bottommost bound of the box.
 * \param color Lines defining the box will be drawn this color.
 * \param thick Lines defining the box will be this many pixels thick.
 * \param fill The box will be filled with color if fill != 0.
 */
void
g15r_pixelBox (g15canvas * canvas, int x1, int y1, int x2, int y2, int color,
	       int thick, int fill)
{
  int i = 0;
  for (i = 0; i < thick; ++i)
    {
      g15r_drawLine (canvas, x1, y1, x2, y1, color);	/* Top    */
      g15r_drawLine (canvas, x1, y1, x1, y2, color);	/* Left   */
      g15r_drawLine (canvas, x2, y1, x2, y2, color);	/* Right  */
      g15r_drawLine (canvas, x1, y2, x2, y2, color);	/* Bottom */
      x1++;
      y1++;
      x2--;
      y2--;
    }

  int x = 0, y = 0;

  if (fill)
    {
      for (x = x1; x <= x2; ++x)
	for (y = y1; y <= y2; ++y)
	  g15r_setPixel (canvas, x, y, color);
    }

}

/**
 * Draws a circle centered at (x, y) with a radius of r.
 * 
 * The circle will be filled if fill != 0.
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found. 
 * \param x Defines horizontal center of the circle.
 * \param y Defines vertical center of circle.
 * \param r Defines radius of circle.
 * \param fill The circle will be filled with color if fill != 0.
 * \param color Lines defining the circle will be drawn this color.
 */
void
g15r_drawCircle (g15canvas * canvas, int x, int y, int r, int fill, int color)
{
  int xx, yy, dd;

  xx = 0;
  yy = r;
  dd = 2 * (1 - r);

  while (yy >= 0)
    {
      if (!fill)
	{
	  g15r_setPixel (canvas, x + xx, y - yy, color);
	  g15r_setPixel (canvas, x + xx, y + yy, color);
	  g15r_setPixel (canvas, x - xx, y - yy, color);
	  g15r_setPixel (canvas, x - xx, y + yy, color);
	}
      else
	{
	  g15r_drawLine (canvas, x - xx, y - yy, x + xx, y - yy, color);
	  g15r_drawLine (canvas, x - xx, y + yy, x + xx, y + yy, color);
	}
      if (dd + yy > 0)
	{
	  yy--;
	  dd = dd - (2 * yy + 1);
	}
      if (xx > dd)
	{
	  xx++;
	  dd = dd + (2 * xx + 1);
	}
    }
}

/**
 * Draws a rounded box around the area bounded by (x1, y1) and (x2, y2).
 * 
 * The box will be filled if fill != 0.
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found. 
 * \param x1 Defines leftmost bound of the box.
 * \param y1 Defines uppermost bound of the box.
 * \param x2 Defines rightmost bound of the box.
 * \param y2 Defines bottommost bound of the box.
 * \param fill The box will be filled with color if fill != 0.
 * \param color Lines defining the box will be drawn this color.
 */
void
g15r_drawRoundBox (g15canvas * canvas, int x1, int y1, int x2, int y2,
		   int fill, int color)
{
  int y, shave = 3;

  if (shave > (x2 - x1) / 2)
    shave = (x2 - x1) / 2;
  if (shave > (y2 - y1) / 2)
    shave = (y2 - y1) / 2;

  if ((x1 != x2) && (y1 != y2))
    {
      if (fill)
	{
	  g15r_drawLine (canvas, x1 + shave, y1, x2 - shave, y1, color);
	  for (y = y1 + 1; y < y1 + shave; y++)
	    g15r_drawLine (canvas, x1 + 1, y, x2 - 1, y, color);
	  for (y = y1 + shave; y <= y2 - shave; y++)
	    g15r_drawLine (canvas, x1, y, x2, y, color);
	  for (y = y2 - shave + 1; y < y2; y++)
	    g15r_drawLine (canvas, x1 + 1, y, x2 - 1, y, color);
	  g15r_drawLine (canvas, x1 + shave, y2, x2 - shave, y2, color);
	  if (shave == 4)
	    {
	      g15r_setPixel (canvas, x1 + 1, y1 + 1,
			     color ==
			     G15_COLOR_WHITE ? G15_COLOR_BLACK :
			     G15_COLOR_WHITE);
	      g15r_setPixel (canvas, x1 + 1, y2 - 1,
			     color ==
			     G15_COLOR_WHITE ? G15_COLOR_BLACK :
			     G15_COLOR_WHITE);
	      g15r_setPixel (canvas, x2 - 1, y1 + 1,
			     color ==
			     G15_COLOR_WHITE ? G15_COLOR_BLACK :
			     G15_COLOR_WHITE);
	      g15r_setPixel (canvas, x2 - 1, y2 - 1,
			     color ==
			     G15_COLOR_WHITE ? G15_COLOR_BLACK :
			     G15_COLOR_WHITE);
	    }
	}
      else
	{
	  g15r_drawLine (canvas, x1 + shave, y1, x2 - shave, y1, color);
	  g15r_drawLine (canvas, x1, y1 + shave, x1, y2 - shave, color);
	  g15r_drawLine (canvas, x2, y1 + shave, x2, y2 - shave, color);
	  g15r_drawLine (canvas, x1 + shave, y2, x2 - shave, y2, color);
	  if (shave > 1)
	    {
	      g15r_drawLine (canvas, x1 + 1, y1 + 1, x1 + shave - 1, y1 + 1,
			     color);
	      g15r_drawLine (canvas, x2 - shave + 1, y1 + 1, x2 - 1, y1 + 1,
			     color);
	      g15r_drawLine (canvas, x1 + 1, y2 - 1, x1 + shave - 1, y2 - 1,
			     color);
	      g15r_drawLine (canvas, x2 - shave + 1, y2 - 1, x2 - 1, y2 - 1,
			     color);
	      g15r_drawLine (canvas, x1 + 1, y1 + 1, x1 + 1, y1 + shave - 1,
			     color);
	      g15r_drawLine (canvas, x1 + 1, y2 - 1, x1 + 1, y2 - shave + 1,
			     color);
	      g15r_drawLine (canvas, x2 - 1, y1 + 1, x2 - 1, y1 + shave - 1,
			     color);
	      g15r_drawLine (canvas, x2 - 1, y2 - 1, x2 - 1, y2 - shave + 1,
			     color);
	    }
	}
    }
}

/**
 * Given a maximum value, and a value between 0 and that maximum value, calculate and draw a bar showing that percentage.
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param x1 Defines leftmost bound of the bar.
 * \param y1 Defines uppermost bound of the bar.
 * \param x2 Defines rightmost bound of the bar.
 * \param y2 Defines bottommost bound of the bar.
 * \param color The bar will be drawn this color.
 * \param num Number of units relative to max filled.
 * \param max Number of units equal to 100% filled.
 * \param type Type of bar.  1=solid bar, 2=solid bar with border, 3 = solid bar with I-frame.
 */
void
g15r_drawBar (g15canvas * canvas, int x1, int y1, int x2, int y2, int color,
	      int num, int max, int type)
{
  float len, length;
  int x;
  if (num > max)
    num = max;

  if (type == 2)
    {
      y1 += 2;
      y2 -= 2;
      x1 += 2;
      x2 -= 2;
    }

  len = ((float) max / (float) num);
  length = (x2 - x1) / len;

  if (type == 1)
    {
      g15r_pixelBox (canvas, x1, y1 - type, x2, y2 + type, color ^ 1, 1, 1);
      g15r_pixelBox (canvas, x1, y1 - type, x2, y2 + type, color, 1, 0);
    }
  else if (type == 2)
    {
      g15r_pixelBox (canvas, x1 - 2, y1 - type, x2 + 2, y2 + type, color ^ 1,
		     1, 1);
      g15r_pixelBox (canvas, x1 - 2, y1 - type, x2 + 2, y2 + type, color, 1,
		     0);
    }
  else if (type == 3)
    {
      g15r_drawLine (canvas, x1, y1 - type, x1, y2 + type, color);
      g15r_drawLine (canvas, x2, y1 - type, x2, y2 + type, color);
      g15r_drawLine (canvas, x1, y1 + ((y2 - y1) / 2), x2,
		     y1 + ((y2 - y1) / 2), color);
    }
  g15r_pixelBox (canvas, x1, y1, (int) ceil (x1 + length), y2, color, 1, 1);
}

/* basic wbmp splash screen loader - assumes image is 160x43 */
int 
g15r_loadWbmpSplash(g15canvas *canvas, char *filename)
{
    int retval;
    int width=0, height=0;
     
    retval = g15r_loadWbmpToBuf(canvas->buffer,filename,
                                &width,
                                &height,
                                G15_BUFFER_LEN);
    return retval;
}

/* draw an icon at location my_x,my_y, from buf. width&height should be icons width/height */
void 
g15r_drawIcon(g15canvas *canvas, char *buf, int my_x, int my_y, int width, int height)
{
    int y,x,val;
    unsigned int pixel_offset = 0;
    unsigned int byte_offset, bit_offset;

    for (y=0; y < height - 1; y++)
      for (x=0; x < width - 1; x++)
        {
		byte_offset = pixel_offset / BYTE_SIZE;
		bit_offset = 7 - (pixel_offset % BYTE_SIZE);
		pixel_offset++;

		val = (buf[byte_offset] & (1 << bit_offset)) >> bit_offset;
		g15r_setPixel (canvas, x + my_x, y + my_y, val);
        }
}

/* basic wbmp loader - loads wbmp into pre-prepared buf.  sets img_height & img_width to image size */
int 
g15r_loadWbmpToBuf(char *buf, char *filename, int *img_width, int *img_height, int maxlen)
{
    int wbmp_fd;
    int retval;
    int x,y,val;
    unsigned int buflen,header=4;
    unsigned char headerbytes[5];
    unsigned int pixel_offset = 0;
    unsigned int byte_offset, bit_offset;
    
    if(maxlen<5 || buf == NULL)
        return -1;
    
    wbmp_fd=open(filename,O_RDONLY);
    if(!wbmp_fd){
        return -1;
    }
    
    retval=read(wbmp_fd,headerbytes,5);
    
    if(retval){
        if (headerbytes[2] & 1) {
            *img_width = ((unsigned char)headerbytes[2] ^ 1) | (unsigned char)headerbytes[3];
            *img_height = headerbytes[4];
            header = 5;
        } else {
            *img_width = headerbytes[2];
            *img_height = headerbytes[3];
            header = 4;
            buf[0]=headerbytes[4];
        }

	int byte_width = *img_width / 8;
	if (*img_width %8)
	  byte_width++;

        buflen = byte_width * (*img_height);
        if(buflen<maxlen)
            retval=read(wbmp_fd,buf+(5-header),buflen);
        else
            retval = -1;
        close(wbmp_fd);
    }

    /* now invert the image */
    for (y = 0; y < *img_height; y++)
      for (x = 0; x < *img_width; x++)
        {
		pixel_offset = y * (*img_width) + x;
		byte_offset = pixel_offset / BYTE_SIZE;
		bit_offset = 7 - (pixel_offset % BYTE_SIZE);

		val = (buf[byte_offset] & (1 << bit_offset)) >> bit_offset;

		if (!val)
	      	  buf[byte_offset] = buf[byte_offset] | 1 << bit_offset;
		else
		  buf[byte_offset] = buf[byte_offset] & ~(1 << bit_offset);
	}

    return retval;
}

void 
g15r_drawBigNum (g15canvas * canvas, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int num)
{
    x1 += 2;
    x2 -= 2;

    switch(num){
        case 45: 
            g15r_pixelBox (canvas, x1, y1+((y2/2)-2), x2, y1+((y2/2)+2), G15_COLOR_BLACK, 1, 1);
            break;
        case 46:
            g15r_pixelBox (canvas, x2-5, y2-5, x2, y2 , G15_COLOR_BLACK, 1, 1);
            break;
        case 48:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1 +5, y1 +5, x2 -5, y2 - 6, G15_COLOR_WHITE, 1, 1);
            break;
        case 49: 
            g15r_pixelBox (canvas, x2-5, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1, y1, x2 -5, y2, G15_COLOR_WHITE, 1, 1);
            break;
        case 50:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1, y1+5, x2 -5, y1+((y2/2)-3), G15_COLOR_WHITE, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1+((y2/2)+3), x2 , y2-6, G15_COLOR_WHITE, 1, 1);
            break;
        case 51:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1, y1+5, x2 -5, y1+((y2/2)-3), G15_COLOR_WHITE, 1, 1);
            g15r_pixelBox (canvas, x1, y1+((y2/2)+3), x2-5 , y2-6, G15_COLOR_WHITE, 1, 1);
            break;
        case 52:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1, y1+((y2/2)+3), x2 -5, y2, G15_COLOR_WHITE, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1, x2-5 , y1+((y2/2)-3), G15_COLOR_WHITE, 1, 1);
            break;
        case 53:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1+5, x2 , y1+((y2/2)-3), G15_COLOR_WHITE, 1, 1);
            g15r_pixelBox (canvas, x1, y1+((y2/2)+3), x2-5 , y2-6, G15_COLOR_WHITE, 1, 1);
            break;
        case 54:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1+5, x2 , y1+((y2/2)-3), G15_COLOR_WHITE, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1+((y2/2)+3), x2-5 , y2-6, G15_COLOR_WHITE, 1, 1);
            break;
        case 55:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1, y1+5, x2 -5, y2, G15_COLOR_WHITE, 1, 1);
            break;
        case 56:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1+5, x2-5 , y1+((y2/2)-3), G15_COLOR_WHITE, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1+((y2/2)+3), x2-5 , y2-6, G15_COLOR_WHITE, 1, 1);
            break;
        case 57:
            g15r_pixelBox (canvas, x1, y1, x2, y2 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x1+5, y1+5, x2-5 , y1+((y2/2)-3), G15_COLOR_WHITE, 1, 1);
            g15r_pixelBox (canvas, x1, y1+((y2/2)+3), x2-5 , y2, G15_COLOR_WHITE, 1, 1);
            break;
        case 58: 
            g15r_pixelBox (canvas, x2-5, y1+5, x2, y1+10 , G15_COLOR_BLACK, 1, 1);
            g15r_pixelBox (canvas, x2-5, y2-10, x2, y2-5 , G15_COLOR_BLACK, 1, 1);
            break;
    }
}

