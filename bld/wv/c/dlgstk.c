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


#include "dbgdefn.h"
#include "dbginfo.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "guidlg.h"
#include "dlgstk.h"

extern void LastStackPos();
extern void LastMachState();

OVL_EXTERN bool StkOrHistoryEvent( gui_window * gui, gui_event gui_ev, bool *param )
{
    unsigned    id;
    int         *resp;

    resp = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUISetFocus( gui, CTL_STK_YES );
        return( TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_STK_CANCEL:
        case CTL_STK_NO:
        case CTL_STK_YES:
            *resp = id;
            GUICloseDialog( gui );
            return( TRUE );
        }
        return( FALSE );
    }
    return( FALSE );
}

static bool DoStackOrHistory( int id )
{
    int         resp;

    resp = CTL_STK_CANCEL;
    ResDlgOpen( &StkOrHistoryEvent, &resp, id );
    switch( resp ) {
    case CTL_STK_CANCEL:
        _SwitchOn( SW_EXECUTE_ABORTED );
        return( FALSE );
    case CTL_STK_YES:
        if( id == DIALOG_STACK ) {
            LastStackPos();
        } else {
            LastMachState();
        }
        break;
    }
    return( TRUE );
}

bool DlgUpTheStack()
{
    LastStackPos();
    return( TRUE );
}

bool DlgBackInTime( bool lost_mem_state )
{
    if( lost_mem_state ) {
        return( DoStackOrHistory( DIALOG_HISTORY ) );
    } else {
        return( WndDisplayMessage( LIT( WARN_Losing_Redo ), LIT( Empty ),
                             GUI_YES_NO ) == GUI_RET_YES );
    }
}

bool DlgIncompleteUndo()
{
    return( WndDisplayMessage( LIT( WARN_Incomplete_Undo ), LIT( Empty ),
                             GUI_YES_NO ) == GUI_RET_YES );
}
