/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/



#include <stdio.h>
#include "dvapi.h"

#define MANAGER_TYPE 0x101B
#define WINDOW_TYPE 0x001B
#define QUERY_TYPE 0x011B

  typedef struct {
                word    type;
                word    size;
                byte    d[10];
                } short_stream;

extern char dvapibuf[260];


void api_cancel()
    {win_stream(win_me(),"\x1B\x10\x01\x00\xCB");}

void app_foreonly(win, yes) dword win; int yes;
        {win_stream(win,(yes) ? "\x1B\x10\x01\x00\x86" :
                                "\x1B\x10\x01\x00\x87");}

void app_goback(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC9");}

void app_gofore(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC1");}

void app_hide(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC7");}

void app_show(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC6");}

void app_suspend(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC8");}

void fld_altmode(win, yes) dword win; int yes;
        {win_stream(win,(yes) ? "\x1B\x10\x01\x00\xBD" :
                                "\x1B\x10\x01\x00\xBC");}

void fld_attr(win, fld, attr) dword win; int fld, attr;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] = 0xF2;
        s.d[1] = fld;
        s.d[2] = attr;
        win_stream(win, &s);
        }

void fld_char(win, fld, chr) dword win; int fld, chr;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] = 0xF1;
        s.d[1] = fld;
        s.d[2] = chr;
        win_stream(win, &s);
        }

void fld_clear(win, fld) dword win; int fld;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 2;
        s.d[0] = 0xF0;
        s.d[1] = fld;
        win_stream(win, &s);
        }

void fld_cursor(win, fld) dword win; int fld;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 2;
        s.d[0] = 0xF4;
        s.d[1] = fld;
        win_stream(win, &s);
        }

void fld_entry(win, fld, buffer) dword win; word fld; char *buffer;
        {
        int fsize;
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 10;
        s.d[0] = 0xF5;
        s.d[1] = fld;
        s.d[9] = 0xE7;
        win_stream(win, &s);
        fsize = (s.d[9] == 0xE7) ? 7:8;
        s.type = WINDOW_TYPE;
        memcpy (&s.d[2], buffer, 8);
        s.size = fsize+2;
        win_stream(win, &s);
        }

void fld_header(win, buffer) dword win; char *buffer;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 7;
        s.d[0] = 0xFC;
        memcpy (&s.d[1], buffer, 6);
        win_stream(win, &s);
        }

void fld_marker(win, mark) dword win; int mark;
        {
        short_stream s;
        s.type = MANAGER_TYPE;
        s.size = 2;
        s.d[0] = 0xAF;
        s.d[1] = mark;
        win_stream(win, &s);
        }

void fld_point(win, fld, row, col) dword win; int fld, row, col;
        {
        short_stream s;
        s.type = MANAGER_TYPE;
        s.size = 4;
        s.d[0] = 0xC4;
        s.d[1] = fld;
        s.d[2] = row;
        s.d[3] = col;
        win_stream(win, &s);
        }

void fld_reset(win) dword win;
        {win_stream(win, "\x1B\x00\x01\x00\xFE");}

void fld_scroll(win, fld, direction, text) dword win; int fld, direction, text;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] =(text) ? 0xCE : 0xCF;
        s.d[1] = 0xF8 + direction;
        s.d[2] = fld;
        win_stream(win, &s);
        }

void fld_swrite(win, fld, text) dword win; word fld; byte *text;
        {fld_write(win, fld, text, strlen(text));}

void fld_type(win, fld, type) dword win; int fld, type;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] = 0xF6;
        s.d[1] = fld;
        s.d[2] = type;
        win_stream(win, &s);
        }

void fld_write(win, fld, buffer, lbuffer)
        dword win;
        word fld, lbuffer;
        byte *buffer;
        {
        dvlockb();
        dvapibuf[0] = 0x1B;
        dvapibuf[1] = 0;
        dvapibuf[2] = lbuffer+2;
        dvapibuf[3] = 0;
        dvapibuf[4] = 0xF3;
        dvapibuf[5] = fld;
        memcpy(&dvapibuf[6], buffer, lbuffer);
        win_stream(win, dvapibuf);
        dvfreeb();
        }

