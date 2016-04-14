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

extern char             NullStr[];

typedef struct pmt_window {
    char        *str;
} pmt_window;
#define WndPmt( wnd ) ( (pmt_window*)WndExtra( wnd ) )

enum {
    CTL_CMD_OK = 100,
    CTL_CMD_CANCEL,
    CTL_CMD_EDIT,
    CTL_CMD_HISTORY,
    CTL_CMD_CHECK,
};

#define DLG_CMD_ROWS    12
#define DLG_CMD_COLS    35

static gui_control_info Controls[] = {
    DLG_EDIT( "",             CTL_CMD_EDIT,     1,  0, 29 ),
    DLG_LIST_BOX( NULL,       CTL_CMD_HISTORY,  1,  2, 29, 5 ),

    DLG_BUTTON( "&OK",        CTL_CMD_OK,       2,  7, 12 ),
    DLG_DEFBUTTON( "&Cancel", CTL_CMD_CANCEL,   18, 7, 28 ),
    DLG_CHECK( "C&heck Me",   CTL_CMD_CHECK,    1,  9, 28 ),
};

#define NUM_CONTROLS ArraySize( Controls )

static char *Stuff[] = { "stuff1", "stuff2", "stuff3" };

static void DoCmd( char *cmd )
{
    Say( cmd );
}


extern bool CmdEvent( gui_window * gui, gui_event gui_ev, void * param )
{
    gui_ctl_id  id;
    char        *text;
    char        *cmd;
    int         i;

    cmd = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUISetText( gui, CTL_CMD_EDIT, "Hi Lisa!" );
        GUISetFocus( gui, CTL_CMD_EDIT );
        GUIClearList( gui, CTL_CMD_HISTORY );
        for( i = 0; i < ArraySize( Stuff ); ++i ) {
            GUIAddText( gui, CTL_CMD_HISTORY, Stuff[i] );
        }
        GUISetCurrSelect( gui, CTL_CMD_HISTORY, 1 );
        return( true );
    case GUI_KEY_CONTROL:
        GUISetCurrSelect( gui, CTL_CMD_HISTORY, 2 );
        cmd = GUIGetText( gui, CTL_CMD_HISTORY );
        GUISetText( gui, CTL_CMD_EDIT, cmd );
        GUISelectAll( gui, CTL_CMD_EDIT, true );
        GUIMemFree( cmd );
        return( true );
    case GUI_CONTROL_DCLICKED:
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_CMD_HISTORY:
            text = GUIGetText( gui, CTL_CMD_HISTORY );
            GUISetText( gui, CTL_CMD_EDIT, text );
            GUIMemFree( text );
            if( gui_ev == GUI_CONTROL_CLICKED )
                return( true );
            /* fall through */
        case CTL_CMD_OK:
            text = GUIGetText( gui, CTL_CMD_EDIT );
            if( text != NULL )
                DoCmd( text );
            GUIMemFree( text );
            break;
        case CTL_CMD_CHECK:
            return( false );
        }
        GUICloseDialog( gui );
        /* fall through */
    case GUI_DESTROY:
        WndFree( cmd );
        return( true );
    default:
        return( false );
    }
}


extern  void    DlgCmd( void )
{
    char        *cmd;

    cmd = WndMustAlloc( 100 );
    GUISetModalDlgs( true );
    DlgOpen( "Enter a command", DLG_CMD_ROWS, DLG_CMD_COLS,
             Controls, NUM_CONTROLS, &CmdEvent, cmd );
    GUISetModalDlgs( false );
}
