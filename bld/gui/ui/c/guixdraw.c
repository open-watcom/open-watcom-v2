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
* Description:  Draw various character graphics.
*
****************************************************************************/


#include "guiwind.h"
#include "guidraw.h"
#include "guixdraw.h"
#include "guix.h"
#include "guixutil.h"
#include "guicontr.h"
#include "guiscale.h"
#include "uiattrs.h"
#include "guigadgt.h"
#include "guiicon.h"
#include "guistat.h"
#include "guixinit.h"
#include "guixdlg.h"
#include <string.h>
#include "walloca.h"


static unsigned char DrawIndex[] =
{
    DRAW_BLANK,                 // GUI_FRAME_TOP
    DRAW_BLANK,                 // GUI_FRAME_UL_CORNER
    DRAW_BLANK,                 // GUI_FRAME_LEFT
    DRAW_BLANK,                 // GUI_FRAME_LL_CORNER
    DRAW_BLANK,                 // GUI_FRAME_BOTTOM
    DRAW_BLANK,                 // GUI_FRAME_LR_CORNER
    DRAW_BLANK,                 // GUI_FRAME_RIGHT
    DRAW_BLANK,                 // GUI_FRAME_UR_CORNER
    DRAW_VERT_LINE,             // GUI_LR_VERT_BAR,
    DRAW_HOR_LINE,              // GUI_LR_HORZ_BAR,
    DRAW_BLANK,                 // GUI_LEFT_TITLE_MARK
    DRAW_BLANK,                 // GUI_RIGHT_TITLE_MARK
    DRAW_VERT_LINE,             // GUI_LEFT_GADGET_MARK
    DRAW_VERT_LINE,             // GUI_RIGHT_GADGET_MARK
    DRAW_BLANK,                 // GUI_TITLE_SPACE
    DRAW_CLOSER,                // GUI_CLOSER
    DRAW_MAXIMIZE,              // GUI_MAXIMIZE_GADGET
    DRAW_MINIMIZE,              // GUI_MINIMIZE_GADGET
    DRAW_RESIZE,                // GUI_RESIZE_GADGET
    DRAW_BLANK,                 // GUI_HOR_SCROLL
    DRAW_BLANK,                 // GUI_VERT_SCROLL
    DRAW_LEFT_POINT,            // GUI_LEFT_SCROLL_ARROW
    DRAW_RIGHT_POINT,           // GUI_RIGHT_SCROLL_ARROW
    DRAW_UP_POINT,              // GUI_UP_SCROLL_ARROW
    DRAW_DOWN_POINT,            // GUI_DOWN_SCROLL_ARROW
    DRAW_SLIDER,                // GUI_SCROLL_SLIDER

    DRAW_HOR_FRAME,             // GUI_INACT_FRAME_TOP
    DRAW_UL_CORNER,             // GUI_INACT_FRAME_UL_CORNER
    DRAW_VERT_FRAME,            // GUI_INACT_FRAME_LEFT
    DRAW_LL_CORNER,             // GUI_INACT_FRAME_LL_CORNER
    DRAW_HOR_FRAME,             // GUI_INACT_FRAME_BOTTOM
    DRAW_LR_CORNER,             // GUI_INACT_FRAME_LR_CORNER
    DRAW_VERT_FRAME,            // GUI_INACT_FRAME_RIGHT
    DRAW_UR_CORNER,             // GUI_INACT_FRAME_UR_CORNER
    DRAW_RIGHT_TITLE_MARK,      // GUI_INACT_LR_VERT_BAR,
    DRAW_C2,                    // GUI_INACT_LR_HORZ_BAR,
    DRAW_LEFT_TITLE_MARK,       // GUI_INACT_LEFT_TITLE_MARK
    DRAW_RIGHT_TITLE_MARK,      // GUI_INACT_RIGHT_TITLE_MARK
    DRAW_LEFT_TITLE_MARK,       // GUI_INACT_LEFT_GADGET_MARK
    DRAW_RIGHT_TITLE_MARK,      // GUI_INACT_RIGHT_GADGET_MARK
    DRAW_BLANK,                 // GUI_INACT_TITLE_SPACE
    DRAW_CLOSER,                // GUI_INACT_CLOSER
    DRAW_MAXIMIZE,              // GUI_INACT_MAXIMIZE_GADGET
    DRAW_MINIMIZE,              // GUI_INACT_MINIMIZE_GADGET
    DRAW_RESIZE,                // GUI_INACT_RESIZE_GADGET
    DRAW_HOR_FRAME_DBL,         // GUI_INACT_HOR_SCROLL
    DRAW_VERT_FRAME_DBL,        // GUI_INACT_VERT_SCROLL
    DRAW_LEFT_POINT,            // GUI_INACT_LEFT_SCROLL_ARROW
    DRAW_RIGHT_POINT,           // GUI_INACT_RIGHT_SCROLL_ARROW
    DRAW_UP_POINT,              // GUI_INACT_UP_SCROLL_ARROW
    DRAW_DOWN_POINT,            // GUI_INACT_DOWN_SCROLL_ARROW
    DRAW_BLOCK,                 // GUI_INACT_SCROLL_SLIDER

    DRAW_BLOCK_SLIDER,          // GUI_DIAL_VERT_SCROLL
    DRAW_UP_POINT,              // GUI_DIAL_UP_SCROLL_ARROW,
    DRAW_DOWN_POINT,            // GUI_DIAL_DOWN_SCROLL_ARROW,
    DRAW_BLOCK,                 // GUI_DIAL_SCROLL_SLIDER
};

