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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stddef.h>
#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgadget.h"
#include "guidlg.h"
#include "sortlist.h"
#include "dbgerr.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgmain.h"
#include "dbginvk.h"
#include "dbgshow.h"
#include "wndsys.h"
#include "dbgwglob.h"
#include "dbgwmac.h"
#include "dbgwset.h"
#include "wndmenu.h"
#include "menudef.h"
#include "dbgmisc.h"


#define TITLE_SIZE      2

#define MIN_KEY_SIZE( wnd ) (8 * WndAvgCharX( wnd )) // something big enough to click on

#define WndMac( wnd ) ( (mac_window *)WndExtra( wnd ) )

enum {
    PIECE_KEY,
    PIECE_WHERE,
    PIECE_WHAT,
    PIECE_TEXT,
    PIECE_LAST
};

typedef struct {
    unsigned            last_id;
    wnd_row             change_row;
    gui_menu_items      menus;
    wnd_macro           *mac;
    boolbit             press_key : 1;
    boolbit             creating  : 1;
    boolbit             changing  : 1;
} mac_window;

static char **WndDisplayNames[] = {
    #define pick( a,b,c,d,e,f ) LITREF_DUI( f ),
    #include "wndnames.h"
    #undef pick
};

static char **WhatList[] = {
    LITREF_DUI( mac_popup_menu ),
    LITREF_DUI( mac_main_menu ),
    LITREF_DUI( mac_command_string ),
};

static char **Titles[] = {
    LITREF_DUI( Key_Name ),
    LITREF_DUI( Mac_Window ),
    LITREF_DUI( Macro_Type ),
    LITREF_DUI( Definition )
};

static gui_ord  Indents[PIECE_LAST];

static gui_menu_struct MacMenu[] = {
    #include "menumac.h"
};

static const char *WndGetName( const void *data_handle, int item )
{
    return( *((const char ***)data_handle)[item] );
}

#ifdef DEADCODE
wnd_macro *MacFindMac( gui_key key, wnd_class_wv wndclass )
{
    wnd_macro   *mac;

    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        if( mac->key == key && mac->wndclass == wndclass ) {
            return( mac );
        }
    }
    return( NULL );
}
#endif

static wnd_macro *MacGetMacro( int row )
{
    wnd_macro   *mac;
    int         count;

    if( row < 0 )
        return( NULL );
    count = 0;
    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        if( count == row )
            break;
        ++count;
    }
    return( mac );
}


static void MacChangeMac( a_window wnd, wnd_macro *mac, gui_key key, wnd_class_wv wndclass, wnd_row row )
{
    cmd_list    *cmds;
    wnd_macro   **owner,*curr;
    wnd_row     i;

    cmds = mac->cmd;
    LockCmdList( cmds );
    MacAddDel( mac->key, mac->wndclass, NULL );
    curr = MacAddDel( key, wndclass, cmds );
    for( owner = &WndMacroList; *owner; owner = &(*owner)->link ) {
        if( *owner == curr ) {
            break;
        }
    }
    *owner = curr->link;
    i = 0;
    for( owner = &WndMacroList; *owner; owner = &(*owner)->link ) {
        if( i == row )
            break;
        ++i;
    }
    curr->link = *owner;
    *owner = curr;
    WndNewCurrent( wnd, i, PIECE_KEY );
}

static void     MacModify( a_window wnd, wnd_row row, wnd_piece piece );

static bool MacModWhat( a_window wnd, wnd_row row )
{
    int         new;
    int         old;
    wnd_macro   *mac;

    /* unused parameters */ (void)wnd;

    mac = MacGetMacro( row );
    old = mac->type;
    new = -1;
    if( mac->wndclass == WND_ALL ) {
        if( DlgPickWithRtn( LIT_DUI( Macro_Type ), WhatList + 1, old - 1, WndGetName, ArraySize( WhatList ) - 1, &new ) ) {
            ++new;
        }
    } else {
        DlgPickWithRtn( LIT_DUI( Macro_Type ), WhatList, old, WndGetName, ArraySize( WhatList ), &new );
    }
    if( new != -1 ) {
        mac->type = new;
        if( new != old ) {
            MacModify( wnd, row, PIECE_TEXT );
        }
        return( true );
    } else {
        return( false );
    }
}


