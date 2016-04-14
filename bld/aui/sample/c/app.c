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
* Description:  AUI library sample application.
*
****************************************************************************/


#include "app.h"
#include "watcom.h"
//#include "stdui.h"

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
void WndDoInput()
{
    // hook called at beginning and end of main event proc
}
void Ring()
{
    // ring the bell
}
#endif

void WndStartFreshAll()
{
    // hook called just before windows are all refreshed
}
void WndEndFreshAll( void )
{
    // hook called just after windows are all refreshed
    WndFlags = 0;
}

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
    gui_ctl_id  menu;
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
        return( true );
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
        static gui_ctl_id last_menu_pos = 0;
        WndCreateFloatingPopup( wnd, NULL, ArraySize( PopTart ), PopTart, &last_menu_pos );
    }

    default:
        return( false );
    }
    WndMainMenuProc( wnd, menu );
    return( true );
}

#ifdef __DOS__
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
        #ifdef __DOS__
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

bool ChkFlags( wnd_update_list flags )
{
    return( flags & WndFlags );
}
