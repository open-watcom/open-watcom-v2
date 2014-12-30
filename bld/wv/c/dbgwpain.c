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


extern wnd_class        ReqWndName( void );
extern void             ConfigLine( char * );
extern char             *GetCmdEntry( const char *, int, char * );
extern bool             WndDlgTxt( const char *buff );
extern char             *GetWndFont( a_window * );
extern bool             ScanStatus( void );
extern char             *GetCmdName( int );


extern const char       WndNameTab[];

static gui_colour_set   *WndClassColour[ WND_NUM_CLASSES ];
gui_colour_set          WndStatusColour = { GUI_BRIGHT_WHITE, GUI_BLUE };

wnd_attr WndPlainAttr = WND_PLAIN;
wnd_attr WndSelectedAttr = WND_SELECTED;
wnd_attr WndCursorAttr = WND_SELECTED;

static gui_colour_set   WndDlgColours[] = {
    { GUI_WHITE,        GUI_BLUE },     // GUI_DLG_NORMAL
    { GUI_BLUE,         GUI_CYAN },     // GUI_DLG_FRAME
    { GUI_BLACK,        GUI_BLUE },     // GUI_DLG_SHADOW
    { GUI_WHITE,        GUI_BLUE },     // GUI_DLG_SCROLL_ICON
    { GUI_WHITE,        GUI_BLUE },     // GUI_DLG_SCROLL_BAR
    { GUI_BLUE,         GUI_WHITE },    // GUI_DLG_BUTTON_PLAIN
    { GUI_BRIGHT_WHITE, GUI_WHITE },    // GUI_DLG_BUTTON_STANDOUT
    { GUI_BLUE,         GUI_CYAN },     // GUI_DLG_BUTTON_ACTIVE
    { GUI_BRIGHT_WHITE, GUI_CYAN },     // GUI_DLG_BUTTON_ACTIVE_STANDOUT
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
        wnd_attr        attr;
        attr_bits       bits;
} attr_map;