int  qry_atread(win) dword win;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 1;
        s.d[0] = 0xD9;
        win_stream(win, &s);
        return(s.d[0] == 0xD9);
        }

int  qry_attr(win) dword win;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 2;
        s.d[0] = 0xE2;
        win_stream(win, &s);
        return((int)s.d[1]);
        }

int  qry_color(win, vid, attr) dword win; int vid, attr;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 4;
        s.d[0] = 0xEC;
        s.d[1] = vid;
        s.d[2] =((attr-1) << 4) + 1;
        win_stream(win, &s);
        return((int)s.d[3]);
        }

int  qry_ctrl(win) dword win;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 1;
        s.d[0] = 0xDC;
        win_stream(win, &s);
        return(s.d[0] == 0xDC);
        }

void qry_cursor(win, row, col) dword win; int *row, *col;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 3;
        s.d[0] = 0xC0;
        win_stream(win, &s);
        *row = s.d[1];
        *col = s.d[2];
        }

int qry_entry(win, fld, buffer) dword win; word fld; char *buffer;
        {
        int fsize;
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 10;
        s.d[0] = 0xF5;
        s.d[1] = fld;
        s.d[9] = 0xE7;
        win_stream(win, &s);
        fsize = (s.d[9] == 0xE7) ? 7:8;
        if (fsize == 7) s.d[9] = 0;
        memcpy (buffer, &s.d[2], 8);
        return (fsize);
        }

void qry_field(win, fld, buffer, lbuffer)
        dword win;
        int fld;
        char *buffer;
        int lbuffer;
        {
        dvlockb();
        dvapibuf[0] = 0x1B;
        dvapibuf[1] = 1;
        dvapibuf[2] = lbuffer+2;
        dvapibuf[3] = 0;
        dvapibuf[4] = 0xF3;
        dvapibuf[5] = fld;
        win_stream(win, dvapibuf);
        memcpy(buffer, &dvapibuf[6], lbuffer);
        dvfreeb();
        }

int  qry_frame(win) dword win;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 1;
        s.d[0] = 0xD6;
        win_stream(win, &s);
        return(s.d[0] == 0xD6);
        }

int  qry_frattr(win,vid) dword win; int vid;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 4;
        s.d[0] = 0xED;
        s.d[1] = vid;
        s.d[2] = 1;
        s.d[3] = 0;
        win_stream(win, &s);
        return((int)s.d[3]);
        }

void qry_header(win, buffer) dword win; char *buffer;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 7;
        s.d[0] = 0xFC;
        win_stream(win, &s);
        memcpy (buffer, &s.d[1], 6);
        }

int  qry_hidden(win) dword win;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 1;
        s.d[0] = 0xD5;
        win_stream(win, &s);
        return(s.d[0] == 0xD5);
        }

int  qry_leave(win) dword win;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 1;
        s.d[0] = 0xDF;
        win_stream(win, &s);
        return(s.d[0] == 0xDF);
        }

void qry_lsize(win, rows, cols) dword win; int *rows, *cols;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 3;
        s.d[0] = 0xC5;
        win_stream(win, &s);
        *rows = s.d[1];
        *cols = s.d[2];
        }

int  qry_logattr(win) dword win;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 1;
        s.d[0] = 0xDA;
        win_stream(win, &s);
        return(s.d[0] == 0xDA);
        }

void qry_origin(win, row, col) dword win; int *row, *col;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 3;
        s.d[0] = 0xC4;
        win_stream(win, &s);
        *row = s.d[1];
        *col = s.d[2];
        }

void qry_position(win, row, col) dword win; int *row, *col;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 3;
        s.d[0] = 0xC2;
        win_stream(win, &s);
        *row = s.d[1];
        *col = s.d[2];
        }

void qry_size(win, rows, cols) dword win; int *rows, *cols;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 3;
        s.d[0] = 0xC3;
        win_stream(win, &s);
        *rows = s.d[1];
        *cols = s.d[2];
        }

