/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "guixutil.h"
#include "guicontr.h"
#include "uiattrs.h"
#include "guigadgt.h"
#include "guiicon.h"
#include "guistat.h"
#include "guixinit.h"
#include "guixdlg.h"
#include <string.h>
#include "walloca.h"
#include "guixwind.h"


char DrawingChars[GUI_NUM_DRAW_CHARS];

#define TITLE_EXTRA_AMOUNT      4
#define GADGET_WIDTH            3


void GUIInitDrawingChars( bool dbcs )
{
#ifdef __LINUX__
    /* unused parameters */ (void)dbcs;

    enum {
        #define pick( a,b,c,d,e ) DRAW_##a = e,
        #include "_guidraw.h"
        #undef pick
    };
    static const char drawcharmap[GUI_NUM_DRAW_CHARS] = {
        #define pick(a) DRAW_##a,
        #include "_drawmap.h"
        #undef pick
    };
    memcpy( DrawingChars, drawcharmap, GUI_NUM_DRAW_CHARS );
#else
    if( dbcs ) {
        enum {
            #define pick( a,b,c,d,e ) DRAW_##a = c,
            #include "_guidraw.h"
            #undef pick
        };
        static const char drawcharmap[GUI_NUM_DRAW_CHARS] = {
            #define pick(a) DRAW_##a,
            #include "_drawmap.h"
            #undef pick
        };
        memcpy( DrawingChars, drawcharmap, GUI_NUM_DRAW_CHARS );
    } else {
        enum {
            #define pick( a,b,c,d,e ) DRAW_##a = b,
            #include "_guidraw.h"
            #undef pick
        };
        static const char drawcharmap[GUI_NUM_DRAW_CHARS] = {
            #define pick(a) DRAW_##a,
            #include "_drawmap.h"
            #undef pick
        };
        memcpy( DrawingChars, drawcharmap, GUI_NUM_DRAW_CHARS );
    }
#endif
}


int GUIAPI GUIGetCharacter( gui_draw_char draw_char )
{
    if( draw_char < GUI_NUM_DRAW_CHARS ) {
        return( (unsigned char)DrawingChars[draw_char] );
    }
    return( 0 );
}

void GUIAPI GUISetCharacter( gui_draw_char draw_char, int ch )
{
    if( draw_char < GUI_NUM_DRAW_CHARS ) {
        DrawingChars[draw_char] = ch;
    }
}

static void DrawChar( gui_window *wnd, ORD row, ORD col, ATTR attr, char chr )
{
    uivtextput( &wnd->vs, row, col, attr, &chr, 1 );
}

