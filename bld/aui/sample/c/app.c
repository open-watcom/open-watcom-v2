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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "app.h"
#include "watcom.h"
#if !defined( __AXP__ ) && !defined( UNIX )
#include "dpmi.h"
#include "i86.h"
#endif
#include "stdui.h"

wnd_update_list WndFlags = 0;
extern gui_colour_set WndColours[];

#if 0
these are optional
extern void WndCleanUp()
{
    // hook called when the application closes down
}
void GUISysInit(void)
{
}

void WndSysInit()
{
    // initialization hook
}
void WndStartFreshAll()
{
    // hook called just before windows are all refreshed
}
void WndEndFreshAll()
{
    // hook called just after windows are all refreshed
}
void WndDoInput()
{
    // hook called at beginning and end of main event proc
}
void Ring()
{
    // ring the bell
}
#endif
typedef struct {
    unsigned    key;
    char        *name;
} keymap;

keymap Key[] = {
#include "keys.h"
};

static gui_menu_struct PopTart[] = {
    { "Open &1", MENU_OPEN1, GUI_ENABLED },
    { "Open &2", MENU_OPEN2, GUI_ENABLED },
    { "Open &3", MENU_OPEN3, GUI_ENABLED },
};

extern  bool    WndProcMacro( a_window *wnd, unsigned key )
{
    int         menu;
    int         i;

    WndStatusText( "" );
    for( i = 0; i < ArraySize( Key ); ++i ) {
        if( Key[ i ].key == key ) {
            WndStatusText( Key[ i ].name );
            break;
        }
    }
    switch( key ) {
    case GUI_KEY_PERIOD:
        WndKeyPopUp( wnd, NULL );
        return( TRUE );
    case GUI_KEY_EQUAL: menu = MENU_MATCH; break;
    case GUI_KEY_n: menu = MENU_NEXT; break;
    case GUI_KEY_p: menu = MENU_PREV; break;
    case GUI_KEY_q: menu = MENU_QUIT; break;
    case GUI_KEY_1: menu = MENU_OPEN1; break;
    case GUI_KEY_a: menu = MENU_OPEN1A; break;
    case GUI_KEY_2: menu = MENU_OPEN2; break;
    case GUI_KEY_3: menu = MENU_OPEN3; break;
    case GUI_KEY_4: menu = MENU_OPEN4; break;
    case GUI_KEY_b: menu = MENU_OPEN4B; break;
    case GUI_KEY_5: menu = MENU_OPEN5; break;
    case GUI_KEY_6: menu = MENU_OPEN6; break;
    case GUI_KEY_7: menu = MENU_OPEN7; break;
    case GUI_KEY_8: menu = MENU_OPEN8; break;
    case GUI_KEY_w: menu = MENU_W1_NEWWORD; break;
    case GUI_KEY_c: menu = MENU_COMMAND; break;
    case GUI_KEY_SLASH: menu = MENU_SEARCH; break;
    case GUI_KEY_x: WndPopUp( wnd, NULL ); break;
    case GUI_KEY_CTRL_X:
    {
        static int last_menu_pos = 0;
        WndCreateFloatingPopup( wnd, NULL, ArraySize( PopTart ), PopTart, &last_menu_pos );
    }

    default:
        return( FALSE );
    }
    WndMainMenuProc( wnd, menu );
    return( TRUE );
}
#ifdef DOS
extern void BIOSSetPage( char pagenb );
#pragma aux BIOSSetPage =                               \
        " push   bp             ",                      \
        " mov    ah, 5          ",                      \
        " int    10h            ",                      \
        " pop    bp             "                       \
        parm [ al ]                                     \
        modify exact [ ah ];
#endif

gui_window_styles WndStyle = GUI_PLAIN+GUI_GMOUSE;
char *WndGadgetHint[] =
{
    "this is help for the folder",
    "this is help for the stop sign"
};

gui_resource WndGadgetArray[] = {
{    BITMAP_SOURCE1, "[f]"},
{    BITMAP_STOP1,   "[X]"},
{    BITMAP_SOURCE2, "|f|"},
{    BITMAP_STOP2,   "|X|"},
};

int WndGadgetArraySize = ArraySize( WndGadgetArray );
int WndGadgetSecondary = 2;
wnd_attr WndGadgetAttr = APP_COLOR_HOTSPOT;

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

gui_resource MainIcon = { ICON_MAIN, "main_icon" };

#ifndef UNIX
static short rm()
{
    static short   rms;
    long    result;

    if( rms == 0 ) {
        result = DPMIAllocateLDTDescriptors( 1 );
        rms = result & 0xffff;
        DPMISetSegmentLimit( rms, 0xfffff );
    }
    return( rms );
}
#endif

extern a_window *WndMain;
void GUImain( void )
{
    char        buff[256];
    char        *p;

    WndMaxDirtyRects = 20;
    getcmd( buff );
    p = buff;
    while( *p == ' ' ) ++p;
    if( p[0] == '-' && p[1] == 'n' ) {
        WndStyle &= ~GUI_GMOUSE;
        p += 2;
    }
    while( *p == ' ' ) ++p;
    if( p[0] == '-' && p[1] == '1' ) {
        #ifdef DOS
            BIOSSetPage( 1 ); // just make sure it works for the debugger
        #endif
        p += 2;
    }
    WndGadgetInit();
    WndInit( "Sample Application" );
    WndCreateStatusWindow( &WndColours[ GUI_MENU_STANDOUT ] );
    GUIGetDialogColours( WndDlgColours );
    WndStatusText( "Hello World!" );
    WndSetIcon( WndMain, &MainIcon );
    WndMainMenuProc( WndMain, MENU_OPEN1 );
} // returning starts the events rolling

