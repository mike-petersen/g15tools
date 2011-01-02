%{
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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pthread.h>
#include <libg15.h>
#include <libg15render.h>
#include <g15daemon_client.h>

#include "g15composer.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define YYPARSE_PARAM param
#define YYLEX_PARAM   ((struct parserData *)param)->scanner
#define YYERROR_VERBOSE true

int yydebug = 0;

%}

%union
{
	int number;
	char *string;
	struct strList *strList;
}

%token <number> T_NUMBER
%token <string> T_STRING
%token T_NEWLINE
%token T_PIXELSET
%token T_PIXELFILL
%token T_PIXELRFILL
%token T_PIXELOVERLAY
%token T_PIXELBOX
%token T_PIXELCLEAR
%token T_DRAWLINE
%token T_DRAWCIRCLE
%token T_DRAWRBOX
%token T_DRAWBAR
%token T_DRAWNUM
%token T_DRAWICON
%token T_DRAWSPRITE
%token T_WBMPSPLASH
%token T_WBMPLOAD
%token T_MODECACHE
%token T_MODEREV
%token T_MODEXOR
%token T_MODEPRI
%token T_FONTLOAD
%token T_FONTPRINT
%token T_TEXTSMALL
%token T_TEXTMED
%token T_TEXTLARGE
%token T_TEXTOVERLAY
%token T_KEYL
%token T_KEYM
%token T_LCDBL
%token T_LCDCON
%token T_SCREENNEW
%token T_SCREENCLOSE

%type <string> nt_string
%type <strList> nt_strings

%pure_parser

%%

nt_commands: /* empty */
	| nt_commands nt_command
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		if (((struct parserData *)param)->leaving != 1 && ((struct parserData *)param)->cmd_only!=1)
		  updateScreen (((struct parserData *)param)->canvas, ((struct parserData *)param)->g15screen_fd, 0);
		else
		  ((struct parserData *)param)->cmd_only=0;

	}

	|

	nt_commands error nt_command
	;

nt_command:
	nt_pixel_command
	| nt_draw_command
	| nt_wbmp_command
	| nt_mode_command
	| nt_font_command
	| nt_text_command
	{
		((struct parserData *)param)->itemptr = ((struct parserData *)param)->listptr->first_string;
		while (((struct parserData *)param)->itemptr != 0)
		  {
		  	free (((struct parserData *)param)->itemptr->string);
			struct strItem *tmpItem = ((struct parserData *)param)->itemptr;
			((struct parserData *)param)->itemptr = ((struct parserData *)param)->itemptr->next_string;
			free (tmpItem);
		  }
		free (((struct parserData *)param)->listptr);
		((struct parserData *)param)->listptr = NULL;
		((struct parserData *)param)->itemptr = NULL;
	}
	| nt_key_command
	| nt_lcd_command
	| nt_screen_command
	;

nt_string:
	T_STRING
	{
		$$ = $1;
	}
	;

nt_strings: /* empty */
	{
		$$ = new_strList ();
	}

	|

	nt_strings nt_string
	{
		add_string ($1, $2);
		$$ = $1;
	}
	;

nt_pixel_command:
	T_PIXELSET T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_setPixel (((struct parserData *)param)->canvas, $2, $3, $4);
	}

	|

	T_PIXELFILL T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_pixelReverseFill (((struct parserData *)param)->canvas, $2, $3, $4, $5, 1, $6);
	}

	|

	T_PIXELRFILL T_NUMBER T_NUMBER T_NUMBER T_NUMBER
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_pixelReverseFill (((struct parserData *)param)->canvas, $2, $3, $4, $5, 0, G15_COLOR_WHITE);
	}

	|

	T_PIXELOVERLAY T_NUMBER T_NUMBER T_NUMBER T_NUMBER nt_string T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
			return (0);
		short output_line[G15_LCD_HEIGHT * G15_LCD_WIDTH];
		int len = strlen ($6);
		int exp = $4 * $5;

		if ((len != exp) || (len > G15_LCD_HEIGHT * G15_LCD_WIDTH))
		{
			fprintf (stderr, "Error: Expected %d pixels but received %d.\n", exp, len);
			return (1);
		}

		int i = 0;

		for (i = 0; i < len; ++i)
		{
				output_line[i] = 0;
				if ($6[i] == '1')
					output_line[i] = 1;
		}
	  	g15r_pixelOverlay (((struct parserData *)param)->canvas, $2, $3, $4, $5, output_line);
		free ($6);
	}

	|

	nt_pixelbox

	|

	T_PIXELCLEAR T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_clearScreen (((struct parserData *)param)->canvas, $2);
	}
	;

