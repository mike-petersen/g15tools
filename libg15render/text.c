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

void
g15r_renderCharacterLarge (g15canvas * canvas, int col, int row,
			   unsigned char character, unsigned int sx,
			   unsigned int sy)
{
  int helper = character * 8;	/* for our font which is 8x8 */

  int top_left_pixel_x = sx + col * (8);	/* 1 pixel spacing */
  int top_left_pixel_y = sy + row * (8);	/* once again 1 pixel spacing */

  int x, y;
  for (y = 0; y < 8; ++y)
    {
      for (x = 0; x < 8; ++x)
	{
	  char font_entry = fontdata_8x8[helper + y];

	  if (font_entry & 1 << (7 - x))
	    g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,
			   G15_COLOR_BLACK);
	  else
	    g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,
			   G15_COLOR_WHITE);

	}
    }
}

void
g15r_renderCharacterMedium (g15canvas * canvas, int col, int row,
			    unsigned char character, unsigned int sx,
			    unsigned int sy)
{
  int helper = character * 7 * 5;	/* for our font which is 6x4 */

  int top_left_pixel_x = sx + col * (5);	/* 1 pixel spacing */
  int top_left_pixel_y = sy + row * (7);	/* once again 1 pixel spacing */

  int x, y;
  for (y = 0; y < 7; ++y)
    {
      for (x = 0; x < 5; ++x)
	{
	  char font_entry = fontdata_7x5[helper + y * 5 + x];
	  if (font_entry)
	    g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,
			   G15_COLOR_BLACK);
	  else
	    g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,
			   G15_COLOR_WHITE);

	}
    }
}

void
g15r_renderCharacterSmall (g15canvas * canvas, int col, int row,
			   unsigned char character, unsigned int sx,
			   unsigned int sy)
{
  int helper = character * 6 * 4;	/* for our font which is 6x4 */

  int top_left_pixel_x = sx + col * (4);	/* 1 pixel spacing */
  int top_left_pixel_y = sy + row * (6);	/* once again 1 pixel spacing */

  int x, y;
  for (y = 0; y < 6; ++y)
    {
      for (x = 0; x < 4; ++x)
	{
	  char font_entry = fontdata_6x4[helper + y * 4 + x];
	  if (font_entry)
	    g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,
			   G15_COLOR_BLACK);
	  else
	    g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,
			   G15_COLOR_WHITE);

	}
    }
}

void
g15r_renderString (g15canvas * canvas, unsigned char stringOut[], int row,
		   int size, unsigned int sx, unsigned int sy)
{

  int i = 0;
  for (i; stringOut[i] != NULL; ++i)
    {
      switch (size)
	{
	case G15_TEXT_SMALL:
	  {
	    g15r_renderCharacterSmall (canvas, i, row, stringOut[i], sx, sy);
	    break;
	  }
	case G15_TEXT_MED:
	  {
	    g15r_renderCharacterMedium (canvas, i, row, stringOut[i], sx, sy);
	    break;
	  }
	case G15_TEXT_LARGE:
	  {
	    g15r_renderCharacterLarge (canvas, i, row, stringOut[i], sx, sy);
	    break;
	  }
	default:
	  break;
	}
    }

}

#ifdef TTF_SUPPORT
/**
 * Load a font for use with FreeType2 font support
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param fontname Absolute pathname to font file to be loaded.
 * \param fontsize Size in points for font to be loaded.
 * \param face_num Slot into which font face will be loaded.
 */
void
g15r_ttfLoad (g15canvas * canvas, char *fontname, int fontsize, int face_num)
{
  int errcode = 0;

  if (face_num < 0)
    face_num = 0;
  if (face_num > G15_MAX_FACE)
    face_num = G15_MAX_FACE;

  if (canvas->ttf_fontsize[face_num])
    FT_Done_Face (canvas->ttf_face[face_num][0]);	/* destroy the last face */

  if (!canvas->ttf_fontsize[face_num] && !fontsize)
    canvas->ttf_fontsize[face_num] = 10;
  else
    canvas->ttf_fontsize[face_num] = fontsize;

  errcode =
    FT_New_Face (canvas->ftLib, fontname, 0, &canvas->ttf_face[face_num][0]);
  if (errcode)
    {
      canvas->ttf_fontsize[face_num] = 0;
    }
  else
    {
      if (canvas->ttf_fontsize[face_num]
	  && FT_IS_SCALABLE (canvas->ttf_face[face_num][0]))
	errcode =
	  FT_Set_Char_Size (canvas->ttf_face[face_num][0], 0,
			    canvas->ttf_fontsize[face_num] * 64, 90, 0);
    }
}

