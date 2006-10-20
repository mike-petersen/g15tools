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

#include "G15Control.h"

G15Control::G15Control ():
G15Base ()
{
}

G15Control::G15Control (string filename):
G15Base (filename)
{
}

G15Control::~G15Control ()
{
}

int
G15Control::run ()
{
  int retval = pthread_create (&this->thread, NULL, G15Control::threadEntry,
			       (void *) this);
  return retval;
}

void
G15Control::parseCommandLine (string cmdline)
{
  int i = 0;
  int len = cmdline.length ();
  while (i < len && (cmdline[i] == ' ' || cmdline[i] == '\t'))
    ++i;

  if (i + 1 >= len || cmdline[i] == '#')
    return;

  char cmdType = cmdline[i];
  switch (cmdType)
    {
    case 'S':
      {
	handleScreenCommand (cmdline.substr (i));
	break;
      }
    default:
      break;
    }
}

void
G15Control::fifoProcessingWorkflow ()
{
  string line = "";
  int fd = doOpen ();

  if (fd != -1)
    {
      while (!leaving)
	{
	  char buffer_character[1];
	  int ret = read (fd, buffer_character, 1);
	  if (ret == 0)
	    {
	      close (fd);
	      fd = doOpen ();
	      if (fd < 0)
		{
		  cout << "Error, reopening failed" << endl;
		  break;
		}
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
		  parseCommandLine (line);
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

/* static */
void *
G15Control::threadEntry (void *pthis)
{
  G15Control *g15c = (G15Control *) pthis;
  g15c->fifoProcessingWorkflow ();
  g15c->G15Control::~G15Control ();
}