bool MacKeyHit( a_window wnd, gui_key key )
{
    mac_window  *wndmac;
    wnd_macro   *mac, *curr;
    wnd_row     row;
    int         new;

    wndmac = WndMac( wnd );
    if( wndmac->press_key ) {
        if( KeyName( key ) == NULL ) {
            RingBell();
            return( true );
        }
        wndmac->press_key = false;
        WndZapped( wnd );
        if( key != GUI_KEY_ESCAPE ) {
            if( wndmac->changing ) {
                wndmac->changing = false;
                mac = MacGetMacro( wndmac->change_row );
                MacChangeMac( wnd, mac, key, mac->wndclass, wndmac->change_row );
            } else if( wndmac->creating ) {
                wndmac->creating = false;
                if( DlgPickWithRtn( LIT_DUI( Enter_Window ), WndDisplayNames, WND_ALL, WndGetName, NUM_WNDCLS, &new ) ) {
                    curr = MacAddDel( key, (wnd_class_wv)new, AllocCmdList( LIT_ENG( Quest_Marks ), strlen( LIT_ENG( Quest_Marks ) ) ) );
                    row = 0;
                    for( mac = WndMacroList; mac != curr; mac = mac->link ) {
                        ++row;
                    }
                    curr->type = -1;
                    if( !MacModWhat( wnd, row ) ) {
                        mac->type = MACRO_COMMAND;
                    }
                    WndVScrollAbs( wnd, row );
                    WndNewCurrent( wnd, row, PIECE_WHAT );
                }
            }
        }
        return( true );
    } else {
        return( false );
    }
}

static bool MacPopupClicked( a_window wnd, gui_ctl_id id )
{
    char                *p;
    gui_ctl_id          main_id;
    mac_window          *wndmac;
    cmd_list            *cmds;

    WndInstallClickHook( NULL );
    if( wnd == NULL || WndClass( wnd ) != WND_MACRO )
        return( false );
    wndmac = WndMac( wnd );
    if( wndmac->mac == NULL )
        return( false );
    p = StrCopy( GetCmdName( CMD_ACCEL ), TxtBuff );
    *p++ = ' ';
    if( wndmac->mac->type == MACRO_MAIN_MENU ) {
        p = GetCmdMain( p );
        *p++ = ' ';
        main_id = MAIN_MENU_ID( id );
        *p++ = '{';
        p = GetMenuLabel( &wndmac->menus, main_id, p, false );
        *p++ = '}';
        *p++ = ' ';
        *p++ = '{';
        p = GetMenuLabel( &wndmac->menus, id, p, false );
    } else {
        *p++ = '{';
        p = GetMenuLabel( &wndmac->menus, id, p, false );
    }
    if( p != NULL ) {
        *p++ = '}';
        *p++ = NULLCHAR;
        cmds = AllocCmdList( TxtBuff, p - TxtBuff );
        MacAddDel( wndmac->mac->key, wndmac->mac->wndclass, cmds );
    }
    return( true );
}


static void MacModMenu( a_window wnd, wnd_row row )
{
    wnd_info            *info;
    gui_point           point;
    gui_ctl_id          dummy;
    mac_window          *wndmac;
    wnd_macro           *mac;

    wndmac= WndMac( wnd );
    mac = MacGetMacro( row );
    info = WndInfoTab[mac->wndclass];
    WndCurrToGUIPoint( wnd, &point );
    WndInstallClickHook( MacPopupClicked );
    wndmac->last_id = 0;
    wndmac->mac = mac;
    if( mac->type == MACRO_MAIN_MENU ) {
        wndmac->menus = WndMainMenuMacro;
    } else {
        wndmac->menus = info->popup;
        WndChangeMenuAll( &info->popup, false, GUI_STYLE_MENU_GRAYED );
    }
    WndCreateFloatingPopup( wnd, &point, &wndmac->menus, &dummy );
}

static void MacModWhere( a_window wnd, wnd_row row )
{
    int                 new;
    wnd_macro           *mac;

    /* unused parameters */ (void)wnd;

    mac = MacGetMacro( row );
    if( DlgPickWithRtn( LIT_DUI( Enter_Window ), WndDisplayNames, mac->wndclass, WndGetName, NUM_WNDCLS, &new ) ) {
//        WndSetRepaint( wnd );
        MacChangeMac( wnd, mac, mac->key, (wnd_class_wv)new, row );
    }
}

static void MacModKey( a_window wnd, wnd_row row )
{
    mac_window          *wndmac= WndMac( wnd );

    wndmac->press_key = true;
    wndmac->changing = true;
    wndmac->change_row = row;
    WndZapped( wnd );
}


static void MacModCmd( a_window wnd, wnd_row row )
{
    cmd_list    *cmds;
    wnd_macro   *mac;
//    char        *p;

    /* unused parameters */ (void)wnd;

    mac = MacGetMacro( row );
    cmds = mac->cmd;
//    p = StrCopy( cmds->buff, TxtBuff );
    StrCopy( cmds->buff, TxtBuff );
    if( DlgNew( LIT_DUI( Enter_New_Command ), TxtBuff, TXT_LEN ) ) {
        cmds = AllocCmdList( TxtBuff, strlen( TxtBuff ) );
        FreeCmdList( mac->cmd );
        mac->cmd = cmds;
    }
}


