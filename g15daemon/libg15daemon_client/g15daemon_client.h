/*
    This file is part of g15daemon.

    g15daemon is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    g15daemon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with g15daemon; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    
    (c) 2006 Mike Lampard, Philip Lawatsch, and others
    
    This daemon listens on localhost port 15550 for client connections,
    and arbitrates LCD display.  Allows for multiple simultaneous clients.
    Client screens can be cycled through by pressing the 'L1' key.
*/

#define G15_WIDTH 160
#define G15_HEIGHT 43

#define G15_BUFSIZE 6880
#define G15DAEMON_VERSION g15daemon_version()

#define G15_PIXELBUF 0
#define G15_TEXTBUF 1
#define G15_WBMPBUF 2

const char *g15daemon_version();

/* open a new connection to the g15daemon.  returns an fd to be used with g15_send & g15_recv */
/* screentype should be either 0 (graphic/pixelbuffer) or 1 (textbuffer). only Graphic buffers are
   supported in this version */
int new_g15_screen(int screentype);

/* close connection - just calls close() */
int g15_close_screen(int sock);

/* these two functions operate in the same way as send & recv, except they wont return 
 * until _all_ len bytes are sent or received, or an error occurs */
int g15_send(int sock, char *buf, unsigned int len);
int g15_recv(int sock, char *buf, unsigned int len);
