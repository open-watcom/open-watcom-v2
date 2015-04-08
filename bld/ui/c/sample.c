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
* Description:  Sample app for the ui library.
*
****************************************************************************/


#ifdef __UNIX__
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#ifndef __UNIX__
#include <process.h>
#endif
#include "uidef.h"
#include "uimenu.h"
#include "uivedit.h"
#include "uigchar.h"
#ifdef __UNIX__
#include "uivirt.h"
#endif

#define TEST_COLOURS
#ifdef TEST_COLOURS
#include "uiattrs.h"
#endif

#define Normal          UIData->attrs[ ATTR_NORMAL ]

extern void sample_dialog( void );

char    radio_full[4];
char    radio_empty[4];
char    check_full[4];
char    check_empty[4];

enum    Symbols { RADIO_BUTTON, CHECK_BOX };
enum    Conditons { ON, OFF };

extern EVENT    LineEvents[];

#define         EV_QUIT         EV_FUNC( 2 )
enum {
        EV_OPEN = EV_FIRST_UNUSED,
        EV_CLOSE,
        EV_CUT,
        EV_PASTE,
        EV_GO,
        EV_SAMPLE_DIALOG,
        EV_EXTRA,
        EV_NOTHING
};

static MENUITEM secondpopup[] = {
        { "Dialog",   EV_SAMPLE_DIALOG, 1 },
        { "",         EV_NO_EVENT,      0 },
        { "Exit",     EV_QUIT,          1 },
        NULL
};

static MENUITEM filemenu[] = {
        { "Open",     EV_OPEN,          0 },
        { "Close",    EV_CLOSE,         0 },
        { "Nothing",  EV_NOTHING,       2 },
        { "Extra",    EV_EXTRA,         0, secondpopup },
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
        { "File",               EV_NO_EVENT,      0 },
        { "Edit",               EV_NO_EVENT,      1 },
        { "LongMenuItem_1",     EV_NO_EVENT,      1 },
        { "LongMenuItem_2",     EV_NO_EVENT,      1 },
        { "LongMenuItem_3",     EV_NO_EVENT,      1 },
        { "LongMenuItem_4",     EV_NO_EVENT,      1 },
        { "LongMenuItem_5",     EV_NO_EVENT,      1 },
        { "MidFile",            EV_NO_EVENT,      2 },
        { "LongMenuItem_6",     EV_NO_EVENT,      1 },
        { "LongMenuItem_7",     EV_NO_EVENT,      1 },
        { "LongMenuItem_8",     EV_NO_EVENT,      1 },
        { "LongMenuItem_9",     EV_NO_EVENT,      1 },
        { "Go!",                EV_GO,            0 },
        { "File",               EV_NO_EVENT,      2 },
        NULL
};

static MENUITEM *pulldownuimenus[] = {
        filemenu,
        editmenu,
        editmenu,
        editmenu,
        editmenu,
        editmenu,
        editmenu,
        filemenu,
        editmenu,
        editmenu,
        editmenu,
        editmenu,
        NULL,
        filemenu
};

