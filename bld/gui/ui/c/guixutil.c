/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <string.h>
#include "guiscale.h"
#include "guixutil.h"
#include "guixdraw.h"
#include "guiutil.h"
#include "guixdlg.h"
#include "guisize.h"
#include "guimenu.h"
#include "guixwind.h"
#include "guiwnclr.h"
#include "guicontr.h"
#include "guihook.h"
#include "guizlist.h"
#include "guigadgt.h"
#include "guixhook.h"
#include "guistat.h"


#define VALIDWINDOW( area, check_min )          \
        ( check_min ?                           \
        ( area->width >= MIN_WIDTH ) &&         \
        ( area->height >= MIN_HEIGHT ) : true )

/*
 * GUISetupStruct - sets up the gui_window structure
 */

bool GUISetupStruct( gui_window *wnd, gui_create_info *dlg_info, bool dialog )
{
    wnd->style = dlg_info->style;
    if( !dialog ) {
        if( !GUICreateMenus( wnd, dlg_info ) ) {
            return( false );
        }
        if( wnd->vbarmenu != NULL ) {
            uimenubar( wnd->vbarmenu );
            GUISetScreen( XMIN, YMIN, XMAX - XMIN, YMAX - YMIN );
        }
    }
    if( !GUIJustSetWindowText( wnd, dlg_info->title ) ) {
        return( false );
    }
    if( !GUISetArea( &wnd->vs.area, &dlg_info->rect, dlg_info->parent, true, dialog ) ) {
        return( false );
    }
    GUISetUseWnd( wnd );
    if( dlg_info->scroll_style & GUI_VSCROLL ) {
        if( !GUICreateGadget( wnd, VERTICAL, wnd->use.width, wnd->use.row,
                           wnd->use.height, &wnd->vgadget, dlg_info->scroll_style ) ) {
            return( false );
        }
    }
    if( dlg_info->scroll_style & GUI_HSCROLL ) {
        if( !GUICreateGadget( wnd, HORIZONTAL, wnd->use.height, wnd->use.col,
                           wnd->use.width, &wnd->hgadget, dlg_info->scroll_style ) ) {
            return( false );
        }
    }
    if( wnd->style & GUI_CURSOR ) {
        GUISetCursorType( wnd, GUI_NORMAL_CURSOR );
    }
    return( GUISetColours( wnd, dlg_info->colours.num_items, dlg_info->colours.colour ) );
}

/*
 * GUISetRedraw -- set the redraw flag for a given window
 */
bool GUIAPI GUISetRedraw( gui_window *wnd, bool redraw )
{
    /* unused parameters */ (void)wnd; (void)redraw;

    return( true );
}

/*
 * GUIIsOpen --
 */

bool GUIIsOpen( gui_window *wnd )
{
    return( wnd->vs.open );
}

void GUISetUseArea( gui_window *wnd, SAREA *area, SAREA *use )
{
    COPYRECTX( *area, *use );
    use->row = 0;
    use->col = 0;
    if( (wnd->style & GUI_VISIBLE) && (wnd->style & GUI_NOFRAME) == 0 ) {
        use->row += HALF_BORDER_AMOUNT;
        use->col += HALF_BORDER_AMOUNT;
        use->width -= BORDER_AMOUNT;
        use->height -= BORDER_AMOUNT;
    }
    if( wnd->status != NULL ) {
        use->height -= wnd->status->area.height;
    }
}

/*
 * GUISetUseWnd --
 */

void GUISetUseWnd( gui_window *wnd )
{
    GUISetUseArea( wnd, &wnd->vs.area, &wnd->use );
}

bool GUIPtInRect( const SAREA *area, ORD row, ORD col )
{
    return( ( col >= area->col ) && ( col < ( area->col + area->width ) ) &&
            ( row >= area->row ) && ( row < ( area->row + area->height ) ) );
}

