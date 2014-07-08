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


#include "dbgdefn.h"
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "dbgwind.h"
#include "dbginfo.h"
#include "dbgadget.h"
#include "guidlg.h"
#include "sortlist.h"
#include "dbgerr.h"
#include "dbgtoggl.h"

extern char             *GetCmdEntry(char *,int ,char *);
extern char             *KeyName( unsigned key );
extern wnd_macro        *MacAddDel( unsigned key, wnd_class class, cmd_list *cmds );
extern char             *StrCopy( char *src, char *dest );
extern void             LockCmdList( cmd_list *cmds );
extern char             *Format( char *buff, char *fmt, ... );
extern char             *GetCmdName( int index );
extern char             *GetMenuLabel( unsigned size, gui_menu_struct *menu, unsigned id, char *buff, bool strip_amp );
extern cmd_list         *AllocCmdList( char *start, size_t );
extern void             FreeCmdList(cmd_list *);
extern void             FiniMacros( void );
extern void             Invoke(char *,int ,char_ring *);
extern char             *ReScan( char * );
extern void             SetUnderLine( a_window *, wnd_line_piece * );

extern char             MainTab[];
extern char             *TxtBuff;
extern wnd_info         *WndInfoTab[];
extern gui_menu_struct  WndMainMenu[];
extern int              WndNumMenus;

static char **WndDisplayNames[] = {
    #define pick( a,b,c,d,e,f ) LITREF( f ),
    #include "wndnames.h"
    #undef pick
};

static char **WhatList[] = {
    LITREF( mac_popup_menu ),
    LITREF( mac_main_menu ),
    LITREF( mac_command_string ),
};

#define TITLE_SIZE      2
static char **Titles[] = { LITREF( Key_Name ), LITREF( Mac_Window ),
                         LITREF( Macro_Type ), LITREF( Definition ) };

enum {
    PIECE_KEY,
    PIECE_WHERE,
    PIECE_WHAT,
    PIECE_TEXT,
    PIECE_LAST
};

#define MIN_KEY_SIZE( wnd ) (8*WndAvgCharX(wnd)) // something big enough to click on

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


static DLGPICKTEXT WndGetName;
static char *WndGetName( void *from, int i )
{
    return( *((char ***)from)[i] );
}

#ifdef DEADCODE
wnd_macro *MacFindMac( unsigned key, wnd_class class )
{
    wnd_macro   *mac;

    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        if( mac->key == key && mac->class == class ) return( mac );
    }
    return( NULL );
}
#endif

static wnd_macro *MacGetMacro( int row )
{
    wnd_macro   *mac;
    int         count;

    count = 0;
    mac = WndMacroList;
    if( row < 0 ) return( NULL );
    for( ;; ) {
        if( mac == NULL ) break;
        if( count == row ) break;
        ++count;
        mac = mac->link;
    }
    return( mac );
}


static void MacChangeMac( a_window *wnd, wnd_macro *mac, unsigned key,
                          wnd_class class, wnd_row row )
{
    cmd_list    *cmds;
    wnd_macro   **owner,*curr;
    wnd_row     i;

    cmds = mac->cmd;
    LockCmdList( cmds );
    MacAddDel( mac->key, mac->class, NULL );
    curr = MacAddDel( key, class, cmds );
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
    wnd_macro           *mac = MacGetMacro( row );

    wnd=wnd;
    old = mac->type;
    if( mac->class == WND_ALL ) {
        new = DlgPickWithRtn( LIT( Macro_Type ), WhatList+1,
                       mac->type == MACRO_COMMAND, WndGetName, ArraySize( WhatList )-1 );
        if( new != -1 ) ++new;
    } else {
        new = DlgPickWithRtn( LIT( Macro_Type ), WhatList,
                       mac->type, WndGetName, ArraySize( WhatList ) );
    }
    if( new != -1 ) {
        mac->type = new;
        if( new != old ) {
            MacModify( wnd, row, PIECE_TEXT );
        }
        return( TRUE );
    } else {
        return( FALSE );
    }
}


