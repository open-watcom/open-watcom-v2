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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vi.h"
#include "win.h"

/*
 * DrawBorder - display border
 */
void DrawBorder( window_id wn )
{
    wind                *w;
    int                 i,j,k,stc,etc,ctc;
    int                 tl,bl,tr,br;
    int                 xtl,xbl,xtr,xbr;
    char                *over,c;
    unsigned short      clr;
    char_info           what,what2;
    char_info           _FAR *scr;
    char_info           *txt;
    int                 topscol,topecol,topccol;
    char                *wb;

    if( EditFlags.Quiet ) {
        return;
    }
    if( !Windows[wn]->has_border ) {
        return;
    }
    w = AccessWindow( wn );
    txt = (char_info *) w->text;
    over = w->overlap;
    scr = (char_info _FAR *) Scrn;

    if( w->has_gadgets ) {
        wb = GadgetString;
    } else {
        wb = WindowBordersNG;
    }

    /*
     * set up for border addendums (title and misc top data)
     */
    if( w->title != NULL ) {
        stc = (w->width-strlen( w->title ))/2-1;
        if( stc < 1) {
            stc = 1;
        }
        etc = stc+strlen( w->title ) + 1;
        ctc = 0;
    }
    if( w->borderdata != NULL ) {
        topscol = w->bordercol;
        topecol = topscol+strlen( w->borderdata )-1;
        topccol = 0;
    }

    tl = (w->x1) + (w->y1)*WindMaxWidth;
    tr = (w->x2) + (w->y1)*WindMaxWidth;
    bl = (w->x1) + (w->y2)*WindMaxWidth;
    br = (w->x2) + (w->y2)*WindMaxWidth;
    xtl = 0;
    xtr = w->width-1;
    xbl = (w->height-1)*w->width;
    xbr = w->width-1 + (w->height-1)*w->width;
    clr = MAKE_ATTR( w, w->border_color1, w->border_color2 );
    what.attr = clr;
    what2.attr = clr;

    /*
     * do the corner pieces
     */
    what.ch = wb[WB_TOPLEFT];
    WRITE_SCREEN_DATA( txt[xtl], what );
    if( over[xtl] == NO_CHAR ) {
        WRITE_SCREEN( scr[tl], what );
    }
    what.ch = wb[WB_TOPRIGHT];
    WRITE_SCREEN_DATA( txt[xtr], what );
    if( over[xtr] == NO_CHAR ) {
        WRITE_SCREEN( scr[tr], what );
    }
    what.ch = wb[WB_BOTTOMLEFT];
    WRITE_SCREEN_DATA( txt[xbl], what );
    if( over[xbl] == NO_CHAR ) {
        WRITE_SCREEN( scr[bl], what );
    }
    what.ch = wb[WB_BOTTOMRIGHT];
    WRITE_SCREEN_DATA( txt[xbr], what );
    if( over[xbr] == NO_CHAR ) {
        WRITE_SCREEN( scr[br], what );
    }

    /*
     * do the left side
     */
    what.ch = wb[WB_LEFTSIDE];
    k = xtl+w->width;
    j = tl+WindMaxWidth;
    for( i=1;i<w->height-1;i++) {
        WRITE_SCREEN_DATA( txt[k], what );
        if( over[k] == NO_CHAR ) {
            WRITE_SCREEN( scr[j],what );
#ifdef __VIO__
            MyVioShowBuf( sizeof( char_info ) *j, 1 );
#endif
        }
        k += w->width;
        j += WindMaxWidth;
    }

    /*
     * do the right side
     */
    what.ch = wb[WB_RIGHTSIDE];
    k = xtr + w->width;
    j = tr + WindMaxWidth;
    for( i=1;i<w->height-1;i++) {
        WRITE_SCREEN_DATA( txt[k], what );
        if( over[k] == NO_CHAR ) {
            WRITE_SCREEN( scr[j], what );
#ifdef __VIO__
            MyVioShowBuf( sizeof( char_info ) *j, 1 );
#endif
        }
        k += w->width;
        j += WindMaxWidth;
    }

    /*
     * now do bottom and top
     */
    what2.ch = wb[WB_TOPBOTTOM];
    for( i=1;i<w->width-1;i++ ) {
        k = xtl + i;
        c = wb[5];
        if( w->title != NULL ) {
            if( i >= stc && i <= etc ) {
                if( i== stc) {
                    c = wb[WB_RIGHTT];
                } else if( i==etc ) {
                    c = wb[WB_LEFTT];
                } else {
                    c = w->title[ctc++];
                }
            }
        }
        if( w->borderdata != NULL ) {
            if( i >= topscol && i <= topecol ) {
                c = w->borderdata[topccol++];
            }
        }
        what.ch = c;
        WRITE_SCREEN_DATA( txt[k], what );
        if( over[k] == NO_CHAR ) {
            WRITE_SCREEN( scr[tl+i] , what );
        }

        k = xbl + i;
        WRITE_SCREEN_DATA( txt[k], what2 );
        if( over[k] == NO_CHAR ) {
            WRITE_SCREEN( scr[bl+i] , what2 );
        }
    }

    /*
     * add side gadgets
     */
    if( w->has_gadgets || w->has_scroll_gadgets ) {
        what.ch = wb[WB_UPTRIANGLE];
        WRITE_SCREEN_DATA( txt[xtr+w->width], what );
        if( over[xtr+w->width] == NO_CHAR ) {
            WRITE_SCREEN( scr[tr+WindMaxWidth], what );
#ifdef __VIO__
            MyVioShowBuf( sizeof( char_info ) *(tr+WindMaxWidth), 1 );
#endif
        }
        what.ch = wb[ WB_DOWNTRIANGLE ];
        WRITE_SCREEN_DATA( txt[xbr-w->width], what );
        if( over[xbr-w->width] == NO_CHAR ) {
            WRITE_SCREEN( scr[br-WindMaxWidth], what );
#ifdef __VIO__
            MyVioShowBuf( sizeof( char_info ) * (br-WindMaxWidth), 1 );
#endif
        }
    }
#ifdef __VIO__
    MyVioShowBuf( sizeof( char_info ) * tl, w->width );
    MyVioShowBuf( sizeof( char_info ) * bl, w->width );
#endif
    if( w->has_gadgets ) {
        DrawVerticalThumb( w, GadgetString[ WB_THUMB ] );
    }
    ReleaseWindow( w );
} /* DrawBorder */

/*
 * SetBorderGadgets - set whether or not border has gadgets
 */
void SetBorderGadgets( window_id id, bool how )
{
   Windows[id]->has_gadgets = how;

} /* SetBorderGadgets */

/*
 * WindowBorderData - set up window border data
 */
void WindowBorderData( window_id wn, char *data, int col )
{
    wind        *w;

    w = Windows[wn];

    if( !w->has_border ) {
        return;
    }

    AddString2( &(w->borderdata), data );
    w->bordercol=col;

} /* WindowBorderData */

/*
 * SetGadgetString
 */
void SetGadgetString( char *str )
{
    if( str == NULL ) {
        if( GadgetString == NULL ) {
            AddString2( &GadgetString, WindowBordersG );
        }
        return;
    }
    if( strlen( str ) < strlen( WindowBordersG ) ) {
        AddString2( &GadgetString, WindowBordersG );
    } else {
        AddString2( &GadgetString, str );
    }

} /* SetGadgetString */
