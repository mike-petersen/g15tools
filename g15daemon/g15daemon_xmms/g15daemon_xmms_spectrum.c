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

/* xmms plugin for the daemon, based on finespectrum plugin available on sourceforge */

#include <g15daemon_client.h>
#include <math.h>
#include <pthread.h>
#include <xmms/plugin.h>
#include <xmms/util.h>
#include <string.h>
#include <glib.h>

#define WIDTH 256

/* Linearity of the amplitude scale (0.5 for linear, keep in [0.1, 0.9]) */
#define linearity 0.33

/* Time factor of the band dinamics. 3 means that the coefficient of the
last value is half of the current one's. (see source) */
#define tau 3

/* Factor used for the diffusion. 4 means that half of the height is
added to the neighbouring bars */
#define dif 3

static gint16 bar_heights[WIDTH];

static gdouble scale, x00, y00;

static void g15analyser_init(void);
static void g15analyser_cleanup(void);
static void g15analyser_playback_start(void);
static void g15analyser_playback_stop(void);
static void g15analyser_render_freq(gint16 data[2][256]);

static unsigned char lcdbuf[6880];
static unsigned int leaving=0;
pthread_t g15send_thread_hd;
static int g15screen_fd = -1;

VisPlugin g15analyser_vp = {
    NULL,
    NULL,
    0,
    "G15daemon Spectrum Analyzer 0.1",
    0,
    1,
    g15analyser_init, /* init */
    g15analyser_cleanup, /* cleanup */
    NULL, /* about */
    NULL, /* configure */
    NULL, /* disable_plugin */
    g15analyser_playback_start, /* playback_start */
    g15analyser_playback_start, /* playback_stop */
    NULL, /* render_pcm */
    g15analyser_render_freq  /* render_freq */
};


VisPlugin *get_vplugin_info(void) {
    return &g15analyser_vp;
}

/* simple graphics functions */

static void setpixel(unsigned char *buf, int x, int y) {
    
    buf[x + (G15_WIDTH * y)]= 1;
}

void *g15send_thread() {
    int i;
    gint16 z=0;
    int vline;
    while(!leaving){
        memset(lcdbuf,0,6880);

        for (i=0; i<G15_WIDTH;i++)
            setpixel(lcdbuf,i,G15_HEIGHT-1);
        
        for (i = 0; i < G15_WIDTH; i+=2) {//we lose a fair bit of resolution, but its for the best
            z=(gint16)bar_heights[i+20]; 
            if(z<0)  
                z = 1;
            if(z>42) 
                z = 42;
            for(vline=G15_HEIGHT-1-z;vline<G15_HEIGHT;vline++)
                setpixel(lcdbuf,i,vline);
        }
    
        g15_send(g15screen_fd,(char*)lcdbuf,6880);

        xmms_usleep(100000);
    }

    return;
}


static void g15analyser_init(void) {

    pthread_attr_t attr;
    memset(lcdbuf,0,6880);
    memset(&attr,0,sizeof(pthread_attr_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    
    if((g15screen_fd = new_g15_screen(G15_PIXELBUF))<0){
        return;
    }
    scale = G15_HEIGHT / ( log((1 - linearity) / linearity) *2 );
    x00 = linearity*linearity*32768.0/(2 * linearity - 1);
    y00 = -log(-x00) * scale;
    
    leaving = 0;
    pthread_create(&g15send_thread_hd, &attr, g15send_thread, 0);
}

static void g15analyser_cleanup(void) {
    
    leaving=1;
    
    if(g15screen_fd)
        close(g15screen_fd);
}

static void g15analyser_playback_start(void) {
    
    memset(lcdbuf,0,6880);
}

static void g15analyser_render_freq(gint16 data[2][256]) {
    
    gint i;
    gdouble y;
    
    if(!g15screen_fd)
        return;
    
    for (i = 0; i < WIDTH; i++) {
        y = (gdouble)data[0][i] * (i + 1); /* Compensating the energy */
        y = ( log(y - x00) * scale + y00 ); /* Logarithmic amplitude */

        y = ( (dif-2)*y + /* FIXME: conditionals should be rolled out of the loop */
                (i==0       ? y : bar_heights[i-1]) +
                (i==WIDTH-1 ? y : bar_heights[i+1])) / dif; /* Add some diffusion */
        y = ((tau-1)*bar_heights[i] + y) / tau; /* Add some dynamics */
        bar_heights[i] = (gint16)y;
    }

    return;
}
































