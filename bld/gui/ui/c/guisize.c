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


#include "guiwind.h"
#include "guiwhole.h"
#include "guiscale.h"
#include "guicontr.h"
#include "guixutil.h"
#include "guisysme.h"
#include "guisize.h"
#include "guigadgt.h"
#include "guistat.h"
#include "guixdlg.h"
#include <stdio.h>
#include <string.h>

static void UnMinimize( gui_window *wnd )
{
    gui_window  *child;

    uivsetactive( &wnd->screen );
    if( wnd->flags & NEEDS_RESIZE_REDRAW ) {
        GUIWndDirty( wnd );
    }
    for( child = wnd->child; child != NULL; child = child->sibling ) {
        uivshow( &child->screen );
        child->screen.open = TRUE;
        uivsetactive( &child->screen );
    }
}

static void RedrawResize( gui_window *wnd, SAREA *old )
{
    if( ( ( wnd->use.width > old->width )  &&
          ( wnd->use.height > old->height ) ) || GUI_WND_MINIMIZED( wnd ) ) {
        GUIWholeWndDirty( wnd );
    } else {
        if( wnd->flags & NEEDS_RESIZE_REDRAW ) {
            if( wnd->use.width > old->width ) {
                COPYAREA( wnd->use, wnd->dirty );
                wnd->dirty.col += old->width;
                wnd->dirty.width = wnd->use.width - old->width;
            }
            if( wnd->use.height > old->height ) {
                COPYAREA( wnd->use, wnd->dirty );
                wnd->dirty.row += old->height;
                wnd->dirty.height = wnd->use.height - old->height;
            }
        }
        if( !EMPTY_AREA( wnd->dirty ) ) {
            wnd->flags |= CONTENTS_INVALID;
        }
        GUIWndUpdate( wnd );
    }
}

static void CheckChildResize( SAREA *child, SAREA *area, resize_dir dir )
{
    int diff;

    if( dir & RESIZE_RIGHT ) {
        if( ( area->col + area->width ) < ( child->col + child->width ) ) {
            area->width = child->col - area->col + child->width;
        }
    } else {
        if( dir & RESIZE_LEFT ) {
            diff = 0;
            if( area->col > child->col ) {
                diff = area->col - child->col;
            }
            area->width += diff;
            area->col -= diff;
        }
    }
    if( dir & RESIZE_DOWN ) {
        if( ( area->row + area->height ) < ( child->row + child->height ) ) {
            area->height = child->row - area->row + child->height;
        }
    } else {
        if( dir & RESIZE_UP ) {
            diff = 0;
            if( area->row > child->row ) {
                diff = area->row - child->row;
            }
            area->height += diff;
            area->row -= diff;
        }
    }
}

void GUISetCheckResizeAreaForChildren( gui_window *wnd, bool check )
{
    if( wnd ) {
        if( check ) {
            wnd->flags |= CHECK_CHILDREN_ON_RESIZE;
        } else {
            wnd->flags &= ~CHECK_CHILDREN_ON_RESIZE;
        }
    }
}

void GUICheckResizeAreaForChildren( gui_window *wnd, SAREA *area,
                                    resize_dir dir )
{
    gui_window  *child_wnd;
    SAREA       *child;
    SAREA       use;
    SAREA       old_use;

    if( ( wnd->flags & CHECK_CHILDREN_ON_RESIZE ) == 0 ) {
        return;
    }

    GUISetUseArea( wnd, area, &use );
    use.row += wnd->screen.area.row;
    use.col += wnd->screen.area.col;
    COPYAREA( use, old_use );
    for( child_wnd = wnd->child; child_wnd != NULL; child_wnd = child_wnd->sibling ) {
        child = &child_wnd->screen.area;
        CheckChildResize( child, &use, dir );
    }
    if( use.row != old_use.row ) {
        area->row += ( use.row - old_use.row );
    }
    if( use.col != old_use.col ) {
        area->col += ( use.col - old_use.col );
    }
    if( use.width != old_use.width ) {
        area->width += ( use.width - old_use.width );
    }
    if( use.height != old_use.height ) {
        area->height += ( use.height - old_use.height );
    }
}

