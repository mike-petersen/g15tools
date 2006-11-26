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

#include "g15composer.tab.h"

#define YY_INPUT(buf,result,max_size) \
    { \
    int c = getc(yyin); \
    result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
    }
%}

%option bison-bridge reentrant noyywrap

%%

^PS			return T_PIXELSET;
^PF			return T_PIXELFILL;
^PR			return T_PIXELRFILL;
^PO			return T_PIXELOVERLAY;
^PB			return T_PIXELBOX;
^PC			return T_PIXELCLEAR;
^DL			return T_DRAWLINE;
^DC			return T_DRAWCIRCLE;
^DR			return T_DRAWRBOX;
^DB			return T_DRAWBAR;
^DN 			return T_DRAWNUM;
^DI 			return T_DRAWICON;
^DS 			return T_DRAWSPRITE;
^WS 			return T_WBMPSPLASH;
^WL 			return T_WBMPLOAD;
^MC			return T_MODECACHE;
^MR			return T_MODEREV;
^MX			return T_MODEXOR;
^MP			return T_MODEPRI;
^FL			return T_FONTLOAD;
^FP			return T_FONTPRINT;
^TS			return T_TEXTSMALL;
^TM			return T_TEXTMED;
^TL			return T_TEXTLARGE;
^TO			return T_TEXTOVERLAY;
^KL			return T_KEYL;
^KM			return T_KEYM;
^LB			return T_LCDBL;
^LC			return T_LCDCON;
^SN			return T_SCREENNEW;
^SC			return T_SCREENCLOSE;
[0-9]+                  yylval->number=atoi(yytext); return T_NUMBER;
\"[^\"\r\n]*\"		yylval->string=strdup(yytext+1); yylval->string[yyleng-2] = NULL; return T_STRING;
\n                      return T_NEWLINE;
[ \t]+                  /* ignore whitespace */;

%%
