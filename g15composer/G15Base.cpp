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

#include "G15Base.h"
#include "G15Composer.h"

G15Base::G15Base()
{
	leaving = false;
	keepFifo = false;
}

G15Base::G15Base(string filename)
{
	leaving = false;
	keepFifo = false;
	fifo_filename = filename;
	mode_t mode = S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH;
	if(mkfifo(fifo_filename.c_str(), mode))
	{
		cout << "Error, could not create FIFO " << fifo_filename << " aborting" << endl;
		leaving = true;
		keepFifo = true;
	}
	chmod(fifo_filename.c_str(), mode);
}

G15Base::~G15Base()
{
	if(!keepFifo)
		unlink(fifo_filename.c_str());
}

int G15Base::run()
{
	return 0;
}

void G15Base::handleScreenCommand(std::string const &input_line)
{
   switch(input_line[1])
   {
   	case 'N':
	{
	   string parse_line;
	   parse_line = input_line.substr(3,input_line.length() - 3);

	   const char *newpipe;
   
  	   bool in_line = false;
	   int line_start = -1, i;
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
	      	   newpipe = temp.c_str();
	        }
	        else
	        {
	           in_line = true;
	           line_start = i+1;
	        }
         
	     }
	   }

   	   G15Composer *g15c = new G15Composer(newpipe);
   	   g15c->run();
   	   pthread_detach(g15c->getThread());
	   break;
	}
	case 'C':
	{
		leaving = true;
		break;
	}
	default:
	{
		break;
	}
   }
}

int G15Base::doOpen()
{
   int fd = -1;

   if (fifo_filename == "-")
      fd = 0;
   else
   {
      fd = open(fifo_filename.c_str(),O_RDONLY);
      if (fd == -1)
      {
         cout << "Error, could not open " << fifo_filename << " aborting" << endl;
         return -1;
      }
   }
   return fd;
}

int G15Base::get_params(int* params, string const &input_line, int start, int count)
{
   char tmp[10];
   int ti = 0;
   int ofs = 0;

   int limit = input_line.length();
   int p = 0;

   ofs = start;
   for(int i=0;i<limit && p<count;++i)
   {
      if(ti > 9) break;
 
      char ch = input_line[start+i];
      if(i+1==limit)
      {
         tmp[ti+1]=0;
         ch = ' ';
      }

      if(ch==' ')
      {
         if(tmp[0]>='A' && tmp[0]<='Z')
         {
            switch(tmp[0])
            {
               case 'S' : params[p] = 0; break;
               case 'M' : params[p] = 1; break;
               case 'L' : params[p] = 2; break;
               default: params[p] = 2;
            }
         }
         else
         {
            params[p] = atoi(tmp);
         }
         ++p;
         ti=0;
      }
      else
      {
         tmp[ti]=ch;
         tmp[ti+1]=0;
         ++ti;
      }
      ofs++;
   }
   return ofs;
}