static void DrawText( gui_window *wnd, ORD row, ORD col, ATTR attr, const char *buff, size_t length )
{
    if( length > 0 ) {
        uivtextput( &wnd->vs, row, col, attr, buff, length );
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
    size_t      width;
    size_t      str_length;
    size_t      indent;
    size_t      title_extra;
    int         closer_amount;
    char        lgadget;
    char        mgadget;
    char        rgadget;
    int         inact;
    bool        inact_gadgets;
    size_t      len;

    if( (wnd->style & GUI_NOFRAME) || !( (wnd->flags & FRAME_INVALID) || (wnd->flags & TITLE_INVALID) ) ) {
        return;
    }
    inact = 0;
    if( ( wnd == GUICurrWnd ) || ( wnd->parent == NULL ) ) {
        attr = WNDATTR( wnd, GUI_FRAME_ACTIVE );
        title_attr = attr;
    } else {
        if( (GUIGetWindowStyles() & GUI_INACT_SAME) == 0 ) {
            inact = 1;
        }
        attr = WNDATTR( wnd, GUI_FRAME_INACTIVE );
        title_attr = WNDATTR( wnd, GUI_TITLE_INACTIVE );
    }
    inact_gadgets = GUIGetWindowStyles() & GUI_INACT_GADGETS;
    if( wnd->flags & FRAME_INVALID ) {
        DrawChar( wnd, wnd->vs.area.height - 1, 0, attr, DRAWC( FRAME_LL_CORNER, inact ) );
        DrawChar( wnd, 0, 0, attr, DRAWC( FRAME_UL_CORNER, inact ) );
        DrawChar( wnd, 0, wnd->vs.area.width - 1, attr, DRAWC( FRAME_UR_CORNER, inact ) );
        DrawChar( wnd, wnd->vs.area.height - 1, wnd->vs.area.width - 1, attr, DRAWC( FRAME_LR_CORNER, inact ) );
        /* bottom border */
        if( GUIUseGadget( wnd, wnd->hgadget ) ) {
            if( GUIDrawGadgetLine( wnd->hgadget ) ) {
                DrawChar( wnd, wnd->vs.area.height - 1,
                            wnd->vs.area.width - 1 - GUIGetScrollOffset(),
                            attr, DRAWC( LR_VERT_BAR, inact ) );
            }
        } else {
            area.col = 1;
            area.row = wnd->vs.area.height - 1;
            area.height = 1;
            area.width = wnd->vs.area.width - 2;
            uivfill( &wnd->vs, area, attr, DRAWC( FRAME_BOTTOM, inact ) );
        }
        /* right border */
        if( GUIUseGadget( wnd, wnd->vgadget ) ) {
            if( GUIDrawGadgetLine( wnd->vgadget ) ) {
                DrawChar( wnd, wnd->vs.area.height - 1 - GUIGetScrollOffset(),
                            wnd->vs.area.width - 1, attr, DRAWC( LR_HORZ_BAR, inact ) );
            }
        } else {
            area.row = 1;
            area.height = wnd->vs.area.height - 2;
            area.col = wnd->vs.area.width - 1;
            area.width = 1;
            uivfill( &wnd->vs, area, attr, DRAWC( FRAME_RIGHT, inact ) );
        }

        /* left border */
        area.row = 1;
        area.col = 0;
        area.height = wnd->vs.area.height - 2;
        area.width = 1;
        uivfill( &wnd->vs, area, attr, DRAWC( FRAME_LEFT, inact ) );

        wnd->flags &= ~FRAME_INVALID;
    }

    if( (wnd->flags & TITLE_INVALID) == 0 ) {
        return;
    }

    /* draw min and max gadgets or horizontal bar */
    width = wnd->use.width;
    closer_amount = 0;
    if( !inact || inact_gadgets ) {
        if( GUI_HAS_CLOSER( wnd ) ) {
            closer_amount = 3;
            width -= closer_amount;
            lgadget = DRAWC( LEFT_GADGET_MARK, inact );
            if( wnd->flags & MAXIMIZED ) {
                mgadget = DRAWC( RESIZE_GADGET, inact );
            } else {
                mgadget = DRAWC( MAXIMIZE_GADGET, inact );
            }
            rgadget = DRAWC( RIGHT_GADGET_MARK, inact );
            DrawChar( wnd, 0, CLOSER_COL - 1, attr, DRAWC( LEFT_GADGET_MARK, inact ) );
            DrawChar( wnd, 0, CLOSER_COL,   attr, DRAWC( CLOSER, inact ) );
            DrawChar( wnd, 0, CLOSER_COL + 1, attr, DRAWC( RIGHT_GADGET_MARK, inact ) );
        }
        if( GUI_RESIZE_GADGETS_USEABLE( wnd ) ) {
            width -= 2 * GADGET_WIDTH;
            indent = wnd->vs.area.width - GADGET_WIDTH - 1;
            if( wnd->style & GUI_MAXIMIZE ) {
                lgadget = DRAWC( LEFT_GADGET_MARK, inact );
                if( wnd->flags & MAXIMIZED ) {
                    mgadget = DRAWC( RESIZE_GADGET, inact );
                } else {
                    mgadget = DRAWC( MAXIMIZE_GADGET, inact );
                }
                rgadget = DRAWC( RIGHT_GADGET_MARK, inact );
            } else {
                lgadget = mgadget = rgadget = DRAWC( FRAME_TOP, inact );
            }
            DrawChar( wnd, 0, indent, attr, lgadget );
            DrawChar( wnd, 0, indent + 1, attr, mgadget );
            DrawChar( wnd, 0, indent + 2, attr, rgadget );

            indent = wnd->vs.area.width - 2 * GADGET_WIDTH - 1;
            if( wnd->style & GUI_MINIMIZE ) {
                lgadget = DRAWC( LEFT_GADGET_MARK, inact );
                if( GUI_WND_MINIMIZED( wnd ) ) {
                    mgadget = DRAWC( RESIZE_GADGET, inact );
                } else {
                    mgadget = DRAWC( MINIMIZE_GADGET, inact );
                }
                rgadget = DRAWC( RIGHT_GADGET_MARK, inact );
            } else {
                lgadget = mgadget = rgadget = DRAWC( FRAME_TOP, inact );
            }
            DrawChar( wnd, 0, indent, attr, lgadget );
            DrawChar( wnd, 0, indent + 1, attr, mgadget );
            DrawChar( wnd, 0, indent + 2, attr, rgadget );
        }
    }

    buffer = alloca( wnd->vs.area.width + 1 );
    buff = buffer;
    memset( buff, DRAWC( FRAME_TOP, inact ), width ); /* width at least 1 */
    if( wnd->vs.title != NULL && *wnd->vs.title != '\0' ) {
        str_length = strlen( wnd->vs.title );
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
            buff[len++] = DRAWC( LEFT_TITLE_MARK, inact );
        }
        if( len > 0 ) {
            DrawText( wnd, 0, wnd->use.col + closer_amount, attr, buffer, len );
        }
        indent = len;
        buff += len;
        len = 0;
        if( title_extra != 0 ) {
            buff[len++] = DRAWC( TITLE_SPACE, inact );
        }
        memcpy( buff + len, wnd->vs.title, str_length );
        len += str_length;
        if( title_extra != 0 ) {
            buff[len++] = DRAWC( TITLE_SPACE, inact );
        }
        if( len > 0 ) {
            DrawText( wnd, 0, wnd->use.col + closer_amount + indent, title_attr, buff, len ) ;
        }
        indent += len;
        buff += len;
        len = 0;
        if( title_extra != 0 ) {
            buff[len++] = DRAWC( RIGHT_TITLE_MARK, inact );
        }
        if( width - indent > 0 ) {
            DrawText( wnd, 0, wnd->use.col + closer_amount + indent, attr, buff, width - indent );
        }
    } else {
        DrawText( wnd, 0, wnd->use.col + closer_amount, attr, buffer, width );
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
    gui_row_num row_num;
    int         hscroll;
    int         vscroll;
    int         frame_adjust;

    /* if this window has a parent that is minimized or this window is
     * minimized, don't draw anything
     */
    if( GUI_WND_MINIMIZED( wnd ) || ( ( wnd->parent != NULL ) && ( GUI_WND_MINIMIZED( wnd->parent ) ) ) ) {
        return;
    }

    if( wnd->style & GUI_NOFRAME ) {
        frame_adjust = 0;
    } else {
        frame_adjust = 1;
    }

    if( (wnd->flags & CONTENTS_INVALID) && !EMPTY_AREA( *area ) ) {
        hscroll = 0;
        vscroll = 0;
        if( GUI_WND_VISIBLE( wnd ) ) {
            if( GUI_DO_HSCROLL( wnd ) ) {
                hscroll = wnd->hgadget->pos;
            }
            if( GUI_DO_VSCROLL( wnd ) ) {
                vscroll = wnd->vgadget->pos;
            }
            COPYRECTX( *area, wnd->dirty );
            if( ( wnd->dirty.col + wnd->dirty.width ) >
                ( wnd->vs.area.col + wnd->vs.area.width ) ) {
                wnd->dirty.width = wnd->vs.area.col + wnd->vs.area.width -
                                   wnd->dirty.col;
            }
            if( ( wnd->dirty.row + wnd->dirty.height ) >
                ( wnd->vs.area.row + wnd->vs.area.height ) ) {
                wnd->dirty.height = wnd->vs.area.row + wnd->vs.area.height -
                                    wnd->dirty.row;
            }
        }
        uivfill( &wnd->vs, wnd->dirty, WNDATTR( wnd, GUI_BACKGROUND ),
                    wnd->background );

        if( GUI_WND_VISIBLE( wnd ) && (wnd->flags & DONT_SEND_PAINT) == 0 ) {
            row_num.start = vscroll + area->row - frame_adjust;
            row_num.num = area->height;
            GUIEVENT( wnd, GUI_PAINT, &row_num );
        }
        for( control = wnd->controls; control != NULL; control = control->sibling ) {
            GUIRefreshControl( control->parent, control->id );
        }
        GUIDrawStatus( wnd );
    }
    WndClean( wnd );
}

static void DrawGadget( gui_window *wnd, p_gadget gadget, gui_flags flag )
{
    if( (wnd->flags & flag) && ( gadget != NULL ) ) {
        GUISetShowGadget( gadget, true, false, 0 );
        wnd->flags &= ~flag;
    }
}
/*
 * GUIWndUpdate -- refresh the portions of the given window which require it
 */

void GUIAPI GUIWndUpdate( gui_window *wnd )
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