bool MacKeyHit( a_window *wnd, unsigned key )
{
    mac_window  *wndmac = WndMac( wnd );
    wnd_macro   *mac,*curr;
    wnd_row     row;
    int         new;

    if( wndmac->press_key ) {
        if( KeyName( key ) == NULL ) {
            Ring();
            return( TRUE );
        }
        wndmac->press_key = FALSE;
        WndZapped( wnd );
        if ( key == GUI_KEY_ESCAPE ) return( TRUE );
        if( wndmac->changing ) {
            wndmac->changing = FALSE;
            mac = MacGetMacro( wndmac->change_row );
            MacChangeMac( wnd, mac, key, mac->class, wndmac->change_row );
        } else if( wndmac->creating ) {
            wndmac->creating = FALSE;
            new = DlgPickWithRtn( LIT( Enter_Window ), WndDisplayNames, WND_ALL, WndGetName, WND_CURRENT );
            if( new == -1 ) return( TRUE );
            curr = MacAddDel( key, new, AllocCmdList( LIT( Quest_Marks ),
                             strlen( LIT( Quest_Marks ) ) ) );
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
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static WNDCLICKHOOK MacPopupClicked;
static bool MacPopupClicked( a_window *wnd, unsigned id )
{
    char                *p;
    int                 main_id;
    mac_window          *wndmac;
    cmd_list            *cmds;

    WndInstallClickHook( NULL );
    if( wnd == NULL || WndClass( wnd ) != WND_MACRO ) return( FALSE );
    wndmac = WndMac( wnd );
    if( wndmac->mac == NULL ) return( FALSE );
    p = StrCopy( GetCmdName( CMD_ACCEL ), TxtBuff );
    *p++ = ' ';
    if( wndmac->mac->type == MACRO_MAIN_MENU ) {
        p = GetCmdEntry( MainTab, 1, p );
        *p++ = ' ';
        main_id = id - id % MAIN_MENU_MULT;
        *p++ = '{';
        p = GetMenuLabel( wndmac->size, wndmac->menu, main_id, p, FALSE );
        *p++ = '}';
        *p++ = ' ';
        *p++ = '{';
        p = GetMenuLabel( wndmac->size, wndmac->menu, id, p, FALSE );
    } else {
        *p++ = '{';
        p = GetMenuLabel( wndmac->size, wndmac->menu, id, p, FALSE );
    }
    if( p != NULL ) {
        *p++ = '}';
        *p++ = '\0';
        cmds = AllocCmdList( TxtBuff, p-TxtBuff );
        MacAddDel( wndmac->mac->key, wndmac->mac->class, cmds );
    }
    return( TRUE );
}


static void MacModMenu( a_window *wnd, wnd_row row )
{
    wnd_info            *info;
    gui_point           point;
    int                 dummy;
    mac_window          *wndmac= WndMac( wnd );
    wnd_macro           *mac = MacGetMacro( row );

    info = WndInfoTab[ mac->class ];
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
        WndChangeMenuAll( info->popupmenu, info->num_popups, FALSE, GUI_GRAYED );
        WndCreateFloatingPopup( wnd, &point, wndmac->size, wndmac->menu, &dummy );
    }
}

static void MacModWhere( a_window *wnd, wnd_row row )
{
    int                 new;
    wnd_macro           *mac = MacGetMacro( row );
    wnd_class           def;

    wnd=wnd;
    def = mac->class == WND_NO_CLASS ? WND_ALL : mac->class;
    new = DlgPickWithRtn( LIT( Enter_Window ), WndDisplayNames, def, WndGetName, WND_CURRENT );
    if( new == -1 ) return;
//    WndRepaint( wnd );
    MacChangeMac( wnd, mac, mac->key, new, row );
}

static void MacModKey( a_window *wnd, wnd_row row )
{
    mac_window          *wndmac= WndMac( wnd );

    wndmac->press_key = TRUE;
    wndmac->changing = TRUE;
    wndmac->change_row = row;
    WndZapped( wnd );
}


static void MacModCmd( a_window *wnd, wnd_row row )
{
    cmd_list    *cmds;
    wnd_macro           *mac = MacGetMacro( row );
//    char        *p;

    wnd=wnd;mac=mac;
    cmds = mac->cmd;
//    p = StrCopy( cmds->buff, TxtBuff );
    StrCopy( cmds->buff, TxtBuff );
    if( DlgNew( LIT( Enter_New_Command ), TxtBuff, TXT_LEN ) ) {
        cmds = AllocCmdList( TxtBuff, strlen( TxtBuff ) );
        FreeCmdList( mac->cmd );
        mac->cmd = cmds;
    }
}


static  WNDMODIFY       MacModify;
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
static  WNDMENU MacMenuItem;
static void     MacMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    wnd_macro           *mac;
    mac_window          *wndmac = WndMac( wnd );
    char                *old;

    wnd=wnd;
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
        WndMenuEnable( wnd, MENU_MAC_CREATE_NEW, TRUE );
        WndMenuEnable( wnd, MENU_MAC_WD, TRUE );
        WndMenuEnable( wnd, MENU_MAC_TD, TRUE );
        break;
    case MENU_MAC_MODIFY:
        MacModify( wnd, row, piece );
        break;
    case MENU_MAC_CREATE_NEW:
        wndmac->press_key = TRUE;
        wndmac->creating = TRUE;
        WndZapped( wnd );
        break;
    case MENU_MAC_DELETE:
        MacAddDel( mac->key, mac->class, NULL );
        break;
    case MENU_MAC_TD:
        FiniMacros();
        old = ReScan( LIT( Empty ) );
        Invoke( TDDBG, strlen( TDDBG ), NULL );
        ReScan( old );
        break;
    case MENU_MAC_WD:
        FiniMacros();
        old = ReScan( LIT( Empty ) );
        Invoke( WDDBG, strlen( WDDBG ), NULL );
        ReScan( old );
        break;
    default:
        break;
    }
}

