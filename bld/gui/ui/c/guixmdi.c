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
#include "guixmdi.h"
#include "guixhook.h"
#include "guimdi.h"
#include "guisize.h"

typedef struct {
    int         height;
    int         width;
    int         num;
    int         mod;
    gui_rect    rect;
    gui_coord   char_size;
} cascade_info;

typedef struct {
    int         height;
    int         width;
    bool        horz;
    int         num_per_row;
    int         num_per_col;
    int         num;
    gui_rect    rect;
    int         mod;
    int         num_windows;
    int         extra;
    int         prev_pos_x;
    int         prev_pos_y;
} tile_info;

static void TileWindows( gui_window *wnd, void *param )
{
    tile_info   *info;
    gui_rect    rect;
    int         pos_x;
    int         pos_y;
    int         diff;
    int         next_pos_x;

    info = (tile_info *)param;
    if( GUIIsMinimized( wnd ) ) {
        return;
    }
    if( info->extra > 0 ) {
        if( info->num ==
            ( info->num_windows - ( info->extra * ( info->num_per_col + 1 ) ) ) ) {
            diff = info->num / info->num_per_col;
            info->num_per_col++;
            info->num += diff;
            info->extra = 0;
            info->mod += diff;
        }
    }
    pos_x = info->num / info->num_per_col;
    pos_y = info->num % info->num_per_col;
    if( pos_y == 0 ) {
        info->prev_pos_y = 0;
    }
    if( pos_x == 0 ) {
        info->prev_pos_x = 0;
    }
    if( info->horz ) {
        rect.x = info->prev_pos_x;
        rect.width = ( info->rect.width - info->prev_pos_x ) /
                     ( info->num_per_row - pos_x );
        rect.y = info->prev_pos_y;
        rect.height = ( info->rect.height - info->prev_pos_y ) /
                      ( info->num_per_col - pos_y );
    } else {
        rect.y = info->prev_pos_x;
        rect.height = ( info->rect.height - info->prev_pos_x ) /
                      ( info->num_per_row - pos_x );
        rect.x = info->prev_pos_y;
        rect.width = ( info->rect.width - info->prev_pos_y ) /
                     ( info->num_per_col - pos_y );
    }
    if( !GUIResizeWindow( wnd, &rect ) ) {
        info->num--;
    }
    GUIGetRect( wnd, &rect );
    info->num++;
    info->num = info->num % info->mod;
    next_pos_x = info->num / info->num_per_col;
    if( info->horz ) {
        info->prev_pos_y = rect.y + rect.height;
        if( next_pos_x != pos_x ) {
            info->prev_pos_x = rect.x + rect.width;
        } else {
            info->prev_pos_x = rect.x;
        }
    } else {
        info->prev_pos_y = rect.x + rect.width;
        if( next_pos_x != pos_x ) {
            info->prev_pos_x = rect.y + rect.height;
        } else {
            info->prev_pos_x = rect.y;
        }
    }
}

static void Tile( gui_window *root, int num_windows, gui_rect *rect,
                  gui_coord *min_size, bool horz )
{
    tile_info   info;
    bool        done;

    info.rect = *rect;
    info.horz = horz;
    info.mod = num_windows;
    info.num_windows = num_windows;
    info.num = 0;
    info.prev_pos_y = 0;
    info.prev_pos_x = 0;

    info.num_per_row = 1;
    while( ( info.num_per_row + 1 ) * ( info.num_per_row + 1 ) <= num_windows ) {
        info.num_per_row++;
    }
    done = false;
    while( !done ) {
        info.num_per_col = num_windows / info.num_per_row;
        if( horz ) {
            info.width = rect->width / info.num_per_row;
            info.height = rect->height / info.num_per_col;
            if( info.height < min_size->y ) {
                info.num_per_row++;
            } else {
                done = true;
            }
        } else {
            info.height = rect->height / info.num_per_row;
            info.width = rect->width / info.num_per_col;
            if( info.width < min_size->x ) {
                info.num_per_row++;
            } else {
                done = true;
            }
        }
    }
    info.extra = num_windows - info.num_per_row * info.num_per_col;
    if( horz ) {
        if( info.width < min_size->x ) {
            info.width = min_size->x;
            info.num_per_row = rect->width / info.width;
            info.mod = info.num_per_row * info.num_per_col;
            info.extra = 0;
        }
        if( info.extra > 0 ) {
            if( ( rect->height / ( info.num_per_col + 1 ) ) < min_size->y ) {
                /* windows in columns with an extra window won't be
                   tall enough */
                info.mod = info.num_per_row * info.num_per_col;
                info.extra = 0;
            }
        }
    } else {
        if( info.height < min_size->y ) {
            info.height = min_size->y;
            info.num_per_row = rect->height / info.height;
            info.mod = info.num_per_row * info.num_per_col;
            info.extra = 0;
        }
        if( info.extra > 0 ) {
            if( ( rect->width / ( info.num_per_col + 1 ) ) < min_size->x ) {
                /* windows in columns with an extra window won't be
                   wide enough */
                info.mod = info.num_per_row * info.num_per_col;
                info.extra = 0;
            }
        }
    }
    GUIEnumChildWindows( root, &TileWindows, &info );
}