int
calc_ttf_true_ypos (FT_Face face, int y, int ttf_fontsize)
{

  if (!FT_IS_SCALABLE (face))
    ttf_fontsize = face->available_sizes->height;

  y += ttf_fontsize - (ttf_fontsize * .25);

  return y;
}

int
calc_ttf_totalstringwidth (FT_Face face, char *str)
{
  FT_GlyphSlot slot = face->glyph;
  FT_UInt glyph_index;
  int i, errcode;
  unsigned int len = strlen (str);
  int width = 0;

  for (i = 0; i < len; i++)
    {
      glyph_index = FT_Get_Char_Index (face, str[i]);
      errcode = FT_Load_Glyph (face, glyph_index, 0);
      width += slot->advance.x >> 6;
    }
  return width;
}

int
calc_ttf_centering (FT_Face face, char *str)
{
  int leftpos;

  leftpos = 80 - (calc_ttf_totalstringwidth (face, str) / 2);
  if (leftpos < 1)
    leftpos = 1;

  return leftpos;
}

int
calc_ttf_right_justify (FT_Face face, char *str)
{
  int leftpos;

  leftpos = 160 - calc_ttf_totalstringwidth (face, str);
  if (leftpos < 1)
    leftpos = 1;

  return leftpos;
}

void
draw_ttf_char (g15canvas * canvas, FT_Bitmap charbitmap,
	       unsigned char character, int x, int y, int color)
{
  FT_Int char_x, char_y, p, q;
  FT_Int x_max = x + charbitmap.width;
  FT_Int y_max = y + charbitmap.rows;
  static FT_Bitmap tmpbuffer;

  /* convert to 8bit format.. */
  FT_Bitmap_Convert (canvas->ftLib, &charbitmap, &tmpbuffer, 1);

  for (char_y = y, q = 0; char_y < y_max; char_y++, q++)
      for (char_x = x, p = 0; char_x < x_max; char_x++, p++)
	  if (tmpbuffer.buffer[q * tmpbuffer.width + p])
	    g15r_setPixel (canvas, char_x, char_y, color);
}

void
draw_ttf_str (g15canvas * canvas, char *str, int x, int y, int color,
	      FT_Face face)
{
  FT_GlyphSlot slot = face->glyph;
  int i, errcode;
  unsigned int len = strlen (str);

  for (i = 0; i < len; i++)
    {
      errcode =
	FT_Load_Char (face, str[i],
		      FT_LOAD_RENDER | FT_LOAD_MONOCHROME |
		      FT_LOAD_TARGET_MONO);
      draw_ttf_char (canvas, slot->bitmap, str[i], x + slot->bitmap_left,
		     y - slot->bitmap_top, color);
      x += slot->advance.x >> 6;
    }
}

/**
 * Render a string with a FreeType2 font
 * 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param x initial x position for string.
 * \param y initial y position for string.
 * \param fontsize Size of string in points.
 * \param face_num Font to be used is loaded in this slot.
 * \param color Text will be drawn this color.
 * \param center Text will be centered if center == 1 and right justified if center == 2.
 * \param print_string Pointer to the string to be printed.
 */
void
g15r_ttfPrint (g15canvas * canvas, int x, int y, int fontsize, int face_num,
	       int color, int center, char *print_string)
{
  int errcode = 0;

  if (canvas->ttf_fontsize[face_num])
    {
      if (fontsize > 0 && FT_IS_SCALABLE (canvas->ttf_face[face_num][0]))
	{
	  canvas->ttf_fontsize[face_num] = fontsize;
	  int errcode =
	    FT_Set_Pixel_Sizes (canvas->ttf_face[face_num][0], 0,
				canvas->ttf_fontsize[face_num]);
	  if (errcode)
	    printf ("Trouble setting the Glyph size!\n");
	}
      y =
	calc_ttf_true_ypos (canvas->ttf_face[face_num][0], y,
			    canvas->ttf_fontsize[face_num]);
      if (center == 1)
	x = calc_ttf_centering (canvas->ttf_face[face_num][0], print_string);
      else if (center == 2)
        x = calc_ttf_right_justify (canvas->ttf_face[face_num][0], print_string);
      draw_ttf_str (canvas, print_string, x, y, color,
		    canvas->ttf_face[face_num][0]);
    }
}
#endif /* TTF_SUPPORT */
