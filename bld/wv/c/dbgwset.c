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
* Description:  Debugger interface configuration.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "dbgdefn.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include "wndregx.h"
#include "dbgio.h"
#include "mad.h"
#include "madcli.h"


extern unsigned int     ScanCmd( char * );
extern void             Scan( void );
extern char             *ScanPos( void );
extern char             *ReScan( char * );
extern bool             ScanEOC( void );
extern bool             ScanItem( bool, char **, size_t * );
extern void             ReqEOC( void );
extern unsigned         SetCurrRadix( unsigned int );
extern char             *GetCmdEntry( char *, int, char * );
extern char             *CnvULongDec( unsigned long, char * );
extern unsigned         ReqExpr( void );
extern unsigned         OptExpr( void );
extern void             WndUserAdd( char *, unsigned int );
extern void             CallSet( void );
extern void             ImplicitSet( void );
extern void             LookSet( void );
extern void             RadixSet( void );
extern void             SourceSet( void );
extern void             LevelSet( void );
extern void             CallConf( void );
extern void             ImplicitConf( void );
extern void             LookConf( void );
extern void             RadixConf( void );
extern void             SourceConf( void );
extern void             LevelConf( void );
extern void             DoConfig( char *, char *, void (**)(), void (**)() );
extern void             ConfigLine( char * );
extern void             WndMenuOn( void );
extern void             WndMenuOff( void );
extern void             LangInit( void );
extern void             LangFini( void );
extern bool             LangLoad( const char *, unsigned );
extern char             *StrCopy( char *, char * );
extern void             WndRedraw( wnd_class );
extern a_window         *WndFindActive( void );
extern cmd_list         *AllocCmdList( char *, size_t );
extern void             FreeCmdList( cmd_list * );
extern char             *Format( char *, char *, ... );
extern void             Recog( unsigned int );
extern void             WndRestoreToFront( a_window* );
extern a_window         *WndFindClass( a_window*, wnd_class );
extern wnd_class        ReqWndName( void );
extern gui_menu_struct  *AddMenuAccel( char *, char *, wnd_class, bool * );
extern void             VarChangeOptions( void );
extern void             AsmChangeOptions( void );
extern void             FuncChangeOptions( void );
extern void             GlobChangeOptions( void );
extern void             ModChangeOptions( void );
extern void             ConfigCmdList( char *cmds, int indent );
extern void             WndDlgTxt( char * );
extern void             WndMenuSetHotKey( gui_menu_struct *, bool, char *);
extern char             *UniqStrAddr( address *addr, char *p ,unsigned );
extern char             *GetCmdName( int );
extern void             RegFindData( mad_type_kind kind, mad_reg_set_data **pdata );
extern mad_handle       FindMAD( char *, unsigned );
extern void             DbgUpdate( update_list );

extern char             OnOffNameTab[];
extern char             *TxtBuff;
extern char             *Language;
extern char             WndNameTab[];
extern tokens           CurrToken;
extern margins          SrcMar;
extern margins          AsmMar;
extern wnd_macro        *WndMacroList;
extern system_config    SysConfig;

static unsigned         TabInterval = 8;


extern void DClickSet( void )
{
    unsigned    new;
    unsigned    old;

    old = SetCurrRadix( 10 );
    new = ReqExpr();
    ReqEOC();
    if( new < MIN_DCLICK || new > MAX_DCLICK ) Error( ERR_NONE, LIT( ERR_BAD_DCLICK ) );
    WndSetDClick( new );
    SetCurrRadix( old );
}


extern void DClickConf( void )
{
    CnvULongDec( WndGetDClick(), TxtBuff );
    ConfigLine( TxtBuff );
}


extern void InputSet( void )
{
    wnd_class   class;
    a_window    *wnd;

    class = ReqWndName();
    ReqEOC();
    wnd = WndFindClass( NULL, class );
    if( wnd == NULL ) {
        GetCmdEntry( WndNameTab, class+1, TxtBuff );
        Error( ERR_NONE, LIT( ERR_WIND_NOT_OPEN ), TxtBuff );
    }
    WndRestoreToFront( wnd );
}


extern void InputConf( void )
{
    a_window  *wnd;

    wnd = WndFindActive();
    if( wnd != NULL && WndHasClass( wnd ) ) {
        GetCmdEntry( WndNameTab, WndClass( wnd )+1, TxtBuff );
        ConfigLine( TxtBuff );
    }
}

static char     *KeyNamePieces[] =
{
    #define pick( x ) #x,
    #include "keynames.h"
    #undef pick
};

