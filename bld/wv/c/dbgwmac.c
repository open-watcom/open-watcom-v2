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


extern const char       MainTab[];
extern wnd_info         *WndInfoTab[];
extern gui_menu_struct  WndMainMenu[];
extern int              WndNumMenus;

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

#define TITLE_SIZE      2
static char **Titles[] = { LITREF_DUI( Key_Name ), LITREF_DUI( Mac_Window ),
                         LITREF_DUI( Macro_Type ), LITREF_DUI( Definition ) };

enum {
    PIECE_KEY,
    PIECE_WHERE,
    PIECE_WHAT,
    PIECE_TEXT,
    PIECE_LAST
};

#define MIN_KEY_SIZE( wnd ) (8 * WndAvgCharX(wnd)) // something big enough to click on

static gui_ord  Indents[PIECE_LAST];

typedef struct {
    unsigned            last_id;
    wnd_row             change_row;
    unsigned            size;
    gui_menu_struct     *menu;
    wnd_macro           *mac;
    unsigned            press_key : 1;
    unsigned            creating : 1;
    unsigned            changing : 1;
} mac_window;
#define WndMac( wnd ) ( (mac_window *)WndExtra( wnd ) )

#include "menudef.h"
static gui_menu_struct MacMenu[] = {
    #include "menumac.h"
};


static const char *WndGetName( const void *data_handle, int item )
{
    return( *((const char ***)data_handle)[item] );
}

#ifdef DEADCODE
wnd_macro *MacFindMac( unsigned key, wnd_class_wv wndclass )
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


static void MacChangeMac( a_window *wnd, wnd_macro *mac, unsigned key,
                          wnd_class_wv wndclass, wnd_row row )
{
    cmd_list    *cmds;
    wnd_macro   **owner,*curr;
    wnd_row     i;

    cmds = mac->cmd;
    LockCmdList( cmds );
    MacAddDel( mac->key, mac->wndclass, NULL );
    curr = MacAddDel( key, wndclass, cmds );
    for( owner = &WndMacroList; *owner; owner = &(*owner)->link ) {
        if( *owner == curr ) break;
    }
    *owner = curr->link;
    i = 0;
    for( owner = &WndMacroList; *owner; owner = &(*owner)->link ) {
        if( i == row ) break;
        ++i;
    }
    curr->link = *owner;
    *owner = curr;
    WndNewCurrent( wnd, i, PIECE_KEY );
}

static void     MacModify( a_window *wnd, int row, int piece );

static bool MacModWhat( a_window *wnd, wnd_row row )
{
    int         new;
    int         old;
    wnd_macro   *mac;

    wnd=wnd;
    mac = MacGetMacro( row );
    old = mac->type;
    if( mac->wndclass == WND_ALL ) {
        new = DlgPickWithRtn( LIT_DUI( Macro_Type ), WhatList + 1,
                       old - 1, WndGetName, ArraySize( WhatList ) - 1 );
        if( new != -1 ) {
            ++new;
        }
    } else {
        new = DlgPickWithRtn( LIT_DUI( Macro_Type ), WhatList,
                       old, WndGetName, ArraySize( WhatList ) );
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


bool MacKeyHit( a_window *wnd, unsigned key )
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
        if ( key == GUI_KEY_ESCAPE )
            return( true );
        if( wndmac->changing ) {
            wndmac->changing = false;
            mac = MacGetMacro( wndmac->change_row );
            MacChangeMac( wnd, mac, key, mac->wndclass, wndmac->change_row );
        } else if( wndmac->creating ) {
            wndmac->creating = false;
            new = DlgPickWithRtn( LIT_DUI( Enter_Window ), WndDisplayNames, WND_ALL, WndGetName, WND_CURRENT );
            if( new == -1 )
                return( true );
            curr = MacAddDel( key, (wnd_class_wv)new, AllocCmdList( LIT_ENG( Quest_Marks ), strlen( LIT_ENG( Quest_Marks ) ) ) );
            row = 0;
            for( mac = WndMacroList; mac != curr; mac = mac->link ) {
                ++row;
            }
            curr->type = -1;
            if( !MacModWhat( wnd, row ) ) {
                mac->type = MACRO_COMMAND;
            }
            WndScrollAbs( wnd, row );
            WndNewCurrent( wnd, row, PIECE_WHAT );
        }
        return( true );
    } else {
        return( false );
    }
}

