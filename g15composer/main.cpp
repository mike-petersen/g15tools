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
#include <sys/stat.h>

#include <libg15.h>
#include <g15daemon_client.h>
#include <libg15render.h>
#include "composer.h"
#include "g15c_logo.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

int g15screen_fd = 0;
static string fifo_filename = "";
g15canvas *canvas;
bool is_script = false;
extern short g15c_logo_data[];
ifstream script;
char mkey_state = 0;
   
void printUsage()
{
   cout << "Usage: g15composer /path/to/fifo [/path/to/script]" << endl;
   cout << "       cat instructions > /path/to/fifo" << endl;
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
         exit(EXIT_FAILURE);
      }
      struct stat sb;
      if (fstat(fd, &sb))
      {
      	 cout << "Error, unable to stat " << filename << " aborting" << endl;
      	 fd = -1;
      	 exit(EXIT_FAILURE);
      }
      if (!S_ISFIFO(sb.st_mode))
      {
      	 cout << "Error, " << filename << " is not a named pipe, aborting" << endl;
      	 fd = -1;
      	 exit(EXIT_FAILURE);
      }
   }
   return fd;
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
	            if (!canvas->mode_cache)
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