char DrawingChars[DRAW_LAST];

#define GET_CHAR( val, inact ) ( DrawingChars[DrawIndex[val+ GUI_INACTIVE_OFFSET * inact]] )

#define TOP( inact )            GET_CHAR( GUI_FRAME_TOP, inact )
#define UL_CORNER( inact )      GET_CHAR( GUI_FRAME_UL_CORNER, inact )
#define LEFT( inact )           GET_CHAR( GUI_FRAME_LEFT, inact )
#define LL_CORNER( inact )      GET_CHAR( GUI_FRAME_LL_CORNER, inact )
#define BOTTOM( inact )         GET_CHAR( GUI_FRAME_BOTTOM, inact )
#define LR_CORNER( inact )      GET_CHAR( GUI_FRAME_LR_CORNER, inact )
#define RIGHT( inact )          GET_CHAR( GUI_FRAME_RIGHT, inact )
#define UR_CORNER( inact )      GET_CHAR( GUI_FRAME_UR_CORNER, inact )
#define LT_MARK( inact )        GET_CHAR( GUI_LEFT_TITLE_MARK, inact )
#define RT_MARK( inact )        GET_CHAR( GUI_RIGHT_TITLE_MARK, inact )
#define LG_MARK( inact )        GET_CHAR( GUI_LEFT_GADGET_MARK, inact )
#define RG_MARK( inact )        GET_CHAR( GUI_RIGHT_GADGET_MARK, inact )
#define TITLE_SP( inact )       GET_CHAR( GUI_TITLE_SPACE, inact )
#define CLOSER( inact )         GET_CHAR( GUI_CLOSER, inact )
#define MIN_GAD( inact )        GET_CHAR( GUI_MINIMIZE_GADGET, inact )
#define MAX_GAD( inact )        GET_CHAR( GUI_MAXIMIZE_GADGET, inact )
#define RESIZE_GAD( inact )     GET_CHAR( GUI_RESIZE_GADGET, inact )
#define LRV_BAR( inact )        GET_CHAR( GUI_LR_VERT_BAR, inact )
#define LRH_BAR( inact )        GET_CHAR( GUI_LR_HORZ_BAR, inact )

/* includes from guixmain.c */
extern gui_window *GUICurrWnd;

#define TITLE_EXTRA_AMOUNT      4
#define GADGET_WIDTH            3

void GUIInitDrawingChars( bool dbcs )
{
#ifdef __LINUX__
    #define draw_pick( a,b,c,d,e ) DrawingChars[DRAW_##a] = e;
    #include "guidraw.h"
    #undef draw_pick
#else
    if( dbcs ) {
        #define draw_pick( a,b,c,d,e ) DrawingChars[DRAW_##a] = c;
        #include "guidraw.h"
        #undef draw_pick
    } else {
        #define draw_pick( a,b,c,d,e ) DrawingChars[DRAW_##a] = b;
        #include "guidraw.h"
        #undef draw_pick
    }
#endif
}


