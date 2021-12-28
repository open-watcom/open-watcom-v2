/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Tool bar stuff
*
****************************************************************************/


#include "commonui.h"
#include "vi.h"
#include "toolbr.h"
#include "color.h"
#include "bitmap.h"
#include "wresdefn.h"
#include "rcstr.grh"


#define BORDER_X( x )           ((x) / 4)
#define BORDER_Y( y )           ((y) / 8)
#define TOOLBAR_HEIGHT( y )     ((y) + 2 * BORDER_Y( y ) + 3)

typedef struct tool_item {
    ss                  tool_head;
    int                 id;
    HBITMAP             hbitmap;
    const char          *name;
    const char          *help;
    const char          *tooltip;
    boolbit             is_blank    : 1;
    boolbit             dont_save   : 1;
    char                cmd[1];
} tool_item;

typedef struct tool_tip {
    const char      *name;
    int             tip_id;
} tool_tip;

static const tool_tip tips[] = {
    { "new", TIP_NEW },
    { "open", TIP_OPEN },
    { "save", TIP_SAVE },
    { "cut", TIP_CUT },
    { "copy", TIP_COPY },
    { "paste", TIP_PASTE },
    { "undo", TIP_UNDO },
    { "redo", TIP_REDO },
    { "find", TIP_FIND },
    { "refind", TIP_REFIND },
    { "bmatch", TIP_BMATCH },
    { "files", TIP_FILES },
    { "prevfile", TIP_PREVFILE },
    { "nextfile", TIP_NEXTFILE }
};

static void             *toolBar = NULL;
static ss               *toolBarHead = NULL;
static ss               *toolBarTail = NULL;
static bool             fixedToolBar;
// static RECT          fixedRect;
static bool             userClose = true;
static HBITMAP          buttonPattern;

RECT                    ToolBarFloatRect;

vi_rc HandleToolCommand( int id )
{
    ss          *p;
    tool_item   *item;

    for( p = toolBarHead; p != NULL; p = p->next ) {
        item = (tool_item *)p;
        if( item->id == id ) {
            return( RunCommandLine( item->cmd ) );
        }
    }
    return( MENU_COMMAND_NOT_HANDLED );
}

static void nukeButtons( void )
{
    ss          *p;
    tool_item   *tool;

    for( p = toolBarHead; p != NULL; ) {
        tool = (tool_item *)p;
        p = p->next;
        if( tool->hbitmap ) {
            DeleteObject( tool->hbitmap );
        }
        MemFree( tool );
    }
    toolBarHead = NULL;
    toolBarTail = NULL;
}

/*
 * toolBarHelp - update tool bar hint text
 */
static void toolBarHelp( HWND hwnd, ctl_id id, bool isdown )
{
    ss                 *p;

    hwnd = hwnd;
    SetMenuHelpString( NULL );
    if( isdown ) {
        for( p = toolBarHead; p != NULL; p = p->next ) {
            tool_item *item = (tool_item *)p;
            if( item->id == id ) {
                SetMenuHelpString( item->help );
                break;
            }
        }
    }
    UpdateStatusWindow();

} /* toolBarHelp */

#if 0
static bool myToolBarProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static void newToolBarWindow( void )
{
    RECT                rect;
    POINT               tl;
    int                 height, width;
    TOOLDISPLAYINFO     dinfo;

    userClose = false;

    GetWindowRect( edit_container_window_id, &rect );
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    tl.x = rect.left;
    tl.y = 0;
    ScreenToClient( root_window_id, &tl );

    if( fixedToolBar ) {
        // make it float
        tl.y = 0;
        height += (fixedRect.bottom - fixedRect.top) + 1;
        dinfo.area = ToolBarFloatRect;
        dinfo.style = TOOLBAR_FLOAT_STYLE;
        dinfo.is_fixed = false;
    } else {
//      tl.y = fixedRect.bottom + 1;
//      height -= (fixedRect.bottom - fixedRect.top) + 1;
        tl.y = fixedRect.bottom;
        height -= (fixedRect.bottom - fixedRect.top);
        dinfo.area = fixedRect;
        dinfo.style = TOOLBAR_FIXED_STYLE;
        dinfo.is_fixed = true;
    }
    dinfo.button_size.x = ToolBarButtonWidth;
    dinfo.button_size.y = ToolBarButtonHeight;
    dinfo.border_size.x = BORDER_X( ToolBarButtonWidth );
    dinfo.border_size.y = BORDER_Y( ToolBarButtonHeight );
    dinfo.background = buttonPattern;
    dinfo.hook = myToolBarProc;
    dinfo.helphook = toolBarHelp;

    ToolBarDisplay( toolBar, &dinfo );

    MoveWindow( edit_container_window_id, tl.x, tl.y, width, height, TRUE );
    ShowWindow( ToolBarWindow( toolBar ), SW_SHOWNORMAL );
    UpdateWindow( ToolBarWindow( toolBar ) );
    fixedToolBar = !fixedToolBar;

    userClose = true;

} /* newToolBarWindow */
#endif

/*
 * myToolBarProc - called by toolbar window proc
 */
