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

#include <vector>

#include "util.h"
#include "text.h"
#include "screen.h"

using namespace std;

void renderCharacterLarge(int col, int row, unsigned char character, unsigned int sx=0, unsigned int sy=0)
{
   int helper = character * 8; // for our font which is 8x8
   
   int top_left_pixel_x = sx+col*(8); // 1 pixel spacing
   int top_left_pixel_y = sy+row*(8); // once again 1 pixel spacing
   
   int x, y;
   for (y=0;y<8;++y)
   {
      for (x=0;x<8;++x)
      {
         char font_entry = fontdata_8x8[helper + y];
        
         if (font_entry & 1<<(7-x))       
            setPixel(top_left_pixel_x + x, top_left_pixel_y + y, 1);
         else
            setPixel(top_left_pixel_x + x, top_left_pixel_y + y, 0);
         
      }
   }
}

void renderCharacterMedium(int col, int row, unsigned char character, unsigned int sx=0, unsigned int sy=0)
{
   int helper = character * 7 * 5; // for our font which is 6x4
   
   int top_left_pixel_x = sx+col*(5); // 1 pixel spacing
   int top_left_pixel_y = sy+row*(7); // once again 1 pixel spacing
   
   int x, y;
   for (y=0;y<7;++y)
   {
      for (x=0;x<5;++x)
      {
         char font_entry = fontdata_7x5[helper + y * 5 + x];
         if (font_entry)       
            setPixel(top_left_pixel_x + x, top_left_pixel_y + y, 1);
         else
            setPixel(top_left_pixel_x + x, top_left_pixel_y + y, 0);
         
      }
   }
}

void renderCharacterSmall(int col, int row, unsigned char character, unsigned int sx=0, unsigned int sy=0)
{
   int helper = character * 6 * 4; // for our font which is 6x4
   
   int top_left_pixel_x = sx+col*(4); // 1 pixel spacing
   int top_left_pixel_y = sy+row*(6); // once again 1 pixel spacing
   
   int x, y;
   for (y=0;y<6;++y)
   {
      for (x=0;x<4;++x)
      {
         char font_entry = fontdata_6x4[helper + y * 4 + x];
         if (font_entry)       
            setPixel(top_left_pixel_x + x, top_left_pixel_y + y, 1);
         else
            setPixel(top_left_pixel_x + x, top_left_pixel_y + y, 0);
         
      }
   }
}

void renderText(vector<string> & lines, int size, unsigned int sx=0, unsigned int sy=0)
{
   unsigned int col = 0;
   unsigned int row = 0;
   for (row = 0; row < lines.size(); ++row)
   {
      for (col=0;col<lines[row].length();++col)  // we take care about drawing too much in setPixel
      {
         // php just sucks
         if (lines[row][col] != '\r')
         {
            if (size == 0)
               renderCharacterSmall(col,row,lines[row][col],sx,sy);
            else if (size == 1)
               renderCharacterMedium(col,row,lines[row][col],sx,sy);
            else if (size == 2)
               renderCharacterLarge(col,row,lines[row][col],sx,sy);
         }
      }
 //     cout << "New row" << endl;
   }
   
}

void handleTextCommand(string const &input_line)
{
   string parse_line;
   int params[4] = { 0, 0, 0, 0 };
   int size = -1;

   if (input_line[1] == 'S')
      size = 0;
   else if (input_line[1] == 'M')
      size = 1;
   else if (input_line[1] == 'L')
      size = 2;
   else if (input_line[1] == 'O') {
      get_params(params, input_line, 3, 3);
      size = params[2];
   }
   
   if (size == -1)
   {
      size = 1;
      parse_line = input_line.substr(2,input_line.length() - 2);
   }
   else
      parse_line = input_line.substr(3,input_line.length() - 3);
 
   unsigned int i;
   
   vector <string> lines;
   bool in_line = false;
   int line_start = -1;
   for (i=0;i<parse_line.length();++i)
   {
      if (parse_line[i] == '\"')
      {
         if (i-2 >= 0 && parse_line[i-1] == '\\')
         {
            parse_line = parse_line.substr(0, i-1) + '"' + parse_line.substr(i+1);
            --i;
         }
         else if (in_line)
         {
            in_line = false;
            string temp = parse_line.substr(line_start,i-line_start);
            lines.push_back(temp);
         }
         else
         {
            in_line = true;
            line_start = i+1;
         }
         
      }
   }

   renderText(lines,size,params[0],params[1]);         
   updateScreen();
}
