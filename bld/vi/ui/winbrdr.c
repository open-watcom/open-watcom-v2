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


#include "vi.h"
#include "win.h"
#if defined( __UNIX__ )
    #include "stdui.h"
#endif

char        WindowBordersNG[] = {
#if defined( __UNIX__ )
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) UnixNG,
#else
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) DosNG,
#endif
    #include "borders.h"
    #undef vi_pick
    '\0'
};

char        WindowBordersG[] =  {
#if defined( __UNIX__ )
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) UnixG,
#else
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) DosG,
#endif
    #include "borders.h"
    #undef vi_pick
    '\0'
};
#define GADGET_SIZE (sizeof( WindowBordersG ) - 1)

void SetGadgetString( char *str )
{
    int     i;

    if( str != NULL ) {
        i = strlen( str );
        if( i > GADGET_SIZE ) {
            i = GADGET_SIZE;
        }
        if( EditVars.GadgetString == NULL ) {
            EditVars.GadgetString = MemAlloc( GADGET_SIZE + 1 );
            EditVars.GadgetString[GADGET_SIZE] = '\0';
        }
        memset( EditVars.GadgetString, ' ', GADGET_SIZE );
        memcpy( EditVars.GadgetString, str, i );
    } else {
        AddString2( &EditVars.GadgetString, WindowBordersG );
    }
}

/*
 * DrawBorder - display border
 */