nt_pixelbox:
	T_PIXELBOX T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_pixelBox (((struct parserData *)param)->canvas, $2, $3, $4, $5, 1, 1, 0);
	}

	|

	T_PIXELBOX T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_pixelBox (((struct parserData *)param)->canvas, $2, $3, $4, $5, $6, 1, 0);
	}

	|

	T_PIXELBOX T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_pixelBox (((struct parserData *)param)->canvas, $2, $3, $4, $5, $6, $7, 0);
	}

	|

	T_PIXELBOX T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_pixelBox (((struct parserData *)param)->canvas, $2, $3, $4, $5, $6, $7, $8);
	}
	;

nt_draw_command:
	T_DRAWLINE T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawLine (((struct parserData *)param)->canvas, $2, $3, $4, $5, $6);
	}

	|

	T_DRAWNUM T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawBigNum (((struct parserData *)param)->canvas, $2, $3, $4, $5, $6, $7);
	}

	|

	T_DRAWICON T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);

		struct bufItem *buf = ((struct parserData *)param)->buflist->first_buf;

		while ((buf->id != $2) && (buf != NULL))
		  buf = buf->next;

		if (buf == NULL)
		  return (-1);

		g15r_drawIcon (((struct parserData *)param)->canvas, buf->buffer, $3, $4, buf->width, buf->height);
	}

	|

	T_DRAWSPRITE T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);

		struct bufItem *buf = ((struct parserData *)param)->buflist->first_buf;

		while ((buf->id != $2) && (buf != NULL))
		  buf = buf->next;

		if (buf == NULL)
		  return (-1);

		g15r_drawSprite (((struct parserData *)param)->canvas, buf->buffer, $3, $4, $5, $6, $7, $8, buf->width);
	}

	|
	nt_drawcircle
	|
	nt_drawrbox
	|
	nt_drawbar
	;

nt_drawcircle:
	T_DRAWCIRCLE T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawCircle (((struct parserData *)param)->canvas, $2, $3, $4, 0, $5);
	}

	|

	T_DRAWCIRCLE T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawCircle (((struct parserData *)param)->canvas, $2, $3, $4, $6, $5);
	}
	;

nt_drawrbox:
	T_DRAWRBOX T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawRoundBox (((struct parserData *)param)->canvas, $2, $3, $4, $5, 0, $6);
	}

	|

	T_DRAWRBOX T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawRoundBox (((struct parserData *)param)->canvas, $2, $3, $4, $5, $7, $6);
	}
	;

nt_drawbar:
	T_DRAWBAR T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawBar (((struct parserData *)param)->canvas, $2, $3, $4, $5, $6, $7, $8, 1);
	}

	|

	T_DRAWBAR T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_drawBar (((struct parserData *)param)->canvas, $2, $3, $4, $5, $6, $7, $8, $9);
	}
	;

nt_wbmp_command:
	T_WBMPSPLASH nt_string T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		g15r_loadWbmpSplash (((struct parserData *)param)->canvas, $2);
		free ($2);
	}

	|

	T_WBMPLOAD T_NUMBER nt_string T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		int width = 0;
		int height = 0;

		char *buffer = g15r_loadWbmpToBuf ($3, &width, &height);
		int ret = add_buf (((struct parserData *)param)->buflist, $2, buffer, width, height);
		free ($3);
	}
	;