void qry_title(win, buffer, lbuffer)
        dword win;
        char *buffer;
        int  lbuffer;
        {
        dvlockb();
        dvapibuf[0] = 0x1B;
        dvapibuf[1] = 1;
        dvapibuf[2] = lbuffer+2;
        dvapibuf[3] = 0;
        dvapibuf[4] = 0xEF;
        dvapibuf[5] = lbuffer;
        win_stream(win, dvapibuf);
        memcpy(buffer,&dvapibuf[6], lbuffer);
        dvfreeb();
        }

int  qry_type(win, fld) dword win; int fld;
        {
        short_stream s;
        s.type = QUERY_TYPE;
        s.size = 3;
        s.d[0] = 0xF6;
        s.d[1] = fld;
        win_stream(win, &s);
        return((int)s.d[2]);
        }

    byte adopt_stream[] = {0x1b,0x10,0,0};

void win_adopt(win) dword win;
        {win_stream(win,"\x1B\x10\x00\x00");}

void win_allow(win, what)
        dword win;
        int what;
        {
        short_stream s;
        s.type = MANAGER_TYPE;
        s.size = 1;
        s.d[0] = what;
        win_stream(win, &s);
        }

void win_atread(win, on) dword win; int on;
        {win_stream(win,(on) ?  "\x1B\x00\x01\x00\xD9" :
                                "\x1B\x00\x01\x00\xD8");}

void win_attach(win, yes) dword win; int yes;
        {win_stream(win,(yes) ? "\x1B\x10\x01\x00\x84" :
                                "\x1B\x10\x01\x00\x85");}

void win_attr(win, attr)
        dword win;
        int attr;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 2;
        s.d[0] = 0xe2;
        s.d[1] = attr;
        win_stream(win, &s);
        }

void win_blanks(win, count) dword win; int count;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        if(count <= 32)
          {
          s.size = 1;
          s.d[0] = count % 32;
          }
        else
          {
          s.size = 2;
          s.d[0] = 0x80 +((count/256) % 8);
          s.d[1] = count % 256;
          }
        win_stream(win, &s);
        }

void win_bottom(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xCA");}

void win_cancel(win, what)
        dword win;
        int what;
        {win_notify(win, what+32);}

void win_color(win, vid, attr, color)
        dword win;
        int vid, attr, color;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 6;
        s.d[0] = 0xBE;          /* change reverse also */
        s.d[1] = 0xEC;
        s.d[2] = vid;
        s.d[3] =((attr-1) << 4) + 1;
        s.d[4] = color;
        s.d[5] = 0xBF;          /* don't change reverse */
        win_stream(win, &s);
        }

void win_ctrl(win, yes) dword win; int yes;
        {win_stream(win,(yes) ? "\x1B\x00\x01\x00\xDC" :
                                "\x1B\x00\x01\x00\xDD");}

void win_dflt(win) dword win;
        {
        short_stream s;
        s.type = MANAGER_TYPE;
        s.size = 1;
        s.d[0] = 0xAE;
        win_stream(win, &s);
        }

void win_disallow(win, what)
        dword win;
        int what;
        {win_allow(win, what+32);}

void win_frame(win, on) dword win; int on;
        {win_stream(win,(on) ?  "\x1B\x00\x01\x00\xD6" :
                                "\x1B\x00\x01\x00\xD7");}

void win_frattr(win, vid, attr) dword win; int vid, attr;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 4;
        s.d[0] = 0xED;
        s.d[1] = vid;
        s.d[2] = 0x88;
        s.d[3] = attr;
        win_stream(win, &s);
        }

void win_hide(win) dword win;
        {win_stream(win, "\x1B\x00\x01\x00\xD5");}

void win_leave(win, yes) dword win; int yes;
        {win_stream(win,(yes) ? "\x1B\x00\x01\x00\xDF" :
                                "\x1B\x00\x01\x00\xDE");}