static VSCREEN mainwin = {
    EV_NO_EVENT,     /* used for mouse clicks          */
    "Window Title",  /* displayed if there is a frame  */
    5, 10, 18, 60,   /* row, col, height, width        */
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

static EVENT             evlist[] = {
        EV_FIRST_EVENT, EV_LAST_KEYBOARD,
        EV_MOUSE_PRESS, EV_MOUSE_RELEASE,
        EV_MOUSE_HOLD_R, EV_MOUSE_REPEAT_M,
        EV_OPEN,        EV_NOTHING,
        EV_NO_EVENT,
        EV_MOUSE_DCLICK,
        EV_MOUSE_REPEAT,
        EV_IDLE,
        EV_MENU_INITPOPUP,
        EV_CLOCK_TICK,
        EV_BACKGROUND_RESIZE,
        EV_NO_EVENT
};


static EVENT oplist[] = {
        EV_NO_EVENT,
        EV_RETURN,
        EV_ESCAPE,
        EV_NO_EVENT
};

typedef struct an_event_string{
    EVENT       ev;
    char        *str;
}an_event_string;

static          an_event_string         evstrs[] = {
    { EV_MOUSE_PRESS,   "EV_MOUSE_PRESS" },
    { EV_MOUSE_RELEASE, "EV_MOUSE_RELEASE" },
    { EV_MOUSE_DRAG,    "EV_MOUSE_DRAG" },
    { EV_MOUSE_REPEAT,  "EV_MOUSE_REPEAT" },
    { EV_MOUSE_DCLICK,  "EV_MOUSE_DCLICK" },
    { EV_MOUSE_PRESS_R, "EV_MOUSE_PRESS_R" },
    { EV_MOUSE_RELEASE_R,"EV_MOUSE_RELEASE_R" },
    { EV_MOUSE_DRAG_R,  "EV_MOUSE_DRAG_R" },
    { EV_MOUSE_REPEAT_R,"EV_MOUSE_REPEAT_R" },
    { EV_MOUSE_DCLICK_R,"EV_MOUSE_DCLICK_R" },
    { EV_MOUSE_PRESS_M, "EV_MOUSE_PRESS_M" },
    { EV_MOUSE_RELEASE_M,"EV_MOUSE_RELEASE_M" },
    { EV_MOUSE_DRAG_M,  "EV_MOUSE_DRAG_M" },
    { EV_MOUSE_REPEAT_M,"EV_MOUSE_REPEAT_M" },
    { EV_MOUSE_DCLICK_M,"EV_MOUSE_DCLICK_M" },
    { EV_ALT_PRESS,     "EV_ALT_PRESS" },
    { EV_ALT_RELEASE,   "EV_ALT_RELEASE" },
    { EV_IDLE,          "EV_IDLE" },
    { EV_MENU_INITPOPUP,"EV_MENU_INITPOPUP" },
    { EV_BACKGROUND_RESIZE, "EV_BACKGROUND_RESIZE" },
    { EV_NO_EVENT, NULL }
};

static SAREA    BandArea;
int             BandOn = 0;


 static void open()
/******************/
{
    EVENT ev;


    if( uivopen( &opwin ) ) {
        uipushlist( oplist );
        uivtextput( &opwin, 1, 2, UIData->attrs[ ATTR_NORMAL ],
                "Enter file name.", 16 );
        inputline.attr = UIData->attrs[ ATTR_EDIT ];
        /* blank out the buffer */
        inputline.index = 0;
        inputline.scroll = 0;
        inputline.update = TRUE;
        for( ; ; ) {
            ev = uiveditline( &opwin, &inputline );
            if( ev != EV_NO_EVENT ) break;
        }
        if( ev == EV_RETURN ) {
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

 void main()
/***********/
{
    EVENT               ev;
    SAREA               area;
    char                buff[80];
    an_event_string     *ptr;
    ORD                 evrow = TOP_ROW;
    int                 mrow, mcol;
    int                 diff;

#ifdef CHARMAP
    if( uistart() ) {
        uiinitgmouse( 2 );  /* the 0=mouseless,1=mouse,2=initialized mouse */
//      uivgaattrs();
        FlipCharacterMap();
#else
    if( uistart() ) {
#ifdef __UNIX__
        _initmouse( 2 );
#else
        initmouse( 2 );
#endif
#endif
        uimenus( barmenu, pulldownuimenus, EV_FUNC( 1 ) );
        UIData->mouse_clk_delay = uiclockdelay( 250 );
        UIData->tick_delay = uiclockdelay( 3000 );
        mainwin.area.height = UIData->height - 7;
        if( uivopen( &mainwin ) ) {
            for( ; ; ) {
                uipushlist( evlist );
                ev = uivgetevent( &mainwin );
                uipoplist();
                if( ev == EV_MOUSE_PRESS_R ) {
                    uimousepos( NULL, &mrow, &mcol );
                    mrow++;
                    mcol++;
                    uipushlist( evlist );
                    ev = uicreatepopup( mrow, mcol, filemenu, FALSE, TRUE, (int)NULL );
                    uipoplist();
                }
                switch ( ev ) {
                    case EV_QUIT:
                        break;
                    case EV_BACKGROUND_RESIZE:
                        uivclose( &mainwin );
                        mainwin.area.height = UIData->height - 7;
                        uivopen( &mainwin );
                        if( evrow > area.height ) {
                            evrow = TOP_ROW;
                        }
                        break;
                    case EV_SAMPLE_DIALOG:
                        sample_dialog();
                        break;
                    case EV_OPEN:
                        open();
                        break;
                    case EV_GO:
                        uispawnstart();
                        uiblankscreen();
                        printf( "Type exit to return\n" );
#if defined( __QNX__ ) || defined( __UNIX__ )
                        system( "" );
#elif defined( __WINDOWS__ )
                        // do nothing
#else
                        {
                            char *command;

                            command = getenv( "COMSPEC" );
                            if( command == NULL ){
                                command = "c:\\command.com";
                            }
                            system( command );
                        }
#endif
                        uispawnend();
//                      uibackground( "wf.img" );
                        area.row = 0;
                        area.col = 0;
                        area.height = 25;
                        area.width = 80;
                        uidirty( area );
                        break;
                    case EV_FUNC(1):
                        area.width = 10;
                        area.height = 10;
                        area.row = 1;
                        area.col = 1;
                        uivattribute( &mainwin, area, (ATTR) 1 );
                        break;
                }
                if( ev == EV_QUIT ) {
                    break;
                } else if( ev != EV_NO_EVENT ) {
                    for( ptr=evstrs; ; ++ptr ){
                        if( ptr->ev == EV_NO_EVENT ) {
                            sprintf( buff, "event 0x%4.4x", ev );
                            break;
                        } else if( ptr->ev == ev ) {
                            sprintf( buff, "event 0x%4.4x (%s)", ev, ptr->str );
                            break;
                        }
                    }
                    uivtextput( &mainwin, evrow, 2,
                              UIData->attrs[ ATTR_NORMAL ],
                              buff, 30 );
                    if( ++evrow >= mainwin.area.height ){
                        evrow = TOP_ROW;
                    }
                    uivtextput( &mainwin, evrow, 2,
                              UIData->attrs[ ATTR_NORMAL ],
                              "", 30 );
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
        uirestorebackground();  /* must be after uiswap */
#ifdef CHARMAP
        FlipCharacterMap();
        uifinigmouse();
#else
#ifdef __UNIX__
        _finimouse();
#else
        finimouse();
#endif
#endif
        uistop();
    }
}

