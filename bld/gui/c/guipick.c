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


#include "guiwind.h"
#include "guidlg.h"
#include "guistr.h"
#include "guipick.h"
#include <string.h>


#define DLG_PICK_ROWS   14
#define DLG_PICK_COLS   30
#define DLG_MAX_COLS    75

static gui_control_info Controls[] = {
    DLG_LIST_BOX(       NULL, CTL_PICK_LIST,            1,  1, 29, 10 ),
    DLG_DEFBUTTON(      NULL, CTL_PICK_OK,              2,  12, 12 ),
    DLG_BUTTON(         NULL, CTL_PICK_CANCEL,  18,  12, 28 ),
};

#define NUM_CONTROLS ( sizeof( Controls ) / sizeof( gui_control_info ) )

bool GUIPickEvent( gui_window *gui, gui_event gui_ev, void *param )
{
    unsigned            id;
    dlg_pick            *dlg;

    dlg = (dlg_pick*)GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUIClearList( gui, CTL_PICK_LIST );
        (*dlg->func)( gui, CTL_PICK_LIST );
        GUISetFocus( gui, CTL_PICK_LIST );
        return( true );
    case GUI_CONTROL_DCLICKED:
        GUI_GETID( param, id );
        if( id == CTL_PICK_LIST ) {
            dlg->chosen = GUIGetCurrSelect( gui, CTL_PICK_LIST );
            GUICloseDialog( gui );
        }
        return( true );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_PICK_OK:
            dlg->chosen = GUIGetCurrSelect( gui, CTL_PICK_LIST );
            /* fall through */
        case CTL_PICK_CANCEL:
            GUICloseDialog( gui );
            break;
        }
        return( true );
    default:
        return( false );
    }
}


int GUIDlgPickWithRtn( const char *title, PICKCALLBACK *pickinit, PICKDLGOPEN *OpenRtn )
{
    dlg_pick    dlg;

    int len = strlen( title );
    if( len < DLG_PICK_COLS )
        len = DLG_PICK_COLS;
    if( len > DLG_MAX_COLS )
        len = DLG_MAX_COLS;

    Controls[1].text = LIT( OK );
    Controls[2].text = LIT( Cancel );
    dlg.func = pickinit;
    dlg.chosen = -1;
    OpenRtn( title, DLG_PICK_ROWS, len, Controls, NUM_CONTROLS, &GUIPickEvent, &dlg );
    return( dlg.chosen );
}


int GUIDlgPick( const char *title, PICKCALLBACK *pickinit )
{
    return( GUIDlgPickWithRtn( title, pickinit, GUIDlgOpen ) );
}