void GUICheckResizeAreaForParent( gui_window *wnd, SAREA *area,
                                  resize_dir dir )
{
    SAREA       parent;
    int         diff;

    if( wnd->parent != NULL ) {
        COPYAREA( wnd->parent->use, parent );
        parent.row += wnd->parent->screen.area.row;
        parent.col += wnd->parent->screen.area.col;
        if( dir & RESIZE_RIGHT ) {
            if( ( area->col + area->width ) > ( parent.col + parent.width ) ) {
                area->width = parent.col + parent.width - area->col;
            }
        } else {
            if( dir & RESIZE_LEFT ) {
                diff = 0;
                if( area->col < parent.col ) {
                    diff = parent.col - area->col;
                }
                area->col += diff;
                area->width -= diff;
            }
        }
        if( dir & RESIZE_DOWN ) {
            if( ( area->row + area->height ) > ( parent.row + parent.height ) ) {
                area->height = parent.row + parent.height - area->row;
            }
        } else {
            if( dir & RESIZE_UP ) {
                diff = 0;
                if( area->row < parent.row ) {
                    diff = parent.row - area->row;
                }
                area->row += diff;
                area->height -= diff;
            }
        }
    }
}

static void ResizeGadget( p_gadget gadget, ORD length, ORD anchor,
                          bool set_range )
{
    GUIInitGadget( gadget, gadget->start, length, anchor );
    if( !set_range ) {
        gadget->total_size = length - 1 + gadget->pos;
    }
    GUISetShowGadget( gadget, TRUE, TRUE, gadget->pos );
}

/*
 * SizeWnd -- resize the windows to the new width and height if it's vaild
 *            to do so
 */

static bool SizeWnd( gui_window *wnd, SAREA *area, gui_flags flag,
                     resize_dir dir )
{
    SAREA       new;
    gui_coord   newsize;
    SAREA       save;
    gui_window  *child;
    bool        was_minimized;
    int         row_diff;
    int         col_diff;

    COPYAREA( *area, new )
    COPYAREA( wnd->use, save );
    if( flag != MINIMIZED ) {
        GUICheckResizeAreaForChildren( wnd, &new, dir );
    }
    GUICheckResizeAreaForParent( wnd, &new, dir );
    row_diff = new.row - wnd->screen.area.row;
    col_diff = new.col - wnd->screen.area.col;
    if( uivresize( &wnd->screen, new ) == NULL ) {
        return( FALSE );
    }
    GUISetUseWnd( wnd );
    GUIResizeControls( wnd, row_diff, col_diff );
    if( GUIHasStatus( wnd ) ) {
        GUIResizeStatus( wnd );
    }
    if( wnd->hgadget != NULL ) {
        ResizeGadget( wnd->hgadget, wnd->use.width, wnd->use.height,
                      GUI_HRANGE_SET( wnd ) );
    }
    if( wnd->vgadget != NULL ) {
        ResizeGadget( wnd->vgadget, wnd->use.height, wnd->use.width,
                      GUI_VRANGE_SET( wnd ) );
    }
    if( GUI_WND_MAXIMIZED( wnd ) ) {
        wnd->flags &= ~MAXIMIZED;
    }
    was_minimized = GUI_WND_MINIMIZED( wnd );
    if( was_minimized ) {
        wnd->flags &= ~MINIMIZED;
    }
    wnd->flags |= flag;
    wnd->flags |= DONT_SEND_PAINT+NEEDS_RESIZE_REDRAW;
    if( was_minimized ) {
        uivsetactive( &wnd->screen );
        GUIWholeWndDirty( wnd );
    } else {
        wnd->flags |= NON_CLIENT_INVALID;
        RedrawResize( wnd, &save );
    }
    wnd->flags |= NEEDS_RESIZE_REDRAW;
    wnd->flags &= ~DONT_SEND_PAINT;
    if( flag != MINIMIZED ) {
        newsize.x = wnd->use.width;
        newsize.y = wnd->use.height;
        GUIScreenToScaleR( &newsize );
        GUIEVENTWND( wnd, GUI_RESIZE, &newsize );
    }
    if( was_minimized ) {
        UnMinimize( wnd );
    } else {
        if( flag == MINIMIZED ) {
            for( child = wnd->child; child != NULL; child = child->sibling ) {
                uivhide( &child->screen );
                child->screen.open = FALSE;
            }
        }
        RedrawResize( wnd, &save );
    }
    wnd->flags &= ~NEEDS_RESIZE_REDRAW;
    GUISetCursor( wnd );
    GUIDrawStatus( wnd );
    return( TRUE );
}