typedef enum {
    #define pick( x ) x,
    #include "keynames.h"
    #undef pick
    CTRL        = 0x2000,
    SHIFT       = 0x4000,
    ALT         = 0x8000,
} key_desc;
#define   STATE  (CTRL+ALT+SHIFT)

typedef struct {
    key_desc    desc;
    unsigned    key;
} key_name;

static key_name KeyNames[] = {
    ENTER,              GUI_KEY_ENTER,
    ESCAPE,             GUI_KEY_ESCAPE,
    BACKSPACE,          GUI_KEY_BACKSPACE,
    TAB,                GUI_KEY_TAB,
    CTRL+ENTER,         GUI_KEY_CTRL_ENTER,
    CTRL+BACKSPACE,     GUI_KEY_CTRL_BACKSPACE,
    CTRL+TAB,           GUI_KEY_CTRL_TAB,
    ALT+ENTER,          GUI_KEY_ALT_ENTER,
    ALT+BACKSPACE,      GUI_KEY_ALT_BACKSPACE,
    ALT+TAB,            GUI_KEY_ALT_TAB,
    SHIFT+TAB,          GUI_KEY_SHIFT_TAB,
    INSERT,             GUI_KEY_INSERT,
    DELETE,             GUI_KEY_DELETE,
    HOME,               GUI_KEY_HOME,
    END,                GUI_KEY_END,
    PAGEUP,             GUI_KEY_PAGEUP,
    PAGEDOWN,           GUI_KEY_PAGEDOWN,
    UP,                 GUI_KEY_UP,
    DOWN,               GUI_KEY_DOWN,
    LEFT,               GUI_KEY_LEFT,
    RIGHT,              GUI_KEY_RIGHT,
    CTRL+INSERT,        GUI_KEY_CTRL_INSERT,
    CTRL+DELETE,        GUI_KEY_CTRL_DELETE,
    CTRL+HOME,          GUI_KEY_CTRL_HOME,
    CTRL+END,           GUI_KEY_CTRL_END,
    CTRL+PAGEUP,        GUI_KEY_CTRL_PAGEUP,
    CTRL+PAGEDOWN,      GUI_KEY_CTRL_PAGEDOWN,
    CTRL+UP,            GUI_KEY_CTRL_UP,
    CTRL+DOWN,          GUI_KEY_CTRL_DOWN,
    CTRL+LEFT,          GUI_KEY_CTRL_LEFT,
    CTRL+RIGHT,         GUI_KEY_CTRL_RIGHT,
    F1,                 GUI_KEY_F1,
    F2,                 GUI_KEY_F2,
    F3,                 GUI_KEY_F3,
    F4,                 GUI_KEY_F4,
    F5,                 GUI_KEY_F5,
    F6,                 GUI_KEY_F6,
    F7,                 GUI_KEY_F7,
    F8,                 GUI_KEY_F8,
    F9,                 GUI_KEY_F9,
    F10,                GUI_KEY_F10,
    F11,                GUI_KEY_F11,
    F12,                GUI_KEY_F12,
    ALT+F1,             GUI_KEY_ALT_F1,
    ALT+F2,             GUI_KEY_ALT_F2,
    ALT+F3,             GUI_KEY_ALT_F3,
    ALT+F4,             GUI_KEY_ALT_F4,
    ALT+F5,             GUI_KEY_ALT_F5,
    ALT+F6,             GUI_KEY_ALT_F6,
    ALT+F7,             GUI_KEY_ALT_F7,
    ALT+F8,             GUI_KEY_ALT_F8,
    ALT+F9,             GUI_KEY_ALT_F9,
    ALT+F10,            GUI_KEY_ALT_F10,
    ALT+F11,            GUI_KEY_ALT_F11,
    ALT+F12,            GUI_KEY_ALT_F12,
    CTRL+F1,            GUI_KEY_CTRL_F1,
    CTRL+F2,            GUI_KEY_CTRL_F2,
    CTRL+F3,            GUI_KEY_CTRL_F3,
    CTRL+F4,            GUI_KEY_CTRL_F4,
    CTRL+F5,            GUI_KEY_CTRL_F5,
    CTRL+F6,            GUI_KEY_CTRL_F6,
    CTRL+F7,            GUI_KEY_CTRL_F7,
    CTRL+F8,            GUI_KEY_CTRL_F8,
    CTRL+F9,            GUI_KEY_CTRL_F9,
    CTRL+F10,           GUI_KEY_CTRL_F10,
    CTRL+F11,           GUI_KEY_CTRL_F11,
    CTRL+F12,           GUI_KEY_CTRL_F12,
    SHIFT+F1,           GUI_KEY_SHIFT_F1,
    SHIFT+F2,           GUI_KEY_SHIFT_F2,
    SHIFT+F3,           GUI_KEY_SHIFT_F3,
    SHIFT+F4,           GUI_KEY_SHIFT_F4,
    SHIFT+F5,           GUI_KEY_SHIFT_F5,
    SHIFT+F6,           GUI_KEY_SHIFT_F6,
    SHIFT+F7,           GUI_KEY_SHIFT_F7,
    SHIFT+F8,           GUI_KEY_SHIFT_F8,
    SHIFT+F9,           GUI_KEY_SHIFT_F9,
    SHIFT+F10,          GUI_KEY_SHIFT_F10,
    SHIFT+F11,          GUI_KEY_SHIFT_F11,
    SHIFT+F12,          GUI_KEY_SHIFT_F12,
    0,          0
};