nt_mode_command:
	T_MODECACHE T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		int was_cached = ((struct parserData *)param)->canvas->mode_cache;
		((struct parserData *)param)->canvas->mode_cache = $2;
		if (was_cached)
		  updateScreen (((struct parserData *)param)->canvas, ((struct parserData *)param)->g15screen_fd, 1);
	}

	|

	T_MODEREV T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		((struct parserData *)param)->canvas->mode_reverse = $2;
	}

	|

	T_MODEXOR T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		((struct parserData *)param)->canvas->mode_xor = $2;
	}

	|

	T_MODEPRI T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);

                int fore = ($2 == 0 ? 1 : 0);
                int rear = ($2 == 1 ? 1 : 0);
                int revert = ($2 == 2 ? 1 : 0);

                int dummy=0;

                int at_front = g15_send_cmd (((struct parserData *)param)->g15screen_fd, G15DAEMON_IS_FOREGROUND, dummy);
                int user_to_front = g15_send_cmd(((struct parserData *)param)->g15screen_fd,G15DAEMON_IS_USER_SELECTED, dummy);

		int sendCmd = 0;
		if (at_front == 1)
		  {
		    if (rear == 1)		/* we want to go to the back */
		      {
			sendCmd = 1;
		      }
		    else if ((user_to_front == 0) && (revert == 1))	/* we want to go to the back if forced to the front */
		      {
			sendCmd = 1;
		      }
		  }
		else
		  {
		    if (fore == 1)		/* we want to go to the front */
		      {
			sendCmd = 1;
		      }
		    else if ((user_to_front == 1) && (revert == 1))	/* we want to take back the foreground if forced to the back */
		      {
			sendCmd = 1;
		      }
		  }
		if (sendCmd == 1){
		  g15_send_cmd(((struct parserData *)param)->g15screen_fd,G15DAEMON_SWITCH_PRIORITIES,dummy);
               }
              ((struct parserData *)param)->cmd_only=1;
	}
	;

nt_font_command:
	T_FONTLOAD T_NUMBER T_NUMBER nt_string T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		#ifdef TTF_SUPPORT
		g15r_ttfLoad (((struct parserData *)param)->canvas, $4, $3, $2);
		#endif
		free ($4);
	}

	|

	T_FONTPRINT T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER T_NUMBER nt_string T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		#ifdef TTF_SUPPORT
		g15r_ttfPrint (((struct parserData *)param)->canvas, $4, $5, $3, $2, $6, $7, $8);
		#endif
		free ($8);
	}
	;

nt_text_command:
	T_TEXTSMALL nt_strings T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		((struct parserData *)param)->listptr = $2;
		((struct parserData *)param)->itemptr = ((struct parserData *)param)->listptr->first_string;

		int row = 0;
		for (row = 0; ((struct parserData *)param)->itemptr != 0; ++row)
		  {
			g15r_renderString (((struct parserData *)param)->canvas, ((struct parserData *)param)->itemptr->string, row, G15_TEXT_SMALL, 0, 0);
			((struct parserData *)param)->itemptr = ((struct parserData *)param)->itemptr->next_string;
		  }
	}

	|

	T_TEXTMED nt_strings T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		((struct parserData *)param)->listptr = $2;
		((struct parserData *)param)->itemptr = ((struct parserData *)param)->listptr->first_string;

		int row = 0;
		for (row = 0; ((struct parserData *)param)->itemptr != 0; ++row)
		  {
			g15r_renderString (((struct parserData *)param)->canvas, ((struct parserData *)param)->itemptr->string, row, G15_TEXT_MED, 0, 0);
			((struct parserData *)param)->itemptr = ((struct parserData *)param)->itemptr->next_string;
		  }
	}

	|

	T_TEXTLARGE nt_strings T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		((struct parserData *)param)->listptr = $2;
		((struct parserData *)param)->itemptr = ((struct parserData *)param)->listptr->first_string;

		int row = 0;
		for (row = 0; ((struct parserData *)param)->itemptr != 0; ++row)
		  {
			g15r_renderString (((struct parserData *)param)->canvas, ((struct parserData *)param)->itemptr->string, row, G15_TEXT_LARGE, 0, 0);
			((struct parserData *)param)->itemptr = ((struct parserData *)param)->itemptr->next_string;
		  }
	}

	|

	T_TEXTOVERLAY T_NUMBER T_NUMBER T_NUMBER T_NUMBER nt_strings T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		((struct parserData *)param)->listptr = $6;
		((struct parserData *)param)->itemptr = ((struct parserData *)param)->listptr->first_string;

		int row = 0;
		for (row = 0; ((struct parserData *)param)->itemptr != 0; ++row)
		  {
			if ($5)
			  {
			  	unsigned int dispcol = 0;
				unsigned int len = strlen (((struct parserData *)param)->itemptr->string);

				dispcol = getDispCol (len, $4, $5);
				g15r_renderString (((struct parserData *)param)->canvas, ((struct parserData *)param)->itemptr->string, row, $4, dispcol, $3);
			  }
			else
			  g15r_renderString (((struct parserData *)param)->canvas, ((struct parserData *)param)->itemptr->string, row, $4, $2, $3);
			((struct parserData *)param)->itemptr = ((struct parserData *)param)->itemptr->next_string;
		  }
	}
	;

