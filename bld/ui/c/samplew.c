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


#define TRUE    1
#define FALSE   0

#include <stdio.h>
#include <string.h>
#include <process.h>
#include <dos.h>

#include "stdui.h"
#include "uimenu.h"
#include "uivedit.h"
#include "uigchar.h"
#include "uiswap.h"

#include <wwindows.h>

extern void ToCharacter( void );
extern void ToGraphical( void );
extern EVENT keyboardevent( void );

#define Normal          UIData->attrs[ ATTR_NORMAL ]

extern void sample_dialog( void );

char    radio_full[4];
char    radio_empty[4];
char    check_full[4];
char    check_empty[4];

enum    Symbols { RADIO_BUTTON, CHECK_BOX };
enum    Conditons { ON, OFF };

extern EVENT    LineEvents[];

#define EV_QUIT         EV_F2
enum {
    EV_OPEN = EV_FIRST_UNUSED,
    EV_CLOSE,
    EV_CUT,
    EV_PASTE,
    EV_GO,
    EV_SAMPLE_DIALOG,
    EV_NOTHING
};

static MENUITEM filemenu[] = {
    { "Open",     EV_OPEN,          0 },
    { "Close",    EV_CLOSE,         0 },
    { "Nothing",  EV_NOTHING,       2 },
    { NULL,       EV_NO_EVENT,      ITEM_SEPARATOR },
    { "Dialog",   EV_SAMPLE_DIALOG, 1 },
    { "",         EV_NO_EVENT,      0 },
    { "Exit",     EV_QUIT,          1 },
    NULL
};

static MENUITEM editmenu[] = {
    { NULL,       EV_NO_EVENT,      0 },
};

static MENUITEM barmenu[] = {
    { "File",     EV_NO_EVENT,      0 },
    { "Edit",     EV_NO_EVENT,      1 },
    { "Go!",      EV_GO,            0 },
    { "File",     EV_NO_EVENT,      2 },
    NULL
};

static MENUITEM *pulldownuimenus[] = {
    filemenu,
    editmenu,
    NULL,
    filemenu
};

static VSCREEN mainwin = {
    EV_NO_EVENT,     /* used for mouse clicks          */
    "Window Title",  /* displayed if there is a frame  */
    3, 10, 18, 60,   /* row, col, height, width        */
    0,               /* flags defined in stdui.h       */
    0, 0,            /* cursor position                */
    C_NORMAL,        /* cursor types define in stdui.h */
    false,           /* bool: virtual screen open      */
    false            /* title is dynamicaly allocated  */
};

static VSCREEN opwin = {
    EV_NO_EVENT,     /* used for mouse clicks          */
    "Open",          /* displayed if there is a frame  */
    10, 30, 5, 20,   /* row, col, height, width        */
    0,               /* flags defined in stdui.h       */
    0, 0,            /* cursor position                */
    C_OFF,           /* cursor types define in stdui.h */
    false,           /* bool: virtual screen open      */
    false            /* title is dynamicaly allocated  */
};

static char Buffer[81] = { '1','2','3','4','5','6','7','8' };

static VEDITLINE inputline = {
    3, 5, 10,    /* row, column, and length of field */
    0,           /* index of leftmost character visible */
    10,          /* maximum allowed length of string */
    Buffer,      /* pointer to buffer */
    0,           /* index of cursor in buffer */
    0x70,        /* attribute used to echo buffer */
    FALSE,       /* bool: user changed buffer contents */
    TRUE,        /* bool: application altered buffer */
};

static EVENT oplist[] = {
    EV_NO_EVENT,
    EV_ENTER,
    EV_ESCAPE,
    EV_NO_EVENT
};

static EVENT             evlist[] = {
    EV_FIRST_EVENT,     EV_LAST_KEYBOARD,
    EV_MOUSE_PRESS,     EV_MOUSE_RELEASE,
    EV_MOUSE_HOLD_R,    EV_MOUSE_REPEAT_M,
    EV_OPEN,            EV_NOTHING,
    EV_NO_EVENT,
    EV_MOUSE_DCLICK,
    EV_MOUSE_REPEAT,
    EV_IDLE,
    EV_MENU_INITPOPUP,
    EV_CLOCK_TICK,
    EV_BACKGROUND_RESIZE,
    EV_CURSOR_UP,
    EV_CURSOR_DOWN,
    EV_CURSOR_LEFT,
    EV_CURSOR_RIGHT,
    EV_NO_EVENT
};