static void     MacModify( a_window wnd, wnd_row row, wnd_piece piece )
{
    wnd_macro   *mac;

    switch( piece ) {
    case PIECE_KEY:
        MacModKey( wnd, row );
        break;
    case PIECE_WHERE:
        MacModWhere( wnd, row );
        break;
    case PIECE_WHAT:
        MacModWhat( wnd, row );
        break;
    case PIECE_TEXT:
        mac = MacGetMacro( row );
        if( mac->type == MACRO_COMMAND ) {
            MacModCmd( wnd, row );
        } else {
            MacModMenu( wnd, row );
        }
        break;
    }
    WndRowDirty( wnd, row );
}


#define TDDBG "tdkeys.dbg"
#define WDDBG "wdkeys.dbg"

static void     MacMenuItem( a_window wnd, gui_ctl_id id, wnd_row row, wnd_piece piece )
{
    wnd_macro           *mac;
    mac_window          *wndmac;
    const char          *old;

    /* unused parameters */ (void)piece;

    wndmac = WndMac( wnd );
    wndmac->last_id = id;
    if( row < 0 ) {
        mac = NULL;
    } else {
        mac = MacGetMacro( row );
    }
    switch( id ) {
    case MENU_INITIALIZE:
        wndmac->mac = NULL;
        if( mac != NULL ) {
            WndMenuEnableAll( wnd );
        } else {
            WndMenuGrayAll( wnd );
        }
        WndMenuEnable( wnd, MENU_MAC_CREATE_NEW, true );
        WndMenuEnable( wnd, MENU_MAC_WD, true );
        WndMenuEnable( wnd, MENU_MAC_TD, true );
        break;
    case MENU_MAC_MODIFY:
        MacModify( wnd, row, piece );
        break;
    case MENU_MAC_CREATE_NEW:
        wndmac->press_key = true;
        wndmac->creating = true;
        WndZapped( wnd );
        break;
    case MENU_MAC_DELETE:
        MacAddDel( mac->key, mac->wndclass, NULL );
        break;
    case MENU_MAC_TD:
        FiniMacros();
        old = ReScan( LIT_ENG( Empty ) );
        Invoke( TDDBG, sizeof( TDDBG ) - 1, NULL );
        ReScan( old );
        break;
    case MENU_MAC_WD:
        FiniMacros();
        old = ReScan( LIT_ENG( Empty ) );
        Invoke( WDDBG, sizeof( WDDBG ) - 1, NULL );
        ReScan( old );
        break;
    default:
        break;
    }
}

static wnd_row MacNumRows( a_window wnd )
{
    wnd_macro   *mac;
    wnd_row     count;

    /* unused parameters */ (void)wnd;

    count = 0;
    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        ++count;
    }
    return( count );
}

static  bool MacGetLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    wnd_macro           *mac;
    cmd_list            *cmds;
    mac_window          *wndmac = WndMac( wnd );
    gui_ctl_id          main_id;
    char                *p;

    if( wndmac->press_key ) {
        if( piece != 0 )
            return( false );
        line->tabstop = false;
        if( row > WndTop( wnd ) + WndRows( wnd ) - 1 ) {
            return( false );
        } else if( row == WndTop( wnd ) + WndRows( wnd ) / 2 ) {
            if( wndmac->changing ) {
                line->text = LIT_DUI( Redefine_Key );
            } else {
                line->text = LIT_DUI( Press_the_Key );
            }
        } else {
            line->text = " ";
        }
        return( true );
    }
    if( row < 0 ) {
        row += TITLE_SIZE;
        if( row == 0 ) {
            line->tabstop = false;
            if( piece >= PIECE_LAST )
                return( false );
            line->indent = Indents[piece];
            line->text = *Titles[piece];
            return( true );
        } else if( row == 1 ) {
            if( piece != 0 )
                return( false );
            SetUnderLine( wnd, line );
            return( true );
        } else {
            return( false );
        }
    } else {
        mac = MacGetMacro( row );
        if( mac == NULL )
            return( false );
        line->tabstop = true;
        line->indent = Indents[piece];
        switch( piece ) {
        case PIECE_KEY:
            p = StrCopy( KeyName( mac->key ), TxtBuff );
            while( WndExtentX( wnd, TxtBuff ) < MIN_KEY_SIZE( wnd ) ) {
                p = StrCopy( " ", p );
            }
            line->text = TxtBuff;
            return( true );
        case PIECE_WHERE:
            line->text = TxtBuff;
            StrCopy( *WndDisplayNames[mac->wndclass], TxtBuff );
            return( true );
        case PIECE_WHAT:
            line->text = *WhatList[mac->type];
            return( true );
        case PIECE_TEXT:
            p = TxtBuff;
            switch( mac->type ) {
            case MACRO_COMMAND:
                cmds = mac->cmd;
                line->text = cmds->buff;
                if( line->text[0] == NULLCHAR ) {
                    line->text = LIT_ENG( Quest_Marks );
                }
                break;
            case MACRO_MAIN_MENU:
                if( mac->menu != NULL ) {
                    main_id = MAIN_MENU_ID( mac->menu->id );
                    p = GetMenuLabel( &WndMainMenu, main_id, TxtBuff, true );
                    *p++ = '/';
                }
                /* fall through */
            case MACRO_POPUP_MENU:
                if( mac->menu == NULL ) {
                    line->text = LIT_ENG( Quest_Marks );
                } else {
                    gui_menu_items  menus;

                    menus.num_items = 1;
                    menus.menu = mac->menu;
                    line->text = TxtBuff;
                    p = GetMenuLabel( &menus, mac->menu->id, p, true );
                }
            }
            return( true );
        }
    }
    return( false );
}


