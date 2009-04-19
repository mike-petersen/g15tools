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
    along with libg15render; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "config.h"
#include "libg15render.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef TTF_SUPPORT
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#endif

#ifndef TTF_SUPPORT /* No truetype support */
int main(char *argv) {
    printf("g15fontconvert - libg15render has no FreeType support compiled in.  This is required for the conversion program.  Leaving now.\n");
    return -1;
}
#else /* TrueType Support */

void packpixel(unsigned char *buffer, int width, int x, int y,int colour) {

    if(x<0) x=0;
    if(y<0) y=0;
    unsigned char c = 0x80 >> (x % 8);
    if(colour)
        buffer[width * y + (x / 8)] |= c;
}

FT_Library lib;
int convertG15Font(char *inFilename, char *oFilename, int size, int gap){

    FT_Face face;
    FT_GlyphSlot glyphslot;
    g15font *font = NULL;
    int i;
    int retval;

    retval = FT_New_Face(lib, inFilename, 0, &face);
    if(retval== FT_Err_Unknown_File_Format){
        return -1;
    }
    else if(retval) {
        return -1;
    }

    glyphslot = face->glyph;
    if(FT_IS_SCALABLE(face)) {
            FT_Set_Char_Size(face, 0, size << 6, 80, 0);
    }else {
        printf("Bitmap Font has fixed height, ignoring requested size\n");
    }
    font = malloc(sizeof(g15font));
    font->font_height = (face->size->metrics.ascender >> 6) - (face->size->metrics.descender >> 6);
    font->ascender_height = face->size->metrics.ascender >> 6;
    font->lineheight = face->size->metrics.height >> 6;
    
    if(gap>-1)
      font->default_gap = gap;
    else
      font->default_gap = 1;
        
    if(font->font_height==0) {
        font->font_height = 2+(face->available_sizes->y_ppem>>6); /* */
        printf("Font height unknown, guessing %i.\n",font->font_height - 1);
    }
    if(font->ascender_height==0) {
        font->ascender_height = font->font_height;
        face->size->metrics.ascender = font->font_height<<6;
        printf("modded ascender to %i\n", (int)face->size->metrics.ascender>>6);
    }
    if(face->size->metrics.descender==0) {
        face->size->metrics.descender = (font->font_height<<6) - (face->available_sizes->size)  ;
        printf("modded descender to %i\n",(int)face->size->metrics.descender>>6);
         font->font_height = (face->size->metrics.ascender >> 6) + (face->size->metrics.descender >> 6);
    }
    if(font->lineheight==0) {
        printf("modded lineheight\n");
        font->lineheight=font->font_height;
    }
    
    printf("Converting\n");

    for(i=0;i<256;i++) {
        unsigned int glyph_index = FT_Get_Char_Index(face,i);

        retval = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT|FT_LOAD_TARGET_MONO|FT_LOAD_NO_BITMAP);  

        glyphslot = face->glyph;

        retval = FT_Render_Glyph(glyphslot, FT_RENDER_MODE_MONO);

        if(retval==0) {

            if(glyph_index>0)
                font->active[i]=1;
            else
                font->active[i]=0;

            int char_x, char_y;

            if(glyphslot->bitmap.width==0)
                glyphslot->bitmap.width = glyphslot->advance.x >> 6;  

            font->glyph[i].width = glyphslot->bitmap.width;

            font->glyph[i].buffer = malloc(glyphslot->bitmap.width*font->font_height);
            memset(font->glyph[i].buffer,0,glyphslot->bitmap.width*font->font_height);


            unsigned char * bufPtr = glyphslot->bitmap.buffer;

            for ( char_y = 0; char_y < glyphslot->bitmap.rows; char_y++)
            {
                char y=(face->size->metrics.ascender >> 6) - (face->glyph->metrics.horiBearingY >> 6);

                for ( char_x = 0; char_x < glyphslot->bitmap.width; char_x++)
                {
                    if((bufPtr[char_x / 8] >> (7 - char_x % 8)) & 1)
                        packpixel(font->glyph[i].buffer,(glyphslot->bitmap.width+7)/8, char_x,char_y+y,1);

                }
                bufPtr += glyphslot->bitmap.pitch;
            }
        }
    }

    FT_Done_Face(face);
    if(g15r_saveG15Font(oFilename,font)<0) {
        printf("Problem saving font\n");
        return -1;
    }
    printf("Font saved to %s\n",oFilename);
    g15r_deleteG15Font(font);
    return 0;
}

void helptext() {
    printf("g15fontconvert - (c) 2008 The G15Tools project\n");
    printf(" -h\t--help\t\t\tThis helptext\n");
    printf(" -s\t--size [size]\t\tSpecify size in pixels of desired font (default 10pixels high)\n");
    printf(" -g\t--gap [gap]\t\tSpecify gap in pixels between characters (default 1pixel)\n");
    printf(" -i\t--infile [filename]\tFilename of font to convert\n");
    printf(" -o\t--outfile [filename]\tname of file to write (default to [infile].fnt\n");
    exit(0);
}

int main(int argc, char **argv) {
    int i;
    int size = 10;
    int gap = -1;
    int have_infile=0;
    int have_outfile=0;
    char infile[128];
    char outfile[128];
    FT_Init_FreeType(&lib);
    
    if(argc<2)
        helptext();

    for (i=0;i<argc;i++) {
        if(0==strncasecmp((char*)argv[i],"-h",2) || 0==strncasecmp((char*)argv[i],"--help",6)){
            helptext();
        }
        if(0==strncasecmp((char*)argv[i],"-s",2) || 0==strncasecmp((char*)argv[i],"--size",6)){
            if(argv[i+1]!=NULL) {
                sscanf(argv[++i],"%i",&size);
            }
        }
        if(0==strncasecmp((char*)argv[i],"-g",2) || 0==strncasecmp((char*)argv[i],"--gap",5)){
            if(argv[i+1]!=NULL) {
                sscanf(argv[++i],"%i",&gap);
            }
        }
        if(0==strncasecmp((char*)argv[i],"-i",2) || 0==strncasecmp((char*)argv[i],"--infile",8)){
            if(argv[i+1]!=NULL) {
                strncpy(infile,argv[++i],127);
                have_infile=1;
            }
        }
        if(0==strncasecmp((char*)argv[i],"-o",2) || 0==strncasecmp((char*)argv[i],"--outfile",9)){
            if(argv[i+1]!=NULL) {
                strncpy(outfile,argv[++i],127);
                have_outfile=1;
            }
        }
    }
    if(!have_infile)
        helptext();
    
    if(!have_outfile)
        snprintf(outfile,128,"%s.fnt",infile);
    if(0!=strncasecmp(".fnt",outfile+(strlen(outfile)-4),4))
       strncat(outfile,".fnt",128);
    
    if(access(infile,O_RDONLY)!=0) {
        printf("g15fontconvert encountered an error:\n");
        printf("Unable to open %s.  Please check the filename and try again.\n",infile);
        exit(1);
    }

    printf("converting %s\n",infile);
    if(convertG15Font(infile, outfile, size,gap)<0)
        printf("problem saving font %s..\n",outfile);
    else
        printf("Done.\n");
    FT_Done_FreeType(lib);
    return 0;
}

#endif /* Truetype support */
