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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/** Render a character in std large font 
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param col size-dependent column to start rendering.
 * \param row size-dependent row to start rendering.
 * \param character ascii character to render.
 * \param sx horizontal top-left pixel location.
 * \param sy vertical top-left pixel location.
*/
void
g15r_renderCharacterLarge (g15canvas * canvas, int col, int row,
			   unsigned char character, unsigned int sx,
			   unsigned int sy)
{
    unsigned char buf[2];
    buf[0]=character;
    buf[1]=0;
    g15r_G15FPrint (canvas, (char*)buf, ((col * 8) + sx), sy, G15_TEXT_LARGE, 0, G15_COLOR_BLACK, row);
}

/** Render a character in std medium font.
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param col size-dependent column to start rendering.
 * \param row size-dependent row to start rendering.
 * \param character ascii character to render.
 * \param sx horizontal top-left pixel location.
 * \param sy vertical top-left pixel location.
*/
void
g15r_renderCharacterMedium (g15canvas * canvas, int col, int row,
			    unsigned char character, unsigned int sx,
			    unsigned int sy)
{
    unsigned char buf[2];
    buf[0]=character;
    buf[1]=0;
    g15r_G15FPrint (canvas, (char*)buf, ((col * 5) + sx), sy, G15_TEXT_MED, 0, G15_COLOR_BLACK, row);

}

/** Render a character in std small font.
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param col size-dependent column to start rendering.
 * \param row size-dependent row to start rendering.
 * \param character ascii character to render.
 * \param sx horizontal top-left pixel location.
 * \param sy vertical top-left pixel location.
*/
void
g15r_renderCharacterSmall (g15canvas * canvas, int col, int row,
			   unsigned char character, unsigned int sx,
			   unsigned int sy)
{
    unsigned char buf[2];
    buf[0]=character;
    buf[1]=0;
    g15r_G15FPrint (canvas, (char*)buf, ((col * 4) + sx), sy, G15_TEXT_SMALL, 0, G15_COLOR_BLACK, row);

}

/** 
 * Render a string in the designated size
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param stringOut An unsigned char pointer to the string which is to be printed.
 * \param row size-dependent row to start rendering.
 * \param size size of printed string.  May be 0-3 for standard sizes, or 5-39 in pixel height.
 * \param sx horizontal top-left pixel location.
 * \param sy vertical top-left pixel location.
*/
void
g15r_renderString (g15canvas * canvas, unsigned char stringOut[], int row,
		   int size, unsigned int sx, unsigned int sy)
{
      g15r_G15FPrint (canvas, (char*)stringOut, sx, sy, size, 0, G15_COLOR_BLACK, row);
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

  y += ttf_fontsize * .75;

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
    else { /* fall back to our default bitmap font */
        g15r_G15FPrint (canvas, print_string, x, y, fontsize, center, color, 0);
    }
}

#endif /* TTF_SUPPORT */

/* G15Font Support */

/** 
 * Load a g15 font from file.
 * \param filename string containing full name and location of font to load.
 * \return pointer to completed g15font structure.
*/
g15font * g15r_loadG15Font(char *filename) {
    FILE *file;
    g15font *font = calloc(1,sizeof(g15font));
    unsigned char buffer[128];
    int i;
    if(access(filename,F_OK)!=0) {
        fprintf(stderr,"loadG15Font: %s doesn't exist or has permissions problem.\n",filename);
        return NULL;
    }
    if(access(filename,R_OK)!=0) {
        fprintf(stderr,"loadG15Font: %s exists but cannot be read. Permissions problem?\n",filename);
        return NULL;
    }
    
    if(!(file=fopen(filename,"rb")))
        return NULL;

    if(fread(buffer,G15_FONT_HEADER_SIZE,1,file)) {
      if(buffer[0] != 'G' ||
         buffer[1] != 'F' ||
         buffer[2] != 'N' ||
         buffer[3] != 'T' )
      {
          fclose(file);
          return NULL;
      }
    } else
      return NULL;
    font->font_height = buffer[4] | (buffer[5] << 8);
    font->ascender_height = buffer[6] | (buffer[7] << 8);
    font->lineheight = buffer[8] | (buffer[9] << 8);
    
    /* any future expansion that require more than one bit to be set should be recorded at the end of the file, */
    /* with the extended-feature bit (not defined as yet, probably 1) set here */
    /* The first byte of the extended packet should indicate the extension data type (none defined yet) */
    /* The second byte should indicate length in bytes of the packet to read, not inclusive of these two bytes */
    /* followed by the extended data.  This should allow for a degree of backward compatibility between future versions */
    /* should they arise. */
    
    /* features = buffer[10] | (buffer[11] << 8) */
    
    font->numchars = buffer[12] | (buffer[13] << 8);
    font->default_gap = buffer[14];
    unsigned int current_alloc = 2048;
    char *glyphBuf =  malloc(current_alloc);
    char *glyphPtr = glyphBuf;
    unsigned int memsize=0;
    for (i=0;i <font->numchars; i++) {
        unsigned char charheader[G15_CHAR_HEADER_SIZE];
        unsigned int character;
        fread(charheader, G15_CHAR_HEADER_SIZE, 1, file);
        character = charheader[0] | (charheader[1] << 8);
        font->glyph[character].width = charheader[2] | (charheader[3] << 8);
        memsize+=(font->font_height * ((font->glyph[character].width + 7) / 8));
        if(memsize>current_alloc) { /* attempt to semi-coherently allocate additional memory */
            current_alloc+=((font->font_height * ((font->glyph[character].width + 7) / 8)*(font->numchars - i)));
            glyphBuf = realloc(glyphBuf,(size_t)current_alloc);
        }
        font->glyph[character].buffer = (unsigned char*)glyphPtr;
        font->glyph[character].gap = 0;
        fread(font->glyph[character].buffer, font->font_height * ((font->glyph[character].width + 7) / 8), 1, file);
        glyphPtr+=(font->font_height * ((font->glyph[character].width + 7) / 8));
    }
    
    fclose(file);
    return (font);  
}

