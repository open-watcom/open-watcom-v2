/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


gui_colour_set          WndStatusColour = {
    GUI_BR_WHITE,
    GUI_BLUE
};

wnd_attr WndPlainAttr    = WND_PLAIN;
wnd_attr WndSelectedAttr = WND_SELECTED;
wnd_attr WndCursorAttr   = WND_SELECTED;
wnd_attr WndTabStopAttr  = WND_TABSTOP;

static gui_colour_set   *WndClassColour[NUM_WNDCLS_ALL];

static gui_colour_set   WndDlgColours[] = {
    #define pick(e,f,b)     { f, b },
    #include "attrdlg.h"
    #undef pick
};

gui_colour_set WndColours[] = {
    { GUI_WHITE,     GUI_BLUE },        /* GUI_MENU_PLAIN           */
    { GUI_BR_WHITE,  GUI_BLUE },        /* GUI_MENU_STANDOUT        */
    { GUI_GREY,      GUI_BLUE },        /* GUI_MENU_GRAYED          */
    { GUI_BR_WHITE,  GUI_BLACK },       /* GUI_MENU_ACTIVE          */
    { GUI_BR_YELLOW, GUI_BLACK },       /* GUI_MENU_ACTIVE_STANDOUT */
    { GUI_BLACK,     GUI_BR_WHITE },    /* GUI_BACKGROUND           */
    { GUI_BR_YELLOW, GUI_BLUE },        /* GUI_MENU_FRAME           */
    { GUI_GREY,      GUI_BR_WHITE },    /* GUI_TITLE_INACTIVE       */
    { GUI_BLUE,      GUI_CYAN },        /* GUI_FRAME_ACTIVE         */
    { GUI_GREY,      GUI_BR_WHITE },    /* GUI_FRAME_INACTIVE       */
    { GUI_BR_WHITE,  GUI_RED },         /* GUI_ICON                 */
    { GUI_GREY,      GUI_BLACK },       /* GUI_MENU_GRAYED_ACTIVE   */
    { GUI_GREY,      GUI_CYAN },        /* GUI_FRAME_RESIZE         */
    { GUI_GREY,      GUI_BLACK },       /* GUI_CONTROL_BACKGROUND   */
    { GUI_BLACK,     GUI_BR_WHITE },    /* WND_PLAIN                */
    { GUI_BR_WHITE,  GUI_BLACK },       /* WND_TABSTOP == ACTIVE    */
    { GUI_BR_WHITE,  GUI_RED },         /* WND_SELECTED             */
    { GUI_RED,       GUI_BR_WHITE },    /* WND_STANDOUT             */
    { GUI_RED,       GUI_BR_WHITE },    /* WND_HOTSPOT              */
    { GUI_BR_RED,    GUI_BLACK },       /* WND_STANDOUT_TABSTOP     */
};

int WndNumColours = { ArraySize( WndColours ) };

#define ATTR_BITS() \
    pick( "Active",   DBG_ATTR_ACTIVE,   0x0001 ) \
    pick( "SElected", DBG_ATTR_SELECTED, 0x0002 ) \
    pick( "STandout", DBG_ATTR_STANDOUT, 0x0004 ) \
    pick( "Disabled", DBG_ATTR_DISABLED, 0x0008 ) \
    pick( "Plain",    DBG_ATTR_PLAIN,    0x0010 ) \
    pick( "Frame",    DBG_ATTR_FRAME,    0x0020 ) \
    pick( "ICon",     DBG_ATTR_ICON,     0x0040 ) \
    pick( "MEnu",     DBG_ATTR_MENU,     0x0080 ) \
    pick( "TItle",    DBG_ATTR_TITLE,    0x0100 ) \
    pick( "BUtton",   DBG_ATTR_BUTTON,   0x0200 ) \
    pick( "SHadow",   DBG_ATTR_SHADOW,   0x0400 )

typedef enum {
    #define pick(t,e,v)     e = v,
        ATTR_BITS()
    #undef pick
} attr_bits;

static const char AttrNameTab[] = {
    #define pick(t,e,v)     t "\0"
        ATTR_BITS()
    #undef pick
};

static attr_bits AttrBits[] = {
    #define pick(t,e,v)     e,
        ATTR_BITS()
    #undef pick
};

typedef struct {
    int             attr;
    attr_bits       bits;
} attr_map;