typedef struct {
    char        name;
    unsigned    key;
} alt_key_name;

static alt_key_name AltKeyNames[] = {
        '`',    GUI_KEY_ALT_BACKQUOTE,
        '1',    GUI_KEY_ALT_1,
        '2',    GUI_KEY_ALT_2,
        '3',    GUI_KEY_ALT_3,
        '4',    GUI_KEY_ALT_4,
        '5',    GUI_KEY_ALT_5,
        '6',    GUI_KEY_ALT_6,
        '7',    GUI_KEY_ALT_7,
        '8',    GUI_KEY_ALT_8,
        '9',    GUI_KEY_ALT_9,
        '0',    GUI_KEY_ALT_0,
        '-',    GUI_KEY_ALT_MINUS,
        '=',    GUI_KEY_ALT_EQUAL,
        '[',    GUI_KEY_ALT_LEFT_BRACKET,
        ']',    GUI_KEY_ALT_RIGHT_BRACKET,
        '\\',   GUI_KEY_ALT_BACKSLASH,
        ';',    GUI_KEY_ALT_SEMICOLON,
        '\'',   GUI_KEY_ALT_QUOTE,
        ',',    GUI_KEY_ALT_COMMA,
        '.',    GUI_KEY_ALT_PERIOD,
        '/',    GUI_KEY_ALT_SLASH,
        0,      0
};

static alt_key_name CtrlKeyNames[] = {
        'a',    GUI_KEY_CTRL_A,
        'b',    GUI_KEY_CTRL_B,
        'c',    GUI_KEY_CTRL_C,
        'd',    GUI_KEY_CTRL_D,
        'e',    GUI_KEY_CTRL_E,
        'f',    GUI_KEY_CTRL_F,
        'g',    GUI_KEY_CTRL_G,
        'h',    GUI_KEY_CTRL_H,
        'i',    GUI_KEY_CTRL_I,
        'j',    GUI_KEY_CTRL_J,
        'k',    GUI_KEY_CTRL_K,
        'l',    GUI_KEY_CTRL_L,
        'm',    GUI_KEY_CTRL_M,
        'n',    GUI_KEY_CTRL_N,
        'o',    GUI_KEY_CTRL_O,
        'p',    GUI_KEY_CTRL_P,
        'q',    GUI_KEY_CTRL_Q,
        'r',    GUI_KEY_CTRL_R,
        's',    GUI_KEY_CTRL_S,
        't',    GUI_KEY_CTRL_T,
        'u',    GUI_KEY_CTRL_U,
        'v',    GUI_KEY_CTRL_V,
        'w',    GUI_KEY_CTRL_W,
        'x',    GUI_KEY_CTRL_X,
        'y',    GUI_KEY_CTRL_Y,
        'z',    GUI_KEY_CTRL_Z,
        '2',    GUI_KEY_CTRL_2,
        '\\',   GUI_KEY_CTRL_BACKSLASH,
        '[',    GUI_KEY_CTRL_LEFT_BRACKET,
        ']',    GUI_KEY_CTRL_RIGHT_BRACKET,
        '6',    GUI_KEY_CTRL_6,
        '-',    GUI_KEY_CTRL_MINUS,
        0,      0
};

#define STR_CTRL        "CTRL-"
#define STR_SHIFT       "SHIFT-"
#define STR_ALT         "ALT-"

char LookUpCtrlKey( unsigned key )
{
    alt_key_name        *alt;

    for( alt = CtrlKeyNames; alt->name != 0; ++alt ) {
        if( alt->key == key ) break;
    }
    return( alt->name );
}

static char *AddOn( char *buff, key_desc desc )
{
    switch( desc & STATE ) {
    case CTRL:
        return( StrCopy( STR_CTRL, buff ) );
    case SHIFT:
        return( StrCopy( STR_SHIFT, buff ) );
    case ALT:
        return( StrCopy( STR_ALT, buff ) );
    default:
        return( buff );
    }
}


