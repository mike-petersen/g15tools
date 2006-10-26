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

struct strList;
struct strItem;

struct strList
{
	struct strItem *first_string;
	struct strItem *last_string;
};

struct strItem
{
	char *string;
	struct strItem *next_string;
};

struct parserData
{
	int background;
	int g15screen_fd;
	g15canvas *canvas;
	struct strList *listptr;
	struct strItem *itemptr;
	char *fifo_filename;
	int mkey_state;
	int leaving;
	int keepFifo;
	void *scanner;
	pthread_t thread;
};

typedef struct strList *List;
typedef struct strItem *String;

int yyerror (char *err);
void printUsage ();
void *threadEntry (void *arg);
struct strList * new_strList ();
void add_string (struct strList *strList, char *string);
void updateScreen (g15canvas *canvas, int g15screen_fd, int force);
int getDispCol (int len, int size, int type);

#endif /* G15COMPOSER_H_ */