typedef struct an_event_string{
    EVENT       ev;
    char        *str;
}an_event_string;

static SAREA    BandArea;
int             BandOn = 0;

static          an_event_string         evstrs[] = {
    { EV_MOUSE_PRESS,       "EV_MOUSE_PRESS" },
    { EV_MOUSE_RELEASE,     "EV_MOUSE_RELEASE" },
    { EV_MOUSE_DRAG,        "EV_MOUSE_DRAG" },
    { EV_MOUSE_REPEAT,      "EV_MOUSE_REPEAT" },
    { EV_MOUSE_DCLICK,      "EV_MOUSE_DCLICK" },
    { EV_MOUSE_PRESS_R,     "EV_MOUSE_PRESS_R" },
    { EV_MOUSE_RELEASE_R,   "EV_MOUSE_RELEASE_R" },
    { EV_MOUSE_DRAG_R,      "EV_MOUSE_DRAG_R" },
    { EV_MOUSE_REPEAT_R,    "EV_MOUSE_REPEAT_R" },
    { EV_MOUSE_DCLICK_R,    "EV_MOUSE_DCLICK_R" },
    { EV_MOUSE_PRESS_M,     "EV_MOUSE_PRESS_M" },
    { EV_MOUSE_RELEASE_M,   "EV_MOUSE_RELEASE_M" },
    { EV_MOUSE_DRAG_M,      "EV_MOUSE_DRAG_M" },
    { EV_MOUSE_REPEAT_M,    "EV_MOUSE_REPEAT_M" },
    { EV_MOUSE_DCLICK_M,    "EV_MOUSE_DCLICK_M" },
    { EV_ALT_PRESS,         "EV_ALT_PRESS" },
    { EV_ALT_RELEASE,       "EV_ALT_RELEASE" },
    { EV_IDLE,              "EV_IDLE" },
    { EV_MENU_INITPOPUP,    "EV_MENU_INITPOPUP" },
    { EV_BACKGROUND_RESIZE, "EV_BACKGROUND_RESIZE" },
    { EV_NO_EVENT,          NULL }
};

static void open( void )
/**********************/
{
    EVENT ev;

    if( uivopen( &opwin ) ) {

        uipushlist( oplist );
        uivtextput( &opwin, 1, 2, UIData->attrs[ATTR_NORMAL], "Enter file name.", 16 );
        inputline.attr = UIData->attrs[ ATTR_EDIT ];
        /* blank out the buffer */
        inputline.index = 0;
        inputline.scroll = 0;
        inputline.update = TRUE;
        for( ; ; ) {
            ev = uiveditline( &opwin, &inputline );
            if( ev != EV_NO_EVENT ) break;
        }
        if( ev == EV_ENTER ) {
            /* open file */
        } else if( ev == EV_ESCAPE ) {
            /* do nothing */
        } else {
            /* must be an event handled in the mainline */
            uiungetevent();
        }
        uipoplist();
        uivclose( &opwin );
    }
}

#define TOP_ROW 8

