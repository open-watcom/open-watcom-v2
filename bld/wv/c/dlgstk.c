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
* Description:  The 'undo' menu (stack unwinding).
*
****************************************************************************/


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "guidlg.h"
#include "dlgstk.h"


extern void LastStackPos( void );
extern void LastMachState( void );


OVL_EXTERN bool StkOrHistoryEvent( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    gui_ctl_id  *resp;

    resp = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUISetFocus( gui, CTL_STK_YES );
        return( true );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_STK_CANCEL:
        case CTL_STK_NO:
        case CTL_STK_YES:
            *resp = id;
            GUICloseDialog( gui );
            return( true );
        }
        return( false );
    }
    return( false );
}

static bool DoStackOrHistory( res_name_or_id dlg_id )
{
    gui_ctl_id  resp;

    resp = CTL_STK_CANCEL;
    ResDlgOpen( &StkOrHistoryEvent, &resp, dlg_id );
    switch( resp ) {
    case CTL_STK_CANCEL:
        _SwitchOn( SW_EXECUTE_ABORTED );
        return( false );
    case CTL_STK_YES:
        if( dlg_id == GUI_MAKEINTRESOURCE( DIALOG_STACK ) ) {
            LastStackPos();
        } else {
            LastMachState();
        }
        break;
    }
    return( true );
}

bool DlgUpTheStack( void )
{
    LastStackPos();
    return( true );
}

bool DlgBackInTime( bool lost_mem_state )
{
    if( lost_mem_state ) {
        return( DoStackOrHistory( GUI_MAKEINTRESOURCE( DIALOG_HISTORY ) ) );
    } else {
        return( WndDisplayMessage( LIT_DUI( WARN_Losing_Redo ), LIT_ENG( Empty ), GUI_YES_NO ) == GUI_RET_YES );
    }
}

bool DlgIncompleteUndo( void )
{
    return( WndDisplayMessage( LIT_DUI( WARN_Incomplete_Undo ), LIT_ENG( Empty ), GUI_YES_NO ) == GUI_RET_YES );
}
