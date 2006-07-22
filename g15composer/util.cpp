/***************************************************************************
 *   Copyright (C) 2006 by Philip Lawatsch <philip@waug.at>                *
 *                         Alex Ibrado <alex@ibrado.org>                   *
 *                     and Anthony J. Mirabella <mirabeaj@gmail.com        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string>
#include "libg15render.h"

using namespace std;

int get_params(int* params, string const &input_line, int start, int count)
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