/** 
 * Save g15font struct to given file.
 * \param ofilename string containing full name and location of font to save.
 * \param font g15font structure containing glyphs.  Glyphs to be saved should have the corresponding active[glyph] set.
 * \return 0 on success, -1 on failure.
*/

int g15r_saveG15Font(char *oFilename, g15font *font) {
    FILE *f;
    unsigned int i;
    unsigned char fntheader[G15_FONT_HEADER_SIZE];
 
    if(font==NULL)
        return -1;
    
    f = fopen(oFilename, "w+b");
    if(f==NULL)
        return -1;

    font->numchars=0;
    for(i=0;i<256;i++) {
        if(font->active[i]) {
            font->numchars++;
        }
    }

    fntheader[0] = 'G';
    fntheader[1] = 'F';
    fntheader[2] = 'N';
    fntheader[3] = 'T';
    fntheader[4] = (unsigned char)font->font_height;
    fntheader[5] = (unsigned char)(font->font_height >> 8);
    fntheader[6] = (unsigned char)font->ascender_height;
    fntheader[7] = (unsigned char)(font->ascender_height >> 8);
    fntheader[8] = (unsigned char)font->lineheight;
    fntheader[9] = (unsigned char)(font->lineheight >> 8);
    /* any future expansion that require more than one bit to be set should be recorded at the end of the file, */
    /* with the extended-feature bit (not defined as yet, probably 1) set here */
    /* The first byte of the extended packet should indicate the extension data type (none defined yet) */
    /* The second byte should indicate length in bytes of the packet to read, not inclusive of these two bytes */
    /* followed by the extended data.  This should allow for a degree of backward compatibility between future versions */
    /* should they arise. */
    fntheader[10] = 0; /* features */
    fntheader[11] = 0; /* features >> 8 */
    fntheader[12] = (unsigned char)font->numchars;
    fntheader[13] = (unsigned char)(font->numchars >> 8);
    fntheader[14] = (unsigned char)font->default_gap;

    fwrite (fntheader, G15_FONT_HEADER_SIZE, 1, f);

    for(i=0;i<256;i++) {
        if(font->active[i]) {
            unsigned char charheader[G15_CHAR_HEADER_SIZE];

            charheader[0] = (unsigned char)i;
            charheader[1] = (unsigned char)(i >> 8);
            charheader[2] = (unsigned char)font->glyph[i].width;
            charheader[3] = (unsigned char)(font->glyph[i].width >> 8);
            fwrite(charheader,G15_CHAR_HEADER_SIZE,1,f);
            fwrite(font->glyph[i].buffer,font->font_height * ((font->glyph[i].width + 7) / 8),1,f);
        }
    }
    fclose(f);
    return 0;
}

/** 
 * De-allocate memory associated with g15font struct, including glyph buffers
  * \param font g15font structure containing glyphs.
*/
void g15r_deleteG15Font(g15font*font){

    if(font) {
        if(font->glyph_buffer!=NULL)
            free(font->glyph_buffer);
        free(font);
    }
}

/** 
 * Calculate width (in pixels) of given string if rendered in font 'font'.
 * \param font Loaded g15font structure as returned by g15r_loadG15Font()
 * \param string Pointer to string for width calculations.
 * \return total width in pixels of given string.
*/
int g15r_testG15FontWidth(g15font *font,char *string){
    int i;
    int totalwidth=0;
    if(font==NULL) return 0;
    
    font->glyph[32].gap = 0;

    for(i=0;i<strlen(string);i++)
        totalwidth += font->glyph[(int)string[i]].width + font->glyph[(int)string[i]].gap + font->default_gap;

    return totalwidth;
}