static bool myToolBarProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg ) {
    case WM_KILLFOCUS:
        UnselectRegion();
        break;
    case WM_LBUTTONDBLCLK:
        // flip the current state of the toolbar - if we are fixed then start to float or vice versa
//      newToolBarWindow();
//      return( true );
        break;
    case WM_MOVE:
    case WM_SIZE:
        // Whenever we are moved or sized as a floating toolbar, we
        // remember our position so that we can restore it when dbl. clicked
        if( !fixedToolBar && userClose ) {
            DefWindowProc( hwnd, msg, wparam, lparam );
            GetWindowRect( hwnd, &ToolBarFloatRect );
        }
        return( false );
    case WM_DESTROY:
        if( userClose ) {
            // the user closed the toolbar so remember this
            EditFlags.Toolbar = false;
        }
        break;
    }
    return( false );

} /* myToolBarProc */

/*
 * getTip - get the string identifier of the tooltip for a given item
 */
static int getTip( const char *name )
{
    int count = sizeof( tips ) / sizeof( tips[0] );
    int i;
    if( name != NULL ) {
        for( i = 0; i < count; i++ ) {
            if( strcmp( name, tips[i].name ) == 0 ) {
                return( tips[i].tip_id );
            }
        }
    }
    return( -1 );

} /* getTip */

/*
 * addToolBarItem - add an item to the tool bar
 */
static void addToolBarItem( tool_item *item )
{
    TOOLITEMINFO        info;

    if( item->is_blank ) {
        info.u.blank_space = 8;
        info.flags = ITEM_BLANK;
    } else {
        info.id = item->id;
        info.u.hbitmap = item->hbitmap;
        info.flags = 0;
    }
    if( item->tooltip == NULL ) {
        info.tip[0] = '\0';
    } else if( IS_INTRESOURCE( item->tooltip ) ) {
        if( LoadString( InstanceHandle, RESOURCE2INT( item->tooltip ), info.tip, MAX_TIP ) <= 0 ) {
            info.tip[0] = '\0';
        }
    } else {
        strcpy( info.tip, item->tooltip );
    }
    info.depressed = false;
    ToolBarAddItem( toolBar, &info );
    InvalidateRect( ToolBarWindow( toolBar ), NULL, FALSE );

} /* addToolBarItem */

/*
 * AddBitmapToToolBar - add a toolbar item ([temp], bitmap, help, [tooltip] and command)
 */
vi_rc AddBitmapToToolBar( const char *data, bool tip )
{
    char                file[FILENAME_MAX];
    char                help[MAX_STR];
    char                tooltip[MAX_STR];
    tool_item           *item;
    int                 cmd_len;
    int                 name_len;
    int                 help_len;
    int                 tooltip_len;
    int                 tip_id;
    bool                dont_save;
    char                *p;

    help[0] = '\0';
    dont_save = false;
    if( strnicmp( data, "temp", 4 ) == 0 ) {
        /* get to the command */
        GetNextWordOrString( &data, help );
        dont_save = true;
    }

    GetNextWordOrString( &data, file );
    GetNextWordOrString( &data, help );
    if( tip ) {
        GetNextWordOrString( &data, tooltip );
    } else {
        tooltip[0] = '\0';
    }

    cmd_len = strlen( data );
    name_len = strlen( file );
    help_len = strlen( help );
    tooltip_len = strlen( tooltip );
    if( tooltip_len > MAX_TIP - 1 )
        tooltip_len = MAX_TIP - 1;
    item = MemAlloc( sizeof( tool_item ) + cmd_len + name_len + help_len + tooltip_len + 3 );
    strcpy( item->cmd, data );
    if( name_len != 0 ) {
        item->id = NextMenuId();
    } else {
        item->is_blank = true;
    }
    item->dont_save = dont_save;
    item->name = p = &item->cmd[cmd_len + 1];
    strcpy( p, file );
    p += name_len + 1;
    item->help = p;
    strcpy( p, help );
    p += help_len + 1;
    item->tooltip = p;
    memcpy( p, tooltip, tooltip_len );
    p[tooltip_len] = '\0';
    item->hbitmap = HNULL;
    if( file[0] != '\0' && item->cmd[0] != '\0' ) {
        item->hbitmap = LoadBitmap( InstanceHandle, file );
        if( item->hbitmap == HNULL ) {
            item->hbitmap = ReadBitmapFile( ToolBarWindow( toolBar ), file, NULL );
        }
        if( tooltip_len == 0 ) {
            tip_id = getTip( item->name );
            if( tip_id > 0 ) {
                item->tooltip = MAKEINTRESOURCE( tip_id );
            }
        }
    }
    if( toolBar != NULL ) {
        addToolBarItem( item );
    }
    AddLLItemAtEnd( &toolBarHead, &toolBarTail, &item->tool_head );
    return( ERR_NO_ERR );

} /* AddBitmapToToolBar */

/*
 * DeleteFromToolBar - delete an item from the toolbar
 */
