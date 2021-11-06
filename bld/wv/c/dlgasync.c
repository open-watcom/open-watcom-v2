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
* Description:  Async run dialog
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgwind.h"
#include "guidlg.h"
#include "dbgerr.h"
#include "dlgasync.h"
#include "trapbrk.h"
#include "remasync.h"
#ifdef __RDOS__
#include "stdui.h"
#endif


typedef struct dlg_async {
    unsigned cond;
} dlg_async;

static gui_window   *AsyncWnd = NULL;
static dlg_async    dlg;

void AsyncNotify( void )
{
   if( AsyncWnd ) {
        dlg.cond = PollAsync();
        if( (dlg.cond & COND_RUNNING) == 0 ) {
#ifdef __RDOS__
            rdos_uisendescape();
#else
            GUICloseDialog( AsyncWnd );
            AsyncWnd = NULL;
#endif
        }
    }
}

static bool AsyncGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id      id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        dlg.cond = 0;
        GUISetFocus( gui, CTL_ASYNC_STOP );
        AsyncWnd = gui;
        return( true );
    case GUI_DIALOG_ESCAPE:
        AsyncWnd = NULL;
        dlg.cond = StopAsync();
        return( true );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_ASYNC_STOP:
            AsyncWnd = NULL;
            dlg.cond = StopAsync();
            GUICloseDialog( gui );
            return( true );
        }
        break;
    case GUI_DESTROY:
        AsyncWnd = NULL;
        return( true );
    }
    return( false );
}

unsigned DlgAsyncRun( void )
{
    ResDlgOpen( AsyncGUIEventProc, 0, DIALOG_ASYNC_RUN );
    return( dlg.cond );
}
