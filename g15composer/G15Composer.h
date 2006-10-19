#ifndef G15COMPOSER_H_
#define G15COMPOSER_H_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include <libg15.h>
#include <g15daemon_client.h>
#include <libg15render.h>
#include "composer.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

class G15Composer
{
public:
	G15Composer();
	virtual ~G15Composer();
	static string filename() {return fifo_filename;}
	void filename(string filename) {fifo_filename = filename;}
	void fifoProcessingWorkflow(bool is_script, string const &filename);
	
private:
	void handlePixelCommand(std::string const &input_line);
	void handleDrawCommand(string const &input_line);
	void handleModeCommand(std::string const &input_line);
	#ifdef TTF_SUPPORT
	void handleFontCommand(string const &input_line);
	#endif
	void handleTextCommand(std::string const &input_line);
	void handleKeyCommand(string const &input_line);
	void handleLCDCommand(string const &input_line);
	void parseCommandLine(string cmdline);
	void updateScreen(bool);
	int g15screen_fd;
	static string fifo_filename;
	g15canvas *canvas;
	char mkey_state;
	
};

#endif /*G15COMPOSER_H_*/
