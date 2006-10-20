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

#ifndef G15CONTROL_H_
#define G15CONTROL_H_

#include "G15Base.h"

class G15Control:public G15Base
{
public:
  G15Control ();
  G15Control (string filename);
  virtual ~ G15Control ();
  int run ();

protected:
  void parseCommandLine (string cmdline);
  void fifoProcessingWorkflow ();
  static void *threadEntry (void *pthis);
};

#endif /*G15CONTROL_H_ */