nt_key_command:
	T_KEYL T_NUMBER T_NEWLINE
	{
	}

	|

	T_KEYM T_NUMBER T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		int sendCmd = 1;
		int LEDon = $3;
		((struct parserData *)param)->mkey_state |= G15DAEMON_MKEYLEDS;
		switch ($2)
		  {
		  case 0:
		    {
		      if (LEDon != 0)
			((struct parserData *)param)->mkey_state |= G15_LED_M1 | G15_LED_M2 | G15_LED_M3;
		      else
			((struct parserData *)param)->mkey_state &= ~G15_LED_M1 & ~G15_LED_M2 & ~G15_LED_M3;
		      break;
		    }
		  case 1:
		    {
		      if (LEDon != 0)
			((struct parserData *)param)->mkey_state |= G15_LED_M1;
		      else
			((struct parserData *)param)->mkey_state &= ~G15_LED_M1;
		      break;
		    }
		  case 2:
		    {
		      if (LEDon != 0)
			((struct parserData *)param)->mkey_state |= G15_LED_M2;
		      else
			((struct parserData *)param)->mkey_state &= ~G15_LED_M2;
		      break;
		    }
		  case 3:
		    {
		      if (LEDon != 0)
			((struct parserData *)param)->mkey_state |= G15_LED_M3;
		      else
			((struct parserData *)param)->mkey_state &= ~G15_LED_M3;
		      break;
		    }
		  default:
		    {
		      sendCmd = 0;
		      break;
		    }
		  }
		((struct parserData *)param)->cmd_only=1;
		if (sendCmd == 1)
		  send (((struct parserData *)param)->g15screen_fd, &((struct parserData *)param)->mkey_state, 1, MSG_OOB);
	}
	;

nt_lcd_command:
	T_LCDBL T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
	        g15_send_cmd(((struct parserData *)param)->g15screen_fd,G15DAEMON_BACKLIGHT, $2);
		((struct parserData *)param)->cmd_only=1;

	}

	|

	T_LCDCON T_NUMBER T_NEWLINE
	{
		if (((struct parserData *)param)->background == 1)
		  return (0);
		char msgbuf = G15DAEMON_CONTRAST | $2;
		send (((struct parserData *)param)->g15screen_fd, &msgbuf, 1, MSG_OOB);
		((struct parserData *)param)->cmd_only=1;
	}
	;

nt_screen_command:
	T_SCREENNEW nt_string T_NEWLINE
	{
		struct parserData *newParam = (struct parserData *) malloc (sizeof (struct parserData));
		newParam->background = 0;
		newParam->threads = ((struct parserData *)param)->threads;
		newParam->fifo_filename = strdup ($2);

	  	pthread_create (&newParam->thread, NULL, threadEntry, (void *) newParam);
		add_thread (newParam);
		free ($2);
	}

	|

	T_SCREENCLOSE T_NEWLINE
	{
		((struct parserData *)param)->leaving = 1;
	}
	;

%%
