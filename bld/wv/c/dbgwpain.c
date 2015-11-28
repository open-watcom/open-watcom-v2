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
* Description:  Paint debugger windows.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgmain.h"
#include "dbgshow.h"
#include "dbgwdlg.h"
#include "dbgwdisp.h"
#include "dbgwpain.h"


extern const char       WndNameTab[];

static gui_colour_set   *WndClassColour[WND_NUM_CLASSES];
gui_colour_set          WndStatusColour = { GUI_BRIGHT_WHITE, GUI_BLUE };

wnd_attr WndPlainAttr    = WND_PLAIN;
wnd_attr WndSelectedAttr = WND_SELECTED;
wnd_attr WndCursorAttr   = WND_SELECTED;

static gui_colour_set   WndDlgColours[] = {
    #define pick(e,f,b)     { f, b },
    #include "attrdlg.h"
    #undef pick
};

gui_colour_set WndColours[] = {
    { GUI_WHITE,        GUI_BLUE },             /* GUI_MENU_PLAIN    */
    { GUI_BRIGHT_WHITE, GUI_BLUE },             /* GUI_MENU_STANDOUT */
    { GUI_GREY,         GUI_BLUE },             /* GUI_MENU_GRAYED */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* GUI_MENU_ACTIVE    */
    { GUI_BRIGHT_YELLOW,GUI_BLACK },            /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* GUI_BACKGROUND */
    { GUI_BRIGHT_YELLOW,GUI_BLUE },             /* GUI_MENU_FRAME */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_TITLE_INACTIVE    */
    { GUI_BLUE,         GUI_CYAN },             /* GUI_FRAME_ACTIVE    */
    { GUI_GREY,         GUI_BRIGHT_WHITE },     /* GUI_FRAME_INACTIVE    */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* GUI_ICON    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_MENU_GRAYED_ACTIVE */
    { GUI_GREY,         GUI_CYAN },             /* GUI_FRAME_RESIZE    */
    { GUI_GREY,         GUI_BLACK },            /* GUI_CONTROL_BACKGROUND */
    { GUI_BLACK,        GUI_BRIGHT_WHITE },     /* WND_PLAIN    */
    { GUI_BRIGHT_WHITE, GUI_BLACK },            /* WND_TABSTOP == ACTIVE  */
    { GUI_BRIGHT_WHITE, GUI_RED },              /* WND_SELECTED */
    { GUI_RED,          GUI_BRIGHT_WHITE },     /* WND_STANDOUT */
    { GUI_RED,          GUI_BRIGHT_WHITE },     /* WND_HOTSPOT */
    { GUI_BRIGHT_RED,   GUI_BLACK },            /* WND_STANDOUT_TABSTOP */
};
int WndNumColours = { ArraySize( WndColours ) };


typedef enum {
    ATTR_ACTIVE         = 0x0001,
    ATTR_SELECTED       = 0x0002,
    ATTR_STANDOUT       = 0x0004,
    ATTR_DISABLED       = 0x0008,
    ATTR_PLAIN          = 0x0010,
    ATTR_FRAME          = 0x0020,
    ATTR_ICON           = 0x0040,
    ATTR_MENU           = 0x0080,
    ATTR_TITLE          = 0x0100,
    ATTR_BUTTON         = 0x0200,
    ATTR_SHADOW         = 0x0400,
} attr_bits;

static const char AttrNameTab[] = {
    "Active\0"
    "SElected\0"
    "STandout\0"
    "Disabled\0"
    "Plain\0"
    "Frame\0"
    "ICon\0"
    "MEnu\0"
    "TItle\0"
    "BUtton\0"
    "SHadow\0"
};

static attr_bits AttrBits[] = {
    ATTR_ACTIVE,
    ATTR_SELECTED,
    ATTR_STANDOUT,
    ATTR_DISABLED,
    ATTR_PLAIN,
    ATTR_FRAME,
    ATTR_ICON,
    ATTR_MENU,
    ATTR_TITLE,
    ATTR_BUTTON,
    ATTR_SHADOW,
};

typedef struct {
    int             attr;
    attr_bits       bits;
} attr_map;

