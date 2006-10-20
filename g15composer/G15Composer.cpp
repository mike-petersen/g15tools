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
#include "g15c_logo.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

G15Composer::G15Composer():
G15Base()
{
	g15composerInit();
}

G15Composer::G15Composer(string filename):
G15Base(filename)
{
	g15composerInit();
}

G15Composer::~G15Composer()
{
   	  g15_close_screen(g15screen_fd);
   	  free(canvas);
	  pthread_exit(&thread);
}

void G15Composer::g15composerInit()
{
	   g15screen_fd = 0;
	   mkey_state = 0;
	   extern short g15c_logo_data[6880];
	   leaving = false;

	   if((g15screen_fd = new_g15_screen(G15_G15RBUF)) < 0)
	   {
	        cout << "Sorry, cant connect to the G15daemon" << endl;
	        exit(-1);
	   }
	
	   canvas = (g15canvas *) malloc(sizeof(g15canvas));
	
	   g15r_initCanvas(canvas);
	   canvas->mode_reverse = 1;
	   g15r_pixelOverlay(canvas, 0, 0, 160, 43, g15c_logo_data);
	   canvas->mode_reverse = 0;
	   updateScreen(true);
	   g15r_clearScreen(canvas, G15_COLOR_WHITE);
}

int G15Composer::run()
{
	int retval = pthread_create(&this->thread, NULL, G15Composer::threadEntry, (void*)this);
	return retval;
}

void G15Composer::updateScreen(bool force)
{
   if(force || !canvas->mode_cache)
      g15_send(g15screen_fd,(char*)canvas->buffer,1048);
}

void G15Composer::handlePixelCommand(string const &input_line)
{
	switch(input_line[1])
	{
		case 'S':
		{
	      	int params[3] = { 0, 0, 0 };
    	  	get_params(params, input_line, 3, 3);
    	  	g15r_setPixel(canvas, params[0], params[1], params[2]);
    	  	break;
		}
		case 'R':
		case 'F':
		{
			int params[5] = { 0, 0, 0, 0, 0 };
	
		   	bool fill = input_line[1] == 'F';
	   		int color = G15_COLOR_WHITE;
	   	   
	   		if(fill)
	   		{
	      		get_params(params, input_line, 3, 5);
	      		color = params[4];
	   		} else
	      		get_params(params, input_line, 3, 4);
	   
	   		g15r_pixelReverseFill(canvas, params[0], params[1], params[2], params[3], fill, color);
	   		break;
   		}
   		case 'O':
   		{
      		int params[4] = { 0, 0, 0, 0 };
      		int ofs = get_params(params, input_line, 3, 4);
      		int exp = params[2]*params[3];
      
      		if(exp != (int) input_line.length()-ofs)
      		{
         		cout << "Error in Pixel Overlay Command (" << params[2] << "x" << params[3] << ")" << endl;
         		cout << "I expect a total of " << exp << " 1s or 0s" << endl;
         		return;
      		}
         
      		short output_line[G15_BUFFER_LEN];
      
      		for(int i=0;i<exp;++i)
      		{
      			output_line[i] = 0;
      			if (input_line[ofs+i] == '1')
      	   		output_line[i] = 1;
      		}
      		g15r_pixelOverlay(canvas, params[0], params[1], params[2], params[3], output_line);
      		break;
   		}
   		case 'B':
   		{
	   		int params[7] = { 0, 0, 0, 0, 1, 1, 0 };
	   		get_params(params, input_line, 3, 7);
	   
	   		int x1 = params[0];
	   		int y1 = params[1];
	   		int x2 = params[2];
	   		int y2 = params[3];
	   		int color = params[4];
	   		int thick = params[5];
	   		int fill  = params[6];
	
       		g15r_pixelBox(canvas, x1, y1, x2, y2, color, thick, fill);
       		break;
   		}
   		case 'C':
   		{
      		g15r_clearScreen(canvas, input_line.length() < 4 || input_line[3] == '1');
      		break;
   		}
   		default:
   		{
   			break;
   		}
	}
   	updateScreen(false);
}

void G15Composer::handleDrawCommand(string const &input_line)
{
	switch(input_line[1])
	{
   		case 'L':
   		{
	   		int params[5] = { 0, 0, 0, 0, 1 };
	   		get_params(params, input_line, 3, 5);
	   
	   		g15r_drawLine(canvas, params[0], params[1], params[2], params[3], params[4]);
	   		break;
   		}
   		case 'C':
   		{
   	   		int params[5] = { 0, 0, 0, 0, 1 };
   	   		get_params(params, input_line, 3, 5);
   	   
   	   		g15r_drawCircle(canvas, params[0], params[1], params[2], params[3], params[4]);
   	   		break;
   		}
   		case 'R':
   		{
   	   		int params[6] = { 0, 0, 0, 0, 0, 1 };
   	   		get_params(params, input_line, 3, 6);
   	   
   	   		g15r_drawRoundBox(canvas, params[0], params[1], params[2], params[3], params[4], params[5]);
   	   		break;
   		}
   		case 'B':
   		{
       		int params[8] = { 0, 0, 0, 0, 1, 0, 0, 1 };
   	   		get_params(params, input_line, 3, 8);
   	   
   	   		g15r_drawBar(canvas, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]);
   	   		break;
   		}
   		default:
   		{
   			break;
   		}
	}
   	updateScreen(false);
}

