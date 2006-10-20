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

#ifndef G15COMPOSER_H_
#define G15COMPOSER_H_

#include <libg15.h>
#include <g15daemon_client.h>
#include <libg15render.h>
#include "G15Base.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

class G15Composer:public G15Base
{
public:
  G15Composer ();
  G15Composer (string filename);
  virtual ~ G15Composer ();
  int run ();

protected:
  void handlePixelCommand (std::string const &input_line);
  void handleDrawCommand (string const &input_line);
  void handleModeCommand (std::string const &input_line);
#ifdef TTF_SUPPORT
  void handleFontCommand (string const &input_line);
#endif
  void handleTextCommand (std::string const &input_line);
  void handleKeyCommand (string const &input_line);
  void handleLCDCommand (string const &input_line);
  void parseCommandLine (string cmdline);
  void fifoProcessingWorkflow ();
  void updateScreen (bool);
  void g15composerInit ();
  static void *threadEntry (void *pthis);
  int g15screen_fd;
  g15canvas *canvas;
  char mkey_state;
};

#endif /*G15COMPOSER_H_ */