static attr_map AttrMap[] = {
    { GUI_MENU_PLAIN,           ATTR_MENU+ATTR_PLAIN },
    { GUI_MENU_STANDOUT,        ATTR_MENU+ATTR_STANDOUT },
    { GUI_MENU_GRAYED,          ATTR_MENU+ATTR_DISABLED },
    { GUI_MENU_ACTIVE,          ATTR_MENU+ATTR_ACTIVE },
    { GUI_MENU_ACTIVE_STANDOUT, ATTR_MENU+ATTR_ACTIVE+ATTR_STANDOUT },
    { GUI_MENU_FRAME,           ATTR_FRAME+ATTR_MENU },
    { GUI_TITLE_INACTIVE,       ATTR_TITLE+ATTR_DISABLED },
    { GUI_FRAME_ACTIVE,         ATTR_FRAME+ATTR_ACTIVE },
    { GUI_FRAME_INACTIVE,       ATTR_FRAME+ATTR_DISABLED },
    { GUI_ICON,                 ATTR_ICON },
    { GUI_MENU_GRAYED_ACTIVE,   ATTR_MENU+ATTR_DISABLED+ATTR_ACTIVE },
    { WND_PLAIN,                ATTR_PLAIN },
    { WND_TABSTOP,              ATTR_ACTIVE },
    { WND_SELECTED,             ATTR_SELECTED },
    { WND_STANDOUT,             ATTR_STANDOUT },
    { WND_HOTSPOT,              ATTR_BUTTON },
    { WND_STANDOUT_TABSTOP,     ATTR_ACTIVE+ATTR_STANDOUT },
};

static attr_map DlgAttrMap[] = {
    { GUI_DLG_NORMAL,                   ATTR_PLAIN },
    { GUI_DLG_FRAME,                    ATTR_FRAME },
    { GUI_DLG_SHADOW,                   ATTR_SHADOW },
    { GUI_DLG_BUTTON_PLAIN,             ATTR_BUTTON+ATTR_PLAIN },
    { GUI_DLG_BUTTON_STANDOUT,          ATTR_BUTTON+ATTR_STANDOUT },
    { GUI_DLG_BUTTON_ACTIVE,            ATTR_BUTTON+ATTR_ACTIVE },
    { GUI_DLG_BUTTON_ACTIVE_STANDOUT,   ATTR_BUTTON+ATTR_ACTIVE+ATTR_STANDOUT },
};

typedef enum {
    CLR_NONE = 0,
    CLR_BLACK           = 0x0001,
    CLR_BLUE            = 0x0002,
    CLR_BRIGHT          = 0x0004,
    CLR_BROWN           = 0x0008,
    CLR_CYAN            = 0x0010,
    CLR_GRAY            = 0x0020,
    CLR_GREEN           = 0x0040,
    CLR_GREY            = 0x0080,
    CLR_MAGENTA         = 0x0100,
    CLR_RED             = 0x0200,
    CLR_WHITE           = 0x0400,
    CLR_YELLOW          = 0x0800,
} colour_bits;

static const char ColourNameTab[] = {
    "BLAck\0"
    "BLUe\0"
    "BRIght\0"
    "BROwn\0"
    "Cyan\0"
    "GRAy\0"
    "GREEn\0"
    "GREY\0"
    "MAgenta\0"
    "Red\0"
    "White\0"
    "Yellow\0"
};

static colour_bits ColourBits[] = {
    CLR_BLACK,
    CLR_BLUE,
    CLR_BRIGHT,
    CLR_BROWN,
    CLR_CYAN,
    CLR_GRAY,
    CLR_GREEN,
    CLR_GREY,
    CLR_MAGENTA,
    CLR_RED,
    CLR_WHITE,
    CLR_YELLOW,
};

typedef struct {
    gui_colour      colour;
    colour_bits     bits;
} colour_map;

static colour_map ColourMap[] = {
    { GUI_BLACK,                CLR_BLACK },
    { GUI_BLUE,                 CLR_BLUE },
    { GUI_GREEN,                CLR_GREEN },
    { GUI_CYAN,                 CLR_CYAN },
    { GUI_RED,                  CLR_RED },
    { GUI_MAGENTA,              CLR_MAGENTA },
    { GUI_BROWN,                CLR_BROWN },
    { GUI_WHITE,                CLR_WHITE },
    { GUI_GREY,                 CLR_GREY },
    { GUI_BRIGHT_BLUE,          CLR_BRIGHT+CLR_BLUE },
    { GUI_BRIGHT_GREEN,         CLR_BRIGHT+CLR_GREEN },
    { GUI_BRIGHT_CYAN,          CLR_BRIGHT+CLR_CYAN },
    { GUI_BRIGHT_RED,           CLR_BRIGHT+CLR_RED },
    { GUI_BRIGHT_MAGENTA,       CLR_BRIGHT+CLR_MAGENTA },
    { GUI_BRIGHT_YELLOW,        CLR_YELLOW },
    { GUI_BRIGHT_YELLOW,        CLR_BRIGHT+CLR_BROWN },
    { GUI_BRIGHT_WHITE,         CLR_BRIGHT+CLR_WHITE },
};

wnd_attr WndMapTabAttr( wnd_attr wndattr )
{
    if( wndattr == WND_STANDOUT )
        return( WND_STANDOUT_TABSTOP );
    return( WND_TABSTOP );
}

wnd_attr WndTabStopAttr = WND_TABSTOP;

