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
#include <fstream>
#include <cstdlib>
#include <string>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <libg15.h>
#include <g15daemon_client.h>
#include <libg15render.h>
#include "composer.h"
#include "g15c_logo.h"

using namespace std;

int g15screen_fd = 0;
static string fifo_filename = "";
g15canvas *canvas;
bool is_script = false;
extern short g15c_logo_data[];
ifstream script;
   
void printUsage()
{
   cout << "Usage: cat instructions | g15composer" << endl;
   cout << endl;
   cout << "Display composer for the Logitech G15 LCD" << endl;
}

/********************************************************/

int main(int argc, char *argv[])
{
   int i=1;
   for (i=1;(i<argc && fifo_filename == "");++i)
   {
      string arg(argv[i]);
      if (arg == "-h" || arg == "--help")
      {
         printUsage();
         return 0;
      }
      else
      {
         fifo_filename = argv[i];
      }
   }
   if (fifo_filename == "")
   {
      cout << "You didnt specify a fifo filename, I'm this disabling writing text to the lcd" << endl;
      cout << "This is a change in behaviour from previous version, I will _NOT_ read data from stdin anymore" << endl;
      return -1;
   }
   if (fifo_filename == "-")
   {
      cout << "Dont try to trick me, I wont read input from stdin anymore" << endl;
      return -1;
   }
   
  if( argc > i ) {
      script.open(argv[i]);
      is_script = script.is_open();
   }

   if(!is_script)
   {   
	   if((g15screen_fd = new_g15_screen(G15_G15RBUF)) < 0)
	   {
	        cout << "Sorry, cant connect to the G15daemon" << endl;
	        return -1;
	   }
	
	   canvas = (g15canvas *) malloc(sizeof(g15canvas));
	
	   g15r_initCanvas(canvas);
	   canvas->mode_reverse = 1;
	   g15r_pixelOverlay(canvas, 0, 0, 160, 43, g15c_logo_data);
	   canvas->mode_reverse = 0;
	   updateScreen(true);
	   g15r_clearScreen(canvas, G15_COLOR_WHITE);
   }
   
   if (fifo_filename != "")
    fifoProcessingWorkflow(is_script, fifo_filename);

   if(is_script)
      script.close();
   else
   {   
   	  g15_close_screen(g15screen_fd);
   	  free(canvas->buffer);
   	  free(canvas);
   }
   return EXIT_SUCCESS;
}

void handlePixelCommand(string const &input_line)
{
   if (input_line[1] == 'S') // Set
   {
      int params[3] = { 0, 0, 0 };
      get_params(params, input_line, 3, 3);
      g15r_setPixel(canvas, params[0], params[1], params[2]);
   }
   else if (input_line[1] == 'R' || input_line[1] == 'F')
   {
	   int params[5] = { 0, 0, 0, 0, 0 };
	
	   bool fill = input_line[1] == 'F';
	   int color = G15_COLOR_WHITE;
	   	   
	   if(fill)
	   {
	      get_params(params, input_line, 3, 5);
	      color = params[4];
	   } else
	      get_params(params, input_line, 3, 4);
	   
	   g15r_pixelReverseFill(canvas, params[0], params[1], params[2], params[3], fill, color);
   }
   else if (input_line[1] == 'O')
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
         
      short output_line[G15_BUFFER_LEN];
      
      for(int i=0;i<exp;++i)
      {
      	output_line[i] = 0;
      	if (input_line[ofs+i] == '1')
      	   output_line[i] = 1;
      }
      g15r_pixelOverlay(canvas, params[0], params[1], params[2], params[3], output_line);
   }
   else if (input_line[1] == 'B')
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
	
       g15r_pixelBox(canvas, x1, y1, x2, y2, color, thick, fill);
   }
   else if (input_line[1] == 'C')
   {
      g15r_clearScreen(canvas, input_line.length() < 4 || input_line[3] == '1');
   }
   updateScreen(false);
}

void handleDrawCommand(string const &input_line)
{
   if (input_line[1] == 'L')
   {
	   int params[5] = { 0, 0, 0, 0, 1 };
	   get_params(params, input_line, 3, 5);
	   
	   g15r_drawLine(canvas, params[0], params[1], params[2], params[3], params[4]);
   }
   else if (input_line[1] == 'C')
   {
   	   int params[5] = { 0, 0, 0, 0, 1 };
   	   get_params(params, input_line, 3, 5);
   	   
   	   g15r_drawCircle(canvas, params[0], params[1], params[2], params[3], params[4]);
   }
   else if (input_line[1] == 'R')
   {
   	   int params[6] = { 0, 0, 0, 0, 0, 1 };
   	   get_params(params, input_line, 3, 6);
   	   
   	   g15r_drawRoundBox(canvas, params[0], params[1], params[2], params[3], params[4], params[5]);
   }
   else if (input_line[1] == 'B')
   {
       int params[8] = { 0, 0, 0, 0, 1, 0, 0, 1 };
   	   get_params(params, input_line, 3, 8);
   	   
   	   g15r_drawBar(canvas, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]);
   }
   updateScreen(false);
}

