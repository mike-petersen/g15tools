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
#include <getopt.h>

#include "screen.h"
#include "text.h"
#include "pixel.h"

using namespace std;

void printUsage()
{
   cout << "Usage: cat instructions | g15composer > /path/to/fifo" << endl;
   cout << endl;
   cout << "Display composer for the Logitech G15 LCD" << endl;
}


/********************************************************/

int main(int argc, char *argv[])
{
   if(argv[1] == "-h" || argv[1] == "help") {
      printUsage();
      return 0;
   }
   
   fstream script;
   bool is_script = false;
   if( argc > optind ) {
      script.open(argv[optind]);
      is_script = script.is_open();
   }

   clearScreen();
   updateScreen(true);
   
   string cmdline;
   while( getline(is_script ? script : cin, cmdline) )
   {
      int i = 0;
      int len = cmdline.length();
      while( i < len && (cmdline[i] == ' ' || cmdline[i] == '\t') )
         ++i;
      
      if( i+1 >= len || cmdline[i] == '#' )
         continue;

      if( cmdline[i] == 'P' && cmdline[i+1] != ' ' ) 
      {
         handlePixelCommand( cmdline.substr(i) );
      }
      else if( cmdline[i] == 'T' ) 
      {
         handleTextCommand( cmdline.substr(i) );
      }
      else if( cmdline[i] == 'M' ) 
      {
         handleModeCommand( cmdline.substr(i) );
      }
      else 
      {
         // Relay to g15daemon
         cout << cmdline.substr(i) << endl;         
      }
   }

   if(is_script)
      script.close();
   
   return EXIT_SUCCESS;


}