static void CascadeWindows( gui_window *wnd, void *param )
{
    cascade_info        *info;
    gui_rect            rect;

    if( GUIIsMinimized( wnd ) ) {
        return;
    }
    info = (cascade_info *)param;
    rect.width = info->width;
    rect.height = info->height;
    rect.x = info->num * info->char_size.x;
    rect.y = info->num * info->char_size.y;
    GUIResizeWindow( wnd, &rect );

    info->num++;
    info->num = info->num % info->mod;
}

static void Cascade( gui_window *root, int num_windows, gui_rect *rect,
                     gui_coord *min_size )
{
    cascade_info        info;
    int                 mod;
    gui_text_metrics    metrics;

    GUIGetTextMetrics( root, &metrics );
    info.height = rect->height - metrics.max.y * ( num_windows - 1 );
    info.mod = num_windows;
    if( info.height < min_size->y ) {
        info.height = min_size->y;
        info.mod = ( rect->height - min_size->y ) / metrics.max.y + 1;
    }
    info.width = rect->width - metrics.max.x * ( info.mod - 1 );
    if( info.width < min_size->x ) {
        info.width = min_size->x;
        mod = ( rect->width - min_size->x ) / metrics.max.x + 1;
        if( mod < info.mod ) {
            info.mod = mod;
        }
    }
    info.rect = *rect;
    info.num = 0;
    info.char_size = metrics.max;
    GUIEnumChildWindows( root, &CascadeWindows, &info );
}

static void Arrange( EVENT ev )
{
    gui_rect            rect;
    int                 num_windows;
    gui_window          *root;
    gui_coord           min_size;
    int                 total_icons;

    root = GUIGetRootWindow();
    total_icons = GUIGetNumIconicWindows();
    num_windows = GUIGetNumChildWindows() - total_icons;
    if( ( root == NULL ) || ( num_windows <= 1 ) ) {
        return;
    }
    GUIGetClientRect( root, &rect );
    GUIGetMinSize( &min_size );
    if( total_icons > 0 ) {
        rect.height -= min_size.y;
    }
    switch( ev ) {
    case GUI_MDI_CASCADE :
        Cascade( root, num_windows, &rect, &min_size );
        break;
    case GUI_MDI_TILE_HORZ :
        Tile( root, num_windows, &rect, &min_size, true );
        break;
    case GUI_MDI_TILE_VERT :
        Tile( root, num_windows, &rect, &min_size, false );
        break;
    }
}

static void ArrangeIcons( gui_window *wnd, void *param )
{
    int         *num;
    SAREA       new;

    num = (int *)param;
    if( GUIIsMinimized( wnd ) ) {
        GUICalcIconArea( *num, wnd->parent, &new );
        if( GUIWndMoveSize( wnd, &new, MINIMIZED, RESIZE_DOWN | RESIZE_RIGHT ) ) {
            (*num)++;
        }
    }
}

static void Icons( void )
{
    gui_window  *root;
    int         num;

    root = GUIGetRootWindow();
    num = 1;
    GUIEnumChildWindows( root, &ArrangeIcons, &num );
}

static bool ProcessEvent( EVENT ev )
{
    gui_window  *wnd;
    gui_ctl_id  id;

    id = EV2ID( ev );
    switch( id ) {
    case GUI_MDI_CASCADE :
    case GUI_MDI_TILE_HORZ :
    case GUI_MDI_TILE_VERT :
        Icons();
        Arrange( id );
        break;
    case GUI_MDI_ARRANGE_ICONS :
        Icons();
        break;
    case GUI_MDI_MORE_WINDOWS :
        GUIMDIMoreWindows();
        break;
    default :
        if( IS_MDIWIN( id ) ) {
            wnd = GUIMDIGetWindow( id );
            if( wnd != NULL ) {
                if( GUIIsMinimized( wnd ) ) {
                    GUIRestoreWindow( wnd );
                }
                GUIBringToFront( wnd );
            }
        } else {
            return( false );
        }
    }
    return( true );
}

bool XInitMDI( gui_window *wnd )
{
    return( wnd->parent != NULL );
}

void GUIXMDIInit( void )
{
    GUISetMDIProcessEvent( &ProcessEvent );
}

void XChangeTitle( gui_window *wnd )
{
    wnd=wnd;
}

bool GUICascadeWindows( void )
{
    return( ProcessEvent( ID2EV( GUI_MDI_CASCADE ) ) );
}