void GUICheckMove( gui_window *wnd, int *row_diff, int *col_diff )
{
    SAREA       parent;

    if( wnd->parent != NULL ) {
        COPYAREA( wnd->parent->use, parent );
        parent.row += wnd->parent->screen.area.row;
        parent.col += wnd->parent->screen.area.col;
        if( ( wnd->screen.area.row + *row_diff + wnd->screen.area.height ) >
            ( parent.row + parent.height ) ) {
            *row_diff = parent.row + parent.height -
                       wnd->screen.area.row - wnd->screen.area.height;
        }
        if( ( wnd->screen.area.col + *col_diff + wnd->screen.area.width ) >
            ( parent.col + parent.width ) ) {
            *col_diff = parent.col + parent.width -
                       wnd->screen.area.col - wnd->screen.area.width;
        }
        if( ( wnd->screen.area.row + *row_diff ) < parent.row ) {
            *row_diff = parent.row - wnd->screen.area.row;
        }
        if( ( wnd->screen.area.col + *col_diff ) < parent.col ) {
            *col_diff = parent.col - wnd->screen.area.col;
        }
    }
}

static void MoveWnd( gui_window *wnd, int row_diff, int col_diff )
{
    gui_window  *curr;

    GUICheckMove( wnd, &row_diff, &col_diff );
    uivmove( &wnd->screen, (ORD)( row_diff + (int)wnd->screen.area.row),
             (ORD)(col_diff + (int)wnd->screen.area.col ) );
    GUISetUseWnd( wnd );
    GUIEVENTWND( wnd, GUI_MOVE, NULL );
    if( !GUI_WND_MINIMIZED( wnd ) ) {
        /* don't move children if parent is minimized */
        for( curr = wnd->child; curr != NULL; curr = curr->sibling ) {
            MoveWnd( curr, row_diff, col_diff );
        }
        GUIResizeControls( wnd, row_diff, col_diff );
    }
}

void GUICheckArea( SAREA *area, resize_dir dir )
{
    SAREA       screen;
    int         diff;

    GUIGetScreenArea( &screen );

    if( area->row < screen.row ) {
        diff = screen.row - area->row;
        area->row = screen.row;
        if( dir & RESIZE_UP ) {
            area->height -= diff;
        }
    }
    if( area->col < screen.col ) {
        diff = screen.col - area->col;
        area->col = screen.col;
        if( dir & RESIZE_LEFT ) {
            area->width -= diff;
        }
    }
    diff = 0;
    if( area->height > screen.height ) {
        diff = screen.height - area->height;
    }
    if( area->height < MIN_HEIGHT ) {
        diff = MIN_HEIGHT - area->height;
    }
    area->height += diff;
    if( dir & RESIZE_UP ) {
        area->row -= diff;
    }

    /* do width */
    diff = 0;
    if( area->width > screen.width ) {
        diff = screen.width - area->width;
    }
    if( area->width < MIN_WIDTH ) {
        diff = MIN_WIDTH - area->width;
    }
    area->width += diff;
    if( dir & RESIZE_LEFT ) {
        area->col -= diff;
    }

    diff = area->row + area->height - screen.row - screen.height;
    if( diff > 0 ) {
        if( ( dir & RESIZE_DOWN ) || ( dir & RESIZE_UP ) ) {
            area->height -= diff;
        }
        if( dir & RESIZE_UP ) {
            area->row += diff;
        }
        if( dir == RESIZE_NONE )  {
            area->row -= diff;
        }
    }
    diff = area->col + area->width - screen.col - screen.width;
    if( diff > 0 ) {
        if( ( dir & RESIZE_RIGHT ) || ( dir & RESIZE_LEFT ) ) {
            area->width -= diff;
        }
        if( dir & RESIZE_LEFT ) {
            area->col += diff;
        }
        if( dir == RESIZE_NONE ) {
            area->col -= diff;
        }
    }
}

/*
 * GUIWndMoveSize
 */

bool GUIWndMoveSize( gui_window *wnd, SAREA *new, gui_flags flag,
                     resize_dir dir )
{

    GUICheckArea( new, dir );
    /* if the window has been resized */
    if( ( new->height != wnd->screen.area.height ) ||
        ( new->width != wnd->screen.area.width  ) || ( flag == MINIMIZED ) ) {
        return( SizeWnd( wnd, new, flag, dir ) );
    } else {
        /* if the window has been moved but not resized */
        if( ( new->row != wnd->screen.area.row ) ||
            ( new->col != wnd->screen.area.col ) ) {
            MoveWnd( wnd, new->row - (int)wnd->screen.area.row,
                     new->col - (int)wnd->screen.area.col );
            wnd->flags |= flag;
        } else {
            if( flag != NONE ) {
                wnd->flags |= flag;
                GUIRedrawTitle( wnd );
            }
        }
    }
    return( TRUE );
}