/** Render a character in given font.
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param font Loaded g15font structure as returned by g15r_loadG15Font()
 * \param character ascii character to render.
 * \param top_left_pixel_x horizontal top-left pixel location.
 * \param top_left_pixel_y vertical top-left pixel location.
 * \param colour desired colour of character when rendered.
*/
int g15r_renderG15Glyph(g15canvas *canvas, g15font *font,unsigned char character,int top_left_pixel_x, int top_left_pixel_y, int colour, int paint_bg)
{
    int x,y,w,bp;

    if(font->glyph[character].buffer==NULL)
        return 0;

    unsigned char *buffer = font->glyph[character].buffer;
    unsigned int height = font->font_height;
    int i = 0;

    int bufferlen = height * ((font->glyph[character].width + 7) / 8);
    top_left_pixel_y-=font->font_height - font->ascender_height - 1 ;

    w = font->glyph[character].width + (7-(font->glyph[character].width % 8 ));
    if(font->glyph[character].width%8==0) 
        w = font->glyph[character].width-1;

    x=0;y=0;

    for (bp=0;bp<bufferlen ;bp++){
        for (i=0;i<8;i++) {
            if( x > w ) { 
                x=0;y++;
            }
            x++;
            if(x<=font->glyph[character].width) {
              if( buffer[bp] & (0x80 >> i))
                g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,colour);
              else
                if(paint_bg)
                    g15r_setPixel (canvas, top_left_pixel_x + x, top_left_pixel_y + y,colour^1);
            }
        } 
    }
    if(character!=32)
        return font->glyph[character].width + font->default_gap;
    else
        return font->glyph[character].width;
}

/** Render a string in given font.
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param font Loaded g15font structure as returned by g15r_loadG15Font()
 * \param string Pointer to string to operate on.
 * \param row vertical font-dependent row to start printing on. can usually be left at 0
 * \param sx horizontal top-left pixel location.
 * \param sy vertical top-left pixel location.
 * \param colour desired colour of character when rendered.
 * \param paint_bg if !0, pixels in the glyph background will also be painted, obstructing any image behind the text.
*/
void g15r_G15FontRenderString (g15canvas * canvas, g15font *font, char *string, int row, unsigned int sx, unsigned int sy, int colour, int paint_bg)
{
    int i=0;
    int prevwidth=0;
    if(font==NULL)
        return;
    sy=sy+(font->ascender_height*row);
    for(i=0;i<strlen(string);i++){
        prevwidth=g15r_renderG15Glyph(canvas, font,string[i],sx+=prevwidth, sy,colour, paint_bg);
    }
}

/** Render a string in the default font.
 * \param canvas A pointer to a g15canvas struct in which the buffer to be operated on is found.
 * \param string Pointer to string to operate on.
 * \param x horizontal top-left pixel location.
 * \param y vertical top-left pixel location.
 * \param size if size>= 4, denotes height in pixels.  if size<4, standard font sizes are used.
 * \param center Desired text justification. 0==left, 1==centered, 2==right justified.
 * \param colour desired colour of character when rendered.
 * \param row vertical font-dependent row to start printing on. can usually be left at 0
*/
/* print string with the default G15Font, with on-demand loading of required sized bitmaps */
void g15r_G15FPrint (g15canvas *canvas, char *string, int x, int y, int size, int center, int colour, int row) {
  static g15font *defaultfont[40];
  char filename[128];
  int xc, paint_bg;
  
  if(size<0)
      size=0;
  if(size>39)
      size=39;

  /* check if previously loaded, otherwise load it now */
  if(!defaultfont[size]) {
    snprintf(filename,128,"%s/G15/default-%.2i.fnt",G15FONT_DIR,size);
    defaultfont[size] = g15r_loadG15Font(filename);
    if((defaultfont[size])==NULL) {
          fprintf(stderr,"libg15render: Unable to load font \"%s\"\n",filename);
          return;
      }
  }
  
  if(size<3)
  {
      paint_bg=1;
      x-=1;
      y-=1;
  }
  else
  {
      paint_bg=0;
  }

  switch(center) {
    case 0:
      g15r_G15FontRenderString (canvas, defaultfont[size], string, row, x, y, colour, paint_bg);
      break;
    case 1:
      xc = g15r_testG15FontWidth(defaultfont[size],string);
      g15r_G15FontRenderString (canvas, defaultfont[size], string, row, 80-(xc/2),y, colour, paint_bg);
      break;
    case 2:
      xc = g15r_testG15FontWidth(defaultfont[size],string);
      g15r_G15FontRenderString (canvas, defaultfont[size], string, row, 160-xc,y, colour, paint_bg);
      break;
  }
}