char *KeyName( unsigned key )
{
    static char         buff[20];
    key_name            *k;
    char                *p;
    alt_key_name        *alt;

    for( k = KeyNames; k->key != 0; ++k ) {
        if( k->key == key ) {
            p = AddOn( buff, k->desc );
            StrCopy( KeyNamePieces[ k->desc & ~STATE ], p );
            return( buff );
        }
    }
    if( key <= 255 && isprint( key ) ) {
        buff[0] = key;
        buff[1] = '\0';
        return( buff );
    }
    for( alt = CtrlKeyNames; alt->name != 0; ++alt ) {
        if( alt->key == key ) {
            p = AddOn( buff, CTRL );
            *p++ = alt->name;
            *p = '\0';
            return( buff );
        }
    }
    for( alt = AltKeyNames; alt->name != 0; ++alt ) {
        if( alt->key == key ) {
            p = AddOn( buff, ALT );
            *p++ = alt->name;
            *p = '\0';
            return( buff );
        }
    }
    return( NULL );
}

static key_desc StripOff( char **start, unsigned *len,
                          char *test, key_desc desc )
{
    int         tlen;

    tlen = strlen( test );
    if( strnicmp( *start, test, tlen ) == 0 ) {
        *start += tlen;
        *len -= tlen;
        return( desc );
    }
    return( 0 );
}


static unsigned MapKey( char *start, unsigned len )
{
    key_name            *k;
    key_desc            desc;
    int                 i;
    alt_key_name        *alt;
    char                ch;

    if( len == 1 ) {
        return( *start );
    }
    desc  = StripOff( &start, &len, STR_CTRL,  CTRL );
    desc |= StripOff( &start, &len, STR_SHIFT, SHIFT );
    desc |= StripOff( &start, &len, STR_ALT,   ALT );
    if( len == 1 ) {
        alt = NULL;
        if( desc == CTRL ) {
            alt = CtrlKeyNames;
        }
        if( desc == ALT ) {
            alt = AltKeyNames;
        }
        if( alt != NULL ) {
            ch = tolower( *start );
            while( alt->name != 0 ) {
                if( alt->name == ch ) {
                    return( alt->key );
                }
                ++alt;
            }
        }
    }
    for( i = 0; i < ArraySize( KeyNamePieces ); ++i ) {
        if( strnicmp( KeyNamePieces[i], start, len ) == 0 ) {
            if( strlen( KeyNamePieces[i] ) == len ) {
                desc += i;
                for( k = KeyNames; k->key != 0; ++k ) {
                    if( k->desc == desc ) return( k->key );
                }
                return( 0 );
            }
        }
    }
    return( 0 );
}


wnd_macro *MacAddDel( unsigned key, wnd_class class, cmd_list *cmds )
{
    wnd_macro           **owner,*curr;
    bool                is_main;

    owner = &WndMacroList;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        if( curr->key == key && curr->class == class ) break;
        owner = &curr->link;
    }
    if( cmds != NULL ) {
        if( curr == NULL ) {
            _Alloc( curr, sizeof( wnd_macro ) );
            if( curr == NULL ) {
                FreeCmdList( cmds );
                Error( ERR_NONE, LIT( ERR_NO_MEMORY ) );
            }
            curr->key = key;
            curr->class = class;
            curr->link = NULL;
            curr->menu = NULL;
            *owner = curr;
        } else {
            WndMenuSetHotKey( curr->menu, curr->type == MACRO_MAIN_MENU, LIT( Empty ) );
            FreeCmdList( curr->cmd );
        }
        curr->cmd = cmds;
        curr->menu = AddMenuAccel( KeyName( key ), cmds->buff, class, &is_main );
        if( curr->menu == NULL ) {
            curr->type = MACRO_COMMAND;
        } else if( is_main ) {
            curr->type = MACRO_MAIN_MENU;
        } else {
            curr->type = MACRO_POPUP_MENU;
        }
    } else {
        if( curr != NULL ) {
            *owner = curr->link;
            WndMenuSetHotKey( curr->menu, curr->type == MACRO_MAIN_MENU, LIT( Empty ) );
            FreeCmdList( curr->cmd );
            _Free( curr );
        }
    }
    DbgUpdate( UP_MACRO_CHANGE );
    return( curr );
}