static int ScanAttr( attr_map *map, int size )
{
    attr_bits   bits;
    int         i;

    bits = 0;
    while( (i = ScanCmd( AttrNameTab )) >= 0 ) {
        bits |= AttrBits[i];
    }
    for( i = 0; i < size; ++i ) {
        if( map[i].bits == bits ) {
            return( map[i].attr );
        }
    }
    Error( ERR_LOC, LIT_DUI( ERR_BAD_WINDOW_ATTR ) );
    return( -1 );
}

static gui_colour ScanColour( void )
{
    colour_bits bits;
    int         i;

    bits = 0;
    while( (i = ScanCmd( ColourNameTab )) >= 0 ) {
        bits |= ColourBits[i];
        if( bits != CLR_NONE && bits != CLR_BRIGHT ) {
            break;
        }
    }
    for( i = 0; i < ArraySize( ColourMap ); ++i ) {
        if( ColourMap[i].bits == bits ) {
            return( ColourMap[i].colour );
        }
    }
    Error( ERR_LOC, LIT_DUI( ERR_BAD_COLOUR_ATTR ) );
    return( -1 );
}

static void set_dlg_attr( gui_dlg_attr dlgattr, gui_colour fore, gui_colour back )
{
    WndDlgColours[dlgattr].fore = fore;
    WndDlgColours[dlgattr].back = back;
    if( dlgattr == GUI_DLG_NORMAL ) {
        WndDlgColours[GUI_DLG_SCROLL_ICON].fore = fore;
        WndDlgColours[GUI_DLG_SCROLL_ICON].fore = fore;
        WndDlgColours[GUI_DLG_SCROLL_BAR].fore = fore;
        WndDlgColours[GUI_DLG_SCROLL_BAR].back = back;
    }
}

static void set_wndcls_attr( wnd_attr wndattr, gui_colour_set *set, gui_colour fore, gui_colour back, bool wndall )
{
    wnd_class       wndcls;

    set[wndattr].fore = fore;
    set[wndattr].back = back;
    if( wndattr == WND_PLAIN ) {
        set[GUI_BACKGROUND].fore = fore;
        set[GUI_BACKGROUND].back = back;
    }
    if( wndall ) {
        for( wndcls = 0; wndcls < WND_NUM_CLASSES; ++wndcls ) {
            if( WndClassColour[wndcls] != NULL ) {
                WndClassColour[wndcls][wndattr].fore = fore;
                WndClassColour[wndcls][wndattr].back = back;
                if( wndattr == WND_PLAIN ) {
                    WndClassColour[wndcls][GUI_BACKGROUND].fore = fore;
                    WndClassColour[wndcls][GUI_BACKGROUND].back = back;
                }
            }
        }
    }
}

void ProcPaint( void )
{
    wnd_class           wndcls;
    gui_colour          fore;
    gui_colour          back;
    gui_colour_set      *set;
    bool                dialog;
    int                 attr;

    dialog = false;
    wndcls = WND_NO_CLASS;
    if( ScanStatus() ) {
        attr = 0;
    } else if( ScanCmd( "DIalog\0" ) == 0 ) {
        dialog = true;
        attr = ScanAttr( DlgAttrMap, ArraySize( DlgAttrMap ) );
    } else {
        wndcls = ReqWndName();
        attr = ScanAttr( AttrMap, ArraySize( AttrMap ) );
    }
    fore = ScanColour();
    ScanCmd( "On\0" );
    back = ScanColour();
    ReqEOC();
    if( attr < 0 )
        return;
    if( wndcls == WND_NO_CLASS ) {
        if( dialog ) {
            GUIGetDialogColours( WndDlgColours );
            set_dlg_attr( (gui_dlg_attr)attr, fore, back );
            GUISetDialogColours( WndDlgColours );
        } else {
            WndStatusColour.fore = fore;
            WndStatusColour.back = back;
            if( WndHaveStatusWindow() ) {
                WndCloseStatusWindow();
                WndCreateStatusWindow( &WndStatusColour );
            }
        }
    } else {
        set = WndClassColour[wndcls];
        if( set == NULL ) {
            set = WndAlloc( sizeof( WndColours ) );
            memcpy( set, WndColours, sizeof( WndColours ) );
            WndClassColour[wndcls] = set;
        }
        set_wndcls_attr( (wnd_attr)attr, set, fore, back, ( wndcls == WND_ALL ) );
        _SwitchOn( SW_PENDING_REPAINT );
    }
}


void ProcPendingPaint( void )
{
    gui_colour_set      *set;
    a_window            *wnd;

    if( _IsOff( SW_PENDING_REPAINT ) ) return;
    _SwitchOff( SW_PENDING_REPAINT );
    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( WndHasClass( wnd ) ) {
            set = WndClassColour[WndClass( wnd )];
        } else {
            set = NULL;
        }
        if( set == NULL ) {
            set = WndClassColour[WND_ALL];
        }
        if( set == NULL ) {
            set = WndColours;
        }
        WndSetFontInfo( wnd, GetWndFont( wnd ) );
        WndSetColours( wnd, WndNumColours, set );
    }
}