static attr_map AttrMap[] = {
    { GUI_MENU_PLAIN,           DBG_ATTR_MENU+DBG_ATTR_PLAIN },
    { GUI_MENU_STANDOUT,        DBG_ATTR_MENU+DBG_ATTR_STANDOUT },
    { GUI_MENU_GRAYED,          DBG_ATTR_MENU+DBG_ATTR_DISABLED },
    { GUI_MENU_ACTIVE,          DBG_ATTR_MENU+DBG_ATTR_ACTIVE },
    { GUI_MENU_ACTIVE_STANDOUT, DBG_ATTR_MENU+DBG_ATTR_ACTIVE+DBG_ATTR_STANDOUT },
    { GUI_MENU_FRAME,           DBG_ATTR_FRAME+DBG_ATTR_MENU },
    { GUI_TITLE_INACTIVE,       DBG_ATTR_TITLE+DBG_ATTR_DISABLED },
    { GUI_FRAME_ACTIVE,         DBG_ATTR_FRAME+DBG_ATTR_ACTIVE },
    { GUI_FRAME_INACTIVE,       DBG_ATTR_FRAME+DBG_ATTR_DISABLED },
    { GUI_ICON,                 DBG_ATTR_ICON },
    { GUI_MENU_GRAYED_ACTIVE,   DBG_ATTR_MENU+DBG_ATTR_DISABLED+DBG_ATTR_ACTIVE },
    { WND_PLAIN,                DBG_ATTR_PLAIN },
    { WND_TABSTOP,              DBG_ATTR_ACTIVE },
    { WND_SELECTED,             DBG_ATTR_SELECTED },
    { WND_STANDOUT,             DBG_ATTR_STANDOUT },
    { WND_HOTSPOT,              DBG_ATTR_BUTTON },
    { WND_STANDOUT_TABSTOP,     DBG_ATTR_ACTIVE+DBG_ATTR_STANDOUT },
};

static attr_map DlgAttrMap[] = {
    { GUI_DLG_NORMAL,                   DBG_ATTR_PLAIN },
    { GUI_DLG_FRAME,                    DBG_ATTR_FRAME },
    { GUI_DLG_SHADOW,                   DBG_ATTR_SHADOW },
    { GUI_DLG_BUTTON_PLAIN,             DBG_ATTR_BUTTON+DBG_ATTR_PLAIN },
    { GUI_DLG_BUTTON_STANDOUT,          DBG_ATTR_BUTTON+DBG_ATTR_STANDOUT },
    { GUI_DLG_BUTTON_ACTIVE,            DBG_ATTR_BUTTON+DBG_ATTR_ACTIVE },
    { GUI_DLG_BUTTON_ACTIVE_STANDOUT,   DBG_ATTR_BUTTON+DBG_ATTR_ACTIVE+DBG_ATTR_STANDOUT },
};

#define COLOUR_BITS() \
    pick( "BLAck",   CLR_BLACK,   0x0001 ) \
    pick( "BLUe",    CLR_BLUE,    0x0002 ) \
    pick( "BRIght",  CLR_BRIGHT,  0x0004 ) \
    pick( "BROwn",   CLR_BROWN,   0x0008 ) \
    pick( "Cyan",    CLR_CYAN,    0x0010 ) \
    pick( "GRAy",    CLR_GRAY,    0x0020 ) \
    pick( "GREEn",   CLR_GREEN,   0x0040 ) \
    pick( "GREY",    CLR_GREY,    0x0080 ) \
    pick( "MAgenta", CLR_MAGENTA, 0x0100 ) \
    pick( "Red",     CLR_RED,     0x0200 ) \
    pick( "White",   CLR_WHITE,   0x0400 ) \
    pick( "Yellow",  CLR_YELLOW,  0x0800 )

typedef enum {
    CLR_NONE = 0,
    #define pick(t,e,v)     e = v,
        COLOUR_BITS()
    #undef pick
} colour_bits;

static const char ColourNameTab[] = {
    #define pick(t,e,v)     t "\0"
        COLOUR_BITS()
    #undef pick
};

static colour_bits ColourBits[] = {
    #define pick(t,e,v)     e,
        COLOUR_BITS()
    #undef pick
};

typedef struct {
    gui_colour      colour;
    colour_bits     bits;
} colour_map;

static colour_map ColourMap[] = {
    { GUI_BLACK,        CLR_BLACK },
    { GUI_BLUE,         CLR_BLUE },
    { GUI_GREEN,        CLR_GREEN },
    { GUI_CYAN,         CLR_CYAN },
    { GUI_RED,          CLR_RED },
    { GUI_MAGENTA,      CLR_MAGENTA },
    { GUI_BROWN,        CLR_BROWN },
    { GUI_WHITE,        CLR_WHITE },
    { GUI_GREY,         CLR_GREY },
    { GUI_BR_BLUE,      CLR_BRIGHT+CLR_BLUE },
    { GUI_BR_GREEN,     CLR_BRIGHT+CLR_GREEN },
    { GUI_BR_CYAN,      CLR_BRIGHT+CLR_CYAN },
    { GUI_BR_RED,       CLR_BRIGHT+CLR_RED },
    { GUI_BR_MAGENTA,   CLR_BRIGHT+CLR_MAGENTA },
    { GUI_BR_YELLOW,    CLR_YELLOW },
    { GUI_BR_YELLOW,    CLR_BRIGHT+CLR_BROWN },
    { GUI_BR_WHITE,     CLR_BRIGHT+CLR_WHITE },
};

