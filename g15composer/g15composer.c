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
#include "g15c_logo.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

int 
yyerror (char *err) 
{
	fprintf (stderr, "Error: %s\n",err);
	return (0);
}

void
printUsage ()
{
  fprintf (stdout, "Usage: g15composer [-b] /path/to/fifo\n");
  fprintf (stdout, "       cat instructions > /path/to/fifo\n\n");
  fprintf (stdout, "Display composer for the Logitech G15 LCD\n");
}

void
*threadEntry (void *arg)
{
	struct parserData *param = (struct parserData *)arg;
	extern short g15c_logo_data[6880];

	param->leaving = 0;
	param->keepFifo = 0;

	mode_t mode = S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH;
  	if (mkfifo (param->fifo_filename, mode))
  	  {
		fprintf (stderr, "Error: Could not create FIFO %s, aborting.\n", param->fifo_filename);
		param->leaving = 1;
		param->keepFifo = 1;
		free (param);
		pthread_exit (NULL);
	  }
	chmod (param->fifo_filename, mode);

	yylex_init (&param->scanner);

	if (!param->background)
  	  {
		param->canvas = (g15canvas *) malloc (sizeof (g15canvas));
		param->g15screen_fd = 0;
	  	if ((param->g15screen_fd = new_g15_screen (G15_G15RBUF)) < 0)
	    	  {
	  		fprintf (stderr, "Sorry, can't connect to g15daemon\n");
			param->leaving = 1;
	    	  }
		g15r_initCanvas (param->canvas);
		param->canvas->mode_reverse = 1;
		g15r_pixelOverlay (param->canvas, 0, 0, 160, 43, g15c_logo_data);
		param->canvas->mode_reverse = 0;
		updateScreen (param->canvas, param->g15screen_fd, 1);
		g15r_clearScreen (param->canvas, G15_COLOR_WHITE);
	  }

	if ((yyset_in (fopen(param->fifo_filename, "r"), param->scanner)) == 0)
	  {
	  	perror (param->fifo_filename);
		param->leaving = 1;
	  }

	int result = 0;
	while (param->leaving == 0)
	  {
		result = yyparse(param);
		fclose (yyget_in(param->scanner));
		if (param->leaving == 0)
		  {
			if ((yyset_in(fopen (param->fifo_filename,"r"), param->scanner)) == 0)
			  {
			  	perror (param->fifo_filename);
				param->leaving = 1;
			  }
			if (param->background == 1)
			  continue;
			if (!param->canvas->mode_cache)
			  g15r_clearScreen (param->canvas, G15_COLOR_WHITE);
		  }
	  }

	if (!param->background)
	  {
		if (param->g15screen_fd)
 	  	  g15_close_screen (param->g15screen_fd);
		if (param->canvas != NULL)
		  free (param->canvas);
	  }

	yylex_destroy (param->scanner);

	if (param->keepFifo == 0)
	  unlink (param->fifo_filename);

	free (param);
	
	pthread_exit(NULL);
}

int 
main (int argc, char *argv[])
{
	struct parserData *param = (struct parserData *) malloc (sizeof (struct parserData));
	param->background = 0;
	param->fifo_filename = NULL;

	int i = 1;
	for (i = 1; (i < argc && param->fifo_filename == NULL); ++i)
	  {
	    if (!strcmp (argv[i],"-h") || !strcmp (argv[i],"--help"))
	      {
	        printUsage ();
	        return 0;
	      }
	    else if (!strcmp (argv[i],"-b"))
	      {
	        param->background = 1;
	      }
	    else
	      {
	        param->fifo_filename = argv[i];
	      }
	  }
	
	if (param->fifo_filename != NULL)
	  {
	  	pthread_create (&param->thread, NULL, threadEntry, (void *) param);
		pthread_join (param->thread, NULL);
	  }
}

struct strList * 
new_strList ()
{
	struct strList *new;
	new = (struct strList *) malloc (sizeof (struct strList));
	new->first_string = 0;
	new->last_string = 0;

	return new;
}

void 
add_string (struct strList *list, char *string)
{
	struct strItem *new;
	new = (struct strItem *) malloc (sizeof (struct strItem));
	new->string = strdup(string);
	new->next_string = NULL;

	if (list->first_string == 0)
	  list->first_string = new;
	else
	  list->last_string->next_string = new;
	list->last_string = new;
	free (string);
}

void
updateScreen (g15canvas *canvas, int g15screen_fd, int force)
{
	if (force || !canvas->mode_cache)
	  g15_send (g15screen_fd, (char *) canvas->buffer, 1048);
}

int
getDispCol (int len, int size, int type)
{
	int dispcol = 0;

	switch (size)
	  {
	  	case 0:
		  {
			if (type == 1)
 	  		  dispcol = (80 - ((len * 4) / 2));
			else if (type == 2)
			  dispcol = (160 - (len * 4));
			break;
		  }
		case 1:
		  {
			if (type == 1)
 	  		  dispcol = (80 - ((len * 5) / 2));
			else if (type == 2)
			  dispcol = (160 - (len * 5));
			break;
		  }
		case 2:
		  {
			if (type == 1)
 	  		  dispcol = (80 - ((len * 8) / 2));
			else if (type == 2)
			  dispcol = (160 - (len * 8));
			break;
		  }
		default:
		  {
		  	dispcol = 0;
			break;
		  }
	  }
	
	if (dispcol < 0)
	  dispcol = 0;

	return dispcol;
}

