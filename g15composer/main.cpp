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

#include "G15Composer.h"
#include "G15Control.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

void
printUsage ()
{
  cout << "Usage: g15composer [-b] /path/to/fifo" << endl;
  cout << "       cat instructions > /path/to/fifo" << endl;
  cout << endl;
  cout << "Display composer for the Logitech G15 LCD" << endl;
}

/********************************************************/

int
main (int argc, char *argv[])
{
  static string fifo_filename = "";
  bool background = false;

  int i = 1;
  for (i = 1; (i < argc && fifo_filename == ""); ++i)
    {
      string arg (argv[i]);
      if (arg == "-h" || arg == "--help")
	{
	  printUsage ();
	  return 0;
	}
      else if (arg == "-b")
	{
	  background = true;
	}
      else
	{
	  fifo_filename = argv[i];
	}
    }
  if (fifo_filename == "")
    {
      cout << "You did not specify a fifo filename.  Aborting." << endl;
      return -1;
    }
  if (fifo_filename == "-")
    {
      cout << "Please don't try to get me to read from stdin." << endl;
      return -1;
    }

  if (fifo_filename != "")
    {
      G15Base *g15c;
      if (background)
	g15c = new G15Control (fifo_filename);
      else
	g15c = new G15Composer (fifo_filename);
      g15c->run ();
      pthread_join (g15c->getThread (), NULL);
      return EXIT_SUCCESS;
    }
  else
    return EXIT_FAILURE;
}