static void CalcIconsDim( gui_window *parent, int *icons_per_row,
                          int *max_rows, SAREA *bound  )
{
    if( parent == NULL ) {
        GUIGetScreenArea( bound );
    } else {
        COPYAREA( parent->use, *bound );
        bound->row += parent->screen.area.row;
        bound->col += parent->screen.area.col;
    }
    *icons_per_row = ( bound->width + 1 ) / ( MIN_WIDTH + 1 );
    *max_rows = ( bound->height + 1 ) / ( MIN_HEIGHT + 1 );
}

static int GetMaxIcons( gui_window *parent )
{
    int         icons_per_row;
    int         max_rows;
    SAREA       bound;

    CalcIconsDim( parent, &icons_per_row, &max_rows, &bound );
    return( icons_per_row * max_rows );
}

void GUICalcIconArea( int num, gui_window *parent, SAREA *new )
{
    int         icon_row;
    int         icons_per_row;
    SAREA       bound;
    int         max_rows;

    CalcIconsDim( parent, &icons_per_row, &max_rows, &bound );

    icon_row = ( num - 1 ) / icons_per_row;
    new->col = bound.col +
               ( MIN_WIDTH + 1 ) * ( num - icon_row * icons_per_row  - 1);
    if( icon_row >= max_rows ) {
        icon_row = icon_row % max_rows;
    }
    new->row = bound.row +
               ( bound.height - MIN_HEIGHT * ( icon_row + 1 ) - icon_row );
    new->width = MIN_WIDTH;
    new->height = MIN_HEIGHT;
}

static bool IconPosUsed( gui_window *curr, int num, gui_window *parent )
{
    SAREA       new;

    GUICalcIconArea( num, parent, &new );
    return( GUIOverlap( &new, &curr->screen.area ) );
}

static void GetIconPos( gui_window *parent, SAREA *new )
{
    gui_window  *curr;
    bool        found;
    int         num;
    int         num_mod;
    int         max_icons;
    int         times_used;

    num = 1;
    if( parent != NULL ) {
        num_mod = 0;
        found = FALSE;
        max_icons = GetMaxIcons( parent );
        while( !found ) {
            if( num - num_mod * max_icons > max_icons ) {
                num_mod++;
            }
            times_used = 0;
            for( curr = parent->child; curr != NULL; curr = curr->sibling ) {
                if( GUI_WND_MINIMIZED( curr ) ) {
                    if( IconPosUsed( curr, num, parent ) ) {
                        times_used++;
                        if( times_used > num_mod ) {
                            break;
                        }
                    }
                }
            }
            if( times_used > num_mod ) {
                num++;
            } else {
                found = TRUE;
            }
        }
    }
    GUICalcIconArea( num, parent, new );
}

static void InitMaxArea( gui_window *parent, SAREA *new )
{
    if( parent == NULL ) {
        GUIGetScreenArea( new );
    } else {
        COPYAREA( parent->use, *new );
        new->row += parent->screen.area.row;
        new->col += parent->screen.area.col;
    }
}

static void SetPrevArea( SAREA *area, gui_window *wnd )
{
    COPYAREA( *area, wnd->prev_area );
    if( wnd->parent != NULL ) {
        wnd->prev_area.row -= wnd->parent->screen.area.row;
        wnd->prev_area.col -= wnd->parent->screen.area.col;
    }
}