static attr_map AttrMap[] = {
    { GUI_MENU_PLAIN,           ATTR_MENU+ATTR_PLAIN },
    { GUI_MENU_STANDOUT,        ATTR_MENU+ATTR_STANDOUT },
    { GUI_MENU_GRAYED,          ATTR_MENU+ATTR_DISABLED },
    { GUI_MENU_ACTIVE,          ATTR_MENU+ATTR_ACTIVE },
    { GUI_MENU_ACTIVE_STANDOUT, ATTR_MENU+ATTR_ACTIVE+ATTR_STANDOUT },
    { 0,                        0 }, // background
    { GUI_MENU_FRAME,           ATTR_FRAME+ATTR_MENU },
    { GUI_TITLE_INACTIVE,       ATTR_TITLE+ATTR_DISABLED },
    { GUI_FRAME_ACTIVE,         ATTR_FRAME+ATTR_ACTIVE },
    { GUI_FRAME_INACTIVE,       ATTR_FRAME+ATTR_DISABLED },
    { GUI_ICON,                 ATTR_ICON },
    { GUI_MENU_GRAYED_ACTIVE,   ATTR_MENU+ATTR_DISABLED+ATTR_ACTIVE },
    { GUI_FRAME_RESIZE,         0 },
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
    { 0,                        0 }, // scroll icon
    { 0,                        0 }, // scroll bar
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

wnd_attr WndMapTabAttr( wnd_attr attr )
{
    if( attr == WND_STANDOUT ) return( WND_STANDOUT_TABSTOP );
    return( WND_TABSTOP );
}

wnd_attr WndTabStopAttr = WND_TABSTOP;

static wnd_attr ScanAttr( attr_map *map, int size )
{
    attr_bits   bits;
    int         i;

    bits = 0;
    for( ;; ) {
        i = ScanCmd( AttrNameTab );
        if( i == 0 ) break;
        bits |= AttrBits[ i-1 ];
    }
    for( i = 0; i < size; ++i ) {
        if( map[i].bits == bits ) {
            return( map[i].attr );
        }
    }
    Error( ERR_LOC, LIT( ERR_BAD_WINDOW_ATTR ) );
    return( 0 );
}

static gui_colour ScanColour( void )
{
    colour_bits bits;
    int         i;

    bits = 0;
    for( ;; ) {
        i = ScanCmd( ColourNameTab );
        if( i == 0 ) break;
        bits |= ColourBits[ i-1 ];
        if( bits != CLR_NONE && bits != CLR_BRIGHT ) break;
    }
    for( i = 0; i < ArraySize( ColourMap ); ++i ) {
        if( ColourMap[i].bits == bits ) {
            return( ColourMap[i].colour );
        }
    }
    Error( ERR_LOC, LIT( ERR_BAD_COLOUR_ATTR ) );
    return( 0 );
}

void ProcPaint( void )
{
    wnd_class           class;
    wnd_attr            attr;
    gui_colour          fore;
    gui_colour          back;
    gui_colour_set      *set;
    bool                dialog;
    int                 dlg_attr;

    dlg_attr = 0;
    dialog = FALSE;
    class = WND_NO_CLASS;
    if( ScanStatus() ) {
    } else if( ScanCmd( "DIalog\0" ) ) {
        dlg_attr = ScanAttr( DlgAttrMap, ArraySize( DlgAttrMap ) );
        dialog = TRUE;
    } else {
        class = ReqWndName();
        attr = ScanAttr( AttrMap, ArraySize( AttrMap ) );
    }
    fore = ScanColour();
    ScanCmd( "On\0" );
    back = ScanColour();
    ReqEOC();
    if( class == WND_NO_CLASS ) {
        if( dialog ) {
            GUIGetDialogColours( WndDlgColours );
            WndDlgColours[ dlg_attr ].fore = fore;
            WndDlgColours[ dlg_attr ].back = back;
            if( dlg_attr == GUI_DLG_NORMAL ) {
                WndDlgColours[ GUI_DLG_SCROLL_ICON ].fore = fore;
                WndDlgColours[ GUI_DLG_SCROLL_ICON ].back = back;
                WndDlgColours[ GUI_DLG_SCROLL_BAR ].fore = fore;
                WndDlgColours[ GUI_DLG_SCROLL_BAR ].back = back;
            }
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
        set = WndClassColour[ class ];
        if( set == NULL ) {
            set = WndAlloc( sizeof( WndColours ) );
            memcpy( set, WndColours, sizeof( WndColours ) );
            WndClassColour[ class ] = set;
        }
        set[ attr ].fore = fore;
        set[ attr ].back = back;
        if( attr == WND_PLAIN ) {
            set[ GUI_BACKGROUND ] = set[ attr ];
        }
        if( class == WND_ALL ) {
            for( class = 0; class < WND_NUM_CLASSES; ++class ) {
                if( WndClassColour[ class ] != NULL ) {
                    WndClassColour[ class ][ attr ] = set[ attr ];
                    if( attr == WND_PLAIN ) {
                        WndClassColour[ class ][ GUI_BACKGROUND ] = set[ attr ];
                    }
                }
            }
        }
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
            set = WndClassColour[ WndClass( wnd ) ];
        } else {
            set = NULL;
        }
        if( set == NULL ) {
            set = WndClassColour[ WND_ALL ];
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
    int         i;

    for( i = 0; i < WND_NUM_CLASSES; ++i ) {
        if( WndClassColour[ i ] != NULL ) {
            WndFree( WndClassColour[ i ] );
            WndClassColour[ i ] = NULL;
        }
    }
}

extern gui_colour_set *GetWndColours( wnd_class class )
{
    if( class == WND_NO_CLASS ) 
        return( WndColours );
    if( WndClassColour[ class ] != NULL ) 
        return( WndClassColour[ class ] );
    if( WndClassColour[ WND_ALL ] != NULL ) 
        return( WndClassColour[ WND_ALL ] );
    return( WndColours );
}


static void GetAttrName( attr_map *map, int i, char *buff )
{
    attr_bits   bits;
    bool        blank;
    char        *p;

    bits = map[i].bits;
    p = buff;
    for( i = 1;; ++i ) {
        if( bits & 1 ) {
            p = GetCmdEntry( AttrNameTab, i, p );
            blank = TRUE;
        } else {
            blank = FALSE;
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
    for( i = 1;; ++i ) {
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
    int         i;
    char        fore[20];
    char        back[20];
    char        attr[30];

    GUIGetDialogColours( WndDlgColours );
    for( i = 0; i < ArraySize( DlgAttrMap ); ++i ) {
        if( i == GUI_DLG_SCROLL_ICON ) continue;
        if( i == GUI_DLG_SCROLL_BAR ) continue;
        GetAttrName( DlgAttrMap, i, attr );
        GetColourName( WndDlgColours[ i ].fore, fore );
        GetColourName( WndDlgColours[ i ].back, back );
        Format( TxtBuff, "%s dialog %s %s on %s", GetCmdName( CMD_PAINT ), attr, fore, back );
        WndDlgTxt( TxtBuff );
    }
}


static void PrintColours( wnd_class class,
                          gui_colour_set *set, gui_colour_set *def )
{
    char        wndname[20];
    char        attr[30];
    char        fore[20];
    char        back[20];
    int         i;

    GetCmdEntry( WndNameTab, class+1, wndname );
    for( i = 0; i < ArraySize( AttrMap ); ++i ) {
        if( i == GUI_BACKGROUND ) continue;
        if( i == GUI_FRAME_RESIZE ) continue;
        if( def == NULL || memcmp( &set[i], &def[i], sizeof( *set ) ) != 0 ) {
            GetAttrName( AttrMap, i, attr );
            GetColourName( set[i].fore, fore );
            GetColourName( set[i].back, back );
            Format( TxtBuff, "%s %s %s %s on %s", GetCmdName( CMD_PAINT ), wndname, attr, fore, back );
            WndDlgTxt( TxtBuff );
        }
    }
}

void ConfigPaint( void )
{
    gui_colour_set *def;
    int                 i;

    def = WndClassColour[ WND_ALL ];
    if( def == NULL ) {
        def = WndColours;
    }
    PrintColours( WND_ALL, def, NULL );
    for( i = 0; i < WND_NUM_CLASSES; ++i ) {
        if( i == WND_ALL ) continue;
        if( WndClassColour[ i ] != NULL ) {
            PrintColours( i, WndClassColour[ i ], def );
        }
    }
    PrintStatusColour();
    PrintDialogColours();
}

void InitPaint( void )
{
}