static bool CheckOverlap( const SAREA *one, const SAREA *two )
{
    if( GUIPtInRect( two, one->row, one->col ) ) {
        return( true );
    }
    if( GUIPtInRect( two, one->row, one->col + one->width - 1 ) ) {
        return( true );
    }
    if( GUIPtInRect( two, one->row + one->height - 1, one->col ) ) {
        return( true );
    }
    if( GUIPtInRect( two, one->row + one->height - 1,
                          one->col + one->width - 1 ) ) {
        return( true );
    }
    return( false );
}

bool GUIOverlap( const SAREA *one, const SAREA *two )
{
    if( CheckOverlap( one, two ) ) {
        return( true );
    } else {
        return( CheckOverlap( two, one ) );
    }
}

bool GUISetDialogArea( gui_window *wnd, SAREA *area, const gui_rect *rect, const SAREA *parent_area )
{
    GUIScaleToScreenRectR( rect, area );
    if( !GUI_IS_DIALOG( wnd ) ) {
        area->row++;
        area->col++;
    }
    return( ( area->row <= ( parent_area->height - 2 ) ) &&
            ( area->col <= ( parent_area->width - 2 ) ) &&
            ( ( area->col + area->width ) <= ( parent_area->width - 2 ) ) );
    /* can have pop down list boxes so don't force height to fit in dialog */
}

void GUIAdjustDialogArea( SAREA *area, int mult )
{
    area->width += mult * BORDER_AMOUNT;
    area->height += mult * BORDER_AMOUNT;
    area->row -= mult * HALF_BORDER_AMOUNT;
    area->col -= mult * HALF_BORDER_AMOUNT;
}


void GUIGetSAREA( gui_window *wnd, SAREA *area )
{
    if( GUI_IS_DIALOG( wnd ) ) {
        GUIGetDlgRect( wnd, area );
    } else {
        COPYRECTX( wnd->vs.area, *area );
    }
}

void GUIGetClientSAREA( gui_window *wnd, SAREA *sarea )
{
    SAREA       area;

    GUIGetSAREA( wnd, &area );
    GUISetUseArea( wnd, &area, sarea );
    sarea->row += area.row;
    sarea->col += area.col;
}

/*
 *  GUISetArea -- set area to screen coordinates that match rect.
 *                If parent is not NULL, rect will be relative.
 *                If dialog is true, the screen bounds not the parent.
 */

bool GUISetArea( SAREA *area, const gui_rect *rect, gui_window *parent_wnd,
                 bool check_min, bool dialog )
{
    bool        valid;
    SAREA       bounding;
    int         new;
    gui_rect    parent_rect;
    gui_rect    act_rect;
    SAREA       parent_area;

    if( dialog || ( parent_wnd == NULL ) ) {
        GUIGetScreenArea( &bounding );
    } else {
        COPYRECTX( parent_wnd->use, bounding );
        GUIGetSAREA( parent_wnd, &parent_area );
        bounding.row += parent_area.row;
        bounding.col += parent_area.col;
    }
    act_rect.x = rect->x;
    act_rect.y = rect->y;
    act_rect.width = rect->width;
    act_rect.height = rect->height;
    if( parent_wnd != NULL ) {
        GUIGetClientRect( parent_wnd, &parent_rect );
        act_rect.x += parent_rect.x;
        act_rect.y += parent_rect.y;
    }
    if( !GUIScaleToScreenRect( &act_rect, area ) ) {
        return( false );
    }

    valid = VALIDWINDOW( area, check_min );
    if( !dialog ) {
        valid = ( area->row >= bounding.row ) &&
                ( area->col >= bounding.col ) && valid;
    }
    if( valid ) {
        if( ( area->col + area->width ) >
            ( bounding.col + bounding.width ) ) {
            if( dialog ) {
                new = bounding.col + bounding.width - area->width - 1;
                if( new > 0 ) {
                    area->col = new;
                } else {
                    area->col = 0;
                    area->width = bounding.col + bounding.width - 1;
                }
            } else {
                new = bounding.col + bounding.width - area->col - 1;
                if( new < 0 ) {
                    return( false );
                } else {
                    area->width = new;
                }
            }
        }
        if( ( area->row + area->height ) >
            ( bounding.row + bounding.height ) ) {
            if( dialog ) {
                new = bounding.row + bounding.height - area->height - 1;
                if( new > 0 ) {
                    area->row = new;
                } else {
                    area->row = 0;
                    area->height = bounding.row + bounding.height - 1;
                }
            } else {
                new = bounding.row + bounding.height - area->row - 1;
                if( new < 0 ) {
                    return( false );
                } else {
                    area->height = new;
                }
            }
        }
        valid = VALIDWINDOW( area, check_min );
    }
    if( dialog && valid ) {
        /* a dialog is a framed window so the area will be adjusted */
        GUIAdjustDialogArea( area, -1 );
    }
    return( valid );
}


