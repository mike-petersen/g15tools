/*
    This file is part of g15lcd.

    g15lcd is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15lcd is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15lcd; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <iostream>
#include <cstdlib>
#include <usb.h>
#include <string>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lcd.h"

extern unsigned char fontdata_8x8[];
extern unsigned char fontdata_7x5[];
extern unsigned char fontdata_6x4[];

using namespace std;
void setPixel(unsigned char *buffer, unsigned int x, unsigned int y, int val)
{
   unsigned int const offset_from_start = 32;
   unsigned int const width = 160;
   unsigned int const height = 43;
 
   if (x >= width || y >= height)
      return;
   
   int row = y / 8;
   int offset = width*row + x;
   int bit = y % 8;
    
   if (val)
      buffer[offset_from_start + offset] = buffer[offset_from_start + offset] | 1 << bit;
   else
      buffer[offset_from_start + offset] = buffer[offset_from_start + offset]  &  ~(1 << bit);
   
}

void renderCharacterLarge(unsigned char *buffer, int col, int row, char character)
{
   int helper = character * 8; // for our font which is 8x8
   
   int top_left_pixel_x = col*(8); // 1 pixel spacing
   int top_left_pixel_y = row*(8); // once again 1 pixel spacing
   
   int x, y;
   for (y=0;y<8;++y)
   {
      for (x=0;x<8;++x)
      {
         char font_entry = fontdata_8x8[helper + y];
        
         if (font_entry & 1<<(7-x))       
            setPixel(buffer,top_left_pixel_x + x, top_left_pixel_y + y, 1);
         else
            setPixel(buffer,top_left_pixel_x + x, top_left_pixel_y + y, 0);
         
      }
   }
}

void renderCharacterMedium(unsigned char *buffer, int col, int row, char character)
{
   int helper = character * 7 * 5; // for our font which is 6x4
   
   int top_left_pixel_x = col*(5); // 1 pixel spacing
   int top_left_pixel_y = row*(7); // once again 1 pixel spacing
   
   int x, y;
   for (y=0;y<7;++y)
   {
      for (x=0;x<5;++x)
      {
         char font_entry = fontdata_7x5[helper + y * 5 + x];
         if (font_entry)       
            setPixel(buffer,top_left_pixel_x + x, top_left_pixel_y + y, 1);
         else
            setPixel(buffer,top_left_pixel_x + x, top_left_pixel_y + y, 0);
         
      }
   }
}
void renderCharacterSmall(unsigned char *buffer, int col, int row, char character)
{
   int helper = character * 6 * 4; // for our font which is 6x4
   
   int top_left_pixel_x = col*(4); // 1 pixel spacing
   int top_left_pixel_y = row*(6); // once again 1 pixel spacing
   
   int x, y;
   for (y=0;y<6;++y)
   {
      for (x=0;x<4;++x)
      {
         char font_entry = fontdata_6x4[helper + y * 4 + x];
         if (font_entry)       
            setPixel(buffer,top_left_pixel_x + x, top_left_pixel_y + y, 1);
         else
            setPixel(buffer,top_left_pixel_x + x, top_left_pixel_y + y, 0);
         
      }
   }
}
void renderText(unsigned char *buffer, vector<string> & lines, int size)
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
               renderCharacterSmall(buffer,col,row,lines[row][col]);
            else if (size == 1)
               renderCharacterMedium(buffer,col,row,lines[row][col]);
            else if (size == 2)
               renderCharacterLarge(buffer,col,row,lines[row][col]);
         }
      }
 //     cout << "New row" << endl;
   }
   
}

void handleTextCommand(unsigned char *buffer, string const &input_line)
{
   string parse_line;
   int size = -1;
   if (input_line[1] == 'S')
      size = 0;
   else if (input_line[1] == 'M')
      size = 1;
   else if (input_line[1] == 'L')
      size = 2;
      
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
         if (in_line)
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
   renderText(buffer,lines,size);
}

void handlePixelCommand(unsigned char *buffer, string const &input_line)
{
   if (input_line.length() < 6882)
   {
      cout << "Error in Pixel Command, the format is " << endl;
      cout << "P 10011011101.... I expect a total of 6880 1 or 0's (total length of this line should be 6882" << endl;
      cout << "I however only received a total of " << input_line.length() - 2 << " pixel values" << endl;
      return;
   }
   
   int i;
   for (i=0;i<6880;++i)
   {
      setPixel(buffer, i%160, i/160, input_line[i+2] == '1');
   }
   
}
int doOpen(string const &filename)
{
   int fd = -1;

   if (filename == "-")
      fd = 0;
   else
   {
      fd = open(filename.c_str(),O_RDONLY);
      if (fd == -1)
      {
         cout << "Error, could not open " << filename << " aborting" << endl;
      }
   }
   return fd;
}

void processInputLine(unsigned char *buffer, pthread_mutex_t *mutex, int *changed, string const &line)
{
   if (line.length() < 3)
   {
      cout << "How about passing useful data into this program?" << endl;
      return;
   }
   
   unsigned char buf[0x03e0];
   memset(buf,0,0x03e0);
   buf[0] = 0x03;
   
   if (line[0] == 'T')
   {
      handleTextCommand(buf,line);
   }
   else if (line[0] == 'P')
   {
      handlePixelCommand(buf,line);
   }
   
   pthread_mutex_lock(mutex);
      memcpy(buffer, buf, 0x03e0);
      *changed = 1;
   pthread_mutex_unlock(mutex);
}

void lcdProcessingWorkflow(unsigned char *buffer, pthread_mutex_t *mutex, int *changed, string const &filename)
{
   string line = "";
   int fd = doOpen(filename);
   
   if (fd != -1)
   {
      bool read_something = true;
      while (read_something)
      {
         char buffer_character[1];
         int ret = read(fd,buffer_character,1);
         if (ret == 0)
         {
            close(fd);
            fd = doOpen(filename);
            if (fd < 0)
            {
               cout << "Error, reopening failed" << endl;
               break;
            }
         }
         else if (ret == -1)
         {
            cout << "Reading failed, aborting lcd thread" << endl;
            break;
         }
         else
         {
            if (buffer_character[0] == '\r')
            {
            }
            else if (buffer_character[0] == '\n')
            {
               processInputLine(buffer, mutex, changed, line);
               line = "";
            }
            else
            {
               line = line + buffer_character[0];
            }
         }
      }
   }

   
}