wnd_attr WndMapTabAttr( wnd_attr wndattr )
{
    if( (wnd_attr_wv)wndattr == WND_STANDOUT )
        return( WND_STANDOUT_TABSTOP );
    return( WND_TABSTOP );
}

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

static void set_wndclass_attr( wnd_attr_wv wndattr, gui_colour_set *set, gui_colour fore, gui_colour back, bool wndall )
{
    wnd_class_wv    wndclass;

    set[wndattr].fore = fore;
    set[wndattr].back = back;
    if( wndattr == WND_PLAIN ) {
        set[GUI_BACKGROUND].fore = fore;
        set[GUI_BACKGROUND].back = back;
    }
    if( wndall ) {
        for( wndclass = 0; wndclass < NUM_WNDCLS_ALL; ++wndclass ) {
            if( WndClassColour[wndclass] != NULL ) {
                WndClassColour[wndclass][wndattr].fore = fore;
                WndClassColour[wndclass][wndattr].back = back;
                if( wndattr == WND_PLAIN ) {
                    WndClassColour[wndclass][GUI_BACKGROUND].fore = fore;
                    WndClassColour[wndclass][GUI_BACKGROUND].back = back;
                }
            }
        }
    }
}

void ProcPaint( void )
{
    wnd_class_wv        wndclass;
    gui_colour          fore;
    gui_colour          back;
    gui_colour_set      *set;
    bool                dialog;
    int                 attr;
    bool                nowndclass;

    dialog = false;
    nowndclass = true;
    if( ScanStatus() ) {
        attr = 0;
    } else if( ScanCmd( "DIalog\0" ) == 0 ) {
        dialog = true;
        attr = ScanAttr( DlgAttrMap, ArraySize( DlgAttrMap ) );
    } else {
        wndclass = ReqWndName();
        nowndclass = false;
        attr = ScanAttr( AttrMap, ArraySize( AttrMap ) );
    }
    fore = ScanColour();
    ScanCmd( "On\0" );
    back = ScanColour();
    ReqEOC();
    if( attr < 0 )
        return;
    if( nowndclass ) {
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
        set = WndClassColour[wndclass];
        if( set == NULL ) {
            set = WndAlloc( sizeof( WndColours ) );
            memcpy( set, WndColours, sizeof( WndColours ) );
            WndClassColour[wndclass] = set;
        }
        set_wndclass_attr( (wnd_attr_wv)attr, set, fore, back, ( wndclass == WND_ALL ) );
        _SwitchOn( SW_PENDING_REPAINT );
    }
}


void ProcPendingPaint( void )
{
    gui_colour_set      *set;
    a_window            wnd;

    if( _IsOff( SW_PENDING_REPAINT ) )
        return;
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
    wnd_class_wv    wndclass;

    for( wndclass = 0; wndclass < NUM_WNDCLS_ALL; ++wndclass ) {
        if( WndClassColour[wndclass] != NULL ) {
            WndFree( WndClassColour[wndclass] );
            WndClassColour[wndclass] = NULL;
        }
    }
}

gui_colour_set *GetWndColours( wnd_class_wv wndclass )
{
    if( WndClassColour[wndclass] != NULL )
        return( WndClassColour[wndclass] );
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
        if( bits == 0 )
            break;
        if( blank ) {
            *p++ = ' ';
        }
    }
    *p = NULLCHAR;
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
    *p = NULLCHAR;
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


static void PrintColours( wnd_class_wv wndclass, gui_colour_set *set, gui_colour_set *def )
{
    char            wndname[20];
    char            attr[30];
    char            fore[20];
    char            back[20];
    int             i;
    wnd_class_wv    wndclass1;

    GetCmdEntry( WndNameTab, wndclass, wndname );
    for( i = 0; i < ArraySize( AttrMap ); ++i ) {
        wndclass1 = AttrMap[i].attr;
        if( def == NULL || memcmp( &set[wndclass1], &def[wndclass1], sizeof( *set ) ) != 0 ) {
            GetAttrName( AttrMap, i, attr );
            GetColourName( set[wndclass1].fore, fore );
            GetColourName( set[wndclass1].back, back );
            Format( TxtBuff, "%s %s %s %s on %s", GetCmdName( CMD_PAINT ), wndname, attr, fore, back );
            WndDlgTxt( TxtBuff );
        }
    }
}

void ConfigPaint( void )
{
    gui_colour_set  *def;
    wnd_class_wv    wndclass;

    def = WndClassColour[WND_ALL];
    if( def == NULL ) {
        def = WndColours;
    }
    PrintColours( WND_ALL, def, NULL );
    for( wndclass = 0; wndclass < NUM_WNDCLS_ALL; ++wndclass ) {
        if( wndclass == WND_ALL )
            continue;
        if( WndClassColour[wndclass] != NULL ) {
            PrintColours( wndclass, WndClassColour[wndclass], def );
        }
    }
    PrintStatusColour();
    PrintDialogColours();
}

void InitPaint( void )
{
}