static int MacCompare( void *pa, void *pb )
{
    if( (*(wnd_macro **)pa)->key < (*(wnd_macro **)pb)->key )
        return( -1 );
    if( (*(wnd_macro **)pa)->key > (*(wnd_macro **)pb)->key )
        return( 1 );
    return( 0 );
}

static void MacReSize( a_window wnd )
{
    wnd_macro       *mac;
    wnd_piece       piece;
    gui_ord         size;
    wnd_class_wv    wndclass;
    gui_ord         max_size[PIECE_LAST];
    int             i;

    for( piece = 0; piece < PIECE_LAST; ++piece ) {
        max_size[piece] = WndExtentX( wnd, *Titles[piece] );
    }
    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        size = WndExtentX( wnd, KeyName( mac->key ) );
        if( size < MIN_KEY_SIZE( wnd ) )
            size = MIN_KEY_SIZE( wnd );
        if( max_size[PIECE_KEY] < size ) {
            max_size[PIECE_KEY] = size;
        }
    }
    for( i = 0; i < ArraySize( WhatList ); ++i ) {
        size = WndExtentX( wnd, *WhatList[i] );
        if( max_size[PIECE_WHAT] < size ) {
            max_size[PIECE_WHAT] = size;
        }
    }
    for( wndclass = 0; wndclass < NUM_WNDCLS; ++wndclass ) {
        size = WndExtentX( wnd, *WndDisplayNames[wndclass] );
        if( max_size[PIECE_WHERE] < size ) {
            max_size[PIECE_WHERE] = size;
        }
    }
    Indents[PIECE_KEY] = WndAvgCharX( wnd );
    Indents[PIECE_WHERE] = Indents[PIECE_KEY] + max_size[PIECE_KEY] + 2 * WndAvgCharX( wnd );
    Indents[PIECE_WHAT] = Indents[PIECE_WHERE] + max_size[PIECE_WHERE] + 2 * WndAvgCharX( wnd );
    Indents[PIECE_TEXT] = Indents[PIECE_WHAT] + max_size[PIECE_WHAT] + 2 * WndAvgCharX( wnd );
}

static void     MacRefresh( a_window wnd )
{
    MacReSize( wnd );
    WndNoSelect( wnd );
    WndSetRepaint( wnd );
}

static bool MacWndEventProc( a_window wnd, gui_event gui_ev, void *parm )
{
    mac_window  *wndmac = WndMac( wnd );

    /* unused parameters */ (void)parm;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        MacRefresh( wnd );
        WndMacroList = SortLinkedList( WndMacroList,
                    offsetof( wnd_macro, link ),
                    MacCompare, WndAlloc, WndFree );
        MacReSize( wnd );
        memset( wndmac, 0, sizeof( *wndmac ) );
        return( true );
    case GUI_RESIZE:
        MacReSize( wnd );
        return( true );
    case GUI_DESTROY:
        WndFree( wndmac );
        return( true );
    }
    return( false );
}

static bool ChkUpdate( void )
{
    return( UpdateFlags & UP_MACRO_CHANGE );
}

wnd_info MacInfo = {
    MacWndEventProc,
    MacRefresh,
    MacGetLine,
    MacMenuItem,
    NoVScroll,
    NoBegPaint,
    NoEndPaint,
    MacModify,
    MacNumRows,
    NoNextRow,
    NoNotify,
    ChkUpdate,
    PopUp( MacMenu )
};

a_window WndMacOpen( void )
{
    mac_window  *wndmac;

    wndmac = WndMustAlloc( sizeof( *wndmac ) );
    return( DbgTitleWndCreate( LIT_DUI( WindowAccelerator ), &MacInfo, WND_MACRO, wndmac, &AclIcon, TITLE_SIZE, true ) );
}