char *GUIMakeEditCopy( char *buffer, int length )
{
    char        *copy;

    if( length > 0 ) {
        copy = (char *)GUIMemAlloc( length + 1 );
        if( copy != NULL ) {
            memcpy( copy, buffer, length );
            copy[length] = '\0';
        }
    } else {
        copy = NULL;
    }
    return( copy );
}

void GUIRedrawTitle( gui_window *wnd )
{
    wnd->flags |= TITLE_INVALID;
    GUIWndUpdate( wnd );
}

void GUIMakeRelative( gui_window *wnd, const guix_point *scr_point, gui_point *point )
{
    SAREA       area;
    SAREA       use;
    guix_ord    scr_x;
    guix_ord    scr_y;

    GUIGetSAREA( wnd, &area );
    GUISetUseArea( wnd, &area, &use );
    scr_x = scr_point->x - use.col - area.col;
    scr_y = scr_point->y - use.row - area.row;
    if( GUI_DO_HSCROLL( wnd ) ) {
        scr_x += wnd->hgadget->pos;
    }
    if( GUI_DO_VSCROLL( wnd ) ) {
        scr_y += wnd->vgadget->pos;
    }
    point->x = GUIScreenToScaleH( scr_x );
    point->y = GUIScreenToScaleV( scr_y );
}

/*
 * GUIJustSetWindowText - set the title text of a window, don't redraw
 */

bool GUIJustSetWindowText( gui_window *wnd, const char *title )
{
    char        *new_title;
    bool        ok;

    new_title = GUIStrDup( title, &ok );
    if( !ok )
        return( false );
    if( wnd->vs.dynamic_title ) {
        GUIMemFree( (void *)wnd->vs.title );
    } else {
        wnd->vs.dynamic_title = true;
    }
    wnd->vs.title = new_title;
    return( true );
}

bool GUIInArea( ORD row, ORD col, SAREA *area )
{
    return( ( ( row >= area->row ) && ( row < ( area->row + area->height ) ) &&
              ( col >= area->col ) && ( col < ( area->col+ area->width   ) ) ) );
}

void GUIDirtyArea( gui_window *wnd, SAREA *area )
{
    wnd->flags |= CONTENTS_INVALID;
    GUIWndRfrshArea( wnd, area );
    uirefresh();
}

gui_window *GUIGetTopWnd( gui_window *wnd )
{
    if( wnd == NULL ) {
        return( NULL );
    }
    while( wnd->parent != NULL ) {
        wnd = wnd->parent;
    }
    return( wnd );
}

gui_window * GUIAPI GUIGetFirstSibling( gui_window *wnd )
{
    if( wnd == NULL || wnd->parent == NULL ) {
        return( NULL );
    }
    return( wnd->parent->child );
}

gui_window * GUIAPI GUIGetParentWindow( gui_window *wnd )
{
    if( wnd == NULL ) {
        return( NULL );
    }
    return( wnd->parent );
}

gui_window *GUIXGetRootWindow( void )
{
    gui_window *curr;

    for( curr = GUIGetFront(); curr != NULL; curr = GUIGetNextWindow( curr ) ) {
        if( curr->flags & IS_ROOT ) {
            return( curr );
        }
    }

    return( NULL );
}