/*===========================================================*/
/*===========================================================*/
/*===========================================================*/
/*===========================================================*/

#if 0



/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

#define GetBIOSData( off, var ) var =                                        \
                                sizeof( var ) == 1 ?                         \
                                    *(uint_8 __far *)MK_PM( BD_SEG, off ) :     \
                                sizeof( var ) == 2 ?                         \
                                    *(uint_16 __far *)MK_PM( BD_SEG, off ) :    \
                                *(uint_32 __far *)MK_PM( BD_SEG, off );

#define SetBIOSData( off, var ) ( sizeof( var ) == 1 ) ?                             \
                                    ( *(uint_8 __far *)MK_PM( BD_SEG, off )          \
                                        = var ) :                            \
                                ( ( sizeof( var ) == 2 ) ?                           \
                                    ( *(uint_16 __far *)MK_PM( BD_SEG, off )         \
                                        = var ) :                            \
                                ( *(uint_32 __far *)MK_PM( BD_SEG, off ) = var ) );

#define MK_PM( s, o )           MK_FP( rm(), ((s) << 4) + (o) )

static uint_16                  VIDPort;
static uint_8                   OldRow;
static uint_8                   OldCol;
static uint_8                   OldTyp;
static uint_16                  CurOffst;
static uint_16                  RegCur;
static uint_16                  InsCur;
static uint_16                  NoCur;

#define VIDMONOINDXREG                  0x03b4
#define VIDCOLRINDXREG                  0x03d4
#define NORM_CURSOR_OFF         0x2000

enum {
    BD_SEG          = 0x40,
    BD_EQUIP_LIST   = 0x10,
    BD_CURR_MODE    = 0x49,
    BD_REGEN_LEN    = 0x4c,
    BD_CURPOS       = 0x50,
    BD_MODE_CTRL    = 0x65,
    BD_VID_CTRL1    = 0x87,
};

extern uint_8 _ReadCRTCReg( uint_16 vidport, uint_8 regnb );
#pragma aux _ReadCRTCReg =                      \
        "out    dx, al          ",              \
        "inc    dx              ",              \
        "in     al, dx          "               \
        parm [ dx ] [ al ]                      \
        modify exact [ al dx ];

extern void _WriteCRTCReg( uint_16 vidport, uint_8 regnb, uint_8 value );
#pragma aux _WriteCRTCReg =                     \
        "out    dx, al          ",              \
        "inc    dx              ",              \
        "mov    al, ah          ",              \
        "out    dx, al          "               \
        parm [ dx ] [ al ] [ ah ]               \
        modify exact [ al dx ];

#define CURS_LOCATION_LOW       0xf
#define CURS_LOCATION_HI        0xe
#define CURS_START_SCANLINE     0xa
#define CURS_END_SCANLINE       0xb

static uint_8 VIDGetRow( uint_16 vidport )
{
    return( _ReadCRTCReg( vidport, CURS_LOCATION_LOW ) );
}

static void VIDSetRow( uint_16 vidport, uint_8 row )
{
    _WriteCRTCReg( vidport, CURS_LOCATION_LOW, row );
}

static void VIDSetCol( uint_16 vidport, uint_8 col )
{
    _WriteCRTCReg( vidport, CURS_LOCATION_HI, col );
}

static void VIDSetPos( uint_16 vidport, uint_16 cursorpos )
{
    VIDSetRow( vidport, cursorpos & 0xff );
    VIDSetCol( vidport, cursorpos >> 8 );
}

static void VIDSetCurTyp( uint_16 vidport, uint_16 cursortyp )
{
    _WriteCRTCReg( vidport, CURS_START_SCANLINE, cursortyp >> 8 );
    _WriteCRTCReg( vidport, CURS_END_SCANLINE, cursortyp & 0xf );
}

static uint_16 VIDGetCurTyp( uint_16 vidport )
{
    return( (uint_16)_ReadCRTCReg( vidport, CURS_START_SCANLINE ) << 8 |
            _ReadCRTCReg( vidport, CURS_END_SCANLINE ) );
}

extern void uiinitcursor( void )
{
    VIDPort = VIDCOLRINDXREG;
        RegCur = VIDGetCurTyp( VIDPort );
        NoCur = NORM_CURSOR_OFF;
    InsCur = ( ( ( RegCur + 0x100 ) >> 1 & 0xff00 ) + 0x100 ) |
             ( RegCur & 0x00ff );
}

#pragma off( unreferenced );
void uisetcursor( ORD row, ORD col, int typ, int attr )
#pragma off( unreferenced );
{
    uint_16     bios_cur_pos;

    if( typ == C_OFF ) {
        uioffcursor();
    } else if( VIDPort &&
               ( ( row != OldRow ) || ( col != OldCol ) ||
               ( typ != OldTyp ) ) ) {
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        bios_cur_pos = BD_CURPOS;
//      if( FlipMech == FLIP_PAGE ) {
//          bios_cur_pos += 2;
//      }
        SetBIOSData( bios_cur_pos + 0, OldCol );
        SetBIOSData( bios_cur_pos + 1, OldRow );
        VIDSetPos( VIDPort, CurOffst + row * UIData->width + col );
        VIDSetCurTyp( VIDPort, typ == C_INSERT ? InsCur : RegCur );
    }
}

extern void uioffcursor( void )
{
    if( ( VIDPort != NULL ) ) {
        OldTyp = C_OFF;
        VIDSetCurTyp( VIDPort, NoCur );
    }
}

extern void uiswapcursor( void )
{
}

extern void uifinicursor( void )
{
}

void uirefresh()
{
    extern void uidorefresh(void);

    uidorefresh();
}
#endif