static WNDNUMROWS MacNumRows;
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

static WNDGETLINE MacGetLine;
static  bool    MacGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    wnd_macro           *mac;
    cmd_list            *cmds;
    mac_window          *wndmac = WndMac( wnd );
    int                 main_id;
    char                *p;

    if( wndmac->press_key ) {
        if( piece != 0 ) return( FALSE );
        line->tabstop = FALSE;
        if( row > WndTop( wnd ) + WndRows( wnd ) - 1 ) {
            return( FALSE );
        } else if( row == WndTop( wnd ) + WndRows( wnd ) / 2 ) {
            if( wndmac->changing ) {
                line->text = LIT( Redefine_Key );
            } else {
                line->text = LIT( Press_the_Key );
            }
        } else {
            line->text = " ";
        }
        return( TRUE );
    }
    if( row < 0 ) {
        row += TITLE_SIZE;
        switch( row ) {
        case 0:
            line->tabstop = FALSE;
            if( piece >= PIECE_LAST ) return( FALSE );
            line->indent = Indents[ piece ];
            line->text = *Titles[ piece ];
            return( TRUE );
        case 1:
            if( piece != 0 ) return( FALSE );
            SetUnderLine( wnd, line );
            return( TRUE );
        default:
            return( FALSE );
        }
    } else {
        mac = MacGetMacro( row );
        if( mac == NULL ) return( FALSE );
        line->tabstop = TRUE;
        line->indent = Indents[ piece ];
        switch( piece ) {
        case PIECE_KEY:
            p = StrCopy( KeyName( mac->key ), TxtBuff );
            while( WndExtentX( wnd, TxtBuff ) < MIN_KEY_SIZE( wnd ) ) {
                p = StrCopy( " ", p );
            }
            line->text = TxtBuff;
            return( TRUE );
        case PIECE_WHERE:
            line->text = TxtBuff;
            StrCopy( *WndDisplayNames[mac->class], TxtBuff );
            return( TRUE );
        case PIECE_WHAT:
            line->text = *WhatList[ mac->type ];
            return( TRUE );
        case PIECE_TEXT:
            p = TxtBuff;
            switch( mac->type ) {
            case MACRO_COMMAND:
                cmds = mac->cmd;
                line->text = cmds->buff;
                if( line->text[0] == '\0' ) {
                    line->text = LIT( Quest_Marks );
                }
                break;
            case MACRO_MAIN_MENU:
                if( mac->menu != NULL ) {
                    main_id = mac->menu->id - mac->menu->id % MAIN_MENU_MULT;
                    p = GetMenuLabel( WndNumMenus, WndMainMenu, main_id, TxtBuff, TRUE );
                    *p++ = '/';
                }
                /* fall thru */
            case MACRO_POPUP_MENU:
                if( mac->menu == NULL ) {
                    line->text = LIT( Quest_Marks );
                } else {
                    line->text = TxtBuff;
                    p = GetMenuLabel( 1, mac->menu, mac->menu->id, p, TRUE );
                }
            }
            return( TRUE );
        }
    }
    return( FALSE );
}


static int MacCompare( void *pa, void *pb )
{
    return( (*(wnd_macro **)pa)->key - (*(wnd_macro **)pb)->key );
}

static void MacReSize( a_window *wnd )
{
    wnd_macro   *mac;
    int         piece;
    gui_ord     size;
    wnd_class   class;
    gui_ord     max[PIECE_LAST];
    int         i;

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
    for( class = 0; class < WND_CURRENT; ++class ) {
        size = WndExtentX( wnd, *WndDisplayNames[ class ] );
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
        return( TRUE );
    case GUI_RESIZE:
        MacReSize( wnd );
        return( TRUE );
    case GUI_DESTROY:
        WndFree( wndmac );
    }
    return( FALSE );
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
    UP_MACRO_CHANGE,
    DefPopUp( MacMenu )
};

extern WNDOPEN WndMacOpen;
extern a_window *WndMacOpen()
{
    mac_window  *wndmac;

    wndmac = WndMustAlloc( sizeof( *wndmac ) );
    return( DbgTitleWndCreate( LIT( WindowAccelerator ), &MacInfo, WND_MACRO, wndmac, &AclIcon,
                          TITLE_SIZE, TRUE ) );
}