void DrawBorder( window_id wn )
{
    wind                *w;
    int                 i, k, stc, etc, ctc;
    int                 tl, bl, tr, br;
    int                 xtl, xbl, xtr, xbr;
    window_id           *over;
    char                c;
    viattr_t            clr;
    char_info           what = {0, 0};
    char_info           what2 = {0, 0};
    char_info           *txt;
    int                 topscol, topecol, topccol;
    char                *wb;
    unsigned            oscr;

    if( EditFlags.Quiet ) {
        return;
    }
    if( !Windows[wn]->has_border ) {
        return;
    }
    w = AccessWindow( wn );
    txt = w->text;
    over = w->overlap;

    if( w->has_gadgets ) {
        wb = EditVars.GadgetString;
    } else {
        wb = WindowBordersNG;
    }

    /*
     * set up for border addendums (title and misc top data)
     */
    stc = 0;
    etc = 0;
    ctc = 0;
    if( w->title != NULL ) {
        stc = (w->width - strlen( w->title )) / 2 - 1;
        if( stc < 1 ) {
            stc = 1;
        }
        etc = stc + strlen( w->title ) + 1;
    }
    topscol = 0;
    topecol = 0;
    topccol = 0;
    if( w->borderdata != NULL ) {
        topscol = w->bordercol;
        topecol = topscol + strlen( w->borderdata ) - 1;
    }

    tl = (w->x1) + (w->y1) * EditVars.WindMaxWidth;
    tr = (w->x2) + (w->y1) * EditVars.WindMaxWidth;
    bl = (w->x1) + (w->y2) * EditVars.WindMaxWidth;
    br = (w->x2) + (w->y2) * EditVars.WindMaxWidth;
    xtl = 0;
    xtr = w->width - 1;
    xbl = (w->height - 1) * w->width;
    xbr = w->width - 1 + (w->height - 1) * w->width;
    clr = MAKE_ATTR( w, w->border_color1, w->border_color2 );
    what.cinfo_attr = clr;
    what2.cinfo_attr = clr;

    /*
     * do the corner pieces
     */
    what.cinfo_char = wb[WB_TOPLEFT];
    WRITE_SCREEN_DATA( txt[xtl], what );
    if( over[xtl] == NO_WINDOW ) {
        WRITE_SCREEN( Scrn[tl], what );
    }
    what.cinfo_char = wb[WB_TOPRIGHT];
    WRITE_SCREEN_DATA( txt[xtr], what );
    if( over[xtr] == NO_WINDOW ) {
        WRITE_SCREEN( Scrn[tr], what );
    }
    what.cinfo_char = wb[WB_BOTTOMLEFT];
    WRITE_SCREEN_DATA( txt[xbl], what );
    if( over[xbl] == NO_WINDOW ) {
        WRITE_SCREEN( Scrn[bl], what );
    }
    what.cinfo_char = wb[WB_BOTTOMRIGHT];
    WRITE_SCREEN_DATA( txt[xbr], what );
    if( over[xbr] == NO_WINDOW ) {
        WRITE_SCREEN( Scrn[br], what );
    }

    /*
     * do the left side
     */
    what.cinfo_char = wb[WB_LEFTSIDE];
    k = xtl + w->width;
    oscr = tl + EditVars.WindMaxWidth;
    for( i = 1; i < w->height - 1; i++ ) {
        WRITE_SCREEN_DATA( txt[k], what );
        if( over[k] == NO_WINDOW ) {
            WRITE_SCREEN( Scrn[oscr], what );
#ifdef __VIO__
            MyVioShowBuf( oscr, 1 );
#endif
        }
        k += w->width;
        oscr += EditVars.WindMaxWidth;
    }

    /*
     * do the right side
     */
    what.cinfo_char = wb[WB_RIGHTSIDE];
    k = xtr + w->width;
    oscr = tr + EditVars.WindMaxWidth;
    for( i = 1; i < w->height - 1; i++ ) {
        WRITE_SCREEN_DATA( txt[k], what );
        if( over[k] == NO_WINDOW ) {
            WRITE_SCREEN( Scrn[oscr], what );
#ifdef __VIO__
            MyVioShowBuf( oscr, 1 );
#endif
        }
        k += w->width;
        oscr += EditVars.WindMaxWidth;
    }

    /*
     * now do bottom and top
     */
    what2.cinfo_char = wb[WB_TOPBOTTOM];
    for( i = 1; i < w->width - 1; i++ ) {
        k = xtl + i;
        c = wb[WB_TOPBOTTOM];
        if( w->title != NULL ) {
            if( i >= stc && i <= etc ) {
                if( i == stc ) {
                    c = wb[WB_RIGHTT];
                } else if( i == etc ) {
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
        what.cinfo_char = c;
        WRITE_SCREEN_DATA( txt[k], what );
        if( over[k] == NO_WINDOW ) {
            WRITE_SCREEN( Scrn[tl + i] , what );
        }

        k = xbl + i;
        WRITE_SCREEN_DATA( txt[k], what2 );
        if( over[k] == NO_WINDOW ) {
            WRITE_SCREEN( Scrn[bl + i] , what2 );
        }
    }

    /*
     * add side gadgets
     */
    if( w->has_gadgets || w->has_scroll_gadgets ) {
        what.cinfo_char = wb[WB_UPTRIANGLE];
        WRITE_SCREEN_DATA( txt[xtr + w->width], what );
        if( over[xtr + w->width] == NO_WINDOW ) {
            oscr = tr + EditVars.WindMaxWidth;
            WRITE_SCREEN( Scrn[oscr], what );
#ifdef __VIO__
            MyVioShowBuf( oscr, 1 );
#endif
        }
        what.cinfo_char = wb[WB_DOWNTRIANGLE];
        WRITE_SCREEN_DATA( txt[xbr - w->width], what );
        if( over[xbr - w->width] == NO_WINDOW ) {
            oscr = br - EditVars.WindMaxWidth;
            WRITE_SCREEN( Scrn[oscr], what );
#ifdef __VIO__
            MyVioShowBuf( oscr, 1 );
#endif
        }
    }
#ifdef __VIO__
    MyVioShowBuf( tl, w->width );
    MyVioShowBuf( bl, w->width );
#endif
    if( w->has_gadgets ) {
        DrawVerticalThumb( w, EditVars.GadgetString[WB_THUMB] );
    }
    ReleaseWindow( w );

} /* DrawBorder */

/*
 * SetBorderGadgets - set whether or not border has gadgets
 */
void SetBorderGadgets( window_id wn, bool how )
{
   Windows[wn]->has_gadgets = how;

} /* SetBorderGadgets */

/*
 * WindowBorderData - set up window border data
 */
void WindowBorderData( window_id wn, const char *data, int col )
{
    wind        *w;

    w = Windows[wn];
    if( w->has_border ) {
        AddString2( &(w->borderdata), data );
        w->bordercol = col;
    }

} /* WindowBorderData */
