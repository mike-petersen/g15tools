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

#define G15DAEMON

#ifndef BLACKnWHITE
#define BLACK 1
#define WHITE 0
#define BLACKnWHITE 
#endif

#define GKEY_OFFSET 167
#define MKEY_OFFSET 185
#define LKEY_OFFSET 189

#define G15KEY_DOWN 1
#define G15KEY_UP 0

#define LCD_WIDTH 160
#define LCD_HEIGHT 43

/* tcp server defines */
#define LISTEN_PORT 15550
#define LISTEN_ADDR "127.0.0.1"
/* any more than this number of simultaneous clients will be rejected. */
#define MAX_CLIENTS 10

typedef struct lcd_s
{
    int lcd_type;
    unsigned char buf[1048];
    int max_x;
    int max_y;
    int connection;
    long int ident;
} lcd_t;

typedef struct lcdnode_s lcdnode_t;
typedef struct lcdlist_s lcdlist_t;

struct lcdnode_s {
    lcdlist_t *list;
    lcdnode_t *prev;
    lcdnode_t *next;
    lcd_t *lcd;
}lcdnode_s;

struct lcdlist_s
{
    lcdnode_t *head;
    lcdnode_t *tail;
    lcdnode_t *current;
}lcdlist_s;

pthread_mutex_t lcdlist_mutex;
pthread_mutex_t g15lib_mutex;

/* server hello */
#define SERV_HELO "G15 daemon HELLO"

/* uinput & keyboard control */
int g15_init_uinput();
void g15_exit_uinput();
void g15_uinput_process_keys(lcdlist_t *displaylist, unsigned int currentkeys, unsigned int lastkeys);

/* call create_lcd for every new client, and quit it when done */
lcd_t * create_lcd ();
void quit_lcd (lcd_t * lcd);
void write_buf_to_g15(lcd_t *lcd);

void setpixel (lcd_t * lcd, unsigned int x1, unsigned int y1, unsigned int color);
void cls (lcd_t * lcd, int color);
void line (lcd_t * lcd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
void rectangle (lcd_t * lcd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int filled, unsigned int color);
void circle (lcd_t * lcd, int x, int y, int r, int filled, int color);
void roundrectangle (lcd_t * lcd, int x1, int y1, int x2, int y2, int filled, int color, int type);
void draw_bitmap_char (lcd_t * lcd, unsigned char chr, int x, int y, int color, int doublesize, int bold);
void draw_bitmap_str (lcd_t * lcd, char *str, int x, int y, int color, int size, int bold);
void lcd_printf(lcd_t *lcd, int x, int y, const char *fmt, ...);
void draw_bar (lcd_t * lcd, int x1, int y1, int x2, int y2, int color, int num, int max);
void draw_bignum (lcd_t * lcd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color, int num);
void lcdclock(lcd_t *lcd);
/* utility functions in utility_func.c */
void pthread_sleep(int seconds);
int pthread_msleep(int milliseconds);

/* linked lists */
lcdlist_t *lcdlist_init();
void lcdlist_destroy(lcdlist_t **displaylist);
lcdnode_t *lcdnode_add(lcdlist_t **display_list);
void lcdnode_remove(lcdnode_t *badnode);

/* create a listening socket */
int init_sockserver();
int g15_clientconnect(lcdlist_t **g15daemon,int listening_socket);
int g15_send(int sock, char *buf, unsigned int len);
int g15_recv(lcdnode_t *lcdnode, int sock, char *buf, unsigned int len);

/* handy function from xine_utils.c */
void *g15_xmalloc(size_t size);