int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpCmdLine, int nShowCmd ) {

    EVENT               ev;
    SAREA               area;
    char                buff[80];
    an_event_string     *ptr;
    ORD                 evrow = TOP_ROW;
    BOOL                fixup = FALSE;
    int                 mrow, mcol;
    int                 diff;

    hInstance = hInstance;
    hPrevInstance = hPrevInstance;
    lpCmdLine = lpCmdLine;
    nShowCmd = nShowCmd;
    if( InitSwapper() ) return( 0 ); // Yes - well, I would make the swapper
                                     // return 1 on error, unlike everything
                                     // else, wouldn't I?
    if( !uistart() ) return( 0 );
    ToCharacter();

    initmouse( 2 );
    uimenus( barmenu, pulldownuimenus, EV_F1 );
    UIData->mouse_clk_delay = uiclockdelay( 250 );
    UIData->tick_delay = uiclockdelay( 3000 );
    mainwin.area.height = UIData->height - 7;

    area.row = 0;
    area.col = 0;
    area.width = UIData->width;
    area.height = UIData->height;
    uidirty( area );
    uirefresh();

    if( uivopen( &mainwin ) ) {
        uirefresh();
        sprintf( buff, "screen height : %d\0", UIData->height );
        uivtextput( &mainwin, TOP_ROW - 1, 2, UIData->attrs[ATTR_NORMAL], buff, 40 );
        for( ; ; ) {
            uipushlist( evlist );
            ev = uivgetevent( NULL );
            uipoplist();
            if( ev == EV_QUIT ) break;
            if( ev == EV_ALT_R ) break;
            if( ev == EV_MOUSE_PRESS_R ) {
                uimousepos( NULL, &mrow, &mcol );
                mrow++;
                mcol++;
                uipushlist( evlist );
                ev = uicreatepopup( mrow, mcol, &filemenu, FALSE, TRUE, NULL );
                uipoplist();
            }
            switch ( ev ) {
                case EV_SAMPLE_DIALOG:
                    sample_dialog();
                    break;
                case EV_OPEN:
                    open();
                    break;
                case EV_F1:
                    area.width = 10;
                    area.height = 10;
                    area.row = 1;
                    area.col = 1;
                    uivattribute( &mainwin, area, (ATTR) 1 );
                    break;
                case EV_CURSOR_RIGHT:
                    mainwin.col++;
                    if( mainwin.col >= mainwin.area.width ) mainwin.col--;
                    fixup = TRUE;
                    break;
                case EV_CURSOR_DOWN:
                    mainwin.row++;
                    if( mainwin.row >= mainwin.area.height ) mainwin.row--;
                    fixup = TRUE;
                    break;
                case EV_CURSOR_LEFT:
                    if( mainwin.col > 0 ) {
                        mainwin.col--;
                        fixup = TRUE;
                    }
                    break;
                case EV_CURSOR_UP:
                    if( mainwin.row > 0 ) {
                        mainwin.row--;
                        fixup = TRUE;
                    }
                    break;
            }
            if( fixup ) {
                fixup = FALSE;
                uivsetcursor( &mainwin );
            }
            if( ev != EV_NO_EVENT ) {
                for( ptr=evstrs; ; ++ptr ){
                    if( ptr->ev == EV_NO_EVENT ) {
                        sprintf( buff, "event 0x%4.4x", ev );
                        break;
                    } else if( ptr->ev == ev ) {
                        strcpy( buff, ptr->str );
                        break;
                    }
                }
                uivtextput( &mainwin, evrow, 2, UIData->attrs[ATTR_NORMAL], buff, 40 );
                if( ++evrow >= mainwin.area.height ){
                    evrow = TOP_ROW;
                }
                uivtextput( &mainwin, evrow, 2, UIData->attrs[ATTR_NORMAL], "", 40 );
                switch( ev ) {
                case EV_MOUSE_PRESS:
                    BandOn = 1;
                    uimousepos( NULL, &mrow, &mcol );
                    BandArea.row = mrow;
                    BandArea.col = mcol;
                    BandArea.width = 0;
                    BandArea.height = 0;
                    uibandinit( BandArea, UIData->attrs[ ATTR_ACTIVE ] );
                    break;
                case EV_MOUSE_DRAG:
                    if( BandOn ) {
                        uimousepos( NULL, &mrow, &mcol );
                        diff = mcol - BandArea.col;
                        if( diff < 0 ) diff = 0;
                        BandArea.width = diff;
                        diff = mrow - BandArea.row;
                        if( diff < 0 ) diff = 0;
                        BandArea.height = diff;
                        uibandmove( BandArea );
                    }
                    break;
                case EV_MOUSE_RELEASE:
                    if( BandOn ) uibandfini();
                    BandOn = 0;
                    break;
                }
            }
        }
        uivclose( &mainwin );
    }
    uinomenus();
    uiswap();
    uirestorebackground();      /* must be after uiswap */
    finimouse();
    ToGraphical();
    uistop();
    FiniSwapper();
    return( 0 );
}