void FiniPaint( void )
{
    wnd_class   wndcls;

    for( wndcls = 0; wndcls < WND_NUM_CLASSES; ++wndcls ) {
        if( WndClassColour[wndcls] != NULL ) {
            WndFree( WndClassColour[wndcls] );
            WndClassColour[wndcls] = NULL;
        }
    }
}

extern gui_colour_set *GetWndColours( wnd_class wndcls )
{
    if( wndcls == WND_NO_CLASS ) 
        return( WndColours );
    if( WndClassColour[wndcls] != NULL ) 
        return( WndClassColour[wndcls] );
    if( WndClassColour[WND_ALL] != NULL ) 
        return( WndClassColour[WND_ALL] );
    return( WndColours );
}


static void GetAttrName( attr_map *map, int i, char *buff )
{
    attr_bits   bits;
    bool        blank;
    char        *p;

    bits = map[i].bits;
    p = buff;
    for( i = 0;; ++i ) {
        if( bits & 1 ) {
            p = GetCmdEntry( AttrNameTab, i, p );
            blank = true;
        } else {
            blank = false;
        }
        bits >>= 1;
        if( bits == 0 ) break;
        if( blank ) {
            *p++ = ' ';
        }
    }
    *p = '\0';
}


static void GetColourName( gui_colour colour, char *buff )
{
    colour_bits bits = 0;
    int         i;
    char        *p;

    for( i = 0; i < ArraySize( ColourMap ); ++i ) {
        if( ColourMap[i].colour == colour ) {
            bits = ColourMap[i].bits;
            break;
        }
    }
    p = buff;
    if( bits & CLR_BRIGHT ) {
        p = StrCopy( "bright ", p );
        bits &= ~CLR_BRIGHT;
    }
    for( i = 0;; ++i ) {
        if( bits & 1 ) {
            p = GetCmdEntry( ColourNameTab, i, p );
            break;
        }
        bits >>= 1;
    }
    *p = '\0';
}


static void PrintStatusColour( void )
{
    char        fore[20];
    char        back[20];

    GetColourName( WndStatusColour.fore, fore );
    GetColourName( WndStatusColour.back, back );
    Format( TxtBuff, "%s status %s on %s", GetCmdName( CMD_PAINT ), fore, back );
    WndDlgTxt( TxtBuff );
}


static void PrintDialogColours( void )
{
    int             i;
    gui_dlg_attr    dlgattr;
    char            fore[20];
    char            back[20];
    char            attr[30];

    GUIGetDialogColours( WndDlgColours );
    for( i = 0; i < ArraySize( DlgAttrMap ); ++i ) {
        dlgattr = DlgAttrMap[i].attr;
        GetAttrName( DlgAttrMap, i, attr );
        GetColourName( WndDlgColours[dlgattr].fore, fore );
        GetColourName( WndDlgColours[dlgattr].back, back );
        Format( TxtBuff, "%s dialog %s %s on %s", GetCmdName( CMD_PAINT ), attr, fore, back );
        WndDlgTxt( TxtBuff );
    }
}


static void PrintColours( wnd_class wndcls, gui_colour_set *set, gui_colour_set *def )
{
    char        wndname[20];
    char        attr[30];
    char        fore[20];
    char        back[20];
    int         i;
    wnd_class   wndcls1;

    GetCmdEntry( WndNameTab, wndcls, wndname );
    for( i = 0; i < ArraySize( AttrMap ); ++i ) {
        wndcls1 = AttrMap[i].attr;
        if( def == NULL || memcmp( &set[wndcls1], &def[wndcls1], sizeof( *set ) ) != 0 ) {
            GetAttrName( AttrMap, i, attr );
            GetColourName( set[wndcls1].fore, fore );
            GetColourName( set[wndcls1].back, back );
            Format( TxtBuff, "%s %s %s %s on %s", GetCmdName( CMD_PAINT ), wndname, attr, fore, back );
            WndDlgTxt( TxtBuff );
        }
    }
}

void ConfigPaint( void )
{
    gui_colour_set  *def;
    wnd_class       wndcls;

    def = WndClassColour[WND_ALL];
    if( def == NULL ) {
        def = WndColours;
    }
    PrintColours( WND_ALL, def, NULL );
    for( wndcls = 0; wndcls < WND_NUM_CLASSES; ++wndcls ) {
        if( wndcls == WND_ALL )
            continue;
        if( WndClassColour[wndcls] != NULL ) {
            PrintColours( wndcls, WndClassColour[wndcls], def );
        }
    }
    PrintStatusColour();
    PrintDialogColours();
}

void InitPaint( void )
{
}