int GUIGetCharacter( gui_draw_char draw_char )
{
    if( draw_char < GUI_NUM_DRAW_CHARS ) {
        return( (unsigned char)DrawingChars[DrawIndex[draw_char]] );
    }
    return( 0 );
}

void GUISetCharacter( gui_draw_char draw_char, int ch )
{
    ch=ch; draw_char=draw_char;
    // not implemented in this revision
    // some apps do a
    // if an app calls GUISetCharacter( GUI_SCROLL_SLIDER, 177 );
    // just delete the call since that is the default now.
}

static void DrawChar( gui_window *wnd, int row, int col, ATTR attr, char chr )
{
    uivtextput( &wnd->screen, row, col, attr, &chr, 1 );
}

static void DrawText( gui_window *wnd, int row, int col, ATTR attr, char *buff, int length )
{
    if( length > 0 ) {
        uivtextput( &wnd->screen, row, col, attr, buff, length );
    }
}

/*
 * DrawFrame -- draw the frame of the window
 */

static void DrawFrame( gui_window *wnd )
{
    SAREA       area;
    ATTR        attr, title_attr;
    char        *buffer;
    char        *buff;
    int         width;
    int         str_length;
    int         indent;
    int         title_extra, closer_amount;
    char        lgadget;
    char        mgadget;
    char        rgadget;
    int         inact;
    bool        inact_gadgets;
    int         len;

    if( (wnd->style & GUI_NOFRAME) ||
        !( (wnd->flags & FRAME_INVALID) || (wnd->flags & TITLE_INVALID) ) ) {
        return;
    }
    inact = 0;
    if( ( wnd == GUICurrWnd ) || ( wnd->parent == NULL ) ) {
        attr = wnd->colours[GUI_FRAME_ACTIVE];
        title_attr = attr;
    } else {
        if( !( GUIGetWindowStyles() & GUI_INACT_SAME ) ) {
            inact = 1;
        }
        attr = wnd->colours[GUI_FRAME_INACTIVE];
        title_attr = wnd->colours[GUI_TITLE_INACTIVE];
    }
    inact_gadgets = GUIGetWindowStyles() & GUI_INACT_GADGETS;
    if( wnd->flags & FRAME_INVALID ) {
        DrawChar( wnd, wnd->screen.area.height-1, 0, attr, LL_CORNER( inact ) );
        DrawChar( wnd, 0, 0, attr, UL_CORNER( inact ) );
        DrawChar( wnd, 0, wnd->screen.area.width-1, attr, UR_CORNER( inact ) );
        DrawChar( wnd, wnd->screen.area.height-1, wnd->screen.area.width-1,
                    attr, LR_CORNER( inact ) );
        /* bottom border */
        if( GUIUseGadget( wnd, wnd->hgadget ) ) {
            if( GUIDrawGadgetLine( wnd->hgadget ) ) {
                DrawChar( wnd, wnd->screen.area.height - 1,
                            wnd->screen.area.width - 1 - GUIGetScrollOffset(),
                            attr, LRV_BAR( inact ) );
            }
        } else {
            area.col = 1;
            area.row = wnd->screen.area.height-1;
            area.height = 1;
            area.width = wnd->screen.area.width - 2;
            uivfill( &wnd->screen, area, attr, BOTTOM( inact ) );
        }
        /* right border */
        if( GUIUseGadget( wnd, wnd->vgadget ) ) {
            if( GUIDrawGadgetLine( wnd->vgadget ) ) {
                DrawChar( wnd, wnd->screen.area.height-1-GUIGetScrollOffset(),
                            wnd->screen.area.width-1, attr, LRH_BAR( inact ) );
            }
        } else {
            area.row = 1;
            area.height = wnd->screen.area.height- 2;
            area.col = wnd->screen.area.width - 1;
            area.width = 1;
            uivfill( &wnd->screen, area, attr, RIGHT( inact ) );
        }

        /* left border */
        area.row = 1;
        area.col = 0;
        area.height = wnd->screen.area.height-2;
        area.width = 1;
        uivfill( &wnd->screen, area, attr, LEFT( inact ) );

        wnd->flags &= ~FRAME_INVALID;
    }

    if( !( wnd->flags & TITLE_INVALID ) ) {
        return;
    }

    /* draw min and max gadgets or horizontal bar */
    width = wnd->use.width;
    closer_amount = 0;
    if( !inact || inact_gadgets ) {
        if( GUI_HAS_CLOSER( wnd ) ) {
            closer_amount = 3;
            width -= closer_amount;
            lgadget = LG_MARK( inact );
            if( wnd->flags & MAXIMIZED ) {
                mgadget = RESIZE_GAD( inact );
            } else {
                mgadget = MAX_GAD( inact );
            }
            rgadget = RG_MARK( inact );
            DrawChar( wnd, 0, CLOSER_COL-1, attr, LG_MARK( inact ) );
            DrawChar( wnd, 0, CLOSER_COL,   attr, CLOSER( inact ) );
            DrawChar( wnd, 0, CLOSER_COL+1, attr, RG_MARK( inact ) );
        }
        if( GUI_RESIZE_GADGETS_USEABLE( wnd ) ) {
            width -= 2 * GADGET_WIDTH;
            indent = wnd->screen.area.width - GADGET_WIDTH - 1;
            if( wnd->style & GUI_MAXIMIZE ) {
                lgadget = LG_MARK( inact );
                if( wnd->flags & MAXIMIZED ) {
                    mgadget = RESIZE_GAD( inact );
                } else {
                    mgadget = MAX_GAD( inact );
                }
                rgadget = RG_MARK( inact );
            } else {
                lgadget = mgadget = rgadget = TOP( inact );
            }
            DrawChar( wnd, 0, indent, attr, lgadget );
            DrawChar( wnd, 0, indent + 1, attr, mgadget );
            DrawChar( wnd, 0, indent + 2, attr, rgadget );

            indent = wnd->screen.area.width - 2 * GADGET_WIDTH - 1;
            if( wnd->style & GUI_MINIMIZE ) {
                lgadget = LG_MARK( inact );
                if( GUI_WND_MINIMIZED( wnd ) ) {
                    mgadget = RESIZE_GAD( inact );
                } else {
                    mgadget = MIN_GAD( inact );
                }
                rgadget = RG_MARK( inact );
            } else {
                lgadget = mgadget = rgadget = TOP( inact );
            }
            DrawChar( wnd, 0, indent, attr, lgadget );
            DrawChar( wnd, 0, indent + 1, attr, mgadget );
            DrawChar( wnd, 0, indent + 2, attr, rgadget );
        }
    }

    buffer = alloca( wnd->screen.area.width + 1 );
    buff = buffer;
    memset( buff, TOP( inact ), width ); /* width at least 1 */
    if( wnd->screen.name != NULL && *wnd->screen.name != NULLCHAR ) {
        str_length = strlen( wnd->screen.name );
        if( ( str_length + TITLE_EXTRA_AMOUNT ) > width ) {
            title_extra = 0;
            if( str_length > width ) {
                str_length = width;
            }
        } else {
            title_extra = TITLE_EXTRA_AMOUNT;
        }
        len = ( width - str_length - title_extra ) / 2;
        if( title_extra != 0 ) {
            buff[len] = LT_MARK( inact );
            len++;
        }
        if( len > 0 ) {
            DrawText( wnd, 0, wnd->use.col + closer_amount, attr,
                      buffer, len );
        }
        indent = len;
        buff += len;
        len = 0;
        if( title_extra != 0 ) {
            buff[len] = TITLE_SP( inact );
            len++;
        }
        memcpy( buff+len, wnd->screen.name, str_length );
        len += str_length;
        if( title_extra != 0 ) {
            buff[len] = TITLE_SP( inact );
            len++;
        }
        if( len > 0 ) {
            DrawText( wnd, 0, wnd->use.col + closer_amount + indent,
                      title_attr, buff, len) ;
        }
        indent += len;
        buff += len;
        len = 0;
        if( title_extra != 0 ) {
            buff[len] = RT_MARK( inact );
            len++;
        }
        if( width - indent > 0 ) {
            DrawText( wnd, 0, wnd->use.col + closer_amount + indent,
                      attr, buff, width - indent );
        }
    } else {
        DrawText( wnd, 0, wnd->use.col + closer_amount, attr,
                  buffer, width );
    }
    wnd->flags &= ~TITLE_INVALID;
}