void GUIZoomWnd( gui_window *wnd, gui_create_styles action )
{
    SAREA       new;
    gui_flags   flag;
    gui_flags   old;
    bool        minmax;

    minmax = GUI_WND_MAXIMIZED( wnd ) || GUI_WND_MINIMIZED( wnd );
    if( ( action == GUI_NONE ) && minmax ) {
        /* return to previous size */
        new = wnd->prev_area;
        if( wnd->parent != NULL ) {
            new.row += wnd->parent->screen.area.row;
            new.col += wnd->parent->screen.area.col;
        }
        old = wnd->flags;
        if( GUIWndMoveSize( wnd, &new, NONE, RESIZE_DOWN | RESIZE_RIGHT ) ) {
            if( GUI_WND_MINIMIZED( wnd ) ) {
                wnd->flags &= ~MINIMIZED;
                GUIRedrawTitle( wnd );
            }
            if( GUI_WND_MAXIMIZED( wnd ) ) {
                wnd->flags &= ~MAXIMIZED;
                GUIRedrawTitle( wnd );
            }
        } else {
            wnd->flags = old;
        }
    } else {
        if( !( wnd->style & action ) ) {
             return;
        }
        /* make fullsized or minimized */
        if( !minmax ) {
            /* record restore area if not already minimized or maxmized */
            SetPrevArea( &wnd->screen.area, wnd );
        }
        if( action == GUI_MAXIMIZE ) {
            InitMaxArea( wnd->parent, &new );
            flag = MAXIMIZED;
        } else {
            flag = MINIMIZED;
            GetIconPos( wnd->parent, &new );
        }
        old = wnd->flags;
        if( !GUIWndMoveSize( wnd, &new, flag, RESIZE_DOWN | RESIZE_RIGHT ) ) {
            /* resize failed - window is not maximized or minimized */
            wnd->flags = old;
        }
    }
    GUISetSystemMenuFlags( wnd );
    if( action == GUI_MINIMIZE ) {
        GUIEVENTWND( wnd, GUI_ICONIFIED, NULL );
    }
}

bool GUIResizeWindow( gui_window *wnd, gui_rect *rect )
{
    SAREA       area;
    bool        ret;
    bool        hidden;
    gui_coord   newsize;

    if( !GUISetArea( &area, rect, wnd->parent, TRUE, GUI_IS_DIALOG( wnd ) ) ) {
        return( FALSE );
    }
    if( GUI_IS_DIALOG( wnd ) ) {
        ret = GUIResizeDialog( wnd, &area );
        newsize.x = wnd->use.width;
        newsize.y = wnd->use.height;
        GUIScreenToScaleR( &newsize );
        GUIEVENTWND( wnd, GUI_RESIZE, &newsize );
    } else {
        hidden = uivshow( &wnd->screen );
#ifdef HELL_FREEZES_OVER
        if( ret ) {
            ret = GUIWndMoveSize( wnd, &area, GUI_NONE, RESIZE_DOWN | RESIZE_RIGHT );
        }
#else
        ret = GUIWndMoveSize( wnd, &area, GUI_NONE, RESIZE_DOWN | RESIZE_RIGHT );
#endif
        if( hidden ) {
            uivhide( &wnd->screen );
        }
    }
    return( ret );
}

void GUISetRestoredSize( gui_window *wnd, gui_rect *rect )
{
    SAREA       area;

    if( GUI_WND_MAXIMIZED( wnd ) || GUI_WND_MINIMIZED( wnd ) ) {
        if( GUISetArea( &area, rect, wnd->parent, TRUE, FALSE ) ) {
            SetPrevArea( &area, wnd );
        }
    } else {
        GUIResizeWindow( wnd, rect );
    }
}

bool GUIGetRestoredSize( gui_window *wnd, gui_rect *rect )
{
    SAREA       pos;

    COPYAREA( wnd->prev_area, pos );
    if( wnd->parent != NULL ) {
        pos.row += wnd->parent->screen.area.row;
        pos.col += wnd->parent->screen.area.col;
    }
    return ( GUIScreenToScaleRect( &pos, rect ) );
}

void GUIMinimizeWindow( gui_window * wnd )
{
    if( !GUI_WND_MINIMIZED( wnd ) ) {
        GUIZoomWnd( wnd, GUI_MINIMIZE );
    }
}

void GUIMaximizeWindow( gui_window * wnd )
{
    if( !GUI_WND_MAXIMIZED( wnd ) ) {
        GUIZoomWnd( wnd, GUI_MAXIMIZE );
    }
}

void GUIRestoreWindow( gui_window * wnd )
{
    if( GUI_WND_MINIMIZED( wnd ) || GUI_WND_MAXIMIZED( wnd ) ) {
        GUIZoomWnd( wnd, GUI_NONE );
    }
}

bool GUIIsMaximized( gui_window *wnd )
{
    return( ( wnd->flags & MAXIMIZED ) != 0 );
}

bool GUIIsMinimized( gui_window *wnd )
{
    return( ( wnd->flags & MINIMIZED ) != 0 );
}