static bool MacPopupClicked( a_window *wnd, gui_ctl_id id )
{
    char                *p;
    gui_ctl_id          main_id;
    mac_window          *wndmac;
    cmd_list            *cmds;

    WndInstallClickHook( NULL );
    if( wnd == NULL || WndClass( wnd ) != WND_MACRO ) return( false );
    wndmac = WndMac( wnd );
    if( wndmac->mac == NULL ) return( false );
    p = StrCopy( GetCmdName( CMD_ACCEL ), TxtBuff );
    *p++ = ' ';
    if( wndmac->mac->type == MACRO_MAIN_MENU ) {
        p = GetCmdEntry( MainTab, 0, p );
        *p++ = ' ';
        main_id = id - id % MAIN_MENU_MULT;
        *p++ = '{';
        p = GetMenuLabel( wndmac->size, wndmac->menu, main_id, p, false );
        *p++ = '}';
        *p++ = ' ';
        *p++ = '{';
        p = GetMenuLabel( wndmac->size, wndmac->menu, id, p, false );
    } else {
        *p++ = '{';
        p = GetMenuLabel( wndmac->size, wndmac->menu, id, p, false );
    }
    if( p != NULL ) {
        *p++ = '}';
        *p++ = '\0';
        cmds = AllocCmdList( TxtBuff, p - TxtBuff );
        MacAddDel( wndmac->mac->key, wndmac->mac->wndclass, cmds );
    }
    return( true );
}


static void MacModMenu( a_window *wnd, wnd_row row )
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
    if( mac->type == MACRO_MAIN_MENU ) {
        wndmac->last_id = 0;
        wndmac->menu = WndMainMenu;
        wndmac->size = WndNumMenus-2;
        wndmac->mac = mac;
        WndCreateFloatingPopup( wnd, &point, wndmac->size, wndmac->menu, &dummy );
    } else {
        wndmac->last_id = 0;
        wndmac->menu = info->popupmenu;
        wndmac->size = info->num_popups;
        wndmac->mac = mac;
        WndChangeMenuAll( info->popupmenu, info->num_popups, false, GUI_GRAYED );
        WndCreateFloatingPopup( wnd, &point, wndmac->size, wndmac->menu, &dummy );
    }
}

static void MacModWhere( a_window *wnd, wnd_row row )
{
    int                 new;
    wnd_macro           *mac;

    wnd=wnd;
    mac = MacGetMacro( row );
    new = DlgPickWithRtn( LIT_DUI( Enter_Window ), WndDisplayNames, mac->wndclass, WndGetName, WND_CURRENT );
    if( new == -1 )
        return;
//    WndRepaint( wnd );
    MacChangeMac( wnd, mac, mac->key, (wnd_class_wv)new, row );
}

static void MacModKey( a_window *wnd, wnd_row row )
{
    mac_window          *wndmac= WndMac( wnd );

    wndmac->press_key = true;
    wndmac->changing = true;
    wndmac->change_row = row;
    WndZapped( wnd );
}


static void MacModCmd( a_window *wnd, wnd_row row )
{
    cmd_list    *cmds;
    wnd_macro   *mac;
//    char        *p;

    wnd=wnd;
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


static void     MacModify( a_window *wnd, int row, int piece )
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

static void     MacMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece )
{
    wnd_macro           *mac;
    mac_window          *wndmac;
    const char          *old;

    wndmac = WndMac( wnd );
    wndmac->last_id = id;
    piece=piece;
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

static int MacNumRows( a_window *wnd )
{
    wnd_macro   *mac;
    int         count;

    wnd=wnd;
    count = 0;
    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        ++count;
    }
    return( count );
}

