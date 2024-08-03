/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  AUI sample program's menus.
*
****************************************************************************/


#include <stdio.h>
#include "app.h"
#include "dlgoptn.h"


void *SrchHistory;

static gui_menu_struct FirstMenu[] = {
    { "&Break",             MENU_BUG,               GUI_STYLE_MENU_ENABLED, "" },
    { "&Search",            MENU_SEARCH,            GUI_STYLE_MENU_ENABLED, "This is help text for Search" },
    { "&Next",              MENU_NEXT,              GUI_STYLE_MENU_ENABLED, "This is help text for Next" },
    { "&Prev",              MENU_PREV,              GUI_STYLE_MENU_ENABLED },
    { "",                   0,                      GUI_STYLE_MENU_SEPARATOR },
    { "&Get File",          MENU_GET_FILE,          GUI_STYLE_MENU_ENABLED },
    { "&Options",           MENU_OPTIONS,           GUI_STYLE_MENU_ENABLED },
    { "&Password",          MENU_PASSWORD,          GUI_STYLE_MENU_ENABLED },
    { "&Command",           MENU_COMMAND,           GUI_STYLE_MENU_ENABLED },
    { "P&ick One",          MENU_PICK,              GUI_STYLE_MENU_ENABLED },
    { "",                   0,                      GUI_STYLE_MENU_SEPARATOR },
    { "&Tools",             MENU_TOOLS,             GUI_STYLE_MENU_ENABLED },
    { "St&atus",            MENU_STATUS,            GUI_STYLE_MENU_ENABLED },
    { "",                   0,                      GUI_STYLE_MENU_SEPARATOR },
    { "&Match",             MENU_MATCH,             GUI_STYLE_MENU_ENABLED },
    { "Sc&ramble Menus",    MENU_SCRAMBLE_MENUS,    GUI_STYLE_MENU_ENABLED },
    { "",                   0,                      GUI_STYLE_MENU_SEPARATOR },
    { "Inactive",           0,                      GUI_STYLE_MENU_GRAYED },
    { "E&xit",              MENU_QUIT,              GUI_STYLE_MENU_ENABLED },
};
static gui_menu_struct SecondSub[] = {
    { "Open &4",    MENU_OPEN4,     GUI_STYLE_MENU_ENABLED },
    { "Open 4&b",   MENU_OPEN4B,    GUI_STYLE_MENU_ENABLED },
    { "Open &5",    MENU_OPEN5,     GUI_STYLE_MENU_ENABLED },
    { "Open &6",    MENU_OPEN6,     GUI_STYLE_MENU_ENABLED },
    { "Open &7",    MENU_OPEN7,     GUI_STYLE_MENU_ENABLED },
    { "Open &8",    MENU_OPEN8,     GUI_STYLE_MENU_ENABLED },
};
static gui_menu_struct SecondMenu[] = {
    { "Open &1",    MENU_OPEN1,     GUI_STYLE_MENU_ENABLED },
    { "Open 1&a",   MENU_OPEN1A,    GUI_STYLE_MENU_ENABLED },
    { "Open &2",    MENU_OPEN2,     GUI_STYLE_MENU_ENABLED },
    { "Open &3",    MENU_OPEN3,     GUI_STYLE_MENU_ENABLED },
    { "&More",      MENU_MORE,      GUI_STYLE_MENU_ENABLED, NULL, ArraySize( SecondSub ), SecondSub },
};
static gui_menu_struct ThirdMenu[] = {
    { "Open &1",    MENU_OPEN1,     GUI_STYLE_MENU_ENABLED },
    { "Open 1&a",   MENU_OPEN1A,    GUI_STYLE_MENU_ENABLED },
};

static gui_menu_struct ForthMenu[] = {
    { "Open &1",    MENU_OPEN1,     GUI_STYLE_MENU_ENABLED },
};

static gui_menu_struct SampleMainMenu[] = {
    { "&First",     MENU_FIRST,     GUI_STYLE_MENU_ENABLED,                             "This is help for First",   WndMenuFields( FirstMenu ) },
    { "&Second",    MENU_SECOND,    GUI_STYLE_MENU_ENABLED,                             "This is help for Second",  WndMenuFields( SecondMenu ) },
    { "&Windows",   MENU_THIRD,     GUI_STYLE_MENU_ENABLED | GUI_STYLE_MENU_MDIWINDOW,  "This is help for Windows", WndMenuFields( ThirdMenu ) },
    { "&Popup",     MENU_POPUP,     GUI_STYLE_MENU_ENABLED | WND_MENU_POPUP,            "This is help for Popup",   WndMenuFields( ForthMenu ) },
};

gui_menu_items WndMainMenu = { ArraySize( SampleMainMenu ), SampleMainMenu };

static char *FilterList = {
    "C Files (*.c;*.h)\0*.c;*.h\0"
    "C++ Files (*.cpp;*.hpp)\0*.cpp;*.hpp\0"
    "Asm Files (*.asm;*.inc)\0*.asm;*.inc\0"
    "All Files (*.*)\0*.*\0"
};

char *PickList[] = { "one", "two", "three", "four" };

static char             FileBrowsed[_MAX_PATH] = "e:\\f\\setup.dbg";

static const char * GUICALLBACK FmtNum( const void *data_handle, int item )
{
    static char buff[20];

    data_handle = data_handle;
    sprintf( buff, "%d", item );
    return( buff );
}


