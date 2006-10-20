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
    along with g15tools; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>
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
#include "G15Control.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

void printUsage()
{
   cout << "Usage: g15composer /path/to/fifo" << endl;
   cout << "       cat instructions > /path/to/fifo" << endl;
   cout << endl;
   cout << "Display composer for the Logitech G15 LCD" << endl;
}

/********************************************************/

int main(int argc, char *argv[])
{
   static string fifo_filename = "";
  
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
   
   if (fifo_filename != "")
   {
   		G15Control *g15c = new G15Control(fifo_filename);
    	g15c->run();
		pthread_join(g15c->getThread(), NULL);
    	return EXIT_SUCCESS;
   }
   else
   {
   		return EXIT_FAILURE;
   }
}
