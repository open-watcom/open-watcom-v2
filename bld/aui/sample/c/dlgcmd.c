/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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

typedef struct pmt_window {
    char        *str;
} pmt_window;
#define WndPmt( wnd ) ( (pmt_window*)WndExtra( wnd ) )

#define DLG_CMD_ROWS    12
#define DLG_CMD_COLS    35

#define DLGNEW_CTLS() \
    pick_p4id( EDIT,    GUI_CTL_EDIT,       "",             1,  0, 29 ) \
    pick_p5id( HISTORY, GUI_CTL_LIST_BOX,   NULL,           1,  2, 29, 4 ) \
    pick_p4id( OK,      GUI_CTL_BUTTON,     "&OK",          2,  7, 11 ) \
    pick_p4id( CANCEL,  GUI_CTL_DEFBUTTON,  "&Cancel",      18, 7, 11 ) \
    pick_p4id( CHECK,   GUI_CTL_CHECK,      "C&heck Me",    1,  9, 28 )

enum {
    #define pick_p4id(id,m,p1,p2,p3,p4)     CTL_ ## id,
    #define pick_p5id(id,m,p1,p2,p3,p4,p5)  CTL_ ## id,
    DLGNEW_CTLS()
    #undef pick_p5id
    #undef pick_p4id
};

enum {
    #define pick_p4id(id,m,p1,p2,p3,p4)     id ## _IDX,
    #define pick_p5id(id,m,p1,p2,p3,p4,p5)  id ## _IDX,
    DLGNEW_CTLS()
    #undef pick_p5id
    #undef pick_p4id
};

static gui_control_info Controls[] = {
    #define pick_p4id(id,m,p1,p2,p3,p4)     m(p1,CTL_ ## id,p2,p3,p4),
    #define pick_p5id(id,m,p1,p2,p3,p4,p5)  m(p1,CTL_ ## id,p2,p3,p4,p5),
    DLGNEW_CTLS()
    #undef pick_p5id
    #undef pick_p4id
};

#define NUM_CONTROLS ArraySize( Controls )

extern char             NullStr[];

static char *Stuff[] = { "stuff1", "stuff2", "stuff3" };

static void DoCmd( char *cmd )
{
    Say( cmd );
}


static bool CmdGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    char        *text;
    char        *cmd;
    int         i;

    cmd = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUISetText( gui, CTL_EDIT, "Hi Lisa!" );
        GUISetFocus( gui, CTL_EDIT );
        GUIClearList( gui, CTL_HISTORY );
        for( i = 0; i < ArraySize( Stuff ); ++i ) {
            GUIAddText( gui, CTL_HISTORY, Stuff[i] );
        }
        GUISetCurrSelect( gui, CTL_HISTORY, 1 );
        return( true );
    case GUI_KEY_CONTROL:
        GUISetCurrSelect( gui, CTL_HISTORY, 2 );
        cmd = GUIGetText( gui, CTL_HISTORY );
        GUISetText( gui, CTL_EDIT, cmd );
        GUISelectAll( gui, CTL_EDIT, true );
        GUIMemFree( cmd );
        return( true );
    case GUI_CONTROL_DCLICKED:
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_HISTORY:
            text = GUIGetText( gui, CTL_HISTORY );
            GUISetText( gui, CTL_EDIT, text );
            GUIMemFree( text );
            if( gui_ev == GUI_CONTROL_CLICKED )
                return( true );
            /* fall through */
        case CTL_OK:
            text = GUIGetText( gui, CTL_EDIT );
            if( text != NULL )
                DoCmd( text );
            GUIMemFree( text );
            /* fall through */
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( true );
        }
        break;
    case GUI_DESTROY:
        WndFree( cmd );
        return( true );
    default:
        break;
    }
    return( false );
}


void    DlgCmd( void )
{
    char        *cmd;

    cmd = WndMustAlloc( 100 );
    GUISetModalDlgs( true );
    DlgOpen( "Enter a command", DLG_CMD_ROWS, DLG_CMD_COLS,
             Controls, NUM_CONTROLS, &CmdGUIEventProc, cmd );
    GUISetModalDlgs( false );
}
