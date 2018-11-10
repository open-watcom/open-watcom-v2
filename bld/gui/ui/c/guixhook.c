/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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
#include "guixhook.h"

static  bool    (*ProcessEvent)( ui_event ) = NULL;

void GUISetMDIProcessEvent( bool (*func)( ui_event ) )
{
    ProcessEvent = func;
}

bool GUIMDIProcessEvent( ui_event ui_ev )
{
    if( ProcessEvent != NULL ) {
        return( (*ProcessEvent)( ui_ev ) );
    }
    return( false );
}

void GUIFreeStatus( gui_window *wnd )
{
    if( GUIHasStatus( wnd ) ) {
        if( wnd->status->text != NULL ) {
            GUIMemFree( wnd->status->text );
        }
        GUIMemFree( wnd->status );
        wnd->status = NULL;
    }
}

void GUIResizeStatus( gui_window *wnd )
{
    if( GUIHasStatus( wnd ) ) {
        wnd->status->area.width = wnd->use.width;
        wnd->status->area.row = wnd->use.height + 1;
    }
}