static  bool MacGetLine( a_window *wnd, int row, int piece, wnd_line_piece *line )
{
    wnd_macro           *mac;
    cmd_list            *cmds;
    mac_window          *wndmac = WndMac( wnd );
    gui_ctl_id          main_id;
    char                *p;

    if( wndmac->press_key ) {
        if( piece != 0 ) return( false );
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
        switch( row ) {
        case 0:
            line->tabstop = false;
            if( piece >= PIECE_LAST ) return( false );
            line->indent = Indents[ piece ];
            line->text = *Titles[ piece ];
            return( true );
        case 1:
            if( piece != 0 ) return( false );
            SetUnderLine( wnd, line );
            return( true );
        default:
            return( false );
        }
    } else {
        mac = MacGetMacro( row );
        if( mac == NULL )
            return( false );
        line->tabstop = true;
        line->indent = Indents[ piece ];
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
            line->text = *WhatList[ mac->type ];
            return( true );
        case PIECE_TEXT:
            p = TxtBuff;
            switch( mac->type ) {
            case MACRO_COMMAND:
                cmds = mac->cmd;
                line->text = cmds->buff;
                if( line->text[0] == '\0' ) {
                    line->text = LIT_ENG( Quest_Marks );
                }
                break;
            case MACRO_MAIN_MENU:
                if( mac->menu != NULL ) {
                    main_id = mac->menu->id - mac->menu->id % MAIN_MENU_MULT;
                    p = GetMenuLabel( WndNumMenus, WndMainMenu, main_id, TxtBuff, true );
                    *p++ = '/';
                }
                /* fall thru */
            case MACRO_POPUP_MENU:
                if( mac->menu == NULL ) {
                    line->text = LIT_ENG( Quest_Marks );
                } else {
                    line->text = TxtBuff;
                    p = GetMenuLabel( 1, mac->menu, mac->menu->id, p, true );
                }
            }
            return( true );
        }
    }
    return( false );
}


static int MacCompare( void *pa, void *pb )
{
    return( (*(wnd_macro **)pa)->key - (*(wnd_macro **)pb)->key );
}

static void MacReSize( a_window *wnd )
{
    wnd_macro       *mac;
    int             piece;
    gui_ord         size;
    wnd_class_wv    wndclass;
    gui_ord         max[PIECE_LAST];
    int             i;

    for( piece = 0; piece < PIECE_LAST; ++piece ) {
        max[ piece ] = WndExtentX( wnd, *Titles[ piece ] );
    }
    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        size = WndExtentX( wnd, KeyName( mac->key ) );
        if( size < MIN_KEY_SIZE( wnd ) ) size = MIN_KEY_SIZE( wnd );
        if( size > max[ PIECE_KEY ] ) max[ PIECE_KEY ] = size;
    }
    for( i = 0; i < ArraySize( WhatList ); ++i ) {
        size = WndExtentX( wnd, *WhatList[ i ] );
        if( size > max[ PIECE_WHAT ] ) max[ PIECE_WHAT ] = size;
    }
    for( wndclass = 0; wndclass < WND_CURRENT; ++wndclass ) {
        size = WndExtentX( wnd, *WndDisplayNames[wndclass] );
        if( size > max[ PIECE_WHERE ] ) max[ PIECE_WHERE ] = size;
    }
    Indents[ PIECE_KEY ] = WndAvgCharX( wnd );
    Indents[ PIECE_WHERE ] = Indents[ PIECE_KEY ] +
                        max[ PIECE_KEY ] + 2 * WndAvgCharX( wnd );
    Indents[ PIECE_WHAT ] = Indents[ PIECE_WHERE ] +
                        max[ PIECE_WHERE ] + 2 * WndAvgCharX( wnd );
    Indents[ PIECE_TEXT ] =  Indents[ PIECE_WHAT ] +
                        max[ PIECE_WHAT ] + 2 * WndAvgCharX( wnd );
}

static WNDREFRESH MacRefresh;
static void     MacRefresh( a_window *wnd )
{
    MacReSize( wnd );
    WndNoSelect( wnd );
    WndRepaint( wnd );
}

static WNDCALLBACK MacEventProc;
static bool MacEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    mac_window  *wndmac = WndMac( wnd );

    parm=parm;
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
    }
    return( false );
}

wnd_info MacInfo = {
    MacEventProc,
    MacRefresh,
    MacGetLine,
    MacMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    MacModify,
    MacNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_MACRO_CHANGE,
    DefPopUp( MacMenu )
};

extern a_window *WndMacOpen( void )
{
    mac_window  *wndmac;

    wndmac = WndMustAlloc( sizeof( *wndmac ) );
    return( DbgTitleWndCreate( LIT_DUI( WindowAccelerator ), &MacInfo, WND_MACRO, wndmac, &AclIcon,
                          TITLE_SIZE, true ) );
}