void G15Composer::handleModeCommand(string const &input_line)
{
	string cmdval = input_line.substr(3,1);
	bool fore = cmdval == "0";
	bool stat = cmdval == "1";
	bool rear = stat;
	bool revert = cmdval == "2";
	char msgbuf[1];

   	switch(input_line[1])
   	{
   		case 'C':
   		{
      		bool was_cached = canvas->mode_cache;
      		canvas->mode_cache = stat;
      		if(was_cached) 
         		updateScreen(true);
         	break;
   		}
   		case 'R':
   		{
      		canvas->mode_reverse = stat;
      		break;
   		}
   		case 'X':
   		{
      		canvas->mode_xor = stat;
      		break;
   		}
   		case 'P':
   		{
      		msgbuf[0] = 'v'; /* Is the display visible? */
      		send(g15screen_fd,msgbuf,1,MSG_OOB);
      		recv(g15screen_fd,msgbuf,1,0);
      		bool at_front = (msgbuf[0] == '1') ? true : false;
      		msgbuf[0] = 'u'; /* Did the user make the display visible? */
      		send(g15screen_fd,msgbuf,1,MSG_OOB);
      		recv(g15screen_fd,msgbuf,1,0);      
      		bool user_to_front = (msgbuf[0] == '1') ? true : false;
      		msgbuf[0] = 'p'; /* We now want to change the priority */
      		if(at_front) {
      			if(rear) /* we want to go to the back */
      			{
      				send(g15screen_fd,msgbuf,1,MSG_OOB);
      			}      	
      			else if (!user_to_front && revert) /* we want to go to the back if forced to the front */
      			{
      				send(g15screen_fd,msgbuf,1,MSG_OOB);
      			}	
      		}
      		else
      		{ 
      			if(fore) /* we want to go to the front */
      			{
      				send(g15screen_fd,msgbuf,1,MSG_OOB);
      			}
      			else if(user_to_front && revert) /* we want to take back the foreground if forced to the back */
      			{
      				send(g15screen_fd,msgbuf,1,MSG_OOB);
      			}
      		}
      		break;
   		}
   		default:
   		{
   			break;
   		}
   }
}

void G15Composer::handleScreenCommand(std::string const &input_line)
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

#ifdef TTF_SUPPORT
void G15Composer::handleFontCommand(string const &input_line)
{
   string parse_line;
   int params[6] = { 0, 0, 0, 0, 0, 0 };
   parse_line = input_line.substr(3,input_line.length() - 3);
   
   const char *fontname;
   
   vector <string> lines;
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
            if (input_line[1] == 'L')
            	fontname = temp.c_str();
            else
            	lines.push_back(temp);
         }
         else
         {
            in_line = true;
            line_start = i+1;
         }
         
      }
   }
   
   switch(input_line[1])
   {
   		case 'L':
   		{
   			get_params(params, input_line, 3, 2);
   
   			int fontface = params[0];
   			int fontsize = params[1];
   			g15r_ttfLoad(canvas, (char *)fontname, fontsize, fontface);
   			break;
   		}
   		case 'P':
   		{
   			get_params(params, input_line, 3, 6);
   
   			int fontface = params[0];
   			int fontsize = params[1];
   			int x = params[2];
   			int y = params[3];
   			int color = params[4];
   			int center = params[5];
   		
   			for (unsigned int row = 0; row < lines.size(); ++row)
   			{
   				g15r_ttfPrint(canvas, x, y, fontsize, fontface, color, center, (char *)lines[row].c_str());
   			}
   			updateScreen(false);
   			break;
   		}
   		default:
   		{
   			break;
   		}
   }
}
#endif /* TTF_SUPPORT */

void G15Composer::handleTextCommand(string const &input_line)
{
   string parse_line;
   int params[4] = { 0, 0, 0, 0 };
   int size = -1;
   bool center = false;

   if (input_line[1] == 'S')
      size = G15_TEXT_SMALL;
   else if (input_line[1] == 'M')
      size = G15_TEXT_MED;
   else if (input_line[1] == 'L')
      size = G15_TEXT_LARGE;
   else if (input_line[1] == 'O') {
      get_params(params, input_line, 3, 4);
      size = params[2];
      center = params[3] ? true: false;
   }
   
   if (size == -1)
   {
      size = G15_TEXT_MED;
      parse_line = input_line.substr(2,input_line.length() - 2);
   }
   else
      parse_line = input_line.substr(3,input_line.length() - 3);
 
   int i;
   
   vector <string> lines;
   bool in_line = false;
   int line_start = -1;
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
            lines.push_back(temp);
         }
         else
         {
            in_line = true;
            line_start = i+1;
         }
         
      }
   }

   unsigned int col = 0;
   unsigned int row = 0;
   unsigned char stringOut[300];
   
   for (row = 0; row < lines.size(); ++row)
   {
      unsigned int len = lines[row].length();
      for (col=0;col<len;++col)  // we take care about drawing too much in setPixel
         if (lines[row][col] != '\r')
         	stringOut[col] = lines[row][col];
      stringOut[col] = (unsigned char)NULL;
      
      if (center)
      {
      	 unsigned int dispcol = 0;
      	 if (size == 0)  dispcol=(80-((len*4)/2));
    	 if (size == 1)  dispcol=(80-((len*5)/2));
    	 if (size == 2)  dispcol=(80-((len*8)/2));
    	 if (dispcol < 0) dispcol = 0;
      	 g15r_renderString(canvas, stringOut, row, size, dispcol, params[1]);
      }
      else
      	 g15r_renderString(canvas, stringOut, row, size, params[0], params[1]);
   }
   
   updateScreen(false);
}

