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
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>
#include <libgen.h>
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
  fprintf (stderr, "Error: %s\n", err);
  return (0);
}

void
printUsage ()
{
  fprintf (stdout, "Usage: g15composer [-b] [-u username] /path/to/fifo\n");
  fprintf (stdout, "       cat instructions > /path/to/fifo\n\n");
  fprintf (stdout, "Display composer for the Logitech G15 LCD\n");
}

void *
threadEntry (void *arg)
{
  struct parserData *param = (struct parserData *) arg;
  extern short g15c_logo_data[6880];
  int tmpfd, errno;
  FILE *fifo;

  param->leaving = 0;

  mode_t mode = S_IRUSR | S_IWUSR | S_IWGRP;
  tmpfd = open (param->fifo_filename, O_WRONLY | O_NDELAY);
  /* Create fifo if it does not exist */
  if (tmpfd == -1 && errno == ENOENT)
    {
      if (mkfifo (param->fifo_filename, mode))
	{
	  fprintf (stderr, "Error: Could not create FIFO %s, aborting.\n",
		   param->fifo_filename);
	  free (param);
	  close (tmpfd);
	  pthread_exit (NULL);
	}
      chmod (param->fifo_filename, mode);
    }
  /* Exit if unable to access fifo */
  else if (tmpfd == -1 && errno != ENXIO)
    {
      fprintf (stderr, "Error: Unable to access %s, aborting.\n",
	       param->fifo_filename);
      free (param);
      close (tmpfd);
      pthread_exit (NULL);
    }
  /* Exit if fifo is regular file or fifo being read */
  else if (tmpfd > 0)
    {
      fprintf (stderr, "Error: No usable FIFO %s, aborting.\n",
	       param->fifo_filename);
      free (param);
      close (tmpfd);
      pthread_exit (NULL);
    }

  close (tmpfd);

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
      param->buflist = new_bufList ();
    }

  fifo = fopen (param->fifo_filename, "r");
  if (fifo == NULL)
    {
      perror (param->fifo_filename);
      param->leaving = 1;
    }
  else
    yyset_in (fifo, param->scanner);

  int result = 0;
  while ((param->leaving == 0) && (param->threads->leaving == 0))
    {
      result = yyparse (param);
      fclose (yyget_in (param->scanner));
      if ((param->leaving == 0) && (param->threads->leaving == 0))
	{
	  fifo = fopen (param->fifo_filename, "r");
	  if (fifo == NULL)
	    {
	      perror (param->fifo_filename);
	      param->leaving = 1;
	    }
	  else
	    yyset_in (fifo, param->scanner);

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

  if (strncmp (param->fifo_filename, "/var/run/", 9))
    unlink (param->fifo_filename);

  del_thread (param);

  free (param);

  pthread_exit (NULL);
}

int
main (int argc, char *argv[])
{
  struct parserData *param =
    (struct parserData *) malloc (sizeof (struct parserData));
  param->background = 0;
  param->fifo_filename = NULL;

  unsigned char user[256];
  unsigned char group[256];
  int reuse_fifo = 0;

  int i = 1;
  for (i = 1; (i < argc && param->fifo_filename == NULL); ++i)
    {
      if (!strcmp (argv[i], "-h") || !strcmp (argv[i], "--help"))
	{
	  printUsage ();
	  return 0;
	}
      else if (!strcmp (argv[i], "-b"))
	{
	  param->background = 1;
	}
      else if (!strcmp (argv[i], "-r"))
        {
	  reuse_fifo = 1;
	}
      else if (!strcmp (argv[i], "-u") || !strcmp (argv[i], "--user"))
	{
	  if (argv[i + 1] != NULL)
	    {
	      strncpy ((char *) user, argv[i + 1], 128);
	      i++;
	    }
	}
      else if (!strcmp (argv[i], "-g") || !strcmp (argv[i], "--group"))
        {
	  if (argv[i + 1] != NULL)
	    {
	      strncpy ((char *) group, argv[i + 1], 128);
	      i++;
	    }
	}
      else
	{
	  param->fifo_filename = argv[i];
	}
    }

  if (param->fifo_filename != NULL)
    {
      char *dirc, *dname;
      dirc = strdup (param->fifo_filename);
      dname = dirname (dirc);

      if (strncmp (dname, "/", 1))
	{
	  char *bname, cwd[256];
	  bname = strdup (param->fifo_filename);
	  getcwd (cwd, 256);
	  sprintf (param->fifo_filename, "%s/%s", cwd, bname);
	}

      if (reuse_fifo)
        {
      	  int tmpfd = open ("/var/run/g15composer", O_WRONLY | O_NDELAY);
      	  if (tmpfd > 0)
	    {
	      FILE *control = fdopen (tmpfd, "w");
	      fprintf (control, "SN \"%s\"\n", param->fifo_filename);
	      fclose (control);
	      close (tmpfd);
	      return 0;
	    }
	}

      struct passwd *euser;

      if (strlen ((char *) user) == 0)
	euser = getpwnam ("nobody");
      else
	euser = getpwnam ((char *) user);

      if (euser == NULL)
	euser = getpwuid (geteuid ());

      if (strlen ((char *) group))
	{
	  setegid (atoi(group));
	}

      if (euser != NULL)
	{
	  if (group == NULL)
	    setegid (euser->pw_gid);
	  seteuid (euser->pw_uid);
	}

      struct threadList *thread_list = new_threadList ();
      param->threads = thread_list;
      pthread_create (&param->thread, NULL, threadEntry, (void *) param);
      add_thread (param);
      pthread_join (param->thread, NULL);

      thread_list->leaving = 1;

      struct threadItem *tmp_thread = thread_list->first_thread;
      struct threadItem *next_thread;
      while (tmp_thread != NULL)
        {
	  next_thread = tmp_thread->next;
	  FILE *to_close = fopen (tmp_thread->data->fifo_filename, "w");
	  fprintf (to_close, "SC\n");
	  pthread_join (tmp_thread->thread, NULL);
	  fclose (to_close);
	  tmp_thread = next_thread;
	}
    }
  else
    {
      fprintf (stderr, "Please provide a FIFO filename to read from.\n");
      return -1;
    }

  return 0;
}

struct strList *
new_strList ()
{
  struct strList *new;
  new = (struct strList *) malloc (sizeof (struct strList));
  if (new == NULL)
    return NULL;
  new->first_string = NULL;
  new->last_string = NULL;

  return new;
}

void
add_string (struct strList *list, char *string)
{
  struct strItem *new;
  new = (struct strItem *) malloc (sizeof (struct strItem));
  if (new == NULL)
    return;
  new->string = strdup (string);
  new->next_string = NULL;

  if (list->first_string == NULL)
    list->first_string = new;
  else
    list->last_string->next_string = new;
  list->last_string = new;
  free (string);
}

struct bufList *
new_bufList ()
{
  struct bufList *new;
  new = (struct bufList *) malloc (sizeof (struct bufList));
  if (new == NULL)
    return NULL;
  new->first_buf = NULL;
  new->last_buf = NULL;

  return new;
}

int
add_buf (struct bufList *bufList, int id, char *buffer, int width, int height)
{
  struct bufItem *new = NULL;
  struct bufItem *tmp;

  tmp = bufList->first_buf;

  while (tmp != NULL)
    {
      if (tmp->id == id)
	{
	  new = tmp;
	  free (new->buffer);
	}
      tmp = tmp->next;
    }

  /* Allocate new if we didn't match by id */
  if (new == NULL)
    new = (struct bufItem *) malloc (sizeof (struct bufItem));

  /* Exit if new still == NULL */
  if (new == NULL)
    return -1;

  new->buffer = buffer;
  new->id = id;
  new->width = width;
  new->height = height;
  new->next = NULL;

  if (bufList->first_buf == NULL)
    bufList->first_buf = new;
  else
    bufList->last_buf->next = new;

  bufList->last_buf = new;

  return 0;
}

struct threadList * 
new_threadList ()
{
  struct threadList *new;
  new = (struct threadList *) malloc (sizeof (struct threadList));
  if (new == NULL)
    return NULL;

  new->first_thread = NULL;
  new->last_thread = NULL;
  new->leaving = 0;
  pthread_mutex_init (&new->mutex, NULL);

  return new;
}

void 
add_thread (struct parserData *data)
{
  pthread_mutex_lock (&data->threads->mutex);

  struct threadItem *new;
  new = (struct threadItem *) malloc (sizeof (struct threadItem));
  if (new == NULL)
    return;
  new->thread = data->thread;
  new->data = data;
  new->next = NULL;

  if (data->threads->first_thread == NULL)
    data->threads->first_thread = new;
  else
    data->threads->last_thread->next = new;
  data->threads->last_thread = new;

  pthread_mutex_unlock (&data->threads->mutex);
}

void
del_thread (struct parserData *data)
{
  pthread_mutex_lock (&data->threads->mutex);

  struct threadItem *old = data->threads->first_thread;
  struct threadItem *prev = data->threads->first_thread;

  while (old != NULL)
  {
    if (old->thread == data->thread)
      {
	if (prev != data->threads->first_thread)
          prev->next = old->next;
	else
	  data->threads->first_thread = old->next;
	
	old->data = NULL;
	old->next = NULL;
	free (old);
	break;
      }

    prev = old;
    old = old->next;
  }

  pthread_mutex_unlock (&data->threads->mutex);
}

void
updateScreen (g15canvas * canvas, int g15screen_fd, int force)
{
  if (force || !canvas->mode_cache)
    g15_send (g15screen_fd, (char *) canvas->buffer, G15_BUFFER_LEN);
}

int
getDispCol (int len, int size, int type)
{
  #define TEXT_CENTER 1
  #define TEXT_RIGHT  2
  #define WIDTH_SMALL 4
  #define WIDTH_MED   5
  #define WIDTH_LARGE 8

  int dispcol = 0;

  switch (size)
    {
    case G15_TEXT_SMALL:
      {
	if (type == TEXT_CENTER)
	  dispcol = ((G15_LCD_WIDTH / 2) - ((len * WIDTH_SMALL) / 2));
	else if (type == TEXT_RIGHT)
	  dispcol = (G15_LCD_WIDTH - (len * WIDTH_SMALL));
	break;
      }
    case G15_TEXT_MED:
      {
	if (type == TEXT_CENTER)
	  dispcol = ((G15_LCD_WIDTH / 2) - ((len * WIDTH_MED) / 2));
	else if (type == TEXT_RIGHT)
	  dispcol = (G15_LCD_WIDTH - (len * WIDTH_MED));
	break;
      }
    case G15_TEXT_LARGE:
      {
	if (type == TEXT_CENTER)
	  dispcol = ((G15_LCD_WIDTH / 2) - ((len * WIDTH_LARGE) / 2));
	else if (type == TEXT_RIGHT)
	  dispcol = (G15_LCD_WIDTH - (len * WIDTH_LARGE));
	break;
      }
    default:
      break;
    }

  if (dispcol < 0)
    dispcol = 0;

  return dispcol;
}