static void WndClean( gui_window *wnd )
{
    wnd->dirty.row = 0;
    wnd->dirty.col = 0;
    wnd->dirty.width = 0;
    wnd->dirty.height = 0;
    wnd->flags &= ~CONTENTS_INVALID;
}

/*
 * GUIWndRfrshArea -- refresh a portion of the use are of the screen
 */

void GUIWndRfrshArea( gui_window *wnd, SAREA *area )
{
    gui_control *control;
    gui_row_num rownum;
    int         hscroll;
    int         vscroll;
    int         frame_adjust;

    /* if this window has a parent that is minimized or this window is
     * minimized, don't draw anything
     */
    if( GUI_WND_MINIMIZED( wnd ) ||
        ( ( wnd->parent != NULL ) && ( GUI_WND_MINIMIZED( wnd->parent ) ) ) ) {
        return;
    }

    if( wnd->style & GUI_NOFRAME ) {
        frame_adjust = 0;
    } else {
        frame_adjust = 1;
    }

    if( ( wnd->flags & CONTENTS_INVALID ) && !EMPTY_AREA( *area ) ) {
        hscroll = 0;
        vscroll = 0;
        if( GUI_WND_VISIBLE( wnd ) ) {
            if( ( wnd->hgadget != NULL ) && !GUI_HSCROLL_EVENTS_SET( wnd ) ) {
                hscroll += wnd->hgadget->pos;
            }
            if( ( wnd->vgadget != NULL ) && !GUI_VSCROLL_EVENTS_SET( wnd ) ) {
                vscroll += wnd->vgadget->pos;
            }
            COPYAREA( *area, wnd->dirty );
            if( ( wnd->dirty.col + wnd->dirty.width ) >
                ( wnd->screen.area.col + wnd->screen.area.width ) ) {
                wnd->dirty.width = wnd->screen.area.col + wnd->screen.area.width -
                                   wnd->dirty.col;
            }
            if( ( wnd->dirty.row + wnd->dirty.height ) >
                ( wnd->screen.area.row + wnd->screen.area.height ) ) {
                wnd->dirty.height = wnd->screen.area.row + wnd->screen.area.height -
                                    wnd->dirty.row;
            }
        }
        uivfill( &wnd->screen, wnd->dirty, wnd->colours[GUI_BACKGROUND],
                    wnd->background );

        if( GUI_WND_VISIBLE( wnd ) && !( wnd->flags & DONT_SEND_PAINT ) ) {
            rownum.start = vscroll + area->row - frame_adjust;
            rownum.num = area->height;
            GUIEVENTWND( wnd, GUI_PAINT, &rownum );
        }
        for( control = wnd->controls; control != NULL;
             control = control->sibling ) {
            GUIRefreshControl( control->parent, control->id );
        }
        GUIDrawStatus( wnd );
    }
    WndClean( wnd );
}

static void DrawGadget( gui_window *wnd, p_gadget gadget, gui_flags flag )
{
    if( ( wnd->flags & flag ) && ( gadget != NULL ) ) {
        GUISetShowGadget( gadget, true, false, 0 );
        wnd->flags &= ~flag;
    }
}
/*
 * GUIWndUpdate -- refresh the portions of the given window which require it
 */

void GUIWndUpdate( gui_window *wnd )
{
    if( !GUIIsOpen( wnd ) ) {
        return;
    }
    if( GUI_WND_VISIBLE( wnd ) && GUI_WND_MINIMIZED( wnd ) ) {
        GUIDrawIcon( wnd );
    } else {
        if( GUI_WND_VISIBLE( wnd ) ) {
            DrawFrame( wnd );
            DrawGadget( wnd, wnd->vgadget, VSCROLL_INVALID );
            DrawGadget( wnd, wnd->hgadget, HSCROLL_INVALID );
        }
        GUIWndRfrshArea( wnd, &wnd->dirty );
    }
}