void G15Composer::handleKeyCommand(string const &input_line)
{
	int params[2] = { 0, 0 };
	get_params(params, input_line, 3, 2);
	
	switch(input_line[1])
	{
		case 'L':
		{
			break;
		}
		case 'M':
		{
			bool sendCmd = true;
			bool LEDon = params[1];
			mkey_state |= G15DAEMON_MKEYLEDS;
			switch(params[0])
			{
				case 0:
				{
					if (LEDon)
						mkey_state |= G15_LED_M1 | G15_LED_M2 | G15_LED_M3;
					else
						mkey_state &= ~G15_LED_M1 & ~G15_LED_M2 & ~G15_LED_M3;
					break;
				}
				case 1:
				{
					if (LEDon)
						mkey_state |= G15_LED_M1;
					else
						mkey_state &= ~G15_LED_M1;
					break;
				}
				case 2:
				{
					if (LEDon)
						mkey_state |= G15_LED_M2;
					else
						mkey_state &= ~G15_LED_M2;
					break;
				}
				case 3:
				{
					if (LEDon)
						mkey_state |= G15_LED_M3;
					else
						mkey_state &= ~G15_LED_M3;
					break;
				}
				default:
				{
					sendCmd = false;
					break;
				}
			}
			if (sendCmd) send(g15screen_fd,&mkey_state,1,MSG_OOB);
			break;
		}
		default:
		{
			break;
		}
	}
}

void G15Composer::handleLCDCommand(string const &input_line)
{
	int params[1] = { 0 };
	get_params(params, input_line, 3, 1);
	
	char msgbuf[1];
	memset(msgbuf, 0, 1);
	bool sendCmd = true;
	switch(input_line[1])
	{
		case 'B':
		{
			msgbuf[1] |= G15DAEMON_BACKLIGHT | params[0];
			break;
		}
		case 'C':
		{
			msgbuf[1] |= G15DAEMON_CONTRAST | params[0];
			break;
		}
		default:
		{
			sendCmd = false;
			break;
		}	
	}
	if (sendCmd) send(g15screen_fd,msgbuf,1,MSG_OOB);
}

void G15Composer::parseCommandLine(string cmdline)
{
	  int i = 0;
	  int len = cmdline.length();
	  while( i < len && (cmdline[i] == ' ' || cmdline[i] == '\t') )
	     ++i;
	  
	  if( i+1 >= len || cmdline[i] == '#' )
	     return;
	
	  char cmdType = cmdline[i];
	  switch(cmdType) {
	  	case 'D':
	  	{
	     	handleDrawCommand(cmdline.substr(i) );
	     	break;         
	  	}
#ifdef TTF_SUPPORT
	  	case 'F':
	  	{
	  	 	handleFontCommand(cmdline.substr(i) );
	  	 	break;
	  	}
#endif
		case 'K':
		{
			handleKeyCommand(cmdline.substr(i) );
			break;
		}
		case 'L':
		{
			handleLCDCommand(cmdline.substr(i) );
			break;
		}
		case 'M':
		{
	     	handleModeCommand(cmdline.substr(i) );
	     	break;
	  	}
	  	case 'P':
	  	{
	  		handlePixelCommand(cmdline.substr(i) );
	  		break;
	  	}
		case 'S':
		{
			handleScreenCommand(cmdline.substr(i) );
			break;
		}
	  	case 'T':
	  	{
	     	handleTextCommand(cmdline.substr(i) );
	     	break;
	  	}
		default:
		{
			break;
		}
	  }
}

void G15Composer::fifoProcessingWorkflow()
{
   string line = "";
   int fd = doOpen();
   
   if (fd != -1)
   {
      while (!leaving)
      {
         char buffer_character[1];
         int ret = read(fd,buffer_character,1);
         if (ret == 0)
         {
            close(fd);
            fd = doOpen();
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

/* static */
void * G15Composer::threadEntry(void * pthis)
{
	G15Composer * g15c = (G15Composer*)pthis;
	g15c->fifoProcessingWorkflow();
	g15c->G15Composer::~G15Composer();
}