static void TimeIt( void )
{
    char                buff[80];
    size_t              len;
    long                iters;
    a_window            wnd;
    gui_text_metrics    dummy;

    wnd = WndFindActive();
    if( wnd == NULL )
        return;
    buff[0]='\0';
    DlgNew( "Enter Iterations", buff, 80 );
    if( buff[0] == '\0' )
        return;
    iters = strtol( buff, NULL, 10 );
    strcpy( buff, "This is just some text" );
    len = strlen( buff );
    while( --iters >= 0 ) {
//      GUIDrawText( WndGui( wnd ), buff, len, 0, 0, WndPlainAttr );
        GUIGetTextMetrics( WndGui( wnd ), &dummy );
//      WndExtentX( wnd, "This is just some text" );
    }
}

static void GetPassword( void )
{
    char        buff[80];

    buff[0] = '\0';
    Password( "What ees da passwurd?", buff, 80 );
    Say( buff );
}

static gui_toolbar_struct ToolBar[] = {
{"1", BITMAP_ONE,       MENU_OPEN1, "Open window number 1" },
{"2", BITMAP_TWO,       MENU_OPEN2, "Open window number 2" },
{"3", BITMAP_THREE,     MENU_OPEN3, "Open window number 3" },
{"4", BITMAP_FOUR,      MENU_OPEN4, "Open window number 4" },
{"5", BITMAP_FIVE,      MENU_OPEN5, "Open window number 5" },
};

static gui_toolbar_items tb_ToolBar = { ArraySize( ToolBar ), ToolBar };

static void OpenTools( void )
{
    WndCreateToolBar( 867, true, &tb_ToolBar );
}

bool     WndMainMenuProc( a_window wnd, gui_ctl_id id )
{
    a_window            new;
    a_window            active;
    int                 i;
    char                *p = 0;

    wnd=wnd;
    active = WndFindActive();
    switch( id ) {
    case MENU_BUG:
        *p = 0;
        TimeIt();
        break;
    case MENU_MATCH:
        if( active == NULL )
            return( true );
        if( WndKeyPiece( active ) == WND_NO_PIECE ) {
            Say( "Match not supported in this window" );
        } else {
            WndStartChoose( wnd );
        }
        return( true );
        break;
    case MENU_PICK:
        i = -1;
        DlgPickWithRtn( "Pick a number", NULL, 2, FmtNum, 2000, &i );
//      i = DlgPick( "Pick on of these!", PickList, 2, ArraySize( PickList ) );
        Say( i == -1 ? "None" : FmtNum( NULL, i ) );
        break;
    case MENU_COMMAND:
        DlgCmd();
        break;
    case MENU_GET_FILE:
        if( DlgFileBrowse( "Enter File Name", FilterList, FileBrowsed,
                           sizeof( FileBrowsed ), FN_OVERWRITEPROMPT ) ) {
            Say( FileBrowsed );
        }
        break;
    case MENU_OPTIONS:
        DlgOptions();
        break;
    case MENU_PASSWORD:
        GetPassword();
        break;
    case MENU_NEXT:
        if( active != NULL )
            WndSearch( active, false, 1 );
        break;
    case MENU_PREV:
        if( active != NULL )
            WndSearch( active, false, -1 );
        break;
    case MENU_SEARCH:
        if( SrchHistory == NULL )
            SrchHistory = WndInitHistory();
        if( active != NULL ) {
            WndSearch( active, false, DlgSearch( active, SrchHistory ) );
        }
        break;
    case MENU_OPEN1:
        new = W1Open();
        WndSetSwitches( new, WSW_MULTILINE_SELECT | WSW_RBUTTON_SELECTS | WSW_SUBWORD_SELECT | WSW_RBUTTON_CHANGE_CURR );
        break;
    case MENU_OPEN2:
        W2Open();
        break;
    case MENU_OPEN3:
        W3Open();
        break;
    case MENU_OPEN4:
        new = W4Open();
        break;
    case MENU_OPEN4B:
        new = W4Open();
        WndSetTitleRows( new, 2 );
        break;
    case MENU_OPEN5:
        W5Open();
        break;
    case MENU_OPEN6:
        W6Open();
        break;
    case MENU_OPEN7:
        W7Open();
        break;
    case MENU_OPEN8:
        W8Open();
        break;
    case MENU_TOOLS:
        OpenTools();
        break;
    case MENU_SCRAMBLE_MENUS:
        {
            gui_menu_struct tmp;
            tmp = SampleMainMenu[0];
            SampleMainMenu[0] = SampleMainMenu[1];
            SampleMainMenu[1] = tmp;
        }
        {
            gui_menu_struct tmp;
            tmp = FirstMenu[0];
            FirstMenu[0] = SecondMenu[0];
            SecondMenu[0] = ThirdMenu[0];
            ThirdMenu[0] = tmp;
        }
        WndSetMainMenu( &WndMainMenu );
        break;
    case MENU_STATUS:
        if( WndHaveStatusWindow() ) {
            WndCloseStatusWindow();
        } else {
            WndCreateStatusWindow( &WndColours[GUI_BACKGROUND] );
            WndStatusText( "Hello World!" );
        }
        break;
    case MENU_OPEN1A:
        new = W1Open();
        WndSetSwitches( new, WSW_LBUTTON_SELECTS | WSW_MAP_CURSOR_TO_SCROLL );
        WndClrSwitches( new, WSW_SELECT_IN_TABSTOP | WSW_ALLOW_POPUP );
        break;
    case MENU_QUIT:
        WndFini();
        break;
    default:
        return( false );
    }
    return( true );
}