bool GUISetCursor( gui_window *wnd )
{
    if( wnd->style & GUI_CURSOR ) {
        if( GUI_WND_MINIMIZED( wnd ) ) {
            uinocursor( &wnd->vs );
        } else {
            uicursor( &wnd->vs, wnd->vs.cursor_row, wnd->vs.cursor_col, wnd->vs.cursor_type );
            return( true );
        }
    }
    return( false );
}

static void DeleteChild( gui_window *parent_wnd, gui_window *child )
{
    gui_window  *curr;
    gui_window  *prev;

    prev = NULL;
    for( curr = parent_wnd->child; curr != NULL; curr = curr->sibling ) {
        if( curr == child ) {
            break;
        }
        prev = curr;
    }
    if( curr != NULL ) {
        if( prev != NULL ) {
            prev->sibling = curr->sibling;
        } else {
            parent_wnd->child = curr->sibling;
        }
    }
}

void GUIAPI GUIWantPartialRows( gui_window *wnd, bool want )
{
    /* unused parameters */ (void)wnd; (void)want;
}

void GUIFreeWindowMemory( gui_window *wnd, bool from_parent, bool dialog )
{
    gui_window  *curr_child;
    gui_window  *next_child;
    gui_window  *front;

    GUIDeleteFromList( wnd );
    if( GUIHasToolBar( wnd ) ) {
        GUICloseToolBar( wnd );
    }
    if( GUIHasStatus( wnd ) ) {
        GUIFreeStatus( wnd );
    }
    if( ( wnd->parent != NULL ) && ( !from_parent ) ) {
        DeleteChild( wnd->parent, wnd );
    }
    if( !dialog ) {
        GUIMDIDelete( wnd );
    }
    front = GUIGetFront();
    if( !dialog && !from_parent && ( front != NULL ) ) {
        GUIBringToFront( front );
    }
    GUIFreeAllControls( wnd, dialog );
    for( curr_child = wnd->child; curr_child != NULL; curr_child = next_child ) {
        next_child = curr_child->sibling;
        if( curr_child != NULL ) {
            GUIEVENT( curr_child, GUI_DESTROY, NULL );
            GUIFreeWindowMemory( curr_child, true, dialog );
        }
    }
    if( wnd->hgadget != NULL ) {
        uifinigadget( wnd->hgadget );
        GUIMemFree( wnd->hgadget );
    }
    if( wnd->vgadget != NULL ) {
        uifinigadget( wnd->vgadget );
        GUIMemFree( wnd->vgadget );
    }
    GUIFreeMenus( wnd );
    GUIFreeHint( wnd );
    GUIMemFree( wnd->icon_name );
    if( !dialog ) {
        uivshow( &wnd->vs );
        wnd->vs.open = true;
        uivclose( &wnd->vs );
    }
    if( GUICurrWnd == wnd ) {
        GUICurrWnd = NULL;
    }
    if( wnd->vs.dynamic_title ) {
        GUIMemFree( (void *)wnd->vs.title );
        wnd->vs.title = NULL;
        wnd->vs.dynamic_title = false;
    }
    GUIFreeColours( wnd );
    GUIMemFree( wnd );
}

static void DoDestroy( gui_window *wnd, bool dialog )
{
    if( wnd != NULL ) {
        GUIEVENT( wnd, GUI_DESTROY, NULL );
        GUIFreeWindowMemory( wnd, false, dialog );
    } else {
        while( (wnd = GUIGetFront()) != NULL ) {
            DoDestroy( wnd, GUI_IS_DIALOG( wnd ) );
        }
    }
}

void GUIDestroyDialog( gui_window *wnd )
{
    DoDestroy( wnd, true );
}

bool GUICloseWnd( gui_window *wnd )
{
    if( wnd != NULL ) {
        if( GUIEVENT( wnd, GUI_CLOSE, NULL ) ) {
            GUIDestroyWnd( wnd );
            return( true );
        }
    }
    return( false );
}

/*
 * GUIDestroyWnd
 */

void GUIAPI GUIDestroyWnd( gui_window *wnd )
{
    DoDestroy( wnd, false );
}