void win_logattr(win, yes) dword win; int yes;
        {win_stream(win,(yes) ? "\x1B\x00\x01\x00\xDA" :
                                "\x1B\x00\x01\x00\xDB");}

void win_lsize(win, rows, cols)
        dword win;
        word rows, cols;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] = 0xC5;
        s.d[1] = rows;
        s.d[2] = cols;
        win_stream(win, &s);
        }

void win_maxsize(win, rows, cols)
        dword win;
        word rows, cols;
        {
        short_stream s;
        s.type = MANAGER_TYPE;
        s.size = 3;
        s.d[0] = 0x89;
        s.d[1] = rows;
        s.d[2] = cols;
        win_stream(win, &s);
        }

void win_minsize(win, rows, cols)
        dword win;
        word rows, cols;
        {
        short_stream s;
        s.type = MANAGER_TYPE;
        s.size = 3;
        s.d[0] = 0x88;
        s.d[1] = rows;
        s.d[2] = cols;
        win_stream(win, &s);
        }

void win_move(win, row, col)
        dword win;
        word row, col;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] = 0xC2;
        s.d[1] = row;
        s.d[2] = col;
        win_stream(win, &s);
        }

void win_notify(win, what)
        dword win;
        int what;
        {win_allow(win, what);}

void win_origin(win, row, col)
        dword win;
        word row, col;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] = 0xC4;
        s.d[1] = row;
        s.d[2] = col;
        win_stream(win, &s);
        }

void win_orphan(win) dword win;
        {win_stream(win,"\x1B\x10\x01\x00\xC5");}

void win_paste(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xCC");}

void win_point(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC3");}

void win_repattr(win, count, attr) dword win; int count,attr;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        if(count <= 32)
          {
          s.size = 2;
          s.d[0] = 0x40 +(count % 32);
          s.d[1] = attr;
          }
        else
          {
          s.size = 3;
          s.d[0] = 0x90 +((count/256) % 8);
          s.d[1] = count % 256;
          s.d[2] = attr;
          }
        win_stream(win, &s);
        }

void win_repchar(win, count, chr) dword win; int count,chr;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        if(count <= 32)
          {
          s.size = 2;
          s.d[0] = 0x20 +(count % 32);
          s.d[1] = chr;
          }
        else
          {
          s.size = 3;
          s.d[0] = 0x88 +((count/256) % 8);
          s.d[1] = count % 256;
          s.d[2] = chr;
          }
        win_stream(win, &s);
        }

void win_resize(win, row, col)
        dword win;
        word row, col;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 3;
        s.d[0] = 0xC3;
        s.d[1] = row;
        s.d[2] = col;
        win_stream(win, &s);
        }

void win_scroll(win, dir, text, top, left, rows, cols)
        dword win; int dir, text, top, left, rows, cols;
        {
        short_stream s;
        s.type = WINDOW_TYPE;
        s.size = 7;
        s.d[0] = 0xC1;
        s.d[1] = top;
        s.d[2] = left;
        s.d[3] =(text) ? 0xCE : 0xCF;
        s.d[4] = 0xE8 + dir;
        s.d[5] = rows;
        s.d[6] = cols;
        win_stream(win, &s);
        }

void win_swrite(win, text) dword win; byte *text;
       {win_write(win, text, strlen(text));}

void win_title(win, title, ltitle)
        dword win;
        byte *title;
        int ltitle;
        {
        dvlockb();
        dvapibuf[0] = 0x1B;
        dvapibuf[1] = 0;
        dvapibuf[2] = ltitle+2;
        dvapibuf[3] = 0;
        dvapibuf[4] = 0xEF;
        dvapibuf[5] = ltitle;
        memcpy(&dvapibuf[6], title, ltitle);
        win_stream(win, dvapibuf);
        dvfreeb();
        }

void win_top(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC2");}

void win_topsys(win) dword win;
        {win_stream(win, "\x1B\x10\x01\x00\xC0");}

void win_unhide(win) dword win;
        {win_stream(win, "\x1B\x00\x01\x00\xD4");}