vi_rc DeleteFromToolBar( const char *data )
{
    char    buffer[MAX_STR];
    int     index;
    ss      *p;

    GetNextWord1( data, buffer );
    index = atoi( buffer );
    // index should be (base 1) index of tool in list
    if( index > 0 ) {
        for( p = toolBarHead; p != NULL; p = p->next ) {
            index -= 1;
            if( index == 0 ) {
                break;
            }
        }
        if( p != NULL ) {
            tool_item *item = (tool_item *)p;
            ToolBarDeleteItem( toolBar, item->id );
            DeleteLLItem( &toolBarHead, &toolBarTail, p );
            if( item->hbitmap != NULL ) {
                DeleteObject( item->hbitmap );
            }
            return( ERR_NO_ERR );
        }
    }
    return( ERR_INVALID_MENU );

} /* DeleteFromToolBar */

/*
 * CloseToolBar - make the toolbar go away.
 */
void CloseToolBar( void )
{
    if( toolBar != NULL ) {
        DeleteObject( buttonPattern );
        ToolBarFini( toolBar );
        toolBar = NULL;
    }

} /* CloseToolBar */

/*
 * DestroyToolBar - make the toolbar go away and destroy all buttons.
 */
void DestroyToolBar( void )
{
    if( toolBar != NULL ) {
        CloseToolBar();
    }
    nukeButtons();

} /* DestroyToolBar */

/*
 * GetToolbarWindow - return the current toolbar window
 */
window_id GetToolbarWindow( void )
{
    return( ToolBarWindow( toolBar ) );

} /* GetToolbarWindow */

/*
 * BarfToolBarData - output toolbar data to config file
 */
void BarfToolBarData( FILE *f )
{
    ss          *p;
    tool_item   *citem;
    const char  *tooltip;

    for( p = toolBarHead; p != NULL; p = p->next ) {
        citem = (tool_item *)p;
        if( citem->dont_save ) {
            /* do nothing */
        } else if( citem->is_blank ) {
            MyFprintf( f, "addtoolbaritem2\n" );
        } else {
            tooltip = citem->tooltip;
            if( IS_INTRESOURCE( citem->tooltip ) )
                tooltip = "";
            MyFprintf( f, "addtoolbaritem2 %s \"%s\" \"%s\" %s\n", citem->name, citem->help, tooltip, citem->cmd );
        }
    }

} /* BarfToolBarData */

/*
 * createToolBar - create the tool bar
 */
static void createToolBar( RECT *rect )
{
    int                 toolbar_height;
    TOOLDISPLAYINFO     dinfo;

    fixedToolBar = true;
    dinfo.button_size.x = EditVars.ToolBarButtonWidth;
    dinfo.button_size.y = EditVars.ToolBarButtonHeight;
    dinfo.border_size.x = BORDER_X( EditVars.ToolBarButtonWidth );
    dinfo.border_size.y = BORDER_Y( EditVars.ToolBarButtonHeight );
    dinfo.style = TOOLBAR_FIXED_STYLE;
    dinfo.is_fixed = true;
    toolbar_height = TOOLBAR_HEIGHT( EditVars.ToolBarButtonHeight );
    dinfo.area = *rect;
    dinfo.area.bottom = ROUNDUP( dinfo.area.top + toolbar_height, 2 ) - 1;
    dinfo.area.top -= 1;
    dinfo.area.bottom -= 1;
    dinfo.area.left -= 1;
    dinfo.area.right += 1;
    dinfo.hook = myToolBarProc;
    dinfo.helphook = toolBarHelp;
    dinfo.background = LoadBitmap( InstanceHandle, "BUTTONPATTERN" );
    dinfo.use_tips = 1;
    buttonPattern = dinfo.background;
    toolBar = ToolBarInit( root_window_id );
#if defined( __NT__ )
    ToolBarChangeSysColors( GetSysColor( COLOR_BTNFACE ),
#else
    ToolBarChangeSysColors( GetRGB( EditVars.ToolBarColor ),
#endif
                            GetSysColor( COLOR_BTNHIGHLIGHT ),
                            GetSysColor( COLOR_BTNSHADOW ) );
    ToolBarDisplay( toolBar, &dinfo );
    if( toolBar != NULL ) {
        // CopyRect( &fixedRect, &dinfo.area );
        // WARNING: These are some pretty stupid arbitrary constants here
        rect->top = dinfo.area.bottom;
        ShowWindow( ToolBarWindow( toolBar ), SW_SHOWNORMAL );
        // UpdateWindow( ToolBarWindow( toolBar ) );
    }

} /* createToolBar */

/*
 * NewToolBar - create a new brand tool bar
 */
void NewToolBar( RECT *rect )
{
    ss          *curr;
    RECT        covered;

    if( toolBar != NULL ) {
        userClose = false;
        CloseToolBar();
        userClose = true;
    }
    if( !EditFlags.Toolbar ) {
        return;
    }
    createToolBar( rect );
    for( curr = toolBarHead; curr != NULL; curr = curr->next ) {
        addToolBarItem( (tool_item *)curr );
    }
    UpdateToolBar( toolBar );
    covered = *rect;
    covered.bottom = rect->top;
    covered.top = 0;
    InvalidateRect( edit_container_window_id, &covered, FALSE );

} /* NewToolBar */