extern void MacroSet( void )
{
    wnd_class   class;
    cmd_list    *cmds;
    unsigned    key;
    char        *start;
    size_t      len;
    bool        scanned;
    char        *p,*q;
    int         i;


    class = ReqWndName();
    key = 0;
    if( ScanItem( TRUE, &start, &len ) ) {
        key = MapKey( start, len );
    }
    if( key == 0 ) Error( ERR_NONE, LIT( ERR_MACRO_NOT_VALID ) );
    scanned = ScanItem( TRUE, &start, &len );
    ReqEOC();
    if( scanned ) {
        p = TxtBuff;
        q = start;
        i = len;
        while( isspace( *q ) ) {
            ++q;
            --i;
        }
        while( --i >= 0 ) {
            if( *q != '\r' ) *p++ = *q;
            ++q;
        }
        *p = '\0';
        cmds = AllocCmdList( TxtBuff, strlen( TxtBuff ) );
    } else {
        cmds = NULL;
    }
    MacAddDel( key, class, cmds );
}

extern  void    MacroConf( void )
{
    char        wnd_name[20];
    wnd_macro     *mac;
    char        *fmt;

    for( mac = WndMacroList; mac != NULL; mac = mac->link ) {
        GetCmdEntry( WndNameTab, mac->class+1, wnd_name );
        if( TxtBuff[ 0 ] == NULLCHAR ) break;
        fmt = isspace( mac->key ) ? "%s {%s} {" : "%s %s {";
        Format( TxtBuff, fmt, wnd_name, KeyName( mac->key ) );
        ConfigLine( TxtBuff );
        ConfigCmdList( ((cmd_list*)mac->cmd)->buff, 0 );
        WndDlgTxt( "}" );
    }
}

extern  void    FiniMacros( void )
{
    wnd_macro   *mac;
    wnd_macro   *junk;

    mac = WndMacroList;
    while( mac != NULL ) {
        junk = mac;
        WndMenuSetHotKey( mac->menu, mac->type == MACRO_MAIN_MENU, LIT( Empty ) );
        mac = mac->link;
        FreeCmdList( junk->cmd );
        _Free( junk );
    }
    WndMacroList = NULL;
}


int TabIntervalGet( void )
{
    return( TabInterval );
}

void TabIntervalSet( int new )
{
    TabInterval = new;
    WndRedraw( WND_FILE );
    WndRedraw( WND_SOURCE );
}

extern void TabSet( void )
{
    int      new;
    unsigned    old;

    old = SetCurrRadix( 10 );
    new = ReqExpr();
    ReqEOC();
    if( new < 0 || new > MAX_TAB ) Error( ERR_NONE, LIT( ERR_BAD_TAB ) );
    TabIntervalSet( new );
    SetCurrRadix( old );
}


extern void TabConf( void )
{
    CnvULongDec( TabInterval, TxtBuff );
    ConfigLine( TxtBuff );
}

static char SearchSettings[] = {
    "CASEIgnore\0"
    "CASEREspect\0"
    "Rx\0"
    "NORx\0"
};
enum {
    SEARCH_IGNORE = 1,
    SEARCH_RESPECT,
    SEARCH_RX,
    SEARCH_NORX
};


extern void SearchSet( void )
{
    char        *start;
    size_t      len;

    while( CurrToken == T_DIV ) {
        Scan();
        switch( ScanCmd( SearchSettings ) ) {
        case SEARCH_IGNORE:
            WndSetSrchIgnoreCase( TRUE );
            break;
        case SEARCH_RESPECT:
            WndSetSrchIgnoreCase( FALSE );
            break;
        case SEARCH_RX:
            WndSetSrchRX( TRUE );
            break;
        case SEARCH_NORX:
            WndSetSrchRX( FALSE );
            break;
        default:
            Error( ERR_LOC, LIT( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
            break;
        }
    }
    if( ScanItem( TRUE, &start, &len ) ) {
        char    c = start[len];
        start[len] = '\0';
        WndSetSrchMagicChars( start );
        start[len] = c;
    }
    ReqEOC();
}


extern void SearchConf( void )
{
    char        *ptr;

    ptr = TxtBuff;
    *ptr++ = '/';
    ptr = GetCmdEntry( SearchSettings,
                       WndGetSrchIgnoreCase() ? SEARCH_IGNORE : SEARCH_RESPECT, ptr );
    *ptr++ = '/';
    ptr = GetCmdEntry( SearchSettings,
                       WndGetSrchRX() ? SEARCH_RX : SEARCH_NORX, ptr );
    ptr = StrCopy( " {", ptr );
    ptr = StrCopy( WndGetSrchMagicChars(), ptr );
    ptr = StrCopy( "}", ptr );
    ConfigLine( TxtBuff );
}