void handleModeCommand(string const &input_line)
{
   string cmdval = input_line.substr(3,1);
   bool fore = cmdval == "0";
   bool stat = cmdval == "1";
   bool revert = cmdval == "2";
   char msgbuf[1];

   if(input_line[1] == 'C')
   {
      bool was_cached = canvas->mode_cache;
      canvas->mode_cache = stat;
      if(was_cached) 
         updateScreen(true);
   }
   else if(input_line[1] == 'R')
      canvas->mode_reverse = stat;
   else if(input_line[1] == 'X')
      canvas->mode_xor = stat;
   else if(input_line[1] == 'P') {
      msgbuf[0] = 'v';
      send(g15screen_fd,msgbuf,1,MSG_OOB);
      recv(g15screen_fd,msgbuf,1,0);
      bool at_front = (msgbuf[0] == '1') ? true : false;
      msgbuf[0] = 'u';
      send(g15screen_fd,msgbuf,1,MSG_OOB);
      recv(g15screen_fd,msgbuf,1,0);      
      bool user_to_front = (msgbuf[0] == '1') ? true : false;
      msgbuf[0] = 'p';
      if(at_front) {
      	if(stat) /* we want to go to the back */
      	{
      		send(g15screen_fd,msgbuf,1,MSG_OOB);
      	}      	
      	else if (!user_to_front && revert) /* we want to go to the back if forced to the front */
      	{
      		send(g15screen_fd,msgbuf,1,MSG_OOB);
      	}	
      }
      else
      { 
      	if(fore) /* we want to go to the front */
      	{
      		send(g15screen_fd,msgbuf,1,MSG_OOB);
      	}
      	else if(user_to_front && revert) /* we want to take back the foreground if forced to the back */
      	{
      		send(g15screen_fd,msgbuf,1,MSG_OOB);
      	}
      }
   }
}

void handleTextCommand(string const &input_line)
{
   string parse_line;
   int params[4] = { 0, 0, 0, 0 };
   int size = -1;

   if (input_line[1] == 'S')
      size = G15_TEXT_SMALL;
   else if (input_line[1] == 'M')
      size = G15_TEXT_MED;
   else if (input_line[1] == 'L')
      size = G15_TEXT_LARGE;
   else if (input_line[1] == 'O') {
      get_params(params, input_line, 3, 3);
      size = params[2];
   }
   
   if (size == -1)
   {
      size = G15_TEXT_MED;
      parse_line = input_line.substr(2,input_line.length() - 2);
   }
   else
      parse_line = input_line.substr(3,input_line.length() - 3);
 
   int i;
   
   vector <string> lines;
   bool in_line = false;
   int line_start = -1;
   for (i=0;i<(int)parse_line.length();++i)
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

   unsigned int col = 0;
   unsigned int row = 0;
   unsigned char stringOut[300];
   
   for (row = 0; row < lines.size(); ++row)
   {
      for (col=0;col<lines[row].length();++col)  // we take care about drawing too much in setPixel
         if (lines[row][col] != '\r')
         	stringOut[col] = lines[row][col];
      stringOut[col] = (unsigned char)NULL;
      g15r_renderString(canvas, stringOut, row, size, params[0], params[1]);
   }
   
   updateScreen(false);
}

void updateScreen(bool force)
{
   if(force || !canvas->mode_cache)
      g15_send(g15screen_fd,(char*)canvas->buffer,1048);
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

void parseCommandLine(string cmdline)
{
	  int i = 0;
	  int len = cmdline.length();
	  while( i < len && (cmdline[i] == ' ' || cmdline[i] == '\t') )
	     ++i;
	  
	  if( i+1 >= len || cmdline[i] == '#' )
	     return;
	
	  if( cmdline[i] == 'P' && cmdline[i+1] != ' ' ) 
	  {
	     handlePixelCommand(cmdline.substr(i) );
	  }
	  else if( cmdline[i] == 'T' ) 
	  {
	     handleTextCommand(cmdline.substr(i) );
	  }
	  else if( cmdline[i] == 'M' ) 
	  {
	     handleModeCommand(cmdline.substr(i) );
	  }
	  else if( cmdline[i] == 'D' )
	  {
	     handleDrawCommand(cmdline.substr(i) );         
	  }
}

void fifoProcessingWorkflow(bool is_script, string const &filename)
{
   string line = "";
   if(!is_script)
   {
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
	            g15r_clearScreen(canvas, G15_COLOR_WHITE);
	         }
	         else if (ret == -1)
	         {
	            cout << "Reading failed, aborting fifo thread" << endl;
	            break;
	         }
	         else
	         {
	            if (buffer_character[0] == '\r')
	            {
	            }
	            else if (buffer_character[0] == '\n')
	            {
	 			  parseCommandLine(line);
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
   else /* is_script */
   {
   		ofstream fifo(filename.c_str());
   		while (getline(script, line))
   		{
   			fifo << line << endl;
   		}
   } 
}

